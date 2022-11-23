/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
void mxchip_WIFI_ISR(uint16_t pin);
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PRES_THREAD_STACK_SIZE 1024
#define SNTP_STACK_SIZE (6 * 1024)
#define CAPFRAME_THREAD_STACK_SIZE 1024
#define LED_STACK_SIZE 1024
#define TOTAL_APP_STACK_SIZE TIMEBASE_STACK_SIZE + LED_STACK_SIZE*2 + TEMP_THREAD_STACK_SIZE + HUM_THREAD_STACK_SIZE + MAG_THREAD_STACK_SIZE + LIGHT_THREAD_STACK_SIZE + PRES_THREAD_STACK_SIZE + MOT_THREAD_STACK_SIZE + CAPFRAME_THREAD_STACK_SIZE + SENDFRAME_THREAD_STACK_SIZE + SNTP_STACK_SIZE + MQTT_STACK_SIZE + QUEUE_STACK_SIZE
#define TIMEBASE_STACK_SIZE 1024
#define LIGHT_THREAD_STACK_SIZE 1024
#define MAG_THREAD_STACK_SIZE 1024
#define MOT_THREAD_STACK_SIZE 1024
#define MQTT_STACK_SIZE (4 * 1024)
#define MQTT_CLIENT_STACK_SIZE (1024 * 10)
#define HUM_THREAD_STACK_SIZE 1024
#define MXCHIP_SPI hspi2
#define SENDFRAME_THREAD_STACK_SIZE 1024
#define TEMP_THREAD_STACK_SIZE 1024
#define QUEUE_STACK_SIZE 1024
#define CAM_D6_Pin GPIO_PIN_6
#define CAM_D6_GPIO_Port GPIOI
#define MXCHIP_FLOW_Pin GPIO_PIN_15
#define MXCHIP_FLOW_GPIO_Port GPIOG
#define MXCHIP_FLOW_EXTI_IRQn EXTI15_IRQn
#define WRLS_UART4_RX_Pin GPIO_PIN_11
#define WRLS_UART4_RX_GPIO_Port GPIOC
#define USB_UCPD_CC1_Pin GPIO_PIN_15
#define USB_UCPD_CC1_GPIO_Port GPIOA
#define CAM_PLUG_Pin GPIO_PIN_1
#define CAM_PLUG_GPIO_Port GPIOI
#define OCTOSPI_F_NCS_Pin GPIO_PIN_5
#define OCTOSPI_F_NCS_GPIO_Port GPIOI
#define CAM_D5_Pin GPIO_PIN_4
#define CAM_D5_GPIO_Port GPIOI
#define OCTOSPI_R_IO5_Pin GPIO_PIN_0
#define OCTOSPI_R_IO5_GPIO_Port GPIOI
#define OCTOSPI_F_IO7_Pin GPIO_PIN_12
#define OCTOSPI_F_IO7_GPIO_Port GPIOH
#define CAM_D7_Pin GPIO_PIN_7
#define CAM_D7_GPIO_Port GPIOI
#define CAM_D3_Pin GPIO_PIN_1
#define CAM_D3_GPIO_Port GPIOE
#define PH3_BOOT0_Pin GPIO_PIN_3
#define PH3_BOOT0_GPIO_Port GPIOH
#define UCPD_PWR_Pin GPIO_PIN_5
#define UCPD_PWR_GPIO_Port GPIOB
#define WRLS_UART4_TX_Pin GPIO_PIN_10
#define WRLS_UART4_TX_GPIO_Port GPIOC
#define T_SWCLK_Pin GPIO_PIN_14
#define T_SWCLK_GPIO_Port GPIOA
#define CAM_D4_Pin GPIO_PIN_14
#define CAM_D4_GPIO_Port GPIOH
#define OCTOSPI_F_IO5_Pin GPIO_PIN_10
#define OCTOSPI_F_IO5_GPIO_Port GPIOH
#define PC14_OSC32_IN_Pin GPIO_PIN_14
#define PC14_OSC32_IN_GPIO_Port GPIOC
#define OCTOSPI_R_DQS_Pin GPIO_PIN_3
#define OCTOSPI_R_DQS_GPIO_Port GPIOE
#define T_SWO_Pin GPIO_PIN_3
#define T_SWO_GPIO_Port GPIOB
#define OCTOSPI_R_IO7_Pin GPIO_PIN_7
#define OCTOSPI_R_IO7_GPIO_Port GPIOD
#define OCTOSPI_F_IO6_Pin GPIO_PIN_11
#define OCTOSPI_F_IO6_GPIO_Port GPIOH
#define CAM_XSDN_Pin GPIO_PIN_3
#define CAM_XSDN_GPIO_Port GPIOI
#define CAM_RSTI_Pin GPIO_PIN_2
#define CAM_RSTI_GPIO_Port GPIOI
#define CAM_HSYNC_Pin GPIO_PIN_8
#define CAM_HSYNC_GPIO_Port GPIOH
#define PC15_OSC32_OUT_Pin GPIO_PIN_15
#define PC15_OSC32_OUT_GPIO_Port GPIOC
#define OCTOSPI_F_IO0_Pin GPIO_PIN_0
#define OCTOSPI_F_IO0_GPIO_Port GPIOF
#define USER_Button_Pin GPIO_PIN_13
#define USER_Button_GPIO_Port GPIOC
#define OCTOSPI_F_IO4_Pin GPIO_PIN_9
#define OCTOSPI_F_IO4_GPIO_Port GPIOH
#define LED_RED_Pin GPIO_PIN_6
#define LED_RED_GPIO_Port GPIOH
#define OCTOSPI_R_IO0_Pin GPIO_PIN_8
#define OCTOSPI_R_IO0_GPIO_Port GPIOF
#define OCTOSPI_F_IO1_Pin GPIO_PIN_1
#define OCTOSPI_F_IO1_GPIO_Port GPIOF
#define OCTOSPI_F_IO2_Pin GPIO_PIN_2
#define OCTOSPI_F_IO2_GPIO_Port GPIOF
#define CAM_VSYNC_Pin GPIO_PIN_7
#define CAM_VSYNC_GPIO_Port GPIOB
#define LED_GREEN_Pin GPIO_PIN_7
#define LED_GREEN_GPIO_Port GPIOH
#define OCTOSPI_R_IO4_Pin GPIO_PIN_2
#define OCTOSPI_R_IO4_GPIO_Port GPIOH
#define T_VCP_RX_Pin GPIO_PIN_10
#define T_VCP_RX_GPIO_Port GPIOA
#define T_SWDIO_Pin GPIO_PIN_13
#define T_SWDIO_GPIO_Port GPIOA
#define USB_C_P_Pin GPIO_PIN_12
#define USB_C_P_GPIO_Port GPIOA
#define OCTOSPI_R_IO2_Pin GPIO_PIN_7
#define OCTOSPI_R_IO2_GPIO_Port GPIOF
#define OCTOSPI_R_IO1_Pin GPIO_PIN_9
#define OCTOSPI_R_IO1_GPIO_Port GPIOF
#define OCTOSPI_F_IO3_Pin GPIO_PIN_3
#define OCTOSPI_F_IO3_GPIO_Port GPIOF
#define OCTOSPI_F_CLK_P_Pin GPIO_PIN_4
#define OCTOSPI_F_CLK_P_GPIO_Port GPIOF
#define CAM_D2_Pin GPIO_PIN_8
#define CAM_D2_GPIO_Port GPIOC
#define T_VCP_TX_Pin GPIO_PIN_9
#define T_VCP_TX_GPIO_Port GPIOA
#define CAM_D1_Pin GPIO_PIN_7
#define CAM_D1_GPIO_Port GPIOC
#define USB_C_PA11_Pin GPIO_PIN_11
#define USB_C_PA11_GPIO_Port GPIOA
#define MIC_CCK1_Pin GPIO_PIN_10
#define MIC_CCK1_GPIO_Port GPIOF
#define OCTOSPI_R_IO3_Pin GPIO_PIN_6
#define OCTOSPI_R_IO3_GPIO_Port GPIOF
#define MIC_SDINx_Pin GPIO_PIN_10
#define MIC_SDINx_GPIO_Port GPIOE
#define WRLS_WKUP_B_Pin GPIO_PIN_6
#define WRLS_WKUP_B_GPIO_Port GPIOG
#define CAM_D0_Pin GPIO_PIN_6
#define CAM_D0_GPIO_Port GPIOC
#define Mems_VL53_xshut_Pin GPIO_PIN_1
#define Mems_VL53_xshut_GPIO_Port GPIOH
#define MIC_CCK0_Pin GPIO_PIN_9
#define MIC_CCK0_GPIO_Port GPIOE
#define Mems_VLX_GPIO_Pin GPIO_PIN_5
#define Mems_VLX_GPIO_GPIO_Port GPIOG
#define MXCHIP_NOTIFY_Pin GPIO_PIN_14
#define MXCHIP_NOTIFY_GPIO_Port GPIOD
#define MXCHIP_NOTIFY_EXTI_IRQn EXTI14_IRQn
#define OCTOSPI_R_IO6_Pin GPIO_PIN_3
#define OCTOSPI_R_IO6_GPIO_Port GPIOC
#define OCTOSPI_F_DQS_Pin GPIO_PIN_12
#define OCTOSPI_F_DQS_GPIO_Port GPIOF
#define USB_UCPD_FLT_Pin GPIO_PIN_8
#define USB_UCPD_FLT_GPIO_Port GPIOE
#define OCTOSPI_R_CLK_P_Pin GPIO_PIN_10
#define OCTOSPI_R_CLK_P_GPIO_Port GPIOB
#define Mems_INT_IIS2MDC_Pin GPIO_PIN_10
#define Mems_INT_IIS2MDC_GPIO_Port GPIOD
#define USB_IANA_Pin GPIO_PIN_13
#define USB_IANA_GPIO_Port GPIOD
#define Mems_INT_LPS22HH_Pin GPIO_PIN_2
#define Mems_INT_LPS22HH_GPIO_Port GPIOG
#define USB_VBUS_SENSE_Pin GPIO_PIN_14
#define USB_VBUS_SENSE_GPIO_Port GPIOF
#define OCTOSPI_R_NCS_Pin GPIO_PIN_11
#define OCTOSPI_R_NCS_GPIO_Port GPIOB
#define MXCHIP_NSS_Pin GPIO_PIN_12
#define MXCHIP_NSS_GPIO_Port GPIOB
#define USB_UCPD_CC2_Pin GPIO_PIN_15
#define USB_UCPD_CC2_GPIO_Port GPIOB
#define Mems_STSAFE_RESET_Pin GPIO_PIN_11
#define Mems_STSAFE_RESET_GPIO_Port GPIOF
#define Mems_ISM330DLC_INT1_Pin GPIO_PIN_11
#define Mems_ISM330DLC_INT1_GPIO_Port GPIOE
#define CAM_PIXCLK_Pin GPIO_PIN_6
#define CAM_PIXCLK_GPIO_Port GPIOA
#define MIC_SDIN0_Pin GPIO_PIN_1
#define MIC_SDIN0_GPIO_Port GPIOB
#define MXCHIP_RESET_Pin GPIO_PIN_15
#define MXCHIP_RESET_GPIO_Port GPIOF
/* USER CODE BEGIN Private defines */
#define ENV_DATA_QUEUE_SIZE      (100)
#define SEND_ENVDATA_PRIORITY    16
#define GET_TEMPERATURE_PRIORITY 15
#define GET_HUMIDITY_PRIORITY    15
#define GET_LIGHT_PRIORITY       15
#define GET_MAGNETIC_PRIORITY    15
#define CAPTURE_FRAME_PRIORITY   15
#define LED_THREAD_PRIORITY      15
#define SEND_FRAME_PRIORITY      14

#define GET_PRESSURE_PRIORITY    11
#define GET_MOTION_PRIORITY      10
#define MQTT_CLIENT_PRIORITY     9      //Receive Controller Data Priority

#define SNTP_INIT_PRIORITY        5
#define IP_INIT_PRIORITY          5
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
