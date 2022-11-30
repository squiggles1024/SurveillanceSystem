/*
 * camera.h
 *
 *  Created on: Sep 15, 2022
 *      Author: evanl
 */

#ifndef SRC_BOARDSUPPORTPACKAGE_BSP_CAMERA_H_
#define SRC_BOARDSUPPORTPACKAGE_BSP_CAMERA_H_
#include <stdint.h>

#define CAMERA_DATA_SIZE_BYTES             (320*240 * 2)
#define CAMERA_DATA_SIZE_WORDS             (CAMERA_DATA_SIZE_BYTES / 4)
#define CAMERA_FRAMEBUFFER1_ADDR           (0x90000000U)
#define CAMERA_FRAMEBUFFER2_ADDR           (0x90025800U)//((CAMERA_FRAMEBUFFER1_ADDR + CAMERA_DATA_SIZE_BYTES))

typedef enum {
    CameraOK = 0,
	CameraParamErr = 1,
	CameraPeripheralCommErr = 2,
	CameraTimeoutErr = 4
}BSP_CameraStatus_t;


BSP_CameraStatus_t BSP_CameraInit(void);
void BSP_CameraStart(uint8_t* Buffer);
void BSP_CameraStop(void);
void BSP_CameraCaptureFrame(uint8_t* Buffer);

#endif /* SRC_BOARDSUPPORTPACKAGE_BSP_CAMERA_H_ */
