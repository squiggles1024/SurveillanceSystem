/*
 * BSP_ram.c
 *
 *  Created on: Sep 16, 2022
 *      Author: evanl
 */


#include "BSP_ram.h"
#include "gpio.h"
#include "octospi.h"
#include <stdint.h>



void BSP_RamInit(void){
    Ram_GPIO_Init();
    OSPI_RamInit();
    OSPI_DelayBlockConfig();
    OSPI_EnableMemMappedMode();
}

void BSP_RamErase(void){
	  uint32_t *pRam = (uint32_t*)OSPI1_RAM_BASE;
	  for(uint32_t i = 0; i < (OSPI1_RAM_LENGTH / 4); i = i + 1){
		  *(pRam + i) = 0;
	  }
}

/*
void BSP_RamTest(void){
    uint32_t test32b_write = 0xDEADBEEF;
    uint32_t test32b_read = 0;
    uint32_t *ram_ptr32b = (uint32_t*)(OSPI1_RAM_BASE);
    *ram_ptr32b = test32b_write;
    test32b_read = *ram_ptr32b;

    uint8_t test8b_write = 0xD1;
    uint8_t test8b_read = 0;
    uint8_t *ram_ptr8b = (uint8_t*)(OSPI1_RAM_BASE + OSPI1_RAM_LENGTH - 1);
    *ram_ptr8b = test8b_write;
    test8b_read = *ram_ptr8b;
}
*/
