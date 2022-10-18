/*
 * BSP_environment.c
 *
 *  Created on: Sep 16, 2022
 *      Author: evanl
 */

#include "../ExternalHardware/HTS221.h"
#include "BSP_IOBus.h"
#include "BSP_environment.h"
#include <stddef.h>
#include <stdint.h>

static HTS221_IO_t TempSensor_IO = {.Init = BSP_I2C2_Init,
		                     .DeInit = BSP_I2C2_DeInit,
							 .Read = BSP_I2C2_ReadRegHTS221,
							 .Write = BSP_I2C2_WriteRegHTS221,
							 .GetTick = BSP_GetTick,
							 .ReadPin = NULL
};
static HTS221_Handle_t TempSensor_Handle = {0};


int32_t BSP_TempHumSensorInit(void)
{
	int32_t ret = 0;
	HTS221_Init_Struct_t Settings = { .DataReadyConfig = HTS221_DataReadyDisable,
			                          .DataReadyOutputMode = HTS221_DRDY_PushPull,
									  .DataReadySignalPolarity = HTS221_DataActiveHigh,
									  .HumidityResolution = HTS221_Hum64Samples,
									  .OutputDataRate = HTS221_1_0Hz,
									  .TemperatureResolution = HTS221_Temp32amples

	};
	ret = HTS221_Init(&TempSensor_Handle, Settings, &TempSensor_IO);
	return ret;
}

int32_t BSP_ReadTemperature(float *result)
{
	return HTS221_ReadTemperature(&TempSensor_Handle, result);
}


int32_t BSP_ReadHumidity(float *result)
{
	return HTS221_ReadHumidity(&TempSensor_Handle, result);
}

int32_t BSP_EnableHeater(void)
{
	return HTS221_EnableHeater(&TempSensor_Handle);
}

int32_t BSP_DisableHeater(void)
{
	return HTS221_DisableHeater(&TempSensor_Handle);
}

