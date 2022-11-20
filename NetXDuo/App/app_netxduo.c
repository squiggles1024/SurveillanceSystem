/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_netxduo.c
  * @author  MCD Application Team
  * @brief   NetXDuo applicative file
  ******************************************************************************
    * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_netxduo.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_azure_rtos.h"
#include "nx_ip.h"
#include CERT_FILE
#include "app_threadx.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define STRLEN(p)                   (sizeof(p) - 1)
#define MQTT_TopicBufferLen         (32)
#define MQTT_MessageBufferLen       (32)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
NX_PACKET_POOL PacketPool;
NX_IP           IP;
NX_DHCP         DHCPClient;
NX_SNTP_CLIENT  SNTPClient;
NXD_MQTT_CLIENT MQTTClient;
static NX_DNS   DNSClient;

TX_THREAD IPInitThread, SNTPInitThread, MQTTThread;

TX_SEMAPHORE IP_ADDR_SEM;
TX_SEMAPHORE SNTP_UPDATE_SEM;
static UINT MessageCount = 0;

static ULONG CurrentTime;

static ULONG MqttClientStack[MQTT_CLIENT_STACK_SIZE];
static CHAR CryptoMetadataClient[11600];
static UCHAR TLSPacketBuffer[4000];
extern const NX_SECURE_TLS_CRYPTO nx_crypto_tls_ciphers;

extern unsigned int mosquitto_der_len;
extern unsigned char mosquitto_der[];

static UCHAR MQTT_TopicBuffer[MQTT_TopicBufferLen];
static UCHAR MQTT_MessageBuffer[MQTT_MessageBufferLen];

TX_EVENT_FLAGS_GROUP MQTT_TREvent;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static VOID IP_Init(ULONG input);
static VOID SNTP_Init(ULONG input);
static VOID MQTT_Loop(ULONG input);
static VOID IP_Address_Changed_CB(NX_IP *IPInstance, VOID *Input);
static VOID Time_Update_CB(NX_SNTP_TIME_MESSAGE  *TimeMsg, NX_SNTP_TIME *LocalTime);
static VOID MQTTMessageRecieved_CB(NXD_MQTT_CLIENT* Client, UINT MsgCount);
static VOID MQTTDisconnected_CB(NXD_MQTT_CLIENT *Client);
static UINT TLS_Init(NXD_MQTT_CLIENT *Client, NX_SECURE_TLS_SESSION *TLS_Session, NX_SECURE_X509_CERT *Cert, NX_SECURE_X509_CERT *TrustedCert);
static ULONG GetSessionTime(void);
/* USER CODE END PFP */
/**
  * @brief  Application NetXDuo Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT MX_NetXDuo_Init(VOID *memory_ptr)
{
  UINT ret = NX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

   /* USER CODE BEGIN App_NetXDuo_MEM_POOL */

  /*ALLOCATE MEMORY FOR PACKET POOL FROM "nx_byte_pool_buffer" IN "app_azure_rtos.c"*/
  CHAR* Ptr;
  ret = tx_byte_allocate(byte_pool, (VOID**) &Ptr, PACKET_POOL_SIZE, TX_NO_WAIT);
  if(ret != TX_SUCCESS)
  {
	  return TX_POOL_ERROR;
  }

  /*CREATE PACKET POOL*/
  ret = nx_packet_pool_create(&PacketPool, "Main Network Packet Pool", PAYLOAD_SIZE, Ptr, PACKET_POOL_SIZE);

  if(ret != NX_SUCCESS)
  {
	  return ret;
  }
  /* USER CODE END App_NetXDuo_MEM_POOL */

  /* USER CODE BEGIN MX_NetXDuo_Init */
  /* ALLOCATE MEMORY FOR IP INSTANCE */
  ret = tx_byte_allocate(byte_pool, (VOID**)&Ptr, IP_HELPER_STACK_SIZE, TX_NO_WAIT);
  if(ret != TX_SUCCESS)
  {
	  return TX_POOL_ERROR;
  }

  /*CREATE IP INSTANCE*/
  ret = nx_ip_create(&IP,                      //IP Object
		             "IP Instance",            //Name
					 NULL_ADDRESS,             //IP Address
					 NULL_ADDRESS,             //Net Mask
					 &PacketPool,              //Packet Pool
					 nx_driver_emw3080_entry,  //IP Network Driver
					 Ptr,                      //Memory Ptr
					 IP_HELPER_STACK_SIZE,     //Memory Size
					 15);                      //Priority of IP Helper thread

  if(ret != NX_SUCCESS)
  {
	  return NX_NOT_ENABLED;
  }

  /*ALLOCATE MEMORY FOR ARP*/
  ret = tx_byte_allocate(byte_pool, (VOID**) &Ptr, ARP_STACK_SIZE, TX_NO_WAIT);
  if(ret != TX_SUCCESS)
  {
	  return TX_POOL_ERROR;
  }

  /*ENABLE ARP*/
  ret = nx_arp_enable(&IP, (VOID**)Ptr, ARP_STACK_SIZE);
  if(ret != NX_SUCCESS)
  {
	  return ret;
  }

  /*CREATE DHCP CLIENT*/
  ret = nx_dhcp_create(&DHCPClient, &IP, "DHCP Client");

  if(ret != NX_SUCCESS)
  {
	  return ret;
  }

  /*ENABLE ICMP (required by any tcp/ip stack)*/
  ret = nx_icmp_enable(&IP);
  if(ret != NX_SUCCESS)
  {
	  return ret;
  }

  /*ENABLE UDP REQUIRED FOR SNTP AND DHCP*/
  ret = nx_udp_enable(&IP);
  if(ret != NX_SUCCESS)
  {
	  return ret;
  }

  /*ENABLE TCP REQUIRED FOR MQTT OVER TLS AND DNS*/
  ret = nx_tcp_enable(&IP);
  if(ret != NX_SUCCESS)
  {
	  return ret;
  }

  /*Create and allocate space for IP Init Thread*/
  ret = tx_byte_allocate(byte_pool, (VOID**) &Ptr, IP_INIT_STACK_SIZE, TX_NO_WAIT);
  if(ret != TX_SUCCESS)
  {
	  return ret;
  }

  ret = tx_thread_create(&IPInitThread,
		                 "IP Init Thread",
						 IP_Init,
                         0,
						 Ptr,
						 IP_INIT_STACK_SIZE,
                         5,
						 5,
						 TX_NO_TIME_SLICE,
						 TX_AUTO_START);

  /*Create and allocate space for SNTP Init Thread*/
  ret = tx_byte_allocate(byte_pool, (VOID**)&Ptr, SNTP_STACK_SIZE, TX_NO_WAIT);
  if(ret != TX_SUCCESS)
  {
	  return ret;
  }

  ret = tx_thread_create(&SNTPInitThread,
		                 "SNTP Init Thread",
						 SNTP_Init,
						 0,
						 Ptr,
						 SNTP_STACK_SIZE,
						 5,
						 5,
						 TX_NO_TIME_SLICE,
						 TX_DONT_START);

  /*Create and allocate space for MQTT Thread*/
  ret = tx_byte_allocate(byte_pool, (VOID**)&Ptr, MQTT_STACK_SIZE, TX_NO_WAIT);
  if(ret != TX_SUCCESS)
  {
	  return ret;
  }

  ret = tx_thread_create(&MQTTThread,
		                 "MQTT Thread",
						 MQTT_Loop,
						 0,
						 Ptr,
						 MQTT_STACK_SIZE,
						 15,
						 15,
						 1,
						 TX_DONT_START);
  ret = tx_event_flags_create(&MQTT_TREvent, "MQTT TxRx Event");
  /* USER CODE END MX_NetXDuo_Init */

  return ret;
}

/* USER CODE BEGIN 1 */
static VOID IP_Init(ULONG input)
{
	  UINT ret = NX_SUCCESS;
	  ULONG AssignedIPAddress = 0;
	  ULONG SubnetMask = 0;

	  /* Create a DNS instance for the Client */
	  ret = nx_dns_create(&DNSClient, &IP, (UCHAR *)"DNS Client");
	  if (ret != NX_SUCCESS)
	  {
	      Error_Handler();
	  }

	  /* Initialize DNS instance with a dummy server */
	  ret = nx_dns_server_add(&DNSClient, DNS_ADDRESS);
	  if (ret != NX_SUCCESS)
	  {
	      Error_Handler();
	  }

	  /*Create Semaphore to wait for IP Address*/
      tx_semaphore_create(&IP_ADDR_SEM, "IP Address Semaphore", 0);
	  ret = nx_ip_address_change_notify(&IP, IP_Address_Changed_CB, NULL);
	  if (ret != NX_SUCCESS)
	  {
	      Error_Handler();
	  }

	  /*Start DHCP Client*/
	  ret = nx_dhcp_start(&DHCPClient);
	  if (ret != NX_SUCCESS)
	  {
	      Error_Handler();
	  }

	  /*Wait for IP Address*/
	  if(tx_semaphore_get(&IP_ADDR_SEM, TX_WAIT_FOREVER) != TX_SUCCESS)
	  {
		  Error_Handler();
	  }

	  /*Retrieve IP Address from Network IF*/
	  ret = nx_ip_address_get(&IP, &AssignedIPAddress, &SubnetMask);
	  if (ret != NX_SUCCESS)
	  {
	      Error_Handler();
	  }


      printf("IP Address Assigned: %lu\r\n", AssignedIPAddress);
	  tx_thread_resume(&SNTPInitThread);

	  //Kill the thread as its not needed anymore.
	  tx_thread_terminate(&IPInitThread);
}

static VOID SNTP_Init(ULONG input)
{
	UINT ret = NX_SUCCESS;
	UINT ServerStatus;
	ULONG Fraction;
	NXD_ADDRESS SNTP_Server_IP;
	SNTP_Server_IP.nxd_ip_version = 4;

	/*Get time.google IP address*/
	ret = nx_dns_host_by_name_get(&DNSClient, (UCHAR *)SNTP_SERVER_URL, &SNTP_Server_IP.nxd_ip_address.v4,SNTP_TIMEOUT);
	if(ret != NX_SUCCESS)
	{
		Error_Handler();
	}

	/*Create the SNTP client*/
	ret = nx_sntp_client_create(&SNTPClient,&IP,0, &PacketPool, NULL,NULL,NULL);
	if(ret != NX_SUCCESS)
	{
		Error_Handler();
	}

	//Create Semaphore waiting for time response
    tx_semaphore_create(&SNTP_UPDATE_SEM, "SNTP Semaphore",0);

    /*Set time update callback*/
	ret = nx_sntp_client_set_time_update_notify(&SNTPClient,Time_Update_CB);
	if (ret != NX_SUCCESS)
	{
	    Error_Handler();
	}

	/* Use the IPv4 service to set up the Client and set the IPv4 SNTP server. */
	ret = nx_sntp_client_initialize_unicast(&SNTPClient, SNTP_Server_IP.nxd_ip_address.v4);
	if (ret != NX_SUCCESS)
	{
	    Error_Handler();
	}

	//Run SNTP Client
	ret = nx_sntp_client_run_unicast(&SNTPClient);
	if (ret != NX_SUCCESS)
	{
	    Error_Handler();
	}

	 /*Wait for Time*/
	if(tx_semaphore_get(&SNTP_UPDATE_SEM, TX_WAIT_FOREVER) != TX_SUCCESS)
	{
		Error_Handler();
	}

	ret = nx_sntp_client_receiving_updates(&SNTPClient,&ServerStatus);
	if(ret != NX_SUCCESS || ServerStatus == NX_FALSE)
	{
		Error_Handler();
	}

    /* We have a valid update.  Get the SNTP Client time.  */
    ret = nx_sntp_client_get_local_time_extended(&SNTPClient, &CurrentTime, &Fraction, NX_NULL, 0);
	if(ret != NX_SUCCESS)
	{
		Error_Handler();
	}

	nx_sntp_client_stop(&SNTPClient);

    /* take off 70 years difference */
    CurrentTime = CurrentTime - EPOCH_TIME_DIFF;

    tx_thread_resume(&MQTTThread);
    tx_thread_terminate(&SNTPInitThread);
}

static VOID MQTT_Loop(ULONG input)
{
	UINT ret = NX_SUCCESS;
	ULONG MQTTEvents = 0;
	NXD_ADDRESS MQTTServerIP;
	UINT ActualTopicLength = 0;
	UINT ActualMessageLength = 0;
	ULONG MessageCounter = 0;

	float QueueRecieveBuffer;

	MQTTServerIP.nxd_ip_address.v4 = MQTT_SERVER_IP;
	MQTTServerIP.nxd_ip_version = 4;
	if(ret != NX_SUCCESS)
	{
		Error_Handler();
	}

	ret = nxd_mqtt_client_create(&MQTTClient,
			                     "MQTT Client",
			                     "Drone Client ID",
								 STRLEN("Drone Client ID"),
								 &IP,
								 &PacketPool,
								 (VOID*) MqttClientStack,
								 MQTT_CLIENT_STACK_SIZE,
								 10,
								 NX_NULL,
								 0);
	if(ret != NX_SUCCESS)
	{
		Error_Handler();
	}

	nxd_mqtt_client_receive_notify_set(&MQTTClient, MQTTMessageRecieved_CB);
	nxd_mqtt_client_disconnect_notify_set(&MQTTClient, MQTTDisconnected_CB);

	if(ret != TX_SUCCESS)
	{
		Error_Handler();
	}


	ret = nxd_mqtt_client_secure_connect(&MQTTClient,
			                             &MQTTServerIP,
									     NXD_MQTT_TLS_PORT,
										 TLS_Init,
										 MQTT_KEEPALIVE_TIMER,
										 NX_TRUE,
										 NX_WAIT_FOREVER);
	if(ret != NX_SUCCESS)
	{
		Error_Handler();
	}

	nxd_mqtt_client_subscribe(&MQTTClient, MQTT_CLIENT_SUB_TOPIC01, STRLEN(MQTT_CLIENT_SUB_TOPIC01), 1);

    while(1)
    {
    	tx_event_flags_get(&MQTT_TREvent, MESSAGE_ALL_EVT_Msk, TX_OR_CLEAR, &MQTTEvents, TX_WAIT_FOREVER);

    	if(MQTTEvents & MESSAGE_RECIEVED_EVT_Msk)
    	{
            while(MessageCount > 0)
            {

            	ret = nxd_mqtt_client_message_get(&MQTTClient,
            			                          MQTT_TopicBuffer,
												  MQTT_TopicBufferLen,
												  &ActualTopicLength,
												  MQTT_MessageBuffer,
												  MQTT_MessageBufferLen,
												  &ActualMessageLength
												  );
            	if(ret != NXD_MQTT_SUCCESS)
            	{
            		Error_Handler();
            	}
            	printf("Message %lu received: TOPIC = %s, MESSAGE = %s\n", MessageCounter + 1, MQTT_TopicBuffer, MQTT_MessageBuffer);
            	MessageCount--;
            	MessageCounter++;
            }
    	}

    	if(MQTTEvents & MESSAGE_TRANSMIT_PUB01_EVT_Msk)
    	{
            tx_queue_receive(&TemperatureQueue,(VOID*)&QueueRecieveBuffer,TX_WAIT_FOREVER);
            nxd_mqtt_client_publish(&MQTTClient, MQTT_CLIENT_PUB_TOPIC01,
            		                STRLEN(MQTT_CLIENT_PUB_TOPIC01),
									(VOID*)&QueueRecieveBuffer,
									sizeof(QueueRecieveBuffer),
									NX_TRUE,
									0,
									NX_WAIT_FOREVER);
    	}

    	if(MQTTEvents & MESSAGE_TRANSMIT_PUB02_EVT_Msk)
    	{
            tx_queue_receive(&HumidityQueue,(VOID*)&QueueRecieveBuffer,TX_WAIT_FOREVER);
            nxd_mqtt_client_publish(&MQTTClient, MQTT_CLIENT_PUB_TOPIC02,
            		                STRLEN(MQTT_CLIENT_PUB_TOPIC02),
									(VOID*)&QueueRecieveBuffer,
									sizeof(QueueRecieveBuffer),
									NX_TRUE,
									0,
									NX_WAIT_FOREVER);
    	}

    	if(MQTTEvents & MESSAGE_TRANSMIT_PUB03_EVT_Msk)
    	{
            tx_queue_receive(&PressureQueue,(VOID*)&QueueRecieveBuffer,TX_WAIT_FOREVER);
            nxd_mqtt_client_publish(&MQTTClient, MQTT_CLIENT_PUB_TOPIC03,
            		                STRLEN(MQTT_CLIENT_PUB_TOPIC03),
									(VOID*)&QueueRecieveBuffer,
									sizeof(QueueRecieveBuffer),
									NX_TRUE,
									0,
									NX_WAIT_FOREVER);
    	}

    	if(MQTTEvents & MESSAGE_TRANSMIT_PUB04_EVT_Msk)
    	{
            tx_queue_receive(&LightQueue,(VOID*)&QueueRecieveBuffer,TX_WAIT_FOREVER);
            nxd_mqtt_client_publish(&MQTTClient, MQTT_CLIENT_PUB_TOPIC04,
            		                STRLEN(MQTT_CLIENT_PUB_TOPIC04),
									(VOID*)&QueueRecieveBuffer,
									sizeof(QueueRecieveBuffer),
									NX_TRUE,
									0,
									NX_WAIT_FOREVER);
    	}

    	if(MQTTEvents & MESSAGE_TRANSMIT_PUB05_EVT_Msk)
    	{
            tx_queue_receive(&MagXQueue,(VOID*)&QueueRecieveBuffer,TX_WAIT_FOREVER);
            nxd_mqtt_client_publish(&MQTTClient, MQTT_CLIENT_PUB_TOPIC05,
            		                STRLEN(MQTT_CLIENT_PUB_TOPIC05),
									(VOID*)&QueueRecieveBuffer,
									sizeof(QueueRecieveBuffer),
									NX_TRUE,
									0,
									NX_WAIT_FOREVER);
    	}

    	if(MQTTEvents & MESSAGE_TRANSMIT_PUB06_EVT_Msk)
    	{
            tx_queue_receive(&MagYQueue,(VOID*)&QueueRecieveBuffer,TX_WAIT_FOREVER);
            nxd_mqtt_client_publish(&MQTTClient, MQTT_CLIENT_PUB_TOPIC06,
            		                STRLEN(MQTT_CLIENT_PUB_TOPIC06),
									(VOID*)&QueueRecieveBuffer,
									sizeof(QueueRecieveBuffer),
									NX_TRUE,
									0,
									NX_WAIT_FOREVER);
    	}


    	if(MQTTEvents & MESSAGE_TRANSMIT_PUB07_EVT_Msk)
    	{
            tx_queue_receive(&MagZQueue,(VOID*)&QueueRecieveBuffer,TX_WAIT_FOREVER);
            nxd_mqtt_client_publish(&MQTTClient, MQTT_CLIENT_PUB_TOPIC07,
            		                STRLEN(MQTT_CLIENT_PUB_TOPIC07),
									(VOID*)&QueueRecieveBuffer,
									sizeof(QueueRecieveBuffer),
									NX_TRUE,
									0,
									NX_WAIT_FOREVER);
    	}


    }
}

static VOID IP_Address_Changed_CB(NX_IP *IPInstance, VOID *Input)
{
	tx_semaphore_put(&IP_ADDR_SEM);
}

static VOID Time_Update_CB(NX_SNTP_TIME_MESSAGE  *TimeMsg, NX_SNTP_TIME *LocalTime)
{
	tx_semaphore_put(&SNTP_UPDATE_SEM);
}

static VOID MQTTMessageRecieved_CB(NXD_MQTT_CLIENT* Client, UINT MsgCount)
{
  //Push Message Count
  MessageCount = MessageCount + MsgCount;
  //Signal Receive Event
  tx_event_flags_set(&MQTT_TREvent,MESSAGE_RECIEVED_EVT_Msk, TX_OR);
}

static VOID MQTTDisconnected_CB(NXD_MQTT_CLIENT *Client)
{
  //Handle Disconnects
  //Try to reconnect?
  //Gracefully Land?
}

static UINT TLS_Init(NXD_MQTT_CLIENT *Client, NX_SECURE_TLS_SESSION *TLS_Session, NX_SECURE_X509_CERT *Cert, NX_SECURE_X509_CERT *TrustedCert)
{
	UINT ret = NX_SUCCESS;

	nx_secure_tls_initialize();
	ret = nx_secure_tls_session_create(TLS_Session, &nx_crypto_tls_ciphers, CryptoMetadataClient, sizeof(CryptoMetadataClient));
	if(ret != NX_SUCCESS)
	{
		Error_Handler();
	}

	memset(Cert, 0, sizeof(NX_SECURE_X509_CERT));

	ret = nx_secure_tls_session_time_function_set(TLS_Session,GetSessionTime);
    if(ret != NX_SUCCESS)
    {
    	Error_Handler();
    }

    ret = nx_secure_tls_session_packet_buffer_set(TLS_Session, TLSPacketBuffer, sizeof(TLSPacketBuffer));
    if(ret != NX_SUCCESS)
    {
    	Error_Handler();
    }

    ret = nx_secure_x509_certificate_initialize(TrustedCert,
    		                                   (UCHAR*)mosquitto_der,
											   mosquitto_der_len,
											   NX_NULL,
											   0,
											   NULL,
											   0,
											   NX_SECURE_X509_KEY_TYPE_NONE);
    if(ret != NX_SUCCESS)
    {
    	Error_Handler();
    }

    ret = nx_secure_tls_trusted_certificate_add(TLS_Session, TrustedCert);
    if(ret != NX_SUCCESS)
    {
    	Error_Handler();
    }

    return ret;
}

static ULONG GetSessionTime(void)
{
	return CurrentTime;
}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
  switch (GPIO_Pin)
  {
    case (GPIO_PIN_1):
      break;

    case (MXCHIP_FLOW_Pin):
      mxchip_WIFI_ISR(MXCHIP_FLOW_Pin);
      nx_driver_emw3080_interrupt();
      break;

    case (MXCHIP_NOTIFY_Pin):
      mxchip_WIFI_ISR(MXCHIP_NOTIFY_Pin);
      nx_driver_emw3080_interrupt();
      break;

    default:
      break;
  }
}
/* USER CODE END 1 */
