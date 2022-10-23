/*
 * LPS22HH.c
 *
 *  Created on: Sep 15, 2022
 *      Author: evanl
 */

#include "LPS22HH.h"
#include <stddef.h>

#define LPS22HH_ResetSignal (1 << 2)
#define LPS22HH_RebootSignal (1 << 7)
#define LPS22HH_StartConvSignal (1)
#define LPS22HH_PDATA_Msk (0x01)

static int32_t LPS22HH_SWReset(LPS22HH_Handle_t *Handle);
static int32_t LPS22HH_Reboot(LPS22HH_Handle_t *Handle);
static int32_t LPS22HH_WriteRegWrapper(void *Handle,uint8_t Reg, uint8_t *Buffer, uint8_t Length);
static int32_t LPS22HH_ReadRegWrapper(void *Handle,uint8_t Reg, uint8_t *Buffer, uint8_t Length);
static int32_t LPS22HH_RegisterInit(LPS22HH_Handle_t *Handle, LPS22HH_Init_Struct_t Settings);
static void ConvertPressureData(uint8_t *buffer, float *pressure);

int32_t LPS22HH_Init(LPS22HH_Handle_t *Handle, LPS22HH_Init_Struct_t Settings, LPS22HH_IO_t *IO)
{
	if(Handle->Status != LPS22HH_Initialized)
	    {

	    if(Handle == NULL)
	    {
		    return LPS22HH_HandleError;
	    }

	    if(IO->Write == NULL || IO->Read == NULL || IO->GetTick == NULL)
	    {
            return LPS22HH_IOError;
	    }

	    Handle->IO = *IO;
	    Handle->DataRate = Settings.ODR;
	    Handle->Context.Handle = Handle;
	    Handle->Context.Read = LPS22HH_ReadRegWrapper;
	    Handle->Context.Write = LPS22HH_WriteRegWrapper;

	    if(Handle->IO.Init != NULL)
	    {
	    	Handle->IO.Init();
	    }

        int32_t ret = LPS22HH_RegisterInit(Handle, Settings);
        if(ret != LPS22HH_Ok)
        {
        	return ret;
        }

	    Handle->Status = LPS22HH_Initialized;
	    }
	return LPS22HH_Ok;
}

int32_t LPS22HH_DeInit(LPS22HH_Handle_t *Handle)
{
	if(Handle == NULL)
	{
	    return LPS22HH_HandleError;
	}

	if(Handle->IO.DeInit != NULL)
	{
		Handle->IO.DeInit();
	}

	Handle->Context.Handle = NULL;
	Handle->Context.Read = NULL;
	Handle->Context.Write = NULL;
	Handle->DataRate = 0;
	Handle->IO.DeInit = NULL;
	Handle->IO.GetTick = NULL;
	Handle->IO.Init = NULL;
	Handle->IO.Read = NULL;
	Handle->IO.ReadPin = NULL;
	Handle->IO.Write = NULL;
	Handle->Status = LPS22HH_Uninitialized;
	return LPS22HH_Ok;
}

int32_t LPS22HH_ResetDevice(LPS22HH_Handle_t *Handle)
{
	int32_t ret = LPS22HH_Ok;
	if(Handle == NULL)
	{
		return LPS22HH_HandleError;
	}

	ret = LPS22HH_Reboot(Handle);
	if(ret != LPS22HH_Ok)
	{
		return ret;
	}


	ret = LPS22HH_SWReset(Handle);
	if(ret != LPS22HH_Ok)
	{
		return ret;
	}

	return LPS22HH_Ok;
}

int32_t LPS22HH_SetReferencePressure(LPS22HH_Handle_t *Handle, float Pressure)
{
	if(Handle->Status != LPS22HH_Initialized)
	{
		return LPS22HH_InitError;
	}

	uint32_t NewRefPressure = Pressure * 4096;
	if(NewRefPressure > 65535)
	{
		return LPS22HH_RefPError;
	}

	return LPS22HH_WriteReg(&Handle->Context, LPS22HH_REG_REF_P_L, (uint8_t*)&NewRefPressure, 2);
}

int32_t LPS22HH_GetReferencePressure(LPS22HH_Handle_t *Handle, float *Pressure)
{
	if(Handle->Status != LPS22HH_Initialized)
	{
		return LPS22HH_InitError;
	}

	uint16_t RefPressure = 0;


	int32_t ret = LPS22HH_ReadReg(&Handle->Context, LPS22HH_REG_REF_P_L, (uint8_t*)&RefPressure, 2);
	if(ret != LPS22HH_Ok)
	{
		return ret;
	}

	*Pressure = RefPressure / 4096.0;
	return LPS22HH_Ok;

}

int32_t LPS22HH_StartConversion(LPS22HH_Handle_t *Handle)
{
	if(Handle->Status != LPS22HH_Initialized)
	{
		return LPS22HH_InitError;
	}

	if(Handle->DataRate != LPS22HH_OneShot)
	{
		return LPS22HH_OneShotError;
	}

	uint8_t buffer = 0;
	int32_t ret = LPS22HH_ReadReg(&Handle->Context,LPS22HH_REG_CTRL_REG2, &buffer, 1);
	if(ret != LPS22HH_Ok) //Read
	{
		return ret;
	}

	buffer |= LPS22HH_StartConvSignal; //Modify

	return LPS22HH_WriteReg(&Handle->Context,LPS22HH_REG_CTRL_REG2, &buffer, 1); //Store

}
int32_t LPS22HH_ReadPressure(LPS22HH_Handle_t *Handle, float *Pressure)
{
	if(Handle->Status != LPS22HH_Initialized)
	{
		return LPS22HH_InitError;
	}

	uint8_t buffer[3] = {0};

	int32_t ret = LPS22HH_ReadReg(&Handle->Context, LPS22HH_REG_STATUS, buffer, 1);
	if(ret != LPS22HH_Ok)
	{
		return ret;
	}

	if(!(buffer[0] & LPS22HH_PDATA_Msk))
	{
		return LPS22HH_NoNewData;
	}

	ret = LPS22HH_ReadReg(&Handle->Context, LPS22HH_REG_PRESSURE_OUT_XL, buffer, 3);
	if(ret != LPS22HH_Ok)
	{
		return ret;
	}

	ConvertPressureData(buffer, Pressure);
	return LPS22HH_Ok;
}


static int32_t LPS22HH_SWReset(LPS22HH_Handle_t *Handle)
{
    if(Handle == NULL)
    {
        return LPS22HH_HandleError;
    }

    if(Handle->Context.Read == NULL || Handle->Context.Write == NULL)
    {
    	return LPS22HH_IOError;
    }


    int32_t ret = 0;
    uint8_t buffer = LPS22HH_ResetSignal;
    ret = LPS22HH_WriteReg(&Handle->Context, LPS22HH_REG_CTRL_REG2, &buffer, 1);
    if(ret != LPS22HH_Ok)
    {
    	return ret;
    }

    uint32_t timer = Handle->IO.GetTick();
    while(buffer & LPS22HH_ResetSignal)
    {
    	ret = LPS22HH_ReadReg(&Handle->Context, LPS22HH_REG_CTRL_REG2, &buffer, 1);
    	if(ret != LPS22HH_Ok)
    	{
    		return ret;
    	}

    	if(timer + 100 < Handle->IO.GetTick())
    	{
    		return LPS22HH_Timeout;
    	}
    }
    return ret;
}

static int32_t LPS22HH_Reboot(LPS22HH_Handle_t *Handle)
{
    if(Handle == NULL)
    {
        return LPS22HH_HandleError;
    }

    if(Handle->Context.Read == NULL || Handle->Context.Write == NULL)
    {
    	return LPS22HH_IOError;
    }

    int32_t ret = 0;
    uint8_t buffer = LPS22HH_RebootSignal;
    ret = LPS22HH_WriteReg(&Handle->Context, LPS22HH_REG_CTRL_REG2, &buffer, 1);
    if(ret != LPS22HH_Ok)
    {
    	return ret;
    }

    uint32_t timer = Handle->IO.GetTick();
    while(buffer & LPS22HH_RebootSignal)
    {
    	ret = LPS22HH_ReadReg(&Handle->Context, LPS22HH_REG_INT_SOURCE, &buffer, 1);
    	if(ret != LPS22HH_Ok)
    	{
    		return ret;
    	}

    	if(timer + 100 < Handle->IO.GetTick())
    	{
    		return LPS22HH_Timeout;
    	}

    }
    return ret;
}

static int32_t LPS22HH_WriteRegWrapper(void *Handle, uint8_t Reg, uint8_t *Buffer, uint8_t Length)
{
	LPS22HH_Handle_t *Dev = Handle;
	if(Dev->IO.Write != NULL)
	{
		return Dev->IO.Write(Reg, Buffer, Length);
	}
    return LPS22HH_IOError;
}

static int32_t LPS22HH_ReadRegWrapper(void *Handle,uint8_t Reg, uint8_t *Buffer, uint8_t Length)
{
	LPS22HH_Handle_t *Dev = Handle;
	if(Dev->IO.Read != NULL)
	{
		return Dev->IO.Read(Reg, Buffer, Length);
	}
    return LPS22HH_IOError;
}

static int32_t LPS22HH_RegisterInit(LPS22HH_Handle_t *Handle, LPS22HH_Init_Struct_t Settings)
{
	int32_t ret = LPS22HH_ResetDevice(Handle);
	if(ret != LPS22HH_Ok)
	{
		return ret;
	}

	uint8_t buffer[2] = {0};
    buffer[0] = Settings.INTERRUPT_CFG;
	ret = LPS22HH_WriteReg(&Handle->Context, LPS22HH_REG_INTERRUPT_CFG, &buffer[0], 1);
	if(ret != LPS22HH_Ok)
	{
		return ret;
	}

	if(Settings.THS_P > 32767)
	{
		buffer[0] = 32767 & 0x00FF;
		buffer[1] = (32767 & 0xFF00) >> 8;
	} else
	{
		buffer[0] = Settings.THS_P & 0x00FF;
		buffer[1] = (Settings.THS_P & 0xFF00) >> 8;
	}
	ret = LPS22HH_WriteReg(&Handle->Context, LPS22HH_REG_THS_P_L, &buffer[0], 2);
	if(ret != LPS22HH_Ok)
	{
		return ret;
	}

	buffer[0] = Settings.IF_CTRL;
	ret = LPS22HH_WriteReg(&Handle->Context, LPS22HH_REG_IF_CTRL, &buffer[0], 1);
	if(ret != LPS22HH_Ok)
	{
		return ret;
	}

	buffer[0] = Settings.ODR | Settings.LPFP | 0x02 | Settings.SIM; //Enable BDU = 0x02
	ret = LPS22HH_WriteReg(&Handle->Context, LPS22HH_REG_CTRL_REG1, &buffer[0], 1);
	if(ret != LPS22HH_Ok)
	{
		return ret;
	}

	buffer[0] = Settings.CTRL_REG2;
	ret = LPS22HH_WriteReg(&Handle->Context, LPS22HH_REG_CTRL_REG2, &buffer[0], 1);
	if(ret != LPS22HH_Ok)
	{
		return ret;
	}

	buffer[0] = Settings.INT_DRDY_Control | Settings.INT_S;
	ret = LPS22HH_WriteReg(&Handle->Context, LPS22HH_REG_CTRL_REG3, &buffer[0], 1);
	if(ret != LPS22HH_Ok)
	{
		return ret;
	}

	buffer[0] = Settings.STOP_ON_WTM | Settings.F_MODE;
	ret = LPS22HH_WriteReg(&Handle->Context, LPS22HH_REG_FIFO_CTRL, &buffer[0], 1);
	if(ret != LPS22HH_Ok)
	{
		return ret;
	}

	if(Settings.WTM > 255)
	{
		buffer[0] = 255;
	} else
	{
		buffer[0] = Settings.WTM;
	}
	ret = LPS22HH_WriteReg(&Handle->Context, LPS22HH_REG_FIFO_WTM, &buffer[0], 1);
	if(ret != LPS22HH_Ok)
	{
		return ret;
	}

	if((Settings.REF_P * 4096) > 65535) //float -> LSB = multiply by 4096
	{
		return LPS22HH_RefPError;
	}

	*((uint16_t*)&buffer) = Settings.REF_P * 4096;
	ret = LPS22HH_WriteReg(&Handle->Context, LPS22HH_REG_REF_P_L, &buffer[0], 2);
	if(ret != LPS22HH_Ok)
	{
		return ret;
	}

	return LPS22HH_Ok;
}

static void ConvertPressureData(uint8_t *buffer, float *pressure)
{
	int32_t Data;
	//determine sign
	if(buffer[2] & 0x80)
	{
		Data = 0xFF000000; //Sign Extend if MSB of 24 Bit number is set
	} else
	{
		Data = 0;
	}
	Data = Data + buffer[2] * 65536 + buffer[1] * 256 + buffer[0]; //Data = Data + Concat buffer[2:0]
	*pressure = Data / 4096.0;
}

