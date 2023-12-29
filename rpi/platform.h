#ifndef _PLATFORM_H_INCLUDED
#define _PLATFORM_H_INCLUDED

#include <stdint.h>

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje preprocesora
*/

#define WIRING_PI

#define PWR_KEY                              0
#define PWR_KEY_GPIO                         NULL

#define MODEM_UART                           "/dev/ttyS0"
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

uint8_t GpioInit(void);
void GpioWrite(void * gpio, uint32_t pin, uint8_t value);
uint8_t GpioRead(void * gpio, uint32_t pin);
uint32_t GetTick(void);

#endif // _PLATFORM_H_INCLUDED
