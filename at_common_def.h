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
#include "modem_gsm_def.h"

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje preprocesora
*/
#define EXPECTED_SMS                       gsm_flags.flag.f0
#define MODEM_GSM_SMS_SEND_OK              gsm_flags.flag.f1
#define MODEM_GSM_SMS_SEND_ER              gsm_flags.flag.f2
#define BASE_CONFIG_DONE                   gsm_flags.flag.f3
/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje typow
*/

typedef union
{
  uint32_t addr;
  struct
  {
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
  };
} Ipv4AddrTypedef;

typedef struct
{
  uint8_t csq;
  uint8_t reg;
  uint8_t sms_num;
  Ipv4AddrTypedef ip;
} ModemStatusTypedef;


typedef struct
{
  unsigned int * item;
  uint8_t label;
} ParameterItemTypedef;
typedef struct
{
  ParameterItemTypedef * list;
  uint8_t num;
} ParameterListTypedef;

typedef struct
{
  uint8_t parameters[MAX_PARAMETRES_BUFFER_LEN];
  uint16_t filling;
  uint16_t size;
} AtCommandParametersTypedef;

typedef uint8_t (*AtFunctionTypedf)(uint8_t, AtCommandParametersTypedef *, void *);
typedef void (* StdFun) (void);

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
  LB_APN,
  LB_IPR,
  LB_IP,
};

typedef struct
{
  uint8_t label;
  uint16_t delay;
  uint16_t cmd_timeout;
  char * ref_str_out;
  char * ref_str_in;
  AtFunctionTypedf fun;
  uint8_t behaviore_err;
  uint8_t behaviore;
  uint8_t action_a;
  uint8_t action_b;
  void * argv;
} AtCommandLineTypedef;

typedef struct
{
  const AtCommandLineTypedef * cmd_list;
  uint16_t cmd_num;
} AtCommandListTypedef;

typedef struct
{
  AtFunctionTypedf fun;
  char * string;
} StringListTypedef;

typedef struct
{
  const StringListTypedef * str_lis;
  uint16_t cmd_num;
} AtCommandStrListTypedef;

typedef struct
{
  Ipv4AddrTypedef dest_ip;
  uint8_t id;
  uint8_t num;
  uint8_t type;
  uint16_t len;
  uint8_t * buffer;
} AtSocketTypedef;

typedef struct
{
  struct queue
  {
    AtCommandLineTypedef * buf[MAX_FLOW_CACHE];
    uint16_t lines[MAX_FLOW_CACHE];
    uint16_t start;
    uint16_t end;
    uint8_t fill_status;
  } queue;
  uint8_t flow_top;
  struct list
  {
    AtCommandLineTypedef * buf[MAX_FLOW_CACHE];
    uint16_t lines[MAX_FLOW_CACHE];
    uint16_t active_line[MAX_FLOW_CACHE];
    uint16_t repeat_cnt[MAX_FLOW_CACHE];
    uint8_t active;
  } list;

} AtCmdFlowTypedef;

typedef struct
{
  AtCommandListTypedef * base;
  AtCommandListTypedef * sms_send;
  StdFun SmsRecieveCallback;
  AtCommandStrListTypedef * error;
  AtCommandStrListTypedef * urc;
} AtScriptInitTypedef;

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje zmiennych
*/

extern Flags8bitTypedef gsm_flags;

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje funkcji
*/

uint8_t PutAtCmdListToFlow(const AtCommandLineTypedef * at_list, uint16_t list_lines, AtCmdFlowTypedef * at_flow);
uint8_t PutNumberToStream(uint32_t number, AtCommandParametersTypedef * v);
uint8_t PutStringToStream(char * string, AtCommandParametersTypedef * v);
uint32_t GetNumberFromStream(AtCommandParametersTypedef * v, uint16_t * index);
char * GetStringFromStream(uint16_t * string_len, AtCommandParametersTypedef * v, uint16_t * index);

#endif /* MODEM_GSM_H_ */
