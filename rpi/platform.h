#ifndef _PLATFORM_H_INCLUDED
#define _PLATFORM_H_INCLUDED

#include <stdint.h>

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje preprocesora
*/

//#define WIRING_PI

#define PWR_KEY_PIN                           26
#define PWR_KEY_GPIO                         "/dev/gpiochip0"

#define MODEM_UART                           "/dev/ttyS0"

#define GPIO_OUTPUT                          1
#define GPIO_INPUT                           0

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

uint8_t GpioInit(void * gpio, uint32_t pin, uint8_t dir, char * label);
void GpioWrite(void * gpio, uint32_t pin, uint8_t value);
uint8_t GpioRead(void * gpio, uint32_t pin);
void GpioCloseAll(void);
uint32_t GetTick(void);

#endif // _PLATFORM_H_INCLUDED
