/*
 * BSP_LED.c
 *
 *  Created on: Oct 27, 2022
 *      Author: evanl
 */

#include "BSP_LED.h"
#include "stm32u5xx_hal.h"
#include "main.h"

void BSP_LEDToggleRed(void)
{
    HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
}

void BSP_LEDToggleGreen(void)
{
    HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
}

void BSP_LEDTurnOffRed(void)
{
   HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
}

void BSP_LEDTurnOnRed(void)
{
	   HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);
}

void BSP_LEDTurnOffGreen(void)
{
	   HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);
}

void BSP_LEDTurnOnGreen(void)
{
	   HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);
}
