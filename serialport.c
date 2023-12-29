#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <termio.h>

#include "serialport.h"

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje preprocesora - lokalna
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
 * @brief Funkcja konfigurujaca port szeregowy
 * 
 * @param baudrate 
 * @param comport 
 * @return int 
 */


int SerialPortConfig(uint32_t baudrate, char * comport)
{
  struct termios config;

  fd = open(comport, O_RDWR | O_NOCTTY | O_NONBLOCK | O_SYNC);

  if (fd < 0)
  {
    printf("Open port error\n");
    exit(-1);
  }

  _SerialPortDebugPrintf("\nPort open\n");

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
 * @brief 
 * 
 * @param fd 
 */
 
void SerialPortClose(int fd)
{
  _SerialPortDebugPrintf("\nPort closed\n");
  close(fd);
}

/**
 * @brief Funkcja wysylajaca przez port szeregowy
 * 
 * @param fd 
 * @param data 
 * @param len 
 * @return uint8_t 
 */
 
uint8_t SerialPortWrite(int fd, uint8_t * data, uint32_t len)
{
#if defined(UART_DEBUG)
  for (uint32_t i = 0; i < len; i++)
  {
    printf("%c", (char)data[i]);
  }
  printf("\n");
#endif
  if (write(fd, data, len) < 0) return RETURN_FAILURE;

  return RETURN_SUCCESS;
}

/**
 * @brief Odbior danych z portu szeregowego
 * 
 * @param fd 
 * @param buffer 
 * @param max_len 
 */
int SerialPortRecieve(int fd, uint8_t buffer, uint16_t max_len)
{
  uint16_t i;
  uint16_t len;

  ioctl(fd, FIONREAD, &len);

  if (len > 0)
  {
    len = read(fd, buffer, max_len - 1);
  }

  return len;
}
