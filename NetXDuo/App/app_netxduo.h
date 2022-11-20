/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_netxduo.h
  * @author  MCD Application Team
  * @brief   NetXDuo applicative header file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_NETXDUO_H__
#define __APP_NETXDUO_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "nx_api.h"

/* USER CODE BEGIN Includes */
#include "nxd_dhcp_client.h"
#include "nxd_sntp_client.h"
#include "nxd_mqtt_client.h"
#include "nxd_dns.h"
#include "nx_driver_emw3080.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
 extern TX_EVENT_FLAGS_GROUP MQTT_TREvent;
 extern TX_THREAD MQTTThread;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#define PAYLOAD_SIZE      (1514 + 28 + 2)
#define PACKET_POOL_SIZE (( PAYLOAD_SIZE + sizeof(NX_PACKET)) * 10)
#define NULL_ADDRESS                (0)
#define IP_HELPER_STACK_SIZE        (2048)
#define ARP_STACK_SIZE              (1024)
#define IP_INIT_STACK_SIZE          (4096)
#define DNS_ADDRESS                 IP_ADDRESS(192, 168, 1, 254)
#define SNTP_SERVER_URL             "time1.google.com"
#define SNTP_TIMEOUT                (10 * NX_IP_PERIODIC_RATE)
#define EPOCH_TIME_DIFF              2208988800     /* is equivalent to 70 years in sec calculated with www.epochconverter.com/date-difference */
#define MQTT_SERVER_IP              IP_ADDRESS(192, 168, 1, 119)
#define MQTT_KEEPALIVE_TIMER        (60)
#define CERT_FILE                   "ca.h"
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
UINT MX_NetXDuo_Init(VOID *memory_ptr);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */


#define MQTT_CLIENT_SUB_TOPIC01   "Drone/Controls"

#define MQTT_CLIENT_PUB_TOPIC01   "Env/Temperature"
#define MQTT_CLIENT_PUB_TOPIC02   "Env/Humidity"
#define MQTT_CLIENT_PUB_TOPIC03   "Env/Pressure"
#define MQTT_CLIENT_PUB_TOPIC04   "Env/Light"
#define MQTT_CLIENT_PUB_TOPIC05   "Env/MagneticX"
#define MQTT_CLIENT_PUB_TOPIC06   "Env/MagneticY"
#define MQTT_CLIENT_PUB_TOPIC07   "Env/MagneticZ"
#define MQTT_CLIENT_PUB_TOPIC08   "Drone/Roll"
#define MQTT_CLIENT_PUB_TOPIC09   "Drone/Pitch"
#define MQTT_CLIENT_PUB_TOPIC10   "Drone/Yaw"
#define MQTT_CLIENT_PUB_TOPIC11   "Drone/RollRate"
#define MQTT_CLIENT_PUB_TOPIC12   "Drone/PitchRate"
#define MQTT_CLIENT_PUB_TOPIC13   "Env/Video"

#define MESSAGE_RECIEVED_EVT_Msk (1 << 0)
#define MESSAGE_TRANSMIT_PUB01_EVT_Msk (1 << 1)
#define MESSAGE_TRANSMIT_PUB02_EVT_Msk (1 << 2)
#define MESSAGE_TRANSMIT_PUB03_EVT_Msk (1 << 3)
#define MESSAGE_TRANSMIT_PUB04_EVT_Msk (1 << 4)
#define MESSAGE_TRANSMIT_PUB05_EVT_Msk (1 << 5)
#define MESSAGE_TRANSMIT_PUB06_EVT_Msk (1 << 6)
#define MESSAGE_TRANSMIT_PUB07_EVT_Msk (1 << 7)
#define MESSAGE_TRANSMIT_PUB08_EVT_Msk (1 << 8)
#define MESSAGE_TRANSMIT_PUB09_EVT_Msk (1 << 9)
#define MESSAGE_TRANSMIT_PUB10_EVT_Msk (1 << 10)
#define MESSAGE_TRANSMIT_PUB11_EVT_Msk (1 << 11)
#define MESSAGE_TRANSMIT_PUB12_EVT_Msk (1 << 12)
#define MESSAGE_TRANSMIT_PUB13_EVT_Msk (1 << 13)
#define MESSAGE_ALL_EVT_Msk (MESSAGE_RECIEVED_EVT_Msk | \
                             MESSAGE_TRANSMIT_PUB01_EVT_Msk | \
							 MESSAGE_TRANSMIT_PUB02_EVT_Msk | \
							 MESSAGE_TRANSMIT_PUB03_EVT_Msk | \
							 MESSAGE_TRANSMIT_PUB04_EVT_Msk | \
							 MESSAGE_TRANSMIT_PUB05_EVT_Msk | \
							 MESSAGE_TRANSMIT_PUB06_EVT_Msk | \
							 MESSAGE_TRANSMIT_PUB07_EVT_Msk | \
							 MESSAGE_TRANSMIT_PUB08_EVT_Msk | \
							 MESSAGE_TRANSMIT_PUB09_EVT_Msk | \
							 MESSAGE_TRANSMIT_PUB10_EVT_Msk | \
							 MESSAGE_TRANSMIT_PUB11_EVT_Msk | \
							 MESSAGE_TRANSMIT_PUB12_EVT_Msk | \
							 MESSAGE_TRANSMIT_PUB13_EVT_Msk)
/* USER CODE END PD */

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

#ifdef __cplusplus
}
#endif
#endif /* __APP_NETXDUO_H__ */
