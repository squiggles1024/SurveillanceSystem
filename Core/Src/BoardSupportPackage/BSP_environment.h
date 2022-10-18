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

#endif /* SRC_BOARDSUPPORTPACKAGE_BSP_ENVIRONMENT_H_ */
