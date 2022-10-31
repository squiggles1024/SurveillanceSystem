/*
 * VEML6030_Registers.c
 *
 *  Created on: Oct 27, 2022
 *      Author: evanl
 */
#include "VEML6030_Registers.h"
#include <stdint.h>
#include <stddef.h>


static int32_t VEML6030_ValidateParams(VEML6030_Context_t *Context, uint8_t Reg, uint8_t *Buffer, uint8_t Length)
{
	int32_t ret = VEML6030_REG_OK;
    if(Context == NULL || Context->Read == NULL || Context->Write == NULL || Context->Handle == NULL)
    {
    	ret |= VEML6030_REG_CONTEXT_ERR;
    }

    if(Reg > VEML6030_REG_ALS_INT)
    {
    	ret |= VEML6030_REG_ADDR_ERR;
    }

    if(Buffer == NULL)
    {
    	ret |= VEML6030_REG_BUFFER_ERR;
    }

    if(Length == 0)
    {
       ret |= VEML6030_REG_LENGTH_ERR;
    }

    return ret;
}

int32_t VEML6030_ReadReg(VEML6030_Context_t *Context,uint8_t Reg, uint8_t *Buffer, uint8_t Length)
{
    int32_t ret = VEML6030_ValidateParams(Context,Reg,Buffer,Length);
    if(ret != VEML6030_REG_OK)
    {
    	return ret;
    }
    ret = Context->Read(Context->Handle,Reg,Buffer,Length);
    return ret;
}

int32_t VEML6030_WriteReg(VEML6030_Context_t *Context,uint8_t Reg, uint8_t *Buffer, uint8_t Length)
{
    int32_t ret = VEML6030_ValidateParams(Context,Reg,Buffer,Length);
    if(ret != VEML6030_REG_OK)
    {
    	return ret;
    }
    ret = Context->Write(Context->Handle,Reg,Buffer,Length);
    return ret;
}
