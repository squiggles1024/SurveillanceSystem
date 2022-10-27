/*
 * BSP_environment.c
 *
 *  Created on: Sep 16, 2022
 *      Author: evanl
 */

#include "../ExternalHardware/HTS221.h"
#include "../ExternalHardware/LPS22HH.h"
#include "../ExternalHardware/IIS2MDC.h"
#include "BSP_IOBus.h"
#include "BSP_environment.h"
#include <stddef.h>
#include <stdint.h>


static HTS221_Handle_t TempSensor_Handle = {0};

//uint8_t test_buffer[0x7C] = {0};

static LPS22HH_Handle_t PressureSensor = {0};

static IIS2MDC_Handle_t Magnetometer = {0};


int32_t BSP_TempHumSensorInit(void)
{
	int32_t ret = 0;
	HTS221_IO_t TempSensor_IO = {.Init = BSP_I2C2_Init,
			                     .DeInit = BSP_I2C2_DeInit,
								 .Read = BSP_I2C2_ReadRegHTS221,
								 .Write = BSP_I2C2_WriteRegHTS221,
								 .GetTick = BSP_GetTick,
								 .ReadPin = NULL
	};

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

int32_t BSP_PressureSensorInit(void)
{
	LPS22HH_IO_t PressureSensor_IO =
	{
			.Init = BSP_LPS22HH_IO_Init,
			.DeInit = NULL,
			.Read = BSP_I2C2_ReadRegLPS22HH,
			.Write = BSP_I2C2_WriteRegLPS22HH,
			.GetTick = BSP_GetTick,
			.ReadPin = BSP_ReadPinLPS22HH
	};

	LPS22HH_Init_Struct_t PressureSettings =
	{
			.INTERRUPT_CFG = LPS22HH_DefaultSetting,
			.THS_P = LPS22HH_DefaultSetting,
			.IF_CTRL = LPS22HH_DefaultSetting,
			.ODR = LPS22HH_1Hz,
			.LPFP = LPS22HH_ODRDiv2,
			.SIM = LPS22HH_DefaultSetting,
			.CTRL_REG2 = LPS22HH_IncrementAddress | LPS22HH_LowNoiseMode,
			.INT_DRDY_Control = LPS22HH_DRDY,
			.INT_S = LPS22HH_DataReady,
			.STOP_ON_WTM = LPS22HH_DefaultSetting,
			.F_MODE = LPS22HH_FifoBypass,
			.WTM = LPS22HH_DefaultSetting,
			.REF_P = LPS22HH_DefaultSetting
	};

	int32_t ret = LPS22HH_Init(&PressureSensor, PressureSettings, &PressureSensor_IO);
	return ret;
}

int32_t BSP_ReadPressure(float *result)
{
	int32_t ret = LPS22HH_ReadPressure(&PressureSensor, result);
	return ret;
}

/*
int32_t BSP_pressuretest(void)
{
    int32_t ret = PressureSensor.IO.Read(0,test_buffer,0x7C);
    return ret;
}
*/

int32_t BSP_MagnetometerInit(void)
{
    IIS2MDC_IO_t MagnetometerIO =
    {
    		.Init = BSP_IIS2MDC_IO_Init,
			.Deinit = NULL,
			.Read = BSP_I2C2_ReadRegIIS2MDC,
			.Write = BSP_I2C2_WriteRegIIS2MDC,
			.GetTick = BSP_GetTick,
			.ReadPin = BSP_ReadPinIIS2MDC
    };

    IIS2MDC_InitStruct_t MagnetometerSettings =
    {
    		.ConfigRegA = IIS2MDC_ODR_10Hz | IIS2MDC_TempCompensationEnabled,
			.ConfigRegB = IIS2MDC_LPF,
			.ConfigRegC = IIS2MDC_DRDY_on_PIN | IIS2MDC_BDU,
			.IntCtrlReg = IIS2MDC_DefaultSetting,
			.IntThreshold = 0,
			.OffsetX = 0,
			.OffsetY = 0,
			.OffsetZ = 0
    };

    int32_t ret = IIS2MDC_Init(&Magnetometer, MagnetometerSettings, &MagnetometerIO);
    return ret;
}

int32_t BSP_ReadMagnetometerXYZ(float *Mx, float *My, float *Mz)
{
    int32_t ret = IIS2MDC_ReadMagnetismXYZ(&Magnetometer, Mx, My, Mz);
    return ret;
}

int32_t BSP_ReadMagnetometerX(float *Mx)
{
	int32_t ret = IIS2MDC_ReadMagnetismX(&Magnetometer, Mx);
	return ret;
}

int32_t BSP_ReadMagnetometerY(float *My)
{
	int32_t ret = IIS2MDC_ReadMagnetismY(&Magnetometer, My);
	return ret;
}

int32_t BSP_ReadMagnetometerZ(float *Mz)
{
	int32_t ret = IIS2MDC_ReadMagnetismZ(&Magnetometer, Mz);
	return ret;
}

int32_t BSP_GetTempPeriod(uint32_t *Period)
{
    int32_t ret = HTS221_GetSamplePeriod(&TempSensor_Handle, Period);
    return ret;
}


int32_t BSP_GetMagneticPeriod(uint32_t *Period)
{
    int32_t ret = IIS2MDC_GetSamplePeriod(&Magnetometer, Period);
    return ret;
}

int32_t BSP_GetPressurePeriod(uint32_t *Period){
    int32_t ret = LPS22HH_GetSamplePeriod(&PressureSensor, Period);
    return ret;
}


