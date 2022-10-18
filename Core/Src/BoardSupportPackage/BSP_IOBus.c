/*
 * BSP_IO_Bus.c
 *
 *  Created on: Sep 19, 2022
 *      Author: evanl
 */

#ifndef SRC_BOARDSUPPORTPACKAGE_BSP_IO_BUS_C_
#define SRC_BOARDSUPPORTPACKAGE_BSP_IO_BUS_C_
#include "i2c.h"
#include "gpio.h"
#include <stdint.h>
#include "BSP_IOBus.h"

#define HTS221_I2C_ADDR (0xBEU)
#define ISM330DHCX_I2C_ADDR (0xD6U)
#define i2c_initialized (1)
#define i2c_uninitialized (0)


void BSP_I2C1_Init(void)
{
	if(hi2c1.State == HAL_I2C_STATE_RESET)
	{
        I2C1_Init();
	}
}

void BSP_I2C1_DeInit(void)
{
	HAL_I2C_DeInit(&hi2c1);
}

void BSP_I2C2_Init(void)
{
	if(hi2c2.State == HAL_I2C_STATE_RESET)
	{
        I2C2_Init();
	}
}

void BSP_I2C2_DeInit(void)
{
	HAL_I2C_DeInit(&hi2c2);
}

int32_t BSP_I2C1_WriteReg(uint16_t Address, uint16_t Reg, uint8_t *pData, uint16_t Length){
    int32_t ret = HAL_I2C_Mem_Write(&hi2c1, Address, Reg, I2C_MEMADD_SIZE_8BIT, pData, Length, 1000);
    return ret;
}

int32_t BSP_I2C1_ReadReg(uint16_t Address, uint16_t Reg, uint8_t *pData, uint16_t Length){
    int32_t ret = HAL_I2C_Mem_Read(&hi2c1, Address, Reg, I2C_MEMADD_SIZE_8BIT, pData, Length, 1000);
    return ret;
}

int32_t BSP_I2C1_WriteReg16(uint16_t Address, uint16_t Reg, uint8_t *pData, uint16_t Length){
    int32_t ret = HAL_I2C_Mem_Write(&hi2c1, Address, Reg, I2C_MEMADD_SIZE_16BIT, pData, Length, 1000);
    return ret;
}

int32_t BSP_I2C1_ReadReg16(uint16_t Address, uint16_t Reg, uint8_t *pData, uint16_t Length){
    int32_t ret = HAL_I2C_Mem_Read(&hi2c1, Address, Reg, I2C_MEMADD_SIZE_16BIT, pData, Length, 1000);
    return ret;
}

int32_t BSP_I2C2_WriteRegHTS221(uint8_t Reg, uint8_t *pData, uint8_t Length){
    int32_t ret = HAL_I2C_Mem_Write(&hi2c2, HTS221_I2C_ADDR, Reg | 0x80, I2C_MEMADD_SIZE_8BIT, pData, Length, 100);
    return ret;
}

int32_t BSP_I2C2_ReadRegHTS221(uint8_t Reg, uint8_t *pData, uint8_t Length){
    int32_t ret = HAL_I2C_Mem_Read(&hi2c2, HTS221_I2C_ADDR, Reg | 0x80, I2C_MEMADD_SIZE_8BIT, pData, Length, 100);
    return ret;
}

int32_t BSP_I2C2_ReadRegISM330DHCX(uint8_t Reg, uint8_t *pData, uint8_t Length)
{
	int32_t ret = HAL_I2C_Mem_Read(&hi2c2, ISM330DHCX_I2C_ADDR, Reg, I2C_MEMADD_SIZE_8BIT, pData, Length, 100);
    return ret;
}

int32_t BSP_I2C2_WriteRegISM330DHCX(uint8_t Reg, uint8_t *pData, uint8_t Length)
{
	int32_t ret = HAL_I2C_Mem_Write(&hi2c2, ISM330DHCX_I2C_ADDR, Reg, I2C_MEMADD_SIZE_8BIT, pData, Length, 100);
    return ret;
}

int32_t BSP_ReadPinISM330DHCX(void)
{
    return HAL_GPIO_ReadPin(Mems_ISM330DLC_INT1_GPIO_Port, Mems_ISM330DLC_INT1_Pin);
}

void BSP_ISM330DHCX_IO_Init(void)
{
    MotionSensor_GPIO_Init();
	BSP_I2C2_Init();
}

uint32_t BSP_GetTick(void)
{
  return HAL_GetTick();
}


#endif /* SRC_BOARDSUPPORTPACKAGE_BSP_IO_BUS_C_ */
