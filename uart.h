#ifndef _UART_H_INCLUDED
#define _UART_H_INCLUDED

#include <termio.h>
#include <stdint.h>

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

typedef struct
{
  uint8_t * rx_buffer_ptr;
  uint16_t rx_buffer_len;
  uint16_t rx_index;
  uint32_t rx_len;
  uint8_t * tx_buffer_ptr;
  uint16_t tx_len;
  uint16_t tx_index;
  uint8_t status;
} TxRxBufferControlTypedef;

typedef enum
{
  ONE_STOP_BIT = 0,
  HALF_STOP_BIT,
  TWO_STOP_BITS,
  ONE_AND_HALF_STOP_BITS
} StopBitsEnum;

typedef enum
{
  UART_8_DATA_BITS = 0,
  UART_9_DATA_BITS,
  UART_7_DATA_BITS,
} DatatBitsEnum;

typedef enum
{
  EVEN_PARITY = 0,
  ODD_PARITY,
  NONE_PARITY
} ParityBitsEnum;

typedef struct
{
  int fd;
  TxRxBufferControlTypedef control;
} UartParametersTypedef;


void SerialPortConfig(UartParametersTypedef * handle, uint32_t baudrate, char * comport);
void SerialPortClose(UartParametersTypedef * handle);
void UartReadInit(UartParametersTypedef * handle, uint8_t * data, uint32_t len, uint32_t timeout);
uint8_t UartWrite(UartParametersTypedef * handle, uint8_t * data, uint32_t len);
void UartRecieve(UartParametersTypedef * handle);

#endif // _UART_H_INCLUDED
