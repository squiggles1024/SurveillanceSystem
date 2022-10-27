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
#include "main.h"
#include "tim.h"
#include "./BoardSupportPackage/BSP_ram.h"
#include "./BoardSupportPackage/BSP_camera.h"
#include "./BoardSupportPackage/BSP_environment.h"
#include "./BoardSupportPackage/BSP_motion.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define THREAD_STACK_SIZE (1024U)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
uint8_t thread_stack1[THREAD_STACK_SIZE];
uint8_t thread_stack2[THREAD_STACK_SIZE];
uint8_t thread_stack3[THREAD_STACK_SIZE];

uint8_t Read_TemperatureStack[THREAD_STACK_SIZE];
uint8_t Read_HumidityStack[THREAD_STACK_SIZE];
uint8_t Read_PressureStack[THREAD_STACK_SIZE];
uint8_t Read_MotionStack[THREAD_STACK_SIZE];
uint8_t Read_MagneticStack[THREAD_STACK_SIZE];


//Debug Variables - Live Expressions
float temp_data;
float humidity_data;
float pressure_data;
float accelx, accely, accelz;
float gyrox, gyroy, gyroz;
float magx, magy, magz;


TX_THREAD thread_ptr1;
TX_THREAD thread_ptr2;
TX_THREAD thread_ptr3;
TX_THREAD Read_TemperatureThreadPtr;
TX_THREAD Read_HumidityThreadPtr;
TX_THREAD Read_PressureThreadPtr;
TX_THREAD Read_MotionThreadPtr;
TX_THREAD Read_MagneticThreadPtr;

TX_MUTEX MutexI2C2;

TX_EVENT_FLAGS_GROUP LED_Evt;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
VOID thread1_evt(ULONG initial_input);
VOID thread2_evt(ULONG initial_input);
VOID thread_LED(ULONG initial_input);
VOID ReadTemperatureThread(ULONG init);
VOID ReadHumidityThread(ULONG init);
VOID ReadPressureThread(ULONG init);
VOID ReadMagneticThread(ULONG init);
VOID ReadMotionThread(ULONG init);
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
  /* USER CODE END App_ThreadX_MEM_POOL */

  /* USER CODE BEGIN App_ThreadX_Init */
  tx_event_flags_create(&LED_Evt, "LED Event");
  ret = tx_thread_create(&thread_ptr1,           //Thread Ptr
		            "thread1",             //Thread Name
					thread1_evt,           //Thread Fn Ptr (address)
					0xBEEF,                //Initial input
					thread_stack1,         //Stack Ptr
					THREAD_STACK_SIZE,     //Stack Size
					15,                    //Priority
					15,                    //Preempt Threshold
					1,                     //Time Slize
					TX_AUTO_START);        //Auto Start or Auto Activate

  ret = tx_thread_create(&thread_ptr2,
		            "thread2",
					thread2_evt,
					0xBEEF,
					thread_stack2,
					THREAD_STACK_SIZE,
					15,
					15,
					1,
					TX_AUTO_START);

  ret = tx_thread_create(&thread_ptr3,
		            "thread3",
					thread_LED,
					0xBEEF,
					thread_stack3,
					THREAD_STACK_SIZE,
					15,
					15,
					1,
					TX_AUTO_START);

  //Motion
  ret = tx_thread_create(&Read_MotionThreadPtr,     //Thread Ptr
		            "Motion Sensor",        //Thread Name
					ReadMotionThread,      //Thread Fn Ptr (address)
					0,                     //Initial input
					Read_MotionStack,      //Stack Ptr
					THREAD_STACK_SIZE,     //Stack Size
					10,                    //Priority
					10,                    //Preempt Threshold
				    5,                     //Time Slize
					TX_AUTO_START);        //Auto Start or Auto Activate

  //Temp
  ret = tx_thread_create(&Read_TemperatureThreadPtr,   //Thread Ptr
		            "Temp Sensor",              //Thread Name
					ReadTemperatureThread,      //Thread Fn Ptr (address)
					0,                     //Initial input
					Read_TemperatureStack, //Stack Ptr
					THREAD_STACK_SIZE,     //Stack Size
					15,                    //Priority
					15,                    //Preempt Threshold
				    1,                     //Time Slize
					TX_AUTO_START);        //Auto Start or Auto Activate
  //Humidity
  ret = tx_thread_create(&Read_HumidityThreadPtr,   //Thread Ptr
		            "Humidity Sensor",              //Thread Name
					ReadHumidityThread,      //Thread Fn Ptr (address)
					0,                     //Initial input
					Read_HumidityStack,    //Stack Ptr
					THREAD_STACK_SIZE,     //Stack Size
					15,                    //Priority
					15,                    //Preempt Threshold
				    1,                     //Time Slize
					TX_AUTO_START);        //Auto Start or Auto Activate
  //Pressure
  ret = tx_thread_create(&Read_PressureThreadPtr,   //Thread Ptr
		            "Pressure Sensor",              //Thread Name
					ReadPressureThread,      //Thread Fn Ptr (address)
					0,                     //Initial input
					Read_PressureStack,    //Stack Ptr
					THREAD_STACK_SIZE,     //Stack Size
					15,                    //Priority
					15,                    //Preempt Threshold
				    1,                     //Time Slize
					TX_AUTO_START);        //Auto Start or Auto Activate

  //Magnet
  ret = tx_thread_create(&Read_MagneticThreadPtr,   //Thread Ptr
		            "Magnetic Sensor",              //Thread Name
					ReadMagneticThread,      //Thread Fn Ptr (address)
					0,                     //Initial input
					Read_MagneticStack,    //Stack Ptr
					THREAD_STACK_SIZE,     //Stack Size
					15,                    //Priority
					15,                    //Preempt Threshold
				    1,                     //Time Slize
					TX_AUTO_START);        //Auto Start or Auto Activate

  ret = tx_mutex_create(&MutexI2C2, "I2C2 Mutex", TX_INHERIT);
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
VOID thread1_evt(ULONG initial_input){
	while(1)
	{
		tx_event_flags_set(&LED_Evt,1,TX_OR);
		tx_thread_suspend(&thread_ptr1);
	}
}

VOID thread2_evt(ULONG initial_input){
	while(1)
	{
		tx_event_flags_set(&LED_Evt,2,TX_OR);
		tx_thread_suspend(&thread_ptr2);
	}
}

VOID thread_LED(ULONG initial_input){
	uint32_t FlagValue = 0;
	while(1)
	{
		FlagValue = LED_Evt.tx_event_flags_group_current;

	    if(FlagValue == 3){
	        HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
	        HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
	        tx_event_flags_set(&LED_Evt, 0xFFFC, TX_AND);
	        tx_thread_resume(&thread_ptr1);
	        tx_thread_resume(&thread_ptr2);
	        tx_thread_sleep(100);
	    }

	}
}

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
			temp_data = Temperature;
		}
		ret = BSP_GetTempPeriod(&SleepTime);
		if(ret == HTS221_Ok)
		{
			//Sleep Time given in milliseconds. OS Timeslice = 10 ms, +1 is to handle possible non-zero remainders.
			if(SleepTime % 10 == 0)
			{
				tx_thread_sleep(SleepTime);
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
			humidity_data = Humidity;
		}
		ret = BSP_GetTempPeriod(&SleepTime);
		if(ret == HTS221_Ok)
		{
			//Sleep Time given in milliseconds. OS Timeslice = 10 ms, +1 is to handle possible non-zero remainders.
			if(SleepTime % 10 == 0)
			{
				tx_thread_sleep(SleepTime);
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
			pressure_data = Pressure;
		}
		ret = BSP_GetPressurePeriod(&SleepTime);
		if(ret == LPS22HH_Ok)
		{
			//Sleep Time given in milliseconds. OS Timeslice = 10 ms, +1 is to handle possible non-zero remainders.
			if(SleepTime % 10 == 0)
			{
				tx_thread_sleep(SleepTime);
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
				tx_thread_sleep(SleepTime);
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


/* USER CODE END 1 */
