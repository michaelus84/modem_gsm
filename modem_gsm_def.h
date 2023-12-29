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
#include "common.h"

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje preprocesora
*/

#define ONLY_MODEM_START                     PRESENT

#define PWR_KEY                              0
#define PWR_KEY_GPIO                         NULL

#define MODEM_UART                           NULL

#if defined(DEBUG)
  #define _DP(...)                           printf(__VA_ARGS__)
#else
  #define _DP(...)
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
