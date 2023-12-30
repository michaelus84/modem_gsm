#ifndef _SERIALPORT_H_INCLUDED
#define _SERIALPORT_H_INCLUDED

#include <stdint.h>

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje preprocesora
*/
#if defined(DEBUG)
  #define _SerialPortDebugPrintf(...)                   printf(__VA_ARGS__)
#else
  #define _SerialPortDebugPrintf(...)
#endif

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

int SerialPortConfig(uint32_t baudrate, char * comport); 
void SerialPortClose(int fd);
uint8_t SerialPortWrite(int fd, uint8_t * data, uint32_t len);
uint16_t SerialPortRecieve(int fd, uint8_t * buffer, uint16_t max_len);

#endif // _SERIALPORT_H_INCLUDED
