/*
 * ISM330DHCX.c
 *
 *  Created on: Sep 15, 2022
 *      Author: evanl
 */


#include "ISM330DHCX.h"
#include "ISM330DHCX_Registers.h"
#include <stddef.h>

static int32_t ISM330DHCX_ReadWrapper(void *Handle, uint8_t Reg, uint8_t *Buffer, uint8_t Length);
static int32_t ISM330DHCX_WriteWrapper(void *Handle, uint8_t Reg, uint8_t *Buffer, uint8_t Length);
static int32_t ISM330DHCX_RegisterInit(ISM330DHCX_Handle_t *Handle,ISM330DHCX_Init_Struct_t Settings);
static void ConvertXLData(ISM330DHCX_AccelFullscale_t AccelResolution ,int16_t *buffer, float *data);
static void ConvertGData(ISM330DHCX_GyroFullScale_t GyroResolution, int16_t *buffer, float *data);
static void ConvertTData(uint8_t *buffer, float *data);

#define INT_PIN_DATA_NOT_READY() (ret == 0 && (Handle->IntPolarity == ISM330DHCX_IRQ_ActiveHigh)) || (ret != 1 && (Handle->IntPolarity == ISM330DHCX_IRQ_ActiveLow))
#define XL_DATA_MASK (1U)
#define G_DATA_MASK  (2U)
#define T_DATA_MASK  (4U)
#define ISM330DHCX_BOOT_BIT_MSK (0x80)


int32_t ISM330DHCX_Init(ISM330DHCX_Handle_t *Handle, ISM330DHCX_Init_Struct_t Settings, ISM330DHCX_IO_t *IO)
{
    if(Handle == NULL)
    {
    	return ISM330DHCX_Error;
    }


    if(ISM330DHCX_RegisterIOBus(Handle, IO) != ISM330DHCX_Ok)
    {
    	return ISM330DHCX_ParamError;
    }

    //Validate if using INT1
    if(Settings.INT1_CTRL != 0 || ((Settings.INT2_CTRL != 0) && (Settings.INT2_on_INT1 == ISM330DHCX_IRQOnBothPins)))
    {
    	if(IO->ReadInt1Pin == NULL){
        	return ISM330DHCX_ParamError; //If INT1 is used in any capacity, ReadInt1Pin is required
    	}

    }

    //Validate if using INT2
    if(Settings.INT2_CTRL != 0 && IO->ReadInt2Pin == NULL)
    {
    	return ISM330DHCX_ParamError; //If INT2 is used in any capacity, ReadInt2Pin is required
    }

    Handle->Context.Read = ISM330DHCX_ReadWrapper;
    Handle->Context.Write = ISM330DHCX_WriteWrapper;
    Handle->Context.Handle = Handle;
    Handle->AccelResolution = Settings.FS_XL;
    Handle->GyroResolution = Settings.FS_G;
    Handle->Int1Mode = Settings.INT1_CTRL;
    Handle->Int2Mode = Settings.INT2_CTRL;
    Handle->IntPinMux = Settings.INT2_on_INT1;
    Handle->IntPolarity = Settings.H_LACTIVE;
    Handle->GyroDataRate = Settings.ODR_G;
    Handle->AccelDataRate = Settings.ODR_XL;

    //Initialize Bus
    if(Handle->IO.Init != NULL)
    {
    	Handle->IO.Init();
    }

    return ISM330DHCX_RegisterInit(Handle, Settings);

}

void ISM330DHCX_DeInit(ISM330DHCX_Handle_t *Handle)
{
	if(Handle->IO.DeInit != NULL)
	{
		Handle->IO.DeInit();
	}

	Handle->IO.Init = NULL;
	Handle->IO.DeInit = NULL;
	Handle->IO.Read = NULL;
	Handle->IO.Write = NULL;
	Handle->IO.GetTick = NULL;
	Handle->IO.ReadInt1Pin = NULL;
	Handle->IO.ReadInt2Pin = NULL;
	Handle->Context.Handle = NULL;
	Handle->Context.Read = NULL;
	Handle->Context.Write = NULL;
    Handle->AccelResolution = 0;
    Handle->GyroResolution = 0;
    Handle->Int1Mode = 0;
    Handle->Int2Mode = 0;
    Handle->IntPinMux = 0;
    Handle->IntPolarity = 0;
}

int32_t ISM330DHCX_ResetDevice(ISM330DHCX_Handle_t *Handle, ISM330DHCX_Init_Struct_t Settings, ISM330DHCX_IO_t *IO)
{
    ISM330DHCX_DeInit(Handle);
    return ISM330DHCX_Init(Handle, Settings, IO);
}

int32_t ISM330DHCX_RegisterIOBus(ISM330DHCX_Handle_t *Handle, ISM330DHCX_IO_t *IO)
{
    //Validate if Read/Write/GetTick have been provided
    if(IO->Read == NULL || IO->Write==NULL || IO->GetTick == NULL)
    {
    	return ISM330DHCX_ParamError; //Write, Read, GetTick are required for basic device functionality
    }

    Handle->IO = *IO;
    return ISM330DHCX_Ok;
}

int32_t ISM330DHCX_ReadAccel(ISM330DHCX_Handle_t *Handle, float *Ax, float *Ay, float *Az)
{
	int32_t ret = ISM330DHCX_Ok;
	int16_t buffer[3] = {0,0,0};
    if(Handle == NULL)
    {
    	return ISM330DHCX_ParamError;
    }

    //Read Status register
    ret = ISM330DHCX_ReadReg(&Handle->Context, ISM330DHCX_REG_STATUS_REG, (uint8_t*)buffer, 1);
    if(ret != ISM330DHCX_Ok)
    {
        return ret;
    }
    //Check the XL Bit
    if(!(buffer[0] & XL_DATA_MASK))
    {
        return ISM330DHCX_DataNotReady;
    }

    //Read the data
    ret = ISM330DHCX_ReadReg(&Handle->Context, ISM330DHCX_REG_OUTX_L_A, (uint8_t*)buffer, 6);
    if(ret != ISM330DHCX_Ok)
    {
    	return ret;
    }
    //Convert the data to Gs
    ConvertXLData(Handle->AccelResolution, &buffer[0], Ax);
    ConvertXLData(Handle->AccelResolution, &buffer[1], Ay);
    ConvertXLData(Handle->AccelResolution, &buffer[2], Az);
    return ISM330DHCX_DataReady;
}

int32_t ISM330DHCX_ReadGyro(ISM330DHCX_Handle_t *Handle, float *Wx, float *Wy, float *Wz)
{
	int32_t ret = ISM330DHCX_Ok;
	int16_t buffer[3] = {0,0,0};
    if(Handle == NULL)
    {
    	return ISM330DHCX_ParamError;
    }

    //Read Status register
    ret = ISM330DHCX_ReadReg(&Handle->Context, ISM330DHCX_REG_STATUS_REG, (uint8_t*)buffer, 1);
    if(ret != ISM330DHCX_Ok)
    {
        return ret;
    }
    //Check the XL Bit
    if(!(buffer[0] & G_DATA_MASK))
    {
        return ISM330DHCX_DataNotReady;
    }

    //Read the data
    ret = ISM330DHCX_ReadReg(&Handle->Context, ISM330DHCX_REG_OUTX_L_G, (uint8_t*)buffer, 6);
    if(ret != ISM330DHCX_Ok)
    {
    	return ret;
    }
    //Convert Gyro Data to DPS
    ConvertGData(Handle->GyroResolution, &buffer[0], Wx);
    ConvertGData(Handle->GyroResolution, &buffer[1], Wy);
    ConvertGData(Handle->GyroResolution, &buffer[2], Wz);
    return ISM330DHCX_DataReady;
}

int32_t ISM330DHCX_ReadTemp(ISM330DHCX_Handle_t *Handle, float *Data)
{
	int32_t ret = ISM330DHCX_Ok;
	uint8_t buffer[2] = {0,0};
    if(Handle == NULL)
    {
    	return ISM330DHCX_ParamError;
    }

    ret = ISM330DHCX_ReadReg(&Handle->Context, ISM330DHCX_REG_STATUS_REG, buffer, 1);
    if(ret != ISM330DHCX_Ok)
    {
    	return ret;
    }
    //Check the T Bit
    if(!(buffer[0] & T_DATA_MASK))
    {
    	   return ISM330DHCX_DataNotReady;
    }

    ret = ISM330DHCX_ReadReg(&Handle->Context, ISM330DHCX_REG_OUT_TEMP_L, buffer, 2);
    if(ret != ISM330DHCX_Ok)
    {
    	return ret;
    }
    ConvertTData(buffer, Data);
    return ISM330DHCX_DataReady;
}

int32_t ISM330DHCX_Reboot(ISM330DHCX_Handle_t *Handle)
{
    //Put Gyro / Accel In power down mode
	uint32_t ret = ISM330DHCX_Ok;
    uint8_t buffer = 0;
    ret = ISM330DHCX_WriteReg(&Handle->Context, ISM330DHCX_REG_CTRL1_XL, &buffer, 1);
    if(ret != ISM330DHCX_Ok)
    {
    	return ret;
    }

    buffer = 0;
    ret = ISM330DHCX_WriteReg(&Handle->Context, ISM330DHCX_REG_CTRL2_G, &buffer, 1);
    if(ret != ISM330DHCX_Ok)
    {
    	return ret;
    }

	//Set BOOT bit in ctrl 3
    buffer = ISM330DHCX_BOOT_BIT_MSK;
    ret = ISM330DHCX_WriteReg(&Handle->Context, ISM330DHCX_REG_CTRL3_C, &buffer, 1);
    if(ret != ISM330DHCX_Ok)
    {
    	return ret;
    }

	//Wait for boot bit to become 0
    while(buffer & ISM330DHCX_BOOT_BIT_MSK)
    {
        ret = ISM330DHCX_ReadReg(&Handle->Context, ISM330DHCX_REG_CTRL3_C, &buffer, 1);
        if(ret != ISM330DHCX_Ok)
        {
        	return ret;
        }
    }
    return ISM330DHCX_Ok;
}

int32_t ISM330DHCX_Reset(ISM330DHCX_Handle_t *Handle)
{
	//Put Gyro / Accel in power down mode
	uint32_t ret = ISM330DHCX_Ok;
    uint8_t buffer = 0;
    ret = ISM330DHCX_WriteReg(&Handle->Context, ISM330DHCX_REG_CTRL1_XL, &buffer, 1);
    if(ret != ISM330DHCX_Ok)
    {
    	return ret;
    }

    buffer = 0;
    ret = ISM330DHCX_WriteReg(&Handle->Context, ISM330DHCX_REG_CTRL2_G, &buffer, 1);
    if(ret != ISM330DHCX_Ok)
    {
    	return ret;
    }
	//Set SW Reset bit in CTRL3
    buffer = ISM330DHCX_ResetSignal;
    ret = ISM330DHCX_WriteReg(&Handle->Context, ISM330DHCX_REG_CTRL3_C, &buffer, 1);
    if(ret != ISM330DHCX_Ok)
    {
    	return ret;
    }
	//Poll SW Reset bit
    while(buffer & ISM330DHCX_ResetSignal)
    {
        ret = ISM330DHCX_ReadReg(&Handle->Context, ISM330DHCX_REG_CTRL3_C, &buffer, 1);
        if(ret != ISM330DHCX_Ok)
        {
        	return ret;
        }
    }
    return ISM330DHCX_Ok;
}

static int32_t ISM330DHCX_ReadWrapper(void *Handle, uint8_t Reg, uint8_t *Buffer, uint8_t Length)
{
    ISM330DHCX_Handle_t *DevicePtr = (ISM330DHCX_Handle_t*)Handle;
    if(DevicePtr->IO.Read != NULL)
    {
        return DevicePtr->IO.Read(Reg,Buffer,Length);
    }
    return ISM330DHCX_ReadRegErr;
}

static int32_t ISM330DHCX_WriteWrapper(void *Handle, uint8_t Reg, uint8_t *Buffer, uint8_t Length)
{
    ISM330DHCX_Handle_t *DevicePtr = (ISM330DHCX_Handle_t*)Handle;
    if(DevicePtr->IO.Write != NULL)
    {
        return DevicePtr->IO.Write(Reg,Buffer,Length);
    }
    return ISM330DHCX_ReadRegErr;
}

static int32_t ISM330DHCX_RegisterInit(ISM330DHCX_Handle_t *Handle,ISM330DHCX_Init_Struct_t Settings)
{
    int32_t ret = ISM330DHCX_Ok;
    uint8_t buffer[12] = {0};
    ISM330DHCX_Reboot(Handle);
    ISM330DHCX_Reset(Handle);

    buffer[0] = Settings.SDO_PU_EN | 0x3F;
    ret = ISM330DHCX_WriteReg(&Handle->Context, ISM330DHCX_REG_PIN_CTRL, buffer, 1);
    if(ret != ISM330DHCX_Ok)
    {
    	return ret;
    }


    buffer[0] = Settings.WTM & 0x00FF; //WTM Threshold is a 9 bit number
    ret = ISM330DHCX_WriteReg(&Handle->Context, ISM330DHCX_REG_FIFO_CTRL1, buffer, 1);
    if(ret != ISM330DHCX_Ok)
    {
    	return ret;
    }


    if(Settings.WTM > 0x00FF)
    {
    	buffer[0] = Settings.STOP_ON_WTM | 0x01;
    } else
    {
    	buffer[0] = Settings.STOP_ON_WTM;
    }
    ret = ISM330DHCX_WriteReg(&Handle->Context, ISM330DHCX_REG_FIFO_CTRL2, buffer, 1);
    if(ret != ISM330DHCX_Ok)
    {
    	return ret;
    }


    buffer[0] = Settings.BDR_GY | Settings.BDR_XL;
    ret = ISM330DHCX_WriteReg(&Handle->Context, ISM330DHCX_REG_FIFO_CTRL3, buffer, 1);
    if(ret != ISM330DHCX_Ok)
    {
    	return ret;
    }


    buffer[0] = Settings.DEC_TS_BATCH | Settings.ODR_T_BATCH | Settings.FIFO_MODE;
    ret = ISM330DHCX_WriteReg(&Handle->Context, ISM330DHCX_REG_FIFO_CTRL4, buffer, 1);
    if(ret != ISM330DHCX_Ok)
    {
    	return ret;
    }


    buffer[0] = Settings.CNT_BDR_TH & 0x00FF;
    ret = ISM330DHCX_WriteReg(&Handle->Context, ISM330DHCX_REG_COUNTER_BDR_REG2, buffer, 1);
    if(ret != ISM330DHCX_Ok)
    {
    	return ret;
    }


    buffer[0] = ((Settings.CNT_BDR_TH & 0x0700) >> 8) | Settings.dataready_pulsed | Settings.TRIG_COUNTER_BDR;
    ret = ISM330DHCX_WriteReg(&Handle->Context, ISM330DHCX_REG_COUNTER_BDR_REG1, buffer, 1);
    if(ret != ISM330DHCX_Ok)
    {
    	return ret;
    }


    buffer[0] = Settings.INT1_CTRL;
    ret = ISM330DHCX_WriteReg(&Handle->Context, ISM330DHCX_REG_INT1_CTRL, buffer, 1);
    if(ret != ISM330DHCX_Ok)
    {
    	return ret;
    }


    buffer[0] = Settings.INT2_CTRL;
    ret = ISM330DHCX_WriteReg(&Handle->Context, ISM330DHCX_REG_INT2_CTRL, buffer, 1);
    if(ret != ISM330DHCX_Ok)
    {
    	return ret;
    }


    buffer[0] = 0x40 | Settings.PP_OD | Settings.H_LACTIVE | Settings.SIM | Settings.IF_INC;
    ret = ISM330DHCX_WriteReg(&Handle->Context, ISM330DHCX_REG_CTRL3_C, buffer, 1);
    if(ret != ISM330DHCX_Ok)
    {
    	return ret;
    }


    buffer[0] = Settings.TriggerMode | Settings.XL_HM_MODE | Settings.USR_OFF_W | Settings.FTYPE;
    ret = ISM330DHCX_WriteReg(&Handle->Context, ISM330DHCX_REG_CTRL6_C, buffer, 1);
    if(ret != ISM330DHCX_Ok)
    {
    	return ret;
    }


    buffer[0] = Settings.G_HM_MODE | Settings.HP_EN_G | Settings.HPM_G | Settings.USR_OFF_ON_OUT;
    ret = ISM330DHCX_WriteReg(&Handle->Context, ISM330DHCX_REG_CTRL7_G, buffer, 1);
    if(ret != ISM330DHCX_Ok)
    {
    	return ret;
    }


    buffer[0] = Settings.HPCF_XL | Settings.HP_REF_MODE_XL | Settings.FASTSETTL_MODE_XL | Settings.HP_SLOPE_XL_EN;
    ret = ISM330DHCX_WriteReg(&Handle->Context, ISM330DHCX_REG_CTRL8_XL, buffer, 1);
    if(ret != ISM330DHCX_Ok)
    {
    	return ret;
    }


    buffer[0] = Settings.DEN_X | Settings.DEN_Y | Settings.DEN_Z | Settings.DEN_XL_G | Settings.DEN_XL_EN | Settings.DEN_LH;
    ret = ISM330DHCX_WriteReg(&Handle->Context, ISM330DHCX_REG_CTRL9_XL, buffer, 1);
    if(ret != ISM330DHCX_Ok)
    {
    	return ret;
    }


    buffer[0] = Settings.TIMESTAMP_EN;
    ret = ISM330DHCX_WriteReg(&Handle->Context, ISM330DHCX_REG_CTRL10_C, buffer, 1);
    if(ret != ISM330DHCX_Ok)
    {
    	return ret;
    }


    buffer[0] = Settings.ODR_XL | Settings.FS_XL | Settings.LPF2_XL_EN;
    ret = ISM330DHCX_WriteReg(&Handle->Context, ISM330DHCX_REG_CTRL1_XL, buffer, 1);
    if(ret != ISM330DHCX_Ok)
    {
    	return ret;
    }


    buffer[0] = Settings.ODR_G | Settings.FS_G;
    ret = ISM330DHCX_WriteReg(&Handle->Context, ISM330DHCX_REG_CTRL2_G, buffer, 1);
    if(ret != ISM330DHCX_Ok)
    {
    	return ret;
    }

    ret = ISM330DHCX_ReadReg(&Handle->Context, ISM330DHCX_REG_OUTX_L_G, buffer, 1);
    //Discard Data that was collected during configuration
    ret = ISM330DHCX_ReadReg(&Handle->Context, ISM330DHCX_REG_OUTX_L_G, buffer, 12);
    if(ret != ISM330DHCX_Ok)
    {
    	return ret;
    }
    Handle->Status = ISM330DHCX_Initialized;
    return ISM330DHCX_Ok;
}

static void ConvertXLData(ISM330DHCX_AccelFullscale_t AccelResolution, int16_t *buffer, float *data)
{
      float ConversionFactor = 0;
      if(AccelResolution == ISM330DHCX_XL_2g)
      {
    	  ConversionFactor = 0.061;
      }else if(AccelResolution == ISM330DHCX_XL_16g)
	  {
    	  ConversionFactor = 0.488;
	  }else if(AccelResolution == ISM330DHCX_XL_8g)
	  {
		  ConversionFactor = 0.244;
	  }else //4g
	  {
		  ConversionFactor = 0.122;
	  }
      *data = (ConversionFactor * (*buffer)) / 1000.0; //Div by 1000 to convert g, not mg
}

static void ConvertGData(ISM330DHCX_GyroFullScale_t GyroResolution, int16_t *buffer, float *data)
{

      float ConversionFactor = 0;
      if(GyroResolution == ISM330DHCX_G_125DPS)
      {
    	  ConversionFactor = 0.004375;
      }else if(GyroResolution == ISM330DHCX_G_250DPS)
	  {
    	  ConversionFactor = 0.00875;
	  }else if(GyroResolution == ISM330DHCX_G_500DPS)
	  {
		  ConversionFactor = 0.0175;
	  }else if(GyroResolution == ISM330DHCX_G_1000DPS)
	  {
		  ConversionFactor = 0.035;
	  }else if(GyroResolution == ISM330DHCX_G_2000DPS)
	  {
		  ConversionFactor = 0.070;
	  }else //4000DPS
	  {
		  ConversionFactor = 0.140;
	  }
      *data = ConversionFactor * (*buffer) / 1000.0; //Div by 1000 to convert to DPS from mDPS
}

static void ConvertTData(uint8_t *buffer, float *data)
{
	int16_t DataSigned = ((int16_t)buffer[1] << 8) | buffer[0];
	*data = (DataSigned / 256.0f) + 25.0;
}

int32_t ISM330DHCX_GetAccelPeriod(ISM330DHCX_Handle_t *Handle, uint32_t *Period)
{
    if(Handle->Status != ISM330DHCX_Initialized)
    {
    	return ISM330DHCX_InitError;
    }

    switch(Handle->AccelDataRate)
    {
    case(ISM330DHCX_XL_PowerDown):
    		*Period = 0;
            return ISM330DHCX_PeriodOneShot;
    		break;
    case(ISM330DHCX_XL_1_6Hz):
    		*Period = 1000 / 1.6;
    		break;
    case(ISM330DHCX_XL_12_5Hz):
    		*Period = 1000 / 12.5;
    		break;
    case(ISM330DHCX_XL_26Hz):
    		*Period = 1000 / 26;
    		break;
    case(ISM330DHCX_XL_52Hz):
    		*Period = 1000 / 52;
    		break;
    case(ISM330DHCX_XL_104Hz):
    		*Period = 1000 / 104;
    		break;
    case(ISM330DHCX_XL_208Hz):
    		*Period = 1000 / 208;
    		break;
    case(ISM330DHCX_XL_416Hz):
    		*Period = 1000 / 416;
    		break;
    case(ISM330DHCX_XL_833Hz):
    		*Period = 1000 / 833;
    		break;
    default:
    	*Period = 0;
    	return ISM330DHCX_SubMilliPeriod;

    }

    return ISM330DHCX_Ok;
}

int32_t ISM330DHCX_GetGyroPeriod(ISM330DHCX_Handle_t *Handle, uint32_t *Period)
{
    if(Handle->Status != ISM330DHCX_Initialized)
    {
    	return ISM330DHCX_InitError;
    }

    switch(Handle->GyroDataRate)
    {
    case(ISM330DHCX_GyroPowerDown):
    		*Period = 0;
            return ISM330DHCX_PeriodOneShot;
    		break;
    case(ISM330DHCX_G_12_5Hz):
    		*Period = 1000 / 12.5;
    		break;
    case(ISM330DHCX_G_26Hz):
    		*Period = 1000 / 26;
    		break;
    case(ISM330DHCX_G_52Hz):
    		*Period = 1000 / 52;
    		break;
    case(ISM330DHCX_G_104Hz):
    		*Period = 1000 / 104;
    		break;
    case(ISM330DHCX_G_208Hz):
    		*Period = 1000 / 208;
    		break;
    case(ISM330DHCX_G_416Hz):
    		*Period = 1000 / 416;
    		break;
    case(ISM330DHCX_G_833Hz):
    		*Period = 1000 / 833;
    		break;
    default:
    	*Period = 0;
    	return ISM330DHCX_SubMilliPeriod;

    }
    return ISM330DHCX_Ok;
}

