/*
 * HTS221.c
 *
 *  Created on: Sep 15, 2022
 *      Author: evanl
 */

#include "HTS221.h"
#include <stdint.h>
#include <stddef.h>

static int32_t HTS221_ReadRegWrapper(void *Handle, uint8_t Reg, uint8_t *Buffer, uint8_t Length);
static int32_t HTS221_WriteRegWrapper(void *Handle, uint8_t Reg, uint8_t *Buffer, uint8_t Length);
static int32_t HTS221_RegisterInit(HTS221_Handle_t *Handle, HTS221_Init_Struct_t Settings);
static int32_t HTS221_CalculateTemperature(HTS221_Handle_t *Handle, float *temperature);
static int32_t HTS221_CalculateHumidity(HTS221_Handle_t *Handle, float *humidity);


int32_t HTS221_Init(HTS221_Handle_t *Handle, HTS221_Init_Struct_t Settings, HTS221_IO_t *IO)
{
    if(Handle == NULL)
    {
    	return HTS221_Error;
    }
    Handle->IO = *IO;
    Handle->Context.Handle = Handle;
    Handle->Context.Read = HTS221_ReadRegWrapper;
    Handle->Context.Write = HTS221_WriteRegWrapper;

    if(Settings.DataReadyConfig == HTS221_DataReadyEnable && Handle->IO.ReadPin == NULL)
    {
    	return HTS221_Error; //Must provide a read pin function if using the data ready pin
    }
    Handle->DRDY_Config = Settings.DataReadyConfig;
    Handle->DRDY_Polarity = Settings.DataReadySignalPolarity;

    //Initialized Bus
    if(Handle->IO.Init != NULL)
    {
        Handle->IO.Init();
    }

    Handle->OutputDataRate = Settings.OutputDataRate;

    //Initialize Device
    return HTS221_RegisterInit(Handle, Settings);
}

void HTS221_DeInit(HTS221_Handle_t *Handle)
{
    if(Handle->IO.DeInit != NULL){
    	Handle->IO.DeInit();
    }
    Handle->IO.DeInit = NULL;
    Handle->IO.GetTick = NULL;
    Handle->IO.Init = NULL;
    Handle->IO.Write = NULL;
    Handle->IO.Read = NULL;
    Handle->IO.ReadPin = NULL;
    Handle->Context.Read = NULL;
    Handle->Context.Write = NULL;
    Handle->Context.Handle = NULL;
    Handle->DRDY_Config = 0;
    Handle->DRDY_Polarity = 0;
}

int32_t HTS221_StartConversion(HTS221_Handle_t *Handle)
{
	uint8_t temp = 0;
	int32_t ret = 0;
	ret = HTS221_ReadReg(&Handle->Context, HTS221_REG_CTRL_REG2, &temp, 1);  //Read Reg
	if(ret != 0)
	{
		return ret;
	}
	temp |= HTS221_START_CONV_SIGNAL;                                        //Modify Reg
	return HTS221_WriteReg(&Handle->Context,HTS221_REG_CTRL_REG2, &temp, 1); //Store Reg
}

int32_t HTS221_EnableHeater(HTS221_Handle_t *Handle)
{
	uint8_t temp = 0;
	int32_t ret = 0;
	ret = HTS221_ReadReg(&Handle->Context, HTS221_REG_CTRL_REG2, &temp, 1);  //Read Reg
	if(ret != 0)
	{
		return ret;
	}

	temp |= HTS221_HEATER_ENABLE;                                            //Modify Reg
	return HTS221_WriteReg(&Handle->Context,HTS221_REG_CTRL_REG2, &temp, 1); //Store Reg

}

int32_t HTS221_DisableHeater(HTS221_Handle_t *Handle)
{
	uint8_t temp = 0;
	int32_t ret = 0;
	ret = HTS221_ReadReg(&Handle->Context, HTS221_REG_CTRL_REG2, &temp, 1);  //Read Reg
	if(ret != 0)
	{
		return ret;
	}

	temp &= ~HTS221_HEATER_ENABLE;                                           //Modify Reg
	return HTS221_WriteReg(&Handle->Context,HTS221_REG_CTRL_REG2, &temp, 1); //Store Reg
}

int32_t HTS221_ResetRegisters(HTS221_Handle_t *Handle)
{
	uint8_t temp = HTS221_RebootSignal;
    return HTS221_WriteReg(&Handle->Context, HTS221_REG_CTRL_REG2, &temp, 1);
}

int32_t HTS221_PowerCycle(HTS221_Handle_t *Handle)
{

    //turn off
	int32_t ret = HTS221_PowerDown(Handle);
	if(ret != HTS221_Ok)
	{
        return ret;
	}
	//delay for 1 timeslice
	uint32_t CurrentTime = Handle->IO.GetTick();
	while(Handle->IO.GetTick() <= (CurrentTime + 1));
	//turn on
	return HTS221_PowerUp(Handle);
}

int32_t HTS221_ReadTemperature(HTS221_Handle_t *Handle, float *temperature)
{
    int32_t ret = 0;
    uint8_t reg = 0;
	//Check if DRDY is used
	if(Handle->DRDY_Config == HTS221_DataReadyEnable)
	{
		//If its used, read it.
		int32_t PinState = Handle->IO.ReadPin();
		//If Its High and Polarity is Active Low OR If its Low and Polarity is Active High, return data not ready
        if((Handle->DRDY_Polarity == HTS221_DataActiveHigh && PinState == 0) || (Handle->DRDY_Polarity == HTS221_DataActiveLow && PinState != 0))
        {
            return HTS221_DataNotReady;
        }
	} else
	{
		//Otherwise, read the status reg
        ret = HTS221_ReadReg(&Handle->Context, HTS221_REG_STATUS_REG, &reg, 1);
        if(ret != HTS221_Ok)
        {
        	return ret;
        } else if ((reg & HTS221_TEMPDATA_Msk) == 0)
        {
        	return HTS221_DataNotReady;
        }
	}
    return HTS221_CalculateTemperature(Handle, temperature);
}
int32_t HTS221_ReadHumidity(HTS221_Handle_t *Handle, float *humidity)
{
    int32_t ret = 0;
    uint8_t reg = 0;
	//Check if DRDY is used
	if(Handle->DRDY_Config == HTS221_DataReadyEnable)
	{
		//If its used, read it.
		int32_t PinState = Handle->IO.ReadPin();
		//If Its High and Polarity is Active Low OR If its Low and Polarity is Active High, return data not ready
        if((Handle->DRDY_Polarity == HTS221_DataActiveHigh && PinState == 0) || (Handle->DRDY_Polarity == HTS221_DataActiveLow && PinState != 0))
        {
            return HTS221_DataNotReady;
        }
	} else
	{
		//Otherwise, read the status reg
        ret = HTS221_ReadReg(&Handle->Context, HTS221_REG_STATUS_REG, &reg, 1);
        if(ret != HTS221_Ok)
        {
        	return ret;
        } else if ((reg & HTS221_HUMDATA_Msk) == 0)
        {
        	return HTS221_DataNotReady;
        }
	}

	return HTS221_CalculateHumidity(Handle, humidity);
}

int32_t HTS221_PowerDown(HTS221_Handle_t *Handle)
{
	uint8_t temp = 0;
	int32_t ret = 0;
	ret = HTS221_ReadReg(&Handle->Context, HTS221_REG_CTRL_REG1, &temp, 1);  //Read Reg
	if(ret != 0)
	{
		return ret;
	}

	temp &= ~HTS221_POWERON_SIGNAL;                                          //Modify Reg
	return HTS221_WriteReg(&Handle->Context,HTS221_REG_CTRL_REG1, &temp, 1); //Store Reg
}

int32_t HTS221_PowerUp(HTS221_Handle_t *Handle)
{
	uint8_t temp = 0;
	int32_t ret = 0;
	ret = HTS221_ReadReg(&Handle->Context, HTS221_REG_CTRL_REG1, &temp, 1);  //Read Reg
	if(ret != 0)
	{
		return ret;
	}

	temp |= HTS221_POWERON_SIGNAL;                                           //Modify Reg
	return HTS221_WriteReg(&Handle->Context,HTS221_REG_CTRL_REG1, &temp, 1); //Store Reg
}


static int32_t HTS221_ReadRegWrapper(void *Handle, uint8_t Reg, uint8_t *Buffer, uint8_t Length)
{
	HTS221_Handle_t *HTS221_Handle = Handle;
	return HTS221_Handle->IO.Read(Reg, Buffer, Length);
}

static int32_t HTS221_WriteRegWrapper(void *Handle, uint8_t Reg, uint8_t *Buffer, uint8_t Length)
{
	HTS221_Handle_t *HTS221_Handle = Handle;
	return HTS221_Handle->IO.Write(Reg, Buffer, Length);
}

static int32_t HTS221_RegisterInit(HTS221_Handle_t *Handle, HTS221_Init_Struct_t Settings)
{
    int32_t ret =  0;
    uint8_t temp = 0;

    //reset registers to factory default
    ret = HTS221_ResetRegisters(Handle);
    if(ret != HTS221_Ok)
    {
    	return ret;
    }

    temp = HTS221_BDU_ENABLE | Settings.OutputDataRate;
    ret = HTS221_WriteReg(&Handle->Context, HTS221_REG_CTRL_REG1, &temp,1);
    if(ret != HTS221_Ok)
    {
    	return ret;
    }

    temp = Settings.DataReadySignalPolarity | Settings.DataReadyOutputMode | Settings.DataReadyConfig;
    return HTS221_WriteReg(&Handle->Context, HTS221_REG_CTRL_REG3, &temp,1);
}

static int32_t HTS221_CalculateTemperature(HTS221_Handle_t *Handle, float *temperature)
{
	int32_t ret = 0;
	uint8_t Buffer[2] = {0};
	int16_t temp = 0;
    float T0_OUT = 0, T1_OUT = 0;
    float T1_DEGC = 0,  T0_DEGC = 0;


    //Fetch T0
    ret = HTS221_ReadReg(&Handle->Context, HTS221_REG_T0_OUT, Buffer, 2);
    if(ret != HTS221_Ok)
    {
    	return ret;
    }
    temp = (Buffer[1] << 8) + Buffer[0];
    T0_OUT = temp * 1.0f;

    //Fetch T1
    ret = HTS221_ReadReg(&Handle->Context, HTS221_REG_T1_OUT, Buffer, 2);
    if(ret != HTS221_Ok)
    {
    	return ret;
    }
    temp = (Buffer[1] << 8) + Buffer[0];
    T1_OUT = temp * 1.0f;

    //Fetch T0DegC
    ret = HTS221_ReadReg(&Handle->Context, HTS221_REG_T0_degC_x8, Buffer, 1); //Get LSB, store in Buffer16
    if(ret != HTS221_Ok)
    {
    	return ret;
    }

    ret = HTS221_ReadReg(&Handle->Context, HTS221_REG_T1_T0_msb, &Buffer[1], 1);  //Get MSB, store in Buffer8
    if(ret != HTS221_Ok)
    {
    	return ret;
    }
    temp = ((Buffer[1] & 0x03) << 8) + Buffer[0];
    T0_DEGC = temp / 8.0f; //Calculate

    //Fetch T1DegC
    ret = HTS221_ReadReg(&Handle->Context, HTS221_REG_T1_degC_x8, Buffer, 1); //Get LSB, store in Buffer16
    if(ret != HTS221_Ok)
    {
    	return ret;
    }
    temp = ((Buffer[1] & 0x0C) << 6) + Buffer[0];
    T1_DEGC = temp / 8.0f; //Calculate

    ret = HTS221_ReadReg(&Handle->Context, HTS221_REG_TEMP_OUT_L, Buffer, 2);
    if(ret != HTS221_Ok)
    {
    	return ret;
    }
    temp = (Buffer[1] << 8) + Buffer[0];
    *temperature = (((T1_DEGC - T0_DEGC) * temp) + ((T1_OUT * T0_DEGC) - (T0_OUT * T1_DEGC))) / (T1_OUT - T0_OUT); //Linear Interpolation
    return HTS221_Ok;
}

static int32_t HTS221_CalculateHumidity(HTS221_Handle_t *Handle, float *humidity)
{
	int32_t ret = 0;
	uint8_t Buffer[2] = {0};
	int16_t temp = 0;
    float H0_OUT = 0, H1_OUT = 0;
    float H1_REL = 0,  H0_REL = 0;
    float result = 0;

    //Fetch H0
    ret = HTS221_ReadReg(&Handle->Context, HTS221_REG_H0_T0_OUT, Buffer, 2);
    if(ret != HTS221_Ok)
    {
    	return ret;
    }
    temp = (Buffer[1] << 8) + Buffer[0];
    H0_OUT = temp * 1.0f;

    //Fetch H1
    ret = HTS221_ReadReg(&Handle->Context, HTS221_REG_H1_T0_OUT, Buffer, 2);
    if(ret != HTS221_Ok)
    {
    	return ret;
    }
    temp = (Buffer[1] << 8) + Buffer[0];
    H1_OUT = temp * 1.0f;

    //Fetch H0_Rh
    ret = HTS221_ReadReg(&Handle->Context, HTS221_REG_H0_rH_x2, Buffer, 1);
    if(ret != HTS221_Ok)
    {
    	return ret;
    }
    H0_REL = Buffer[0] / 2.0f;

    //Fetch H0_Rh
    ret = HTS221_ReadReg(&Handle->Context, HTS221_REG_H1_rH_x2, Buffer, 1);
    if(ret != HTS221_Ok)
    {
    	return ret;
    }
    H1_REL = Buffer[0] / 2.0f;

    //Fetch Measurement
    ret = HTS221_ReadReg(&Handle->Context, HTS221_REG_HUMIDITY_OUT_L, Buffer, 2);
    if(ret != HTS221_Ok)
    {
    	return ret;
    }
    temp = (Buffer[1] << 8) + Buffer[0];
    result = (((H1_REL - H0_REL) * temp) + ((H1_OUT * H0_REL) - (H0_OUT * H1_REL))) / (H1_OUT - H0_OUT); //Linear Interpolation
    if(result > 100.0)
    {
    	*humidity = 100.0;
    } else if (result < 0.0)
    {
    	*humidity = 0.0;
    } else
    {
    	*humidity = result;
    }

    return HTS221_Ok;
}

int32_t HTS221_GetSamplePeriod(HTS221_Handle_t *Handle, uint32_t *Period)
{
	switch(Handle->OutputDataRate)
	{
	case(HTS221_OneShotMode):
			*Period = 0;
	        return HTS221_PeriodOneShot;
			break;
	case(HTS221_1_0Hz):
			*Period = 1000;
			break;
	case(HTS221_7_0p0Hz):
			*Period = 143;
	        break;
	case(HTS221_12_5Hz):
			*Period = 80;
	        break;
	default:
		   return HTS221_ODR_Error;
	}
	return HTS221_Ok;
}
