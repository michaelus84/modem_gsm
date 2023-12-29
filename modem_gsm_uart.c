#include <stdlib.h>

#include "modem_gsm_uart.h"
#include "modem_gsm_def.h"
#include "common.h"
#include "serialport.h"

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje prprocesora - lokalna
*/
/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje typow lokalnych
*/


/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje funkcji wewnetrznych
*/


/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje stalych
*/

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje zmiennych
*/


/*
-------------------------------------------------------------------------------------------------------------------------------------------
Funkcje
*/

/**
 * @brief Funkcja konfigurujaca port uart
 *
 * @param handle
 * @param baudrate
 * @param comport
 */
void UartConfig(UartParametersTypedef * handle, uint32_t baudrate, char * comport)
{
  handle->fd = SerialPortConfig(baudrate, comport);
}

/**
 * @brief Construct a new Serial Port Close object
 *
 * @param handle
 */
void UartClose(UartParametersTypedef * handle)
{
   SerialPortClose(handle->fd);
}

/**
 * @brief Funkcja wysylajaca po uart
 *
 * @param handle
 * @param data
 * @param len
 * @return uint8_t
 */
uint8_t UartWrite(UartParametersTypedef * handle, uint8_t * data, uint32_t len)
{
#if defined(DEBUG)
  for (uint32_t i = 0; i < len; i++)
  {
    printf("%c", (char)data[i]);
  }
  printf("\n");
#endif
  SerialPortWrite(handle->fd, data, len);

  return RETURN_SUCCESS;
}

/**
 * @brief Funkcja przygotowujaca odbierane danych
 *
 * @param handle
 * @param data
 * @param len
 * @param timeout
 */
void UartReadInit(UartParametersTypedef * handle, uint8_t * data, uint32_t len, uint32_t timeout)
{
  handle->control.rx_buffer_len = len;
  handle->control.rx_buffer_ptr = data;
  handle->control.rx_index = 0;
  handle->control.rx_len = 0;
}

/**
 * @brief Odbior danych i zapis do bufora okreznego
 */
void UartRecieve(UartParametersTypedef * handle)
{
  uint16_t i;
  uint16_t len;
  uint8_t uart_buffer[2048];

  len = SerialPortRecieve(handle->fd, uart_buffer, sizeof(uart_buffer) - 1);

  if (len > 0)
  {
    for (i = 0; i < len; i++)
    {
      handle->control.rx_buffer_ptr[handle->control.rx_index] = uart_buffer[i];
      _DP("%c", uart_buffer[i]);
      IncrementIndex(&handle->control.rx_index, 1, handle->control.rx_buffer_len);
    }
    handle->control.rx_len += len;
  }
}
