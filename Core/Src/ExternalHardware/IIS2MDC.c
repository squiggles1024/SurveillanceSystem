/*
 * IIS2MDC.c
 *
 *  Created on: Sep 15, 2022
 *      Author: evanl
 */

#include "IIS2MDC.h"
#include <stddef.h>

#define IIS2MDC_ResetSignal (1 << 5)
#define IIS2MDC_RebootSignal (1 << 6)
#define IIS2MDC_StartConvSignal (1)
#define IIS2MDC_XDATA_Msk (0x04)
#define IIS2MDC_YDATA_Msk (0x02)
#define IIS2MDC_ZDATA_Msk (0x01)
#define IIS2MDC_ODR_Msk (0x0F)

static int32_t IIS2MDC_SWReset(IIS2MDC_Handle_t *Handle);
static int32_t IIS2MDC_Reboot(IIS2MDC_Handle_t *Handle);
static int32_t IIS2MDC_WriteRegWrapper(void *Handle,uint8_t Reg, uint8_t *Buffer, uint8_t Length);
static int32_t IIS2MDC_ReadRegWrapper(void *Handle,uint8_t Reg, uint8_t *Buffer, uint8_t Length);
static int32_t IIS2MDC_RegisterInit(IIS2MDC_Handle_t *Handle, IIS2MDC_InitStruct_t Settings);
static void ConvertMagData(uint8_t *buffer, float *magnetism);
static void DeConvertMagData(float magnetism, uint8_t *buffer);


int32_t IIS2MDC_Init(IIS2MDC_Handle_t *Handle, IIS2MDC_InitStruct_t Settings, IIS2MDC_IO_t *IO)
{
    if(Handle->Status != IIS2MDC_Initialized)
    {
        if(Handle == NULL)
        {
            return IIS2MDC_HandleError;
        }

        if(IO->Read == NULL || IO->Write == NULL || IO->GetTick == NULL)
        {
        	return IIS2MDC_IOError;
        }

        if(Settings.IntThreshold > 49150.5  || Settings.OffsetX > 49150.5 || Settings.OffsetY > 49150.5  || Settings.OffsetZ > 49150.5)
        {
        	return IIS2MDC_FloatError;
        }

        if(Settings.IntThreshold < -49150.5 || Settings.OffsetX < -49150.5 || Settings.OffsetY < -49150.5  || Settings.OffsetZ < -49150.5)
        {
        	return IIS2MDC_FloatError;
        }

        Handle->IO = *IO;
        Handle->Context.Handle = Handle;
        Handle->Context.Read = IIS2MDC_ReadRegWrapper;
        Handle->Context.Write = IIS2MDC_WriteRegWrapper;
        if((Settings.ConfigRegA & IIS2MDC_ODR_Msk) == IIS2MDC_ODR_OneShot)
        {
        	Handle->DataMode = IIS2MDC_OneShotMode;
        } else
        {
        	Handle->DataMode = IIS2MDC_Continuous;
        }

        Handle->OutputDataRate = Settings.ConfigRegA & IIS2MDC_ODR_Msk;
        int32_t ret = IIS2MDC_RegisterInit(Handle, Settings);
        if(ret != IIS2MDC_Ok)
        {
        	return ret;
        }
        Handle->Status = IIS2MDC_Initialized;
    }
    return IIS2MDC_Ok;
}

int32_t IIS2MDC_DeInit(IIS2MDC_Handle_t *Handle)
{
    if(Handle == NULL)
    {
    	return IIS2MDC_HandleError;
    }

    if(Handle->IO.Deinit != NULL)
    {
    	Handle->IO.Deinit();
    }

    Handle->IO.Init = NULL;
    Handle->IO.Deinit = NULL;
    Handle->IO.Write = NULL;
    Handle->IO.Read = NULL;
    Handle->IO.GetTick = NULL;
    Handle->IO.ReadPin = NULL;
    Handle->DataMode = 0;
    Handle->Context.Handle = NULL;
    Handle->Context.Read = NULL;
    Handle->Context.Write = NULL;
    Handle->Status = IIS2MDC_Uninitialized;

    return IIS2MDC_Ok;
}

int32_t IIS2MDC_ResetDevice(IIS2MDC_Handle_t *Handle)
{
	int32_t ret = IIS2MDC_Ok;
	if(Handle == NULL)
	{
		return IIS2MDC_HandleError;
	}

    if(Handle->IO.Write == NULL || Handle->IO.Read == NULL || Handle->IO.GetTick == NULL)
    {
    	return IIS2MDC_IOError;
    }

	ret = IIS2MDC_Reboot(Handle);
	if(ret != IIS2MDC_Ok)
	{
		return ret;
	}


	ret = IIS2MDC_SWReset(Handle);
	if(ret != IIS2MDC_Ok)
	{
		return ret;
	}

	return IIS2MDC_Ok;
}

int32_t IIS2MDC_SetOffsetMagnetismX(IIS2MDC_Handle_t *Handle, float Offset)
{
	if(Handle->Status != IIS2MDC_Initialized)
	{
		return IIS2MDC_InitError;
	}
	uint32_t ret = 0;
	uint8_t Buffer[2] = {0};
	DeConvertMagData(Offset, Buffer);
	ret = IIS2MDC_WriteReg(&Handle->Context,IIS2MDC_REG_OFFSET_X_REG_L,Buffer,2);
	if(ret != IIS2MDC_Ok)
	{
		return ret;
	}
	return IIS2MDC_Ok;
}

int32_t IIS2MDC_GetOffsetMagnetismX(IIS2MDC_Handle_t *Handle, float *Offset)
{
	if(Handle->Status != IIS2MDC_Initialized)
	{
		return IIS2MDC_InitError;
	}
	uint8_t Buffer[2] = {0};
	uint32_t ret = IIS2MDC_ReadReg(&Handle->Context,IIS2MDC_REG_OFFSET_X_REG_L,Buffer,2);
	if(ret != IIS2MDC_Ok)
	{
		return ret;
	}
	ConvertMagData(Buffer,Offset);
    return IIS2MDC_Ok;
}

int32_t IIS2MDC_SetOffsetMagnetismY(IIS2MDC_Handle_t *Handle, float Offset)
{
	if(Handle->Status != IIS2MDC_Initialized)
	{
		return IIS2MDC_InitError;
	}
	uint32_t ret = 0;
	uint8_t Buffer[2] = {0};
	DeConvertMagData(Offset, Buffer);
	ret = IIS2MDC_WriteReg(&Handle->Context,IIS2MDC_REG_OFFSET_Y_REG_L,Buffer,2);
	if(ret != IIS2MDC_Ok)
	{
		return ret;
	}
	return IIS2MDC_Ok;
}

int32_t IIS2MDC_GetOffsetMagnetismY(IIS2MDC_Handle_t *Handle, float *Offset)
{
	if(Handle->Status != IIS2MDC_Initialized)
	{
		return IIS2MDC_InitError;
	}
	uint8_t Buffer[2] = {0};
	uint32_t ret = IIS2MDC_ReadReg(&Handle->Context,IIS2MDC_REG_OFFSET_Y_REG_L,Buffer,2);
	if(ret != IIS2MDC_Ok)
	{
		return ret;
	}
	ConvertMagData(Buffer,Offset);
    return IIS2MDC_Ok;
}

int32_t IIS2MDC_SetOffsetMagnetismZ(IIS2MDC_Handle_t *Handle, float Offset)
{
	if(Handle->Status != IIS2MDC_Initialized)
	{
		return IIS2MDC_InitError;
	}
	uint32_t ret = 0;
	uint8_t Buffer[2] = {0};
	DeConvertMagData(Offset, Buffer);
	ret = IIS2MDC_WriteReg(&Handle->Context,IIS2MDC_REG_OFFSET_Z_REG_L,Buffer,2);
	if(ret != IIS2MDC_Ok)
	{
		return ret;
	}
	return IIS2MDC_Ok;
}

int32_t IIS2MDC_GetOffsetMagnetismZ(IIS2MDC_Handle_t *Handle, float *Offset)
{
	if(Handle->Status != IIS2MDC_Initialized)
	{
		return IIS2MDC_InitError;
	}
	uint8_t Buffer[2] = {0};
	uint32_t ret = IIS2MDC_ReadReg(&Handle->Context,IIS2MDC_REG_OFFSET_Z_REG_L,Buffer,2);
	if(ret != IIS2MDC_Ok)
	{
		return ret;
	}
	ConvertMagData(Buffer,Offset);
    return IIS2MDC_Ok;
}

int32_t IIS2MDC_StartConversion(IIS2MDC_Handle_t *Handle)
{
	if(Handle->Status != IIS2MDC_Initialized)
	{
		return IIS2MDC_InitError;
	}

	if(Handle->DataMode != IIS2MDC_OneShotMode)
	{
        return IIS2MDC_OneShotError;
	}

	uint8_t buffer = 0;
	uint32_t ret = IIS2MDC_ReadReg(&Handle->Context, IIS2MDC_REG_CFG_REG_A, &buffer, 1);
	if(ret != IIS2MDC_Ok)
	{
		return ret;
	}
	buffer |= IIS2MDC_ODR_OneShot;
	ret = IIS2MDC_WriteReg(&Handle->Context, IIS2MDC_REG_CFG_REG_A, &buffer, 1);
	if(ret != IIS2MDC_Ok)
	{
		return ret;
	}
	return IIS2MDC_Ok;
}

int32_t IIS2MDC_ReadMagnetismXYZ(IIS2MDC_Handle_t *Handle, float *Mx, float *My, float *Mz)
{
	if(Handle->Status != IIS2MDC_Initialized)
	{
		return IIS2MDC_InitError;
	}

	uint8_t Buffer[6] = {0};
	uint32_t ret = IIS2MDC_ReadReg(&Handle->Context, IIS2MDC_REG_STATUS_REG, Buffer, 1);
	if(ret != IIS2MDC_Ok)
	{
		return ret;
	}
	if(Buffer[0] & (IIS2MDC_XDATA_Msk | IIS2MDC_YDATA_Msk | IIS2MDC_ZDATA_Msk))
	{
		ret = IIS2MDC_ReadReg(&Handle->Context, IIS2MDC_REG_OUTX_L_REG, Buffer, 6);
		if(ret != IIS2MDC_Ok)
		{
			return ret;
		}
		ConvertMagData(Buffer, Mx);
		ConvertMagData(&Buffer[2], My);
		ConvertMagData(&Buffer[4], Mz);
		return IIS2MDC_DataReady;
	}
	return IIS2MDC_DataNotReady;
}

int32_t IIS2MDC_ReadMagnetismX(IIS2MDC_Handle_t *Handle, float *Mx)
{
	if(Handle->Status != IIS2MDC_Initialized)
	{
		return IIS2MDC_InitError;
	}

	uint8_t Buffer[2] = {0};
	uint32_t ret = IIS2MDC_ReadReg(&Handle->Context, IIS2MDC_REG_STATUS_REG, Buffer, 1);
	if(ret != IIS2MDC_Ok)
	{
		return ret;
	}
	if(Buffer[0] & IIS2MDC_XDATA_Msk)
	{
		ret = IIS2MDC_ReadReg(&Handle->Context, IIS2MDC_REG_OUTX_L_REG, Buffer, 2);
		if(ret != IIS2MDC_Ok)
		{
			return ret;
		}
		ConvertMagData(Buffer, Mx);
		return IIS2MDC_DataReady;
	}
	return IIS2MDC_DataNotReady;
}

int32_t IIS2MDC_ReadMagnetismY(IIS2MDC_Handle_t *Handle, float *My)
{
	if(Handle->Status != IIS2MDC_Initialized)
	{
		return IIS2MDC_InitError;
	}

	uint8_t Buffer[2] = {0};
	uint32_t ret = IIS2MDC_ReadReg(&Handle->Context, IIS2MDC_REG_STATUS_REG, Buffer, 1);
	if(ret != IIS2MDC_Ok)
	{
		return ret;
	}
	if(Buffer[0] & IIS2MDC_YDATA_Msk)
	{
		ret = IIS2MDC_ReadReg(&Handle->Context, IIS2MDC_REG_OUTY_L_REG, Buffer, 2);
		if(ret != IIS2MDC_Ok)
		{
			return ret;
		}
		ConvertMagData(Buffer, My);
		return IIS2MDC_DataReady;
	}
	return IIS2MDC_DataNotReady;
}

int32_t IIS2MDC_ReadMagnetismZ(IIS2MDC_Handle_t *Handle, float *Mz)
{
	if(Handle->Status != IIS2MDC_Initialized)
	{
		return IIS2MDC_InitError;
	}

	uint8_t Buffer[2] = {0};
	uint32_t ret = IIS2MDC_ReadReg(&Handle->Context, IIS2MDC_REG_STATUS_REG, Buffer, 1);
	if(ret != IIS2MDC_Ok)
	{
		return ret;
	}
	if(Buffer[0] & IIS2MDC_ZDATA_Msk)
	{
		ret = IIS2MDC_ReadReg(&Handle->Context, IIS2MDC_REG_OUTZ_L_REG, Buffer, 2);
		if(ret != IIS2MDC_Ok)
		{
			return ret;
		}
		ConvertMagData(Buffer, Mz);
		return IIS2MDC_DataReady;
	}
	return IIS2MDC_DataNotReady;
}


static int32_t IIS2MDC_WriteRegWrapper(void *Handle,uint8_t Reg, uint8_t *Buffer, uint8_t Length)
{
	IIS2MDC_Handle_t *Dev= Handle;
	if(Dev->IO.Write != NULL)
	{
		return Dev->IO.Write(Reg,Buffer,Length);
	}
    return IIS2MDC_IOError;
}

static int32_t IIS2MDC_ReadRegWrapper(void *Handle,uint8_t Reg, uint8_t *Buffer, uint8_t Length)
{
	IIS2MDC_Handle_t *Dev= Handle;
	if(Dev->IO.Read != NULL)
	{
		return Dev->IO.Read(Reg,Buffer,Length);
	}
    return IIS2MDC_IOError;
}

static int32_t IIS2MDC_RegisterInit(IIS2MDC_Handle_t *Handle, IIS2MDC_InitStruct_t Settings)
{
	int32_t ret = IIS2MDC_ResetDevice(Handle);
	if(ret != IIS2MDC_Ok)
	{
		return ret;
	}

	uint8_t Buffer[2] = {0};

	DeConvertMagData(Settings.IntThreshold, Buffer);
	ret = IIS2MDC_WriteReg(&Handle->Context,IIS2MDC_REG_INT_THS_H_REG,Buffer,2);
	if(ret != IIS2MDC_Ok)
	{
		return ret;
	}


	DeConvertMagData(Settings.OffsetX, Buffer);
	ret = IIS2MDC_WriteReg(&Handle->Context,IIS2MDC_REG_OFFSET_X_REG_L,Buffer,2);
	if(ret != IIS2MDC_Ok)
	{
		return ret;
	}

	DeConvertMagData(Settings.OffsetY, Buffer);
	ret = IIS2MDC_WriteReg(&Handle->Context,IIS2MDC_REG_OFFSET_Y_REG_L,Buffer,2);
	if(ret != IIS2MDC_Ok)
	{
		return ret;
	}

	DeConvertMagData(Settings.OffsetZ, Buffer);
	ret = IIS2MDC_WriteReg(&Handle->Context,IIS2MDC_REG_OFFSET_Z_REG_L,Buffer,2);
	if(ret != IIS2MDC_Ok)
	{
		return ret;
	}

	Buffer[0] = Settings.ConfigRegA;
	ret = IIS2MDC_WriteReg(&Handle->Context,IIS2MDC_REG_CFG_REG_A,Buffer,1);
	if(ret != IIS2MDC_Ok)
	{
		return ret;
	}

	Buffer[0] = Settings.ConfigRegB;
	ret = IIS2MDC_WriteReg(&Handle->Context,IIS2MDC_REG_CFG_REG_B,Buffer,1);
	if(ret != IIS2MDC_Ok)
	{
		return ret;
	}

	Buffer[0] = Settings.ConfigRegC;
	ret = IIS2MDC_WriteReg(&Handle->Context,IIS2MDC_REG_CFG_REG_C,Buffer,1);
	if(ret != IIS2MDC_Ok)
	{
		return ret;
	}

	Buffer[0] = Settings.IntCtrlReg;
	ret = IIS2MDC_WriteReg(&Handle->Context,IIS2MDC_REG_INT_CTRL_REG,Buffer,1);
	if(ret != IIS2MDC_Ok)
	{
		return ret;
	}

    return IIS2MDC_Ok;
}

static int32_t IIS2MDC_SWReset(IIS2MDC_Handle_t *Handle)
{
    int32_t ret = 0;
    uint8_t Buffer = IIS2MDC_ResetSignal;
    ret = IIS2MDC_WriteReg(&Handle->Context, IIS2MDC_REG_CFG_REG_A, &Buffer, 1);
    if(ret != IIS2MDC_Ok)
    {
    	return ret;
    }

    uint32_t timeout = Handle->IO.GetTick();
    while(Buffer & IIS2MDC_ResetSignal)
    {
        ret = IIS2MDC_ReadReg(&Handle->Context, IIS2MDC_REG_CFG_REG_A, &Buffer, 1);
        if(Handle->IO.GetTick() > timeout + 100)
        {
        	return IIS2MDC_Timeout;
        }
    }
    return IIS2MDC_Ok;
}

static int32_t IIS2MDC_Reboot(IIS2MDC_Handle_t *Handle)
{
    int32_t ret = 0;
    uint8_t Buffer = IIS2MDC_RebootSignal;
    ret = IIS2MDC_WriteReg(&Handle->Context, IIS2MDC_REG_CFG_REG_A, &Buffer, 1);
    if(ret != IIS2MDC_Ok)
    {
    	return ret;
    }
    uint32_t timeout = Handle->IO.GetTick();
    while((timeout + 20) > Handle->IO.GetTick()); //Delay 20ms
    return IIS2MDC_Ok;
}

static void ConvertMagData(uint8_t *buffer, float *magnetism)
{
	int16_t result = (buffer[1] * 256) + buffer[0];
	*magnetism = result * 1.5f;
}

static void DeConvertMagData(float magnetism, uint8_t *buffer)
{
    int16_t result = magnetism / 1.5f;
    buffer[1] = (result & 0xFF00) >> 8;
    buffer[0] = result & 0x00FF;
}

int32_t IIS2MDC_GetSamplePeriod(IIS2MDC_Handle_t *Handle, uint32_t *Period)
{
	if(Handle->Status != IIS2MDC_Initialized)
	{
		return IIS2MDC_InitError;
	}

	switch(Handle->OutputDataRate)
	{
	case(IIS2MDC_ODR_OneShot):
			*Period = 0;
	        return IIS2MDC_ODR_PeriodOneShot;
	case(IIS2MDC_ODR_10Hz):
			*Period =  (1000 / 10);
	        break;
	case(IIS2MDC_ODR_20Hz):
			*Period = (1000 / 20);
	        break;
	case(IIS2MDC_ODR_50Hz):
			*Period = (1000 / 50);
	        break;
	case(IIS2MDC_ODR_100Hz):
			*Period = (1000 / 100);
	        break;
	default:
		    return IIS2MDC_ODR_Error;
	}

	return IIS2MDC_Ok;
}
