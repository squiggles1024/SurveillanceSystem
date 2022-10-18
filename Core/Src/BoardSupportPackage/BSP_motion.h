/*
 * BSP_motion.h
 *
 *  Created on: Sep 16, 2022
 *      Author: evanl
 */

#ifndef SRC_BOARDSUPPORTPACKAGE_BSP_MOTION_H_
#define SRC_BOARDSUPPORTPACKAGE_BSP_MOTION_H_
#include <stdint.h>

int32_t BSP_MotionSensorInit(void);
int32_t BSP_ReadAccelXYZ(float *x, float *y, float *z);
int32_t BSP_ReadGyroXYZ(float *x, float *y, float *z);
int32_t BSP_ReadAccelX(float *Ax);
int32_t BSP_ReadGyroX(float *Wx);
int32_t BSP_ReadAccelY(float *Ay);
int32_t BSP_ReadGyroY(float *Wy);
int32_t BSP_ReadAccelZ(float *Az);
int32_t BSP_ReadGyroZ(float *Wz);
int32_t BSP_ReadTemperature(float *temp);

#endif /* SRC_BOARDSUPPORTPACKAGE_BSP_MOTION_H_ */
