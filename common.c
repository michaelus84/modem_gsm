#include "def.h"
#include <linux/gpio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <stdlib.h>


/**
 * Funkcja od zwiekszenia indeksu bufora
 *
 * @param index_ptr - wskaznik na indeks
 * @param pin    - o ile przesunac
 * @param range     - zakres bufora
 *
 */
void IncrementIndex(uint16_t * index_ptr, uint16_t pin, uint16_t range)
{
  uint32_t new_index;

  new_index = (uint32_t) *index_ptr + pin;
  // sprawdz, czy wynik miesci sie w zakresie wielkosci bufora
  if (new_index >= range) new_index -= range;
  // zapisz wynik
  *index_ptr = new_index;
}

/**
 * @brief 
 * 
 * @param pin 
 * @param value 
 */
void GpioWrite(void * gpio, uint32_t pin, uint8_t value)
{
  struct gpiohandle_request request;
  struct gpiohandle_data data;

  int fd;

  fd = open((char *)gpio, O_RDONLY);

  if (fd < 0)
  {
    return;
  }

  request.lineoffsets[0] = pin;
  request.flags = GPIOHANDLE_REQUEST_OUTPUT;
  request.lines = 1;

  if (ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &request) < 0)
  {
    close(request.fd);
    return;
  }

  data.values[0] = value;
  ioctl(request.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);

  close(request.fd);
}

/**
 * @brief 
 * 
 * @param pin 
 * @return uint8_t 
 */
uint8_t GpioRead(void * gpio, uint32_t pin)
{
  struct gpiohandle_request request;
  struct gpiohandle_data data;

  int fd;

  fd = open((char *)gpio, O_RDONLY);

  if (fd < 0)
  {
    return 0;
  }

  request.lineoffsets[0] = pin;
  request.flags = GPIOHANDLE_REQUEST_INPUT;
  request.lines = 1;

  if (ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &request) < 0)
  {
    close(fd);
    return 0;
  }

  if (ioctl(request.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data) < 0)
  {
    data.values[0] = 0;
  };

  close(request.fd);

  return data.values[0];
}
