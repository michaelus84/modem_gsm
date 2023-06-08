/*
 * common.h
 *
 *  Created on: 16.03.2022
 *      Author: Michal Skiwski (m.skiwski@microbotic.pl)
 */

#ifndef __COMMON_H
#define __COMMON_H

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje prprocesora
*/


/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje typow
*/

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje zmiennych
*/

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje Funkcje
*/

void IncrementIndex(uint16_t * index_ptr, uint16_t offset, uint16_t range);
uint8_t GpioInit(void);
void GpioWrite(void * gpio, uint32_t pin, uint8_t value);
uint8_t GpioRead(void * gpio, uint32_t pin);
uint32_t GetTick(void);

#endif /* __CMMON_H */