/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_threadx.c
  * @author  MCD Application Team
  * @brief   ThreadX applicative file
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
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_azure_rtos.h"
#include "main.h"
#include "tim.h"
#include "dcache.h"
#include "./BoardSupportPackage/BSP_ram.h"
#include "./BoardSupportPackage/BSP_camera.h"
#include "./BoardSupportPackage/BSP_environment.h"
#include "./BoardSupportPackage/BSP_motion.h"
#include "./BoardSupportPackage/BSP_LED.h"
#include "app_netxduo.h"
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */


//Debug Variables - Live Expressions
float temp_data;
float humidity_data;
float pressure_data;
float accelx, accely, accelz;
float gyrox, gyroy, gyroz;
float magx, magy, magz;
float light;
uint32_t Frame0Send = 0;
uint32_t Frame1Send = 0;
//#define TRACEX_BUFFER_SIZE (64000)
//uint8_t tracex_buffer[TRACEX_BUFFER_SIZE] __attribute__ ((section (".trace")));


TX_THREAD LED_Red_Toggle;
TX_THREAD LED_Green_Toggle;
TX_THREAD Read_TemperatureThreadPtr;
TX_THREAD Read_HumidityThreadPtr;
TX_THREAD Read_PressureThreadPtr;
TX_THREAD Read_MotionThreadPtr;
TX_THREAD Read_MagneticThreadPtr;
TX_THREAD Read_LightThreadPtr;

TX_THREAD CaptureFrameThreadPtr;
TX_THREAD SendFrameThreadPtr;

TX_MUTEX MutexI2C2;
TX_MUTEX MutexDCMI;
TX_SEMAPHORE CameraBufferData[2];

TX_QUEUE TemperatureQueue;
TX_QUEUE LightQueue;
TX_QUEUE PressureQueue;
TX_QUEUE HumidityQueue;
TX_QUEUE MagXQueue;
TX_QUEUE MagYQueue;
TX_QUEUE MagZQueue;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
VOID RedLEDToggleThread(ULONG init);
VOID GreenLEDToggleThread(ULONG init);
VOID ReadTemperatureThread(ULONG init);
VOID ReadHumidityThread(ULONG init);
VOID ReadPressureThread(ULONG init);
VOID ReadMagneticThread(ULONG init);
VOID ReadMotionThread(ULONG init);
VOID ReadLightThread(ULONG init);
VOID CaptureFrameThread(ULONG init);
VOID SendFrameThread(ULONG init);

static VOID DataSendNotify(TX_QUEUE *QueuePtr);
/* USER CODE END PFP */

/**
  * @brief  Application ThreadX Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT App_ThreadX_Init(VOID *memory_ptr)
{
  UINT ret = TX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

   /* USER CODE BEGIN App_ThreadX_MEM_POOL */
  (void)byte_pool;
  UCHAR *Ptr = byte_pool->tx_byte_pool_start;
  /* USER CODE END App_ThreadX_MEM_POOL */

  /* USER CODE BEGIN App_ThreadX_Init */

  //LED Threads

  if(tx_byte_allocate(byte_pool, (VOID **) &Ptr, LED_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
	  return TX_POOL_ERROR;
  }

  ret = tx_thread_create(&LED_Red_Toggle,           //Thread Ptr
		            "Red LED",             //Thread Name
					RedLEDToggleThread,           //Thread Fn Ptr (address)
					0,                //Initial input
					Ptr,         //Stack Ptr
					LED_STACK_SIZE,     //Stack Size
					15,                    //Priority
					15,                    //Preempt Threshold
					1,                     //Time Slize
					TX_AUTO_START);        //Auto Start or Auto Activate
  ret = tx_byte_allocate(byte_pool, (VOID **) &Ptr, LED_STACK_SIZE, TX_NO_WAIT);

  if(ret != TX_SUCCESS)
  {
	  return TX_POOL_ERROR;
  }
  ret = tx_thread_create(&LED_Green_Toggle,           //Thread Ptr
		            "Green LED",             //Thread Name
					GreenLEDToggleThread,           //Thread Fn Ptr (address)
					0,                //Initial input
					Ptr,         //Stack Ptr
					LED_STACK_SIZE,     //Stack Size
					15,                    //Priority
					15,                    //Preempt Threshold
					1,                     //Time Slice
					TX_AUTO_START);        //Auto Start or Auto Activate

  //Motion
  if(tx_byte_allocate(byte_pool, (VOID **) &Ptr, MOT_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
	  return TX_POOL_ERROR;
  }

  ret = tx_thread_create(&Read_MotionThreadPtr,     //Thread Ptr
		            "Motion Sensor",        //Thread Name
					ReadMotionThread,      //Thread Fn Ptr (address)
					0,                     //Initial input
					Ptr,      //Stack Ptr
					MOT_THREAD_STACK_SIZE,     //Stack Size
					10,                    //Priority
					10,                    //Preempt Threshold
				    5,                     //Time Slize
					TX_AUTO_START);        //Auto Start or Auto Activate

  //Temp
  if(tx_byte_allocate(byte_pool, (VOID **) &Ptr, TEMP_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
	  return TX_POOL_ERROR;
  }

  ret = tx_thread_create(&Read_TemperatureThreadPtr,   //Thread Ptr
		            "Temp Sensor",              //Thread Name
					ReadTemperatureThread,      //Thread Fn Ptr (address)
					0,                     //Initial input
					Ptr, //Stack Ptr
					TEMP_THREAD_STACK_SIZE,     //Stack Size
					15,                    //Priority
					15,                    //Preempt Threshold
				    1,                     //Time Slize
					TX_AUTO_START);        //Auto Start or Auto Activate
  //Humidity
  if(tx_byte_allocate(byte_pool, (VOID **) &Ptr, HUM_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
	  return TX_POOL_ERROR;
  }

  ret = tx_thread_create(&Read_HumidityThreadPtr,   //Thread Ptr
		            "Humidity Sensor",              //Thread Name
					ReadHumidityThread,      //Thread Fn Ptr (address)
					0,                     //Initial input
					Ptr,    //Stack Ptr
					HUM_THREAD_STACK_SIZE,     //Stack Size
					15,                    //Priority
					15,                    //Preempt Threshold
				    1,                     //Time Slize
					TX_AUTO_START);        //Auto Start or Auto Activate
  //Pressure
  if(tx_byte_allocate(byte_pool, (VOID **) &Ptr, PRES_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
	  return TX_POOL_ERROR;
  }

  ret = tx_thread_create(&Read_PressureThreadPtr,   //Thread Ptr
		            "Pressure Sensor",              //Thread Name
					ReadPressureThread,      //Thread Fn Ptr (address)
					0,                     //Initial input
					Ptr,    //Stack Ptr
					PRES_THREAD_STACK_SIZE,     //Stack Size
					15,                    //Priority
					15,                    //Preempt Threshold
				    1,                     //Time Slize
					TX_AUTO_START);        //Auto Start or Auto Activate

  //Magnet
  if(tx_byte_allocate(byte_pool, (VOID **) &Ptr, MAG_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
	  return TX_POOL_ERROR;
  }

  ret = tx_thread_create(&Read_MagneticThreadPtr,   //Thread Ptr
		            "Magnetic Sensor",              //Thread Name
					ReadMagneticThread,      //Thread Fn Ptr (address)
					0,                     //Initial input
					Ptr,    //Stack Ptr
					MAG_THREAD_STACK_SIZE,     //Stack Size
					15,                    //Priority
					15,                    //Preempt Threshold
				    1,                     //Time Slize
					TX_AUTO_START);        //Auto Start or Auto Activate
  //Light
  if(tx_byte_allocate(byte_pool, (VOID **) &Ptr, LIGHT_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
	  return TX_POOL_ERROR;
  }

  ret = tx_thread_create(&Read_LightThreadPtr,   //Thread Ptr
		            "Light Sensor",              //Thread Name
					ReadLightThread,      //Thread Fn Ptr (address)
					0,                     //Initial input
					Ptr,    //Stack Ptr
					LIGHT_THREAD_STACK_SIZE,     //Stack Size
					15,                    //Priority
					15,                    //Preempt Threshold
				    1,                     //Time Slize
					TX_AUTO_START);        //Auto Start or Auto Activate

  //Capture Frame
  if(tx_byte_allocate(byte_pool, (VOID **) &Ptr, CAPFRAME_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
	  return TX_POOL_ERROR;
  }

  ret = tx_thread_create(&CaptureFrameThreadPtr,   //Thread Ptr
		            "Capture Frame Thread",              //Thread Name
					CaptureFrameThread,      //Thread Fn Ptr (address)
					0,                     //Initial input
					Ptr,    //Stack Ptr
					CAPFRAME_THREAD_STACK_SIZE,     //Stack Size
					15,                    //Priority
					15,                    //Preempt Threshold
				    1,                     //Time Slize
					TX_AUTO_START);        //Auto Start or Auto Activate

  //Send Frame
  if(tx_byte_allocate(byte_pool, (VOID **) &Ptr, SENDFRAME_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
	  return TX_POOL_ERROR;
  }

  ret = tx_thread_create(&SendFrameThreadPtr,   //Thread Ptr
		            "Send Frame Thread",              //Thread Name
					SendFrameThread,      //Thread Fn Ptr (address)
					0,                     //Initial input
					Ptr,    //Stack Ptr
					SENDFRAME_THREAD_STACK_SIZE,     //Stack Size
					15,                    //Priority
					15,                    //Preempt Threshold
				    1,                     //Time Slize
					TX_AUTO_START);        //Auto Start or Auto Activate

  ret = tx_mutex_create(&MutexI2C2, "I2C2 Mutex", TX_INHERIT);
  ret = tx_semaphore_create(&CameraBufferData[0], "Camera Buffer 1 Semaphore", 0);
  ret = tx_semaphore_create(&CameraBufferData[1], "Camera Buffer 2 Semaphore", 0);

  ret = tx_queue_create(&TemperatureQueue, "Temperature Queue", TX_1_ULONG, Ptr, 1*sizeof(float));
  ret = tx_queue_create(&PressureQueue, "Pressure Queue", TX_1_ULONG, Ptr, 1*sizeof(float));
  ret = tx_queue_create(&HumidityQueue, "Humidity Queue", TX_1_ULONG, Ptr, 1*sizeof(float));
  ret = tx_queue_create(&LightQueue, "Light Queue", TX_1_ULONG, Ptr, 1*sizeof(float));
  ret = tx_queue_create(&MagXQueue, "MagX Queue", TX_1_ULONG, Ptr, 1*sizeof(float));
  ret = tx_queue_create(&MagYQueue, "MagY Queue", TX_1_ULONG, Ptr, 1*sizeof(float));
  ret = tx_queue_create(&MagZQueue, "MagZ Queue", TX_1_ULONG, Ptr, 1*sizeof(float));

  ret = tx_queue_send_notify(&TemperatureQueue,DataSendNotify);
  ret = tx_queue_send_notify(&PressureQueue,DataSendNotify);
  ret = tx_queue_send_notify(&HumidityQueue,DataSendNotify);
  ret = tx_queue_send_notify(&LightQueue,DataSendNotify);
  ret = tx_queue_send_notify(&MagXQueue,DataSendNotify);
  ret = tx_queue_send_notify(&MagYQueue,DataSendNotify);
  ret = tx_queue_send_notify(&MagZQueue,DataSendNotify);
  //tx_trace_enable(&tracex_buffer, TRACEX_BUFFER_SIZE,30);
  /* USER CODE END App_ThreadX_Init */

  return ret;
}

  /**
  * @brief  MX_ThreadX_Init
  * @param  None
  * @retval None
  */
void MX_ThreadX_Init(void)
{
  /* USER CODE BEGIN  Before_Kernel_Start */

  /* USER CODE END  Before_Kernel_Start */

  tx_kernel_enter();

  /* USER CODE BEGIN  Kernel_Start_Error */

  /* USER CODE END  Kernel_Start_Error */
}

/* USER CODE BEGIN 1 */

VOID ReadTemperatureThread(ULONG init)
{
	int32_t ret = HTS221_Ok;
	uint32_t SleepTime = 0;
	float Temperature = 0;
	while(1)
	{
		//Take I2C Mutex (if available)
		tx_mutex_get(&MutexI2C2,TX_WAIT_FOREVER);
		ret = BSP_ReadTemperature(&Temperature);
		tx_mutex_put(&MutexI2C2);
		if(ret == HTS221_DataReady)
		{
            ret = tx_queue_send(&TemperatureQueue, (VOID*)&temp_data, TX_WAIT_FOREVER);
			temp_data = Temperature;
		}
		ret = BSP_GetTempPeriod(&SleepTime);
		if(ret == HTS221_Ok)
		{
			//Sleep Time given in milliseconds. OS Timeslice = 10 ms, +1 is to handle possible non-zero remainders.
			if(SleepTime % 10 == 0)
			{
				tx_thread_sleep(SleepTime / 10);
			} else
			{
				tx_thread_sleep((SleepTime / 10) + 1);
			}
		}
	}
}

VOID ReadHumidityThread(ULONG init)
{
	int32_t ret = HTS221_Ok;
	uint32_t SleepTime = 0;
	float Humidity = 0;
	while(1)
	{
		//Take I2C Mutex (if available)
		tx_mutex_get(&MutexI2C2,TX_WAIT_FOREVER);
		ret = BSP_ReadHumidity(&Humidity);
		tx_mutex_put(&MutexI2C2);
		if(ret == HTS221_DataReady)
		{
            ret = tx_queue_send(&HumidityQueue, (VOID*)&humidity_data, TX_WAIT_FOREVER);
			humidity_data = Humidity;
		}
		ret = BSP_GetTempPeriod(&SleepTime);
		if(ret == HTS221_Ok)
		{
			//Sleep Time given in milliseconds. OS Timeslice = 10 ms, +1 is to handle possible non-zero remainders.
			if(SleepTime % 10 == 0)
			{
				tx_thread_sleep(SleepTime / 10);
			} else
			{
				tx_thread_sleep((SleepTime / 10) + 1);
			}
		}
	}
}

VOID ReadPressureThread(ULONG init)
{
	int32_t ret = LPS22HH_Ok;
	uint32_t SleepTime = 0;
	float Pressure = 0;
	while(1)
	{
		tx_mutex_get(&MutexI2C2,TX_WAIT_FOREVER);
		ret = BSP_ReadPressure(&Pressure);
		tx_mutex_put(&MutexI2C2);
		if(ret == LPS22HH_DataReady)
		{
			ret = tx_queue_send(&PressureQueue, (VOID*)&pressure_data, TX_WAIT_FOREVER);
			pressure_data = Pressure;
		}
		ret = BSP_GetPressurePeriod(&SleepTime);
		if(ret == LPS22HH_Ok)
		{
			//Sleep Time given in milliseconds. OS Timeslice = 10 ms, +1 is to handle possible non-zero remainders.
			if(SleepTime % 10 == 0)
			{
				tx_thread_sleep(SleepTime / 10);
			} else
			{
				tx_thread_sleep((SleepTime / 10) + 1);
			}
		}
	}
}

VOID ReadMagneticThread(ULONG init)
{
	int32_t ret = IIS2MDC_Ok;
	uint32_t SleepTime = 0;
	float MagX = 0;
	float MagY = 0;
	float MagZ = 0;
	while(1)
	{
		tx_mutex_get(&MutexI2C2,TX_WAIT_FOREVER);
		ret = BSP_ReadMagnetometerXYZ(&MagX, &MagY, &MagZ);
		tx_mutex_put(&MutexI2C2);
		if(ret == IIS2MDC_DataReady)
		{

			//ret = tx_queue_send(&MagXQueue, (VOID*)&magx, TX_WAIT_FOREVER);
			//ret = tx_queue_send(&MagYQueue, (VOID*)&magy, TX_WAIT_FOREVER);
			//ret = tx_queue_send(&MagZQueue, (VOID*)&magz, TX_WAIT_FOREVER);
			magx = MagX;
			magy = MagY;
			magz = MagZ;
		}
		ret = BSP_GetMagneticPeriod(&SleepTime);
		if(ret == IIS2MDC_Ok)
		{
			//Sleep Time given in milliseconds. OS Timeslice = 10 ms, +1 is to handle possible non-zero remainders.
			if(SleepTime % 10 == 0)
			{
				tx_thread_sleep(SleepTime / 10);
			} else
			{
				tx_thread_sleep((SleepTime / 10) + 1);
			}
		}
	}
}

VOID ReadMotionThread(ULONG init)
{
	int32_t ret = IIS2MDC_Ok;
	float AccelX = 0;
	float AccelY = 0;
	float AccelZ = 0;
	float GyroX = 0;
	float GyroY = 0;
	float GyroZ = 0;
	//Synchronize with hardware timer
	BSP_SynchronizeIRQ();
	while(1)
	{
	    tx_mutex_get(&MutexI2C2,TX_NO_WAIT);
	    ret = BSP_ReadAccelXYZ(&AccelX, &AccelY, &AccelZ);
	    if(ret == ISM330DHCX_DataReady)
	    {
		    accelx = AccelX;
		    accely = AccelY;
		    accelz = AccelZ;
	    }
	    ret = BSP_ReadGyroXYZ(&GyroX, &GyroY, &GyroZ);
	    if(ret == ISM330DHCX_DataReady)
	    {
		    gyrox = GyroX;
		    gyroy = GyroY;
		    gyroz = GyroZ;
	    }
	    tx_mutex_put(&MutexI2C2);
	    tx_thread_suspend(&Read_MotionThreadPtr);
	}
}

void TIM7_ResumeMotionThread(void)
{
	if(Read_MotionThreadPtr.tx_thread_state == TX_SUSPENDED)
	{
	    tx_thread_resume(&Read_MotionThreadPtr);
	}
}

VOID ReadLightThread(ULONG init)
{
	float Light = 0;
	int32_t ret = 0;
	while(1)
	{
	    tx_mutex_get(&MutexI2C2,TX_WAIT_FOREVER);
		ret = BSP_ReadAmbientLight(&Light);
	    tx_mutex_put(&MutexI2C2);
		if(ret == VEML6030_Ok)
		{
			light = Light;
		}
		tx_thread_sleep(100);
	}
}

VOID RedLEDToggleThread(ULONG init)
{
	while(1)
	{
		BSP_LEDToggleRed();
		tx_thread_sleep(100);
	}
}

VOID GreenLEDToggleThread(ULONG init)
{
	while(1)
	{
		BSP_LEDToggleGreen();
		tx_thread_sleep(100);
	}
}

VOID CaptureFrameThread(ULONG init)
{
    while(1)
    {
        if(CameraBufferData[0].tx_semaphore_count == 0)
        {
        	BSP_CameraStart((uint8_t*)CAMERA_FRAMEBUFFER1_ADDR);
            tx_thread_suspend(&CaptureFrameThreadPtr);
        }

        if(CameraBufferData[1].tx_semaphore_count == 0)
        {
        	BSP_CameraStart((uint8_t*)CAMERA_FRAMEBUFFER2_ADDR);
            tx_thread_suspend(&CaptureFrameThreadPtr);
        }
    }
}


VOID SendFrameThread(ULONG init)
{

    while(1)
    {
    	if(CameraBufferData[1].tx_semaphore_count == 0 && CameraBufferData[0].tx_semaphore_count == 0)
    	{
            tx_thread_relinquish();
    	}

    	if(CameraBufferData[0].tx_semaphore_count >= 1)
    	{
    		//Send Frame1 Here
    		Frame0Send++;
    		tx_semaphore_get(&CameraBufferData[0],TX_NO_WAIT);
    	}

    	if(CameraBufferData[1].tx_semaphore_count >= 1)
    	{
    		//Send Frame2 Here
    		Frame1Send++;
    		tx_semaphore_get(&CameraBufferData[1],TX_NO_WAIT);
    	}
    }
}


static uint32_t FrameCount = 0;
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{

	BSP_CameraStop();
	if(FrameCount % 2 == 0)
	{
		HAL_DCACHE_CleanInvalidByAddr_IT(&hdcache1, (const uint32_t *const)CAMERA_FRAMEBUFFER1_ADDR, CAMERA_DATA_SIZE_BYTES);

	} else
	{
		HAL_DCACHE_CleanInvalidByAddr_IT(&hdcache1, (const uint32_t *const)CAMERA_FRAMEBUFFER2_ADDR, CAMERA_DATA_SIZE_BYTES);
	}
	tx_thread_resume(&CaptureFrameThreadPtr);

}

void HAL_DCACHE_CleanAndInvalidateByAddrCallback(DCACHE_HandleTypeDef *hdcache)
{

	if((FrameCount % 2)== 0)
	{
		tx_semaphore_put(&CameraBufferData[0]);
	} else
	{
		tx_semaphore_put(&CameraBufferData[1]);
	}
	FrameCount++;
}

static VOID DataSendNotify(TX_QUEUE *QueuePtr)
{
	if(QueuePtr == &TemperatureQueue)
	{
        tx_event_flags_set(&MQTT_TREvent,MESSAGE_TRANSMIT_PUB01_EVT_Msk, TX_OR);
	}
	else if(QueuePtr == &LightQueue)
	{
		tx_event_flags_set(&MQTT_TREvent,MESSAGE_TRANSMIT_PUB04_EVT_Msk, TX_OR);
	}
	else if(QueuePtr == &PressureQueue)
	{
		tx_event_flags_set(&MQTT_TREvent,MESSAGE_TRANSMIT_PUB03_EVT_Msk, TX_OR);
	}
	else if(QueuePtr == &HumidityQueue)
	{
		tx_event_flags_set(&MQTT_TREvent,MESSAGE_TRANSMIT_PUB02_EVT_Msk, TX_OR);
	}
	else if(QueuePtr == &MagXQueue)
	{
		tx_event_flags_set(&MQTT_TREvent,MESSAGE_TRANSMIT_PUB05_EVT_Msk, TX_OR);
	}
	else if(QueuePtr == &MagYQueue)
	{
		tx_event_flags_set(&MQTT_TREvent,MESSAGE_TRANSMIT_PUB06_EVT_Msk, TX_OR);
	}
	else if(QueuePtr == &MagZQueue)
	{
		tx_event_flags_set(&MQTT_TREvent,MESSAGE_TRANSMIT_PUB07_EVT_Msk, TX_OR);
	}
}


/* USER CODE END 1 */
