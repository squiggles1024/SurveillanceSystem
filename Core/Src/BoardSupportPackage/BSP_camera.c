/*
 * camera.c
 *
 *  Created on: Sep 15, 2022
 *      Author: evanl
 */

#include "BSP_camera.h"
#include "BSP_IOBus.h"
#include "../ExternalHardware/OV5640.h"
#include "gpio.h"
#include "gpdma.h"
#include "dcmi.h"

#define CAMERA_OV5640_ADDRESS          0x78U
#define CAMERA_PF OV5640_RGB565

extern DMA_QListTypeDef ProjectDMAQueue;

static void BSP_CameraHwReset(void);
static void BSP_CameraHWInit(void);

BSP_CameraStatus_t BSP_CameraInit(void)
{
	MX_GPDMA1_Init();
	MX_DCMI_Init();
	BSP_CameraHwReset();//Reset it
    BSP_CameraHWInit(); //Init OV5460 driver
	return CameraOK;
}

void BSP_CameraStart(uint8_t* Buffer){
	  HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)Buffer, CAMERA_DATA_SIZE);
}

void BSP_CameraStop(void){
	  HAL_DCMI_Stop(&hdcmi);
}

static void BSP_CameraHwReset(void)
{
    /* Camera sensor RESET sequence */
    /* Assert the camera STANDBY and RSTI pins */

    HAL_GPIO_WritePin(CAM_XSDN_GPIO_Port, CAM_XSDN_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(CAM_RSTI_GPIO_Port, CAM_RSTI_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);   /* RST and XSDN signals asserted during 100ms */

    /* De-assert the camera STANDBY pin (active high) */
    HAL_GPIO_WritePin(CAM_XSDN_GPIO_Port, CAM_XSDN_Pin, GPIO_PIN_RESET);
    HAL_Delay(3);     /* RST de-asserted and XSDN de-asserted during 3ms */

    /* De-assert the camera RSTI pin (active low) */
    HAL_GPIO_WritePin(CAM_RSTI_GPIO_Port, CAM_RSTI_Pin, GPIO_PIN_SET);
    HAL_Delay(20);     /* RST de-asserted during 20ms */
}

static void BSP_CameraHWInit(void){
	  uint32_t temp = 0;
	  OV5640_IO_t              IO;
	  static OV5640_Handle_t   OV5640_Handle;
	  IO.Address = CAMERA_OV5640_ADDRESS;
	  IO.Init = BSP_I2C1_Init;
	  IO.DeInit = BSP_I2C1_DeInit;
      IO.Read = BSP_I2C1_ReadReg16;
      IO.Write = BSP_I2C1_WriteReg16;
      IO.GetTick = BSP_GetTick;
      OV5640_LinkBus(&OV5640_Handle, &IO);
      OV5640_ReadID(&OV5640_Handle,&temp);
      OV5640_Init(&OV5640_Handle, OV5640_R640x480, CAMERA_PF);
}

void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
    UNUSED(hdcmi);
}

