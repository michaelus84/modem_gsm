/*
 * def.h
 *
 *  Created on: 15.03.2022
 *      Author: Michal Skiwski (m.skiwski)
 */

#ifndef __DEF_H
#define __DEF_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje prprocesora
*/

#if !defined(FALSE)
#define FALSE                                 0
#endif

#if !defined(TRUE)
#define TRUE                                  1
#endif

#define IRQ_PIN                               3

#define PWR_KEY                               18
#define PWR_KEY_GPIO                          "/dev/gpiochip0"

#define MODEM_UART                            "/dev/ttyS0"

#if defined(DEBUG)
  #define _DebugPrintf(...)                   printf(__VA_ARGS__)
#else
  #define _DebugPrintf(...)
#endif




#define MAX_PARAMETRES_BUFFER_LEN             1024
#define TAG_NUMBER                            1
#define TAG_STRING                            2
#define TAG_DATA                              3

#define MAX_FLOW_CACHE                        10

#define RETURN_SUCCESS                        0
#define RETURN_FAILURE                        1
#define RETURN_BUSY                           2
#define RETURN_IDLE                           3

#define _IsVisibleChar(x)                     ((x >= ' ') && (x <= '~'))
#define _NumOfRows(x)                         (sizeof(x)/sizeof(x[0]))
#define _IsDigit(d)                           (((d) >= '0') && ((d) <= '9'))

#define SMS_QUEUE_DEPTH                    20

#define EXPECTED_SMS                       gsm_flags.flag.f0
#define MODEM_GSM_SMS_SEND_OK              gsm_flags.flag.f1
#define MODEM_GSM_SMS_SEND_ER              gsm_flags.flag.f2

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje typow
*/

typedef union
{
  uint8_t flags;
  struct
  {
    uint8_t f0:1;
    uint8_t f1:1;
    uint8_t f2:1;
    uint8_t f3:1;
    uint8_t f4:1;
    uint8_t f5:1;
    uint8_t f6:1;
    uint8_t f7:1;
  } flag;
} Flags8bitTypedef;

#endif // __DEF_H
