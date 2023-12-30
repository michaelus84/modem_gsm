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
static int request_fd[GPIO_V2_LINES_MAX];

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Funkcje
*/

/**
 * @brief 
 * 
 */
uint8_t GpioInit(void * gpio, uint32_t pin, uint8_t dir, char * label)
{
  #if defined(WIRING_PI)
  if (wiringPiSetup() < 0)
    return RETURN_FAILURE;
  pinMode (pin, OUTPUT);
  #else
  struct gpio_v2_line_request request = {0};

  int fd;

  fd = open((const char *)gpio, O_RDONLY);

  if (fd < 0)
  {
    _DP("%s: open gpio failed\n", __func__);
    return RETURN_FAILURE;
  }

  request.offsets[0] = pin;
  strncpy(request.consumer, label, GPIO_MAX_NAME_SIZE);
  if (dir == GPIO_OUTPUT)
  {
    request.config.flags = GPIO_V2_LINE_FLAG_OUTPUT;
  }
  else
  {
    request.config.flags = GPIO_V2_LINE_FLAG_INPUT;
  }
  request.num_lines = 1;

  if (ioctl(fd, GPIO_V2_GET_LINE_IOCTL, &request) < 0)
  {
    _DP("%s: config gpio failed\n", __func__);
    close(request.fd);
    return RETURN_FAILURE;
  }
  close(fd);
  request_fd[pin] = request.fd;
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
  struct gpio_v2_line_values data = {0};

  data.bits = value;
  data.mask = 1;

  if (ioctl(request_fd[pin], GPIO_V2_LINE_SET_VALUES_IOCTL, &data) < 0)
  {
    _DP("%s: write gpio pin %d (%d)failed\n", __func__, pin, request_fd[pin]);
  }
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
  struct gpio_v2_line_values data = {0};

  data.mask = 1;

  if (ioctl(request_fd[pin], GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data) < 0)
  {
    _DP("%s: read gpio failed\n", __func__);
    data.bits = 0;
  };

  return data.bits;
  #endif
}

/**
 * @brief 
 * 
 */
void GpioCloseAll(void)
{
  for (uint8_t i = 0; i < GPIO_V2_LINES_MAX; i++)
  {
    if (request_fd[i] > 0)
    {
      close(request_fd[i]);
    }
  }
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
