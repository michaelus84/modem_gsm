#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <stdlib.h>

#include "uart.h"
#include "def.h"
#include "common.h"

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
void SerialPortConfig(UartParametersTypedef * handle, uint32_t baudrate, char * comport)
{
  struct termios config;
  int fd;

  if (handle->fd) return;

  fd = open(comport, O_RDWR | O_NOCTTY | O_NONBLOCK | O_SYNC);

  if (fd < 0)
  {
    printf("Open port error\n");
    exit(-1);
  }

  _DebugPrintf("\nPort open\n");

  handle->fd = fd;

  config.c_cflag = baudrate | CS8 | CLOCAL | CREAD;

  config.c_iflag = IGNPAR;
  config.c_oflag = 0;
  config.c_lflag = 0;//ICANON;
  config.c_cc[VMIN] = 10;
  config.c_cc[VTIME] = 10;

  tcsetattr(fd,TCSANOW, &config);
  tcflush(fd, TCIFLUSH);
}

/**
 * @brief Construct a new Serial Port Close object
 * 
 * @param handle 
 */
void SerialPortClose(UartParametersTypedef * handle)
{
  _DebugPrintf("\nPort closed\n");
  close(handle->fd);
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
  if (write(handle->fd, data, len) < 0) return RETURN_FAILURE;

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

  ioctl(handle->fd, FIONREAD, &len);

  if (len > 0)
  {
    len = read(handle->fd, uart_buffer, sizeof(uart_buffer) - 1);

    for (i = 0; i < len; i++)
    {
      handle->control.rx_buffer_ptr[handle->control.rx_index] = uart_buffer[i];
      _DebugPrintf("%c", uart_buffer[i]);
      IncrementIndex(&handle->control.rx_index, 1, handle->control.rx_buffer_len);
    }
    handle->control.rx_len += len;
  }
}
