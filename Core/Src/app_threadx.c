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
TX_THREAD thread_ptr1;
TX_THREAD thread_ptr2;
TX_THREAD thread_ptr3;
TX_EVENT_FLAGS_GROUP LED_Evt;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
VOID thread1_evt(ULONG initial_input);
VOID thread2_evt(ULONG initial_input);
VOID thread_LED(ULONG initial_input);
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
  tx_thread_create(&thread_ptr1,
		            "thread1",
					thread1_evt,
					0xBEEF,
					thread_stack1,
					THREAD_STACK_SIZE,
					15,
					15,
					1,
					TX_AUTO_START);

  tx_thread_create(&thread_ptr2,
		            "thread2",
					thread2_evt,
					0xBEEF,
					thread_stack2,
					THREAD_STACK_SIZE,
					15,
					15,
					1,
					TX_AUTO_START);

  tx_thread_create(&thread_ptr3,
		            "thread3",
					thread_LED,
					0xBEEF,
					thread_stack3,
					THREAD_STACK_SIZE,
					15,
					15,
					1,
					TX_AUTO_START);
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
/* USER CODE END 1 */
