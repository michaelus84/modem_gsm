#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include "serialport.h"
#include "../modem_gsm_def.h"
#include <termio.h>

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
uint32_t const Baud_Table[62][2] =
{
  {B50     , 50     },
  {B75     , 75     },
  {B110    , 110    },
  {B134    , 134    },
  {B150    , 150    },
  {B200    , 200    },
  {B300    , 300    },
  {B600    , 600    },
  {B1200   , 1200   },
  {B1800   , 1800   },
  {B2400   , 2400   },
  {B4800   , 4800   },
  {B9600   , 9600   },
  {B19200  , 19200  },
  {B38400  , 38400  },
  {B57600  , 57600  },
  {B115200 , 115200 },
  {B230400 , 230400 },
  {B460800 , 460800 },
  {B500000 , 500000 },
  {B576000 , 576000 },
  {B921600 , 921600 },
  {B1000000, 1000000},
  {B1152000, 1152000},
  {B1500000, 1500000},
  {B2000000, 2000000},
  {B2500000, 2500000},
  {B3000000, 3000000},
  {B3500000, 3500000},
  {B4000000, 4000000},
};

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
  uint32_t baud = 0;

  int fd;

  fd = open(comport, O_RDWR | O_NOCTTY | O_NONBLOCK | O_SYNC);

  if (fd < 0)
  {
    printf("Open port error\n");
    exit(-1);
  }

  _SerialPortDebugPrintf("\nPort open\n");

  for (uint8_t i = 0; i < _NumOfRows(Baud_Table); i++)
  {
    if (baudrate == Baud_Table[i][1])
    {
      baud = Baud_Table[i][0];
      break;
    }
  }

  if (!baud)
  {
    printf("\nWrong baudrate\n");
    exit(-1);
  }

  config.c_cflag = baud | CS8 | CLOCAL | CREAD;

  config.c_iflag = IGNPAR;
  config.c_oflag = 0;
  config.c_lflag = 0;//ICANON;
  config.c_cc[VMIN] = 10;
  config.c_cc[VTIME] = 10;

  tcsetattr(fd,TCSANOW, &config);
  tcflush(fd, TCIFLUSH);

  return fd;
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
  if (write(fd, data, len) < 0)
  {
    return RETURN_FAILURE;
  }

  return RETURN_SUCCESS;
}

/**
 * @brief Odbior danych z portu szeregowego
 * 
 * @param fd 
 * @param buffer 
 * @param max_len 
 */
uint16_t SerialPortRecieve(int fd, uint8_t * buffer, uint16_t max_len)
{
  uint16_t len;
  int ret;

  ret = ioctl(fd, FIONREAD, &len);
  if (ret < 0)
  {
    printf("Get number of bytes error\n");
  }

  if (len > 0)
  {
    len = read(fd, buffer, max_len);
  }

  return len;
}
