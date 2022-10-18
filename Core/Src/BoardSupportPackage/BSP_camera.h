/*
 * camera.h
 *
 *  Created on: Sep 15, 2022
 *      Author: evanl
 */

#ifndef SRC_BOARDSUPPORTPACKAGE_BSP_CAMERA_H_
#define SRC_BOARDSUPPORTPACKAGE_BSP_CAMERA_H_
#include <stdint.h>

#define CAMERA_DATA_SIZE              ((620 * 480 * 2) / 4) //800 x 600 resolution, 3 bytes per pixel, 1 word per 4 bytes

typedef enum {
    CameraOK = 0,
	CameraParamErr = 1,
	CameraPeripheralCommErr = 2,
	CameraTimeoutErr = 4
}BSP_CameraStatus_t;

typedef struct{
 int test;
}CameraHandle_t;

BSP_CameraStatus_t BSP_CameraInit(void);
void BSP_CameraStart(uint8_t* Buffer);
void BSP_CameraStop(void);

#endif /* SRC_BOARDSUPPORTPACKAGE_BSP_CAMERA_H_ */
