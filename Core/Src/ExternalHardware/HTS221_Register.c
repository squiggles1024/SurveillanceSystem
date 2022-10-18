/*
 * HTS221_Register.c
 *
 *  Created on: Oct 3, 2022
 *      Author: evanl
 */
#include "HTS221_Registers.h"
#include "stddef.h"

static int32_t HTS221_ValidateParams(uint8_t Reg, uint8_t *Buffer, uint8_t Length);

int32_t HTS221_ReadReg(HTS221_Context_t *Context, uint8_t Reg, uint8_t *Buffer, uint8_t Length)
{
	int32_t ret = HTS221_ValidateParams(Reg,Buffer,Length);
	if(ret != HTS221_REG_NO_ERR)
	{
        return ret;
	}

    if(Context->Read != NULL)
    {
        return Context->Read(Context->Handle, Reg, Buffer, Length);
    }

    return HTS221_REG_CONTEXT_ERR;
}

int32_t HTS221_WriteReg(HTS221_Context_t *Context,uint8_t Reg, uint8_t *Buffer, uint8_t Length)
{
	int32_t ret = HTS221_ValidateParams(Reg,Buffer,Length);
	if(ret != HTS221_REG_NO_ERR)
	{
        return ret;
	}

	if(Context->Write != NULL)
	{
	    return Context->Write(Context->Handle, Reg, Buffer, Length);
	}

	return HTS221_REG_CONTEXT_ERR;
}

static int32_t HTS221_ValidateParams(uint8_t Reg, uint8_t *Buffer, uint8_t Length)
{
	uint32_t ret = 0;

	if(Reg < HTS221_REG_WHO_AM_I || Reg > 0x3F)
	{
		 ret |= HTS221_REG_PARAM_ERR;
	}

	if(Buffer == NULL)
	{
		ret |= HTS221_BUFFER_PARAM_ERR;
	}

	if(Length == 0 || Length > (0x3F - HTS221_REG_WHO_AM_I))
	{  //0x3F - 0x0F = 0x30 readable registers
        ret |= HTS221_LENGTH_PARAM_ERR;
	}

	return ret;
}
