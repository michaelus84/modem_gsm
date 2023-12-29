#include "platform.h"
#include <linux/gpio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <stdlib.h>
#if defined(WIRING_PI)
#include <wiringPi.h>
#endif
#include "../modem_gsm_def.h"
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
 * @brief 
 * 
 */
uint8_t GpioInit(void * gpio, uint32_t pin, uint8_t dir)
{
  #if defined(WIRING_PI)
  if (wiringPiSetup() < 0)
    return RETURN_FAILURE;
  pinMode (PWR_KEY, OUTPUT);
  #endif
  return RETURN_SUCCESS;
}

/**
 * @brief 
 * 
 * @param pin 
 * @param value 
 */
void GpioWrite(void * gpio, uint32_t pin, uint8_t value)
{
  #if defined(WIRING_PI)
  digitalWrite(pin, value);
  #else
  struct gpiohandle_request request;
  struct gpiohandle_data data;

  int fd;

  fd = open((char *)gpio, O_RDONLY);

  if (fd < 0)
  {
    _DP("%s: open gpio failed\n", __func__);
    return;
  }

  request.lineoffsets[0] = pin;
  request.flags = GPIOHANDLE_REQUEST_OUTPUT;
  request.lines = 1;

  if (ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &request) < 0)
  {
    _DP("%s: config gpio failed\n", __func__);
    close(request.fd);
    return;
  }

  data.values[0] = value;
  if (ioctl(request.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) < 0)
  {
    _DP("%s: write gpio failed\n", __func__);
  }

  close(request.fd);
  #endif
}

/**
 * @brief 
 * 
 * @param pin 
 * @return uint8_t 
 */
uint8_t GpioRead(void * gpio, uint32_t pin)
{
  #if defined(WIRING_PI)
  return digitalRead(pin);
  #else
  struct gpiohandle_request request;
  struct gpiohandle_data data;

  int fd;

  fd = open((char *)gpio, O_RDONLY);

  if (fd < 0)
  {
    _DP("%s: open gpio failed\n", __func__);
    return 0;
  }

  request.lineoffsets[0] = pin;
  request.flags = GPIOHANDLE_REQUEST_INPUT;
  request.lines = 1;

  if (ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &request) < 0)
  {
    _DP("%s: config gpio failed\n", __func__);
    close(fd);
    return 0;
  }

  if (ioctl(request.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data) < 0)
  {
    _DP("%s: read gpio failed\n", __func__);
    data.values[0] = 0;
  };

  close(request.fd);

  return data.values[0];
  #endif
}

/**
 * @brief Funkcja zwracajaca licznik milisekund
 *
 * @return uint32_t
 */
uint32_t GetTick(void)
{
  struct timeval tp;
  gettimeofday(&tp, NULL);
  uint32_t ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
  return ms;
}
