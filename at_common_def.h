/*
 * at_common_types.h
 *
 *  Created on: 9 lip 2019
 *      Author: msk
 */

#ifndef __AT_COMMON_TYPES_H
#define __AT_COMMON_TYPES_H

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
#define FALSE                                 0                                     //!< definicja niepradwy
#endif

#if !defined(TRUE)
#define TRUE                                  1                                     //!< definicja prawdy
#endif

#define IRQ_PIN                               3                                     //!<

#define PWR_KEY                               1                                     //!<
#define PWR_KEY_GPIO                          NULL                                  //!<

#define _DebugPrintf(...)                     printf(__VA_ARGS__)                   //!< makro funkcji wypisywania na terminal

#define PHONE_NUMBER_LEN                      12                                    //!<
#define PHONE_NUMBER_STR_LEN                  (PHONE_NUMBER_LEN + 1)                //!<

#define SMS_DATE_LEN                          8                                     //!<
#define SMS_DATE_STR_LEN                      (SMS_DATE_LEN + 1)                    //!<

#define SMS_TIME_LEN                          11                                    //!<
#define SMS_TIME_STR_LEN                      (SMS_TIME_LEN + 1)                    //!<

#define SMS_LEN                               161                                   //!<

#define DATE_LEN                              (SMS_TIME_LEN + SMS_DATE_LEN + 1)     //!<
#define DATE_STR_LEN                          (DATE_LEN + 1)                        //!<

#define MAX_PARAMETRES_BUFFER_LEN             1024
#define TAG_NUMBER                            1
#define TAG_STRING                            2
#define TAG_DATA                              3

#define MAX_FLOW_CACHE                        10

#define RETURN_SUCCESS                        0                                     //!< zwracanie kodu akcji zakonczonej powodzeniem
#define RETURN_FAILURE                        1                                     //!< zwracanie kodu akcji zakonczonej niepowodzeniem
#define RETURN_BUSY                           2                                     //!< zwracanie kody o zajeciu
#define RETURN_IDLE                           3                                     //!< kod kotowosci do akcji (brak zadania)

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
  }flag;
} Flags8bitTypedef;                  //!<

typedef struct
{
  uint8_t csq;
  uint8_t reg;
  uint8_t sms_num;
} ModemStatusTypedef;

typedef struct
{
  uint8_t parameters[MAX_PARAMETRES_BUFFER_LEN];
  uint16_t filling;
  uint16_t size;
} AtCommandParametersTypedef;

typedef uint8_t (*AtFunctionTypedf)(uint8_t, AtCommandParametersTypedef *);

enum
{
  REBOOT = 0,
  JUMP,
  EXIT,
  REPEAT_UNTIL,
  IGNORE,
  DEFAULT,
};

enum
{
  MODEM_INIT = 0,
  MODEM_REINIT,
  MODEM_PWR_KEY,
  MODEM_PWR_KEY_RELEASE,
  MODEM_RUN,
  MODEM_WAIT,
};

enum
{
  OK = 0,
  ER,
  NUM,
};

enum
{
  AT_SEND = 0,
  AT_RESPONSE,
  AT_OK,
  AT_ERROR,
  AT_TIMEOUT,
  AT_FAULT,
  AT_IDLE,
  AT_SMS,
  AT_END
};

enum
{
  AT_SEND_STAGE = 0,
  AT_ITR_STAGE,
  AT_OK_STAGE,
  AT_ERROR_STAGE,
  AT_URC_STAGE,
  AT_TIMEOUT_STAGE
};

enum
{
  LB_EMPTY = 1,
  LB_ECHO,
  LB_CFUN,
  LB_CMEE,
  LB_CGAT,
  LB_CMGF,
  LB_CSCS,
  LB_CSCA,
  LB_CSQ,
  LB_CREG,
  LB_CMGS,
  LB_CUSD,
};

typedef struct
{
  uint8_t label;
  uint16_t delay;
  uint16_t cmd_timeout;
  char *reference_string_out;
  char *reference_string_in;
  AtFunctionTypedf preparation_fun;
  uint8_t behaviore_err;
  uint8_t behaviore;
  uint8_t aux_a;
  uint8_t aux_b;
} AtCommandLineTypedef;

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje Funkcje
*/


#endif /* MODEM_GSM_H_ */
