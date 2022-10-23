/*
 * BSP_environment.h
 *
 *  Created on: Sep 16, 2022
 *      Author: evanl
 */

#ifndef SRC_BOARDSUPPORTPACKAGE_BSP_ENVIRONMENT_H_
#define SRC_BOARDSUPPORTPACKAGE_BSP_ENVIRONMENT_H_
#include <stdint.h>

int32_t BSP_TempHumSensorInit(void);
int32_t BSP_ReadTemperature(float *result);
int32_t BSP_ReadHumidity(float *result);
int32_t BSP_EnableHeater(void);
int32_t BSP_DisableHeater(void);

int32_t BSP_PressureSensorInit(void);
int32_t BSP_ReadPressure(float *result);
//int32_t BSP_pressuretest(void);

int32_t BSP_MagnetometerInit(void);
int32_t BSP_ReadMagnetometerXYZ(float *Mx, float *My, float *Mz);
int32_t BSP_ReadMagnetometerX(float *Mx);
int32_t BSP_ReadMagnetometerY(float *My);
int32_t BSP_ReadMagnetometerZ(float *Mz);

#endif /* SRC_BOARDSUPPORTPACKAGE_BSP_ENVIRONMENT_H_ */
