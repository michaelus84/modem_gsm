/*
-------------------------------------------------------------------------------------------------------------------------------------------
*/
#include "sim800_script.h"
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

// funkcje wjscia/wyjsci na potrzeby komend AT
static uint8_t AtSmsSend(uint8_t stage, AtCommandParametersTypedef * values);
static uint8_t AtSmsRecieve(uint8_t stage, AtCommandParametersTypedef * values);
static uint8_t AtIpAddr(uint8_t stage, AtCommandParametersTypedef * values);
static uint8_t AtCsq(uint8_t stage, AtCommandParametersTypedef * values);
static uint8_t AtReg(uint8_t stage, AtCommandParametersTypedef * values);
static uint8_t AtRing(uint8_t stage, AtCommandParametersTypedef * values);
static uint8_t AtAccount(uint8_t stage, AtCommandParametersTypedef * values);
static uint8_t AtApn(uint8_t stage, AtCommandParametersTypedef * values);

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje stalych
*/

static StringListTypedef const Error_List[] =
{
  {NULL, "ERROR"         },
  {NULL, "+CME ERROR: %d"},
  {NULL, "+CMS ERROR: %d"}
};

/**
 * @brief Lista URC do sprawdzenia
 *
 */
static StringListTypedef const UrcList[] =
{
  {&AtSmsRecieve, "+CMTI: \"SM\",%d"                                                        },
  {&AtRing      , "RING"                                                                    },
  {&AtReg       , "+CREG: %d"                                                               },
  {&AtAccount   , "+CUSD: %d, \"Pozostalo Ci %d,%dzl do wykorzystania do %d-%d-%d %d:%d:%d."}
};

static AtCommandStrListTypedef Sim800_Error_List = {Error_List, _NumOfRows(Error_List)};
static AtCommandStrListTypedef Sim800_Urc_List   = {UrcList   , _NumOfRows(UrcList)   };

/**
 * @brief Lista komend AT do wyslania
 *
 */
static AtCommandLineTypedef const Sim800_At_Commands[] =
{
  {LB_EMPTY, 5,  3, ""                        , NULL                          , NULL      , IGNORE, DEFAULT,      0, 0},
  {LB_ECHO,  0,  3, "E0"                      , NULL                          , NULL      , IGNORE, DEFAULT,      0, 0},
  {LB_IPR,   0,  3, "+IPR=115200"             , NULL                          , NULL      , IGNORE, DEFAULT,      0, 0},
  {LB_CMEE,  0,  3, "+CMEE=1"                 , NULL                          , NULL      , IGNORE, EXIT,         0, 0},
  {LB_CFUN,  5,180, "+CFUN=1"                 , NULL                          , NULL      , IGNORE, DEFAULT,      0, 0},
  {LB_APN,   0,  3, "+CGDCONT=1,\"IP\",\"%s\"", NULL                          , &AtApn    , IGNORE, DEFAULT,      0, 0},
  {LB_CGAT,  0,180, "+CGATT=1"                , NULL                          , NULL      , REBOOT, DEFAULT,      0, 0},
  {LB_CGAT,  0,180, "+CGACT=1,1"              , NULL                          , NULL      , REBOOT, DEFAULT,      0, 0},
  {LB_CSQ,   0,  3, "+CSQ"                    , "+CSQ: %d,%d"                 , &AtCsq    , IGNORE, DEFAULT,      0, 0},
  {LB_IP,    0,  3, "+CGPADDR=1"              , "+CGPADDR: 1,\"%d,%d,%d,%d\"" , &AtIpAddr , IGNORE, DEFAULT,      0, 0},
  {LB_CREG,  5,  3, "+CREG?"                  , "+CREG: %d,%d"                , &AtReg    , JUMP  , JUMP,    LB_CSQ, 0}
};

/**
 * @brief Lista komend od wyslania SMS-a
 *
 */
static AtCommandLineTypedef const Sim800_At_Sms_Send[] =
{
  {0, 0, 60, "+CMGS=\"%s\""          , "+CMGS: %d"   , &AtSmsSend, IGNORE, DEFAULT,      0, 0},  // wyslanie SMS-a
};

static AtCommandLineTypedef const Sim800_At_Sms_Recieve[] =
{
  {0, 0, 60, "+CMGR=1" , "+CMGR: \"%s\",\"%s\",\"\",\"%s,%s\"", &AtSmsRecieve, IGNORE, DEFAULT,   0, 0},  // odczytanie SMS-a
  {0, 0, 60, "+CMGD=1,4", NULL                                , NULL         , IGNORE, DEFAULT,   0, 0},  // usuniecie SMS-a
};

static AtCommandLineTypedef const Sim800_At_Ring[] =
{
  {       0, 0, 60, "H"                     , NULL          , NULL      , IGNORE, DEFAULT,      0, 0},  // odrzuc polaczenie
};

static AtCommandListTypedef Sim800_At_Commands_List = {Sim800_At_Commands, _NumOfRows(Sim800_At_Commands)};
static AtCommandListTypedef Sim800_At_Sms_Send_List = {Sim800_At_Sms_Send, _NumOfRows(Sim800_At_Sms_Send)};

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje zmiennych
*/

static AtCmdFlowTypedef * sim800_cmd_flow;
static ModemStatusTypedef * modem_status;
static AtScriptInitTypedef * sim800_ops;

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Funkcje
*/

/**
 * @brief 
 * 
 * @param cmd_flow 
 */
void Sim800InitScript(AtScriptInitTypedef * ops, AtCmdFlowTypedef * cmd_flow, ModemStatusTypedef * status)
{
  sim800_cmd_flow = cmd_flow;
  modem_status = status;

  if (NULL == ops || NULL == cmd_flow)
    return;

  memset(ops, 0, sizeof(AtScriptInitTypedef));

  ops->base = &Sim800_At_Commands_List;
  ops->sms_send = &Sim800_At_Sms_Send_List;
  ops->error = &Sim800_Error_List;
  ops->urc = &Sim800_Urc_List;

  sim800_ops = ops;
}

/**
 * @brief  * @brief  Funkcja pomocnicza od wyslania SMS-a
 *
 * @param stage     - etap dla ktorego wykonac akcje
 * @param values    - parametr do przetworzenia
 * @return uint8_t  - TRUE lub FALSE
 */
static uint8_t AtSmsSend(uint8_t stage, AtCommandParametersTypedef * values)
{
  switch (stage)
  {
    case AT_SEND_STAGE:

      break;

    case AT_ITR_STAGE:

      break;

    case AT_OK_STAGE:

      break;

    case AT_ERROR_STAGE:

      break;

    default:
      break;
  }
  return TRUE;
}

/**
 * @brief
 *
 * @param stage
 * @param values
 * @return uint8_t
 */
static uint8_t AtSmsRecieve(uint8_t stage, AtCommandParametersTypedef * values)
{
  uint16_t index = 0;

  switch (stage)
  {
    case AT_SEND_STAGE:

      EXPECTED_SMS = 0;
      break;

    case AT_ITR_STAGE:

      EXPECTED_SMS = 1;
      break;

    case AT_URC_STAGE:
      modem_status->sms_num = GetNumberFromStream(values, &index);
      PutAtCmdListToFlow((AtCommandLineTypedef *)Sim800_At_Sms_Recieve, _NumOfRows(Sim800_At_Sms_Recieve), sim800_cmd_flow);
      break;

    case AT_OK_STAGE:
      if (sim800_ops->SmsRecieveCallback != NULL)
      {
        sim800_ops->SmsRecieveCallback();
      }
      break;

    case AT_ERROR_STAGE:

      break;

    default:
      break;
  }
  return TRUE;
}

/**
 * @brief
 *
 * @param stage
 * @param values
 * @return uint8_t
 */
static uint8_t AtIpAddr(uint8_t stage, AtCommandParametersTypedef * values)
{
  uint16_t index = 0;

  switch (stage)
  {
    case AT_SEND_STAGE:

      break;

    case AT_ITR_STAGE:
      modem_status->ip.a = GetNumberFromStream(values, &index);
      modem_status->ip.b = GetNumberFromStream(values, &index);
      modem_status->ip.c = GetNumberFromStream(values, &index);
      modem_status->ip.d = GetNumberFromStream(values, &index);
      break;

    case AT_OK_STAGE:

      break;

    case AT_ERROR_STAGE:

      break;

    default:
      break;
  }
  return TRUE;
}

/**
 * @brief
 *
 * @param stage
 * @param values
 * @return uint8_t
 */
static uint8_t AtApn(uint8_t stage, AtCommandParametersTypedef * values)
{
  char * str = "plus";

  switch (stage)
  {
    case AT_SEND_STAGE:

      break;

    case AT_ITR_STAGE:
      PutStringToStream(str, values);
      break;

    case AT_OK_STAGE:

      break;

    case AT_ERROR_STAGE:

      break;

    default:
      break;
  }
  return TRUE;
}

/**
 * @brief
 *
 * @param stage
 * @param values
 * @return uint8_t
 */
static uint8_t AtCsq(uint8_t stage, AtCommandParametersTypedef * values)
{
  uint16_t index = 0;

  switch (stage)
  {
    case AT_SEND_STAGE:

      break;

    case AT_ITR_STAGE:
      modem_status->csq = GetNumberFromStream(values, &index);
      break;

    case AT_OK_STAGE:

      break;

    case AT_ERROR_STAGE:

      break;

    default:
      break;
  }
  return TRUE;
}

/**
 * @brief
 *
 * @param stage
 * @param values
 * @return uint8_t
 */
static uint8_t AtReg(uint8_t stage, AtCommandParametersTypedef * values)
{
  uint16_t index = 0;

  switch (stage)
  {
    case AT_SEND_STAGE:

      break;

    case AT_ITR_STAGE:
      modem_status->reg = GetNumberFromStream(values, &index);
      modem_status->reg = GetNumberFromStream(values, &index);
      break;

    case AT_OK_STAGE:

      break;

    case AT_ERROR_STAGE:

      break;

    default:
      break;
  }
  return TRUE;
}

/**
 * @brief Funkcja odrzucajaca polaczenia dzwiekowe
 *
 * @param stage
 * @param values
 * @return uint8_t
 */
static uint8_t AtRing(uint8_t stage, AtCommandParametersTypedef * values)
{
  if (AT_URC_STAGE == stage)
    PutAtCmdListToFlow((AtCommandLineTypedef *)Sim800_At_Ring, _NumOfRows(Sim800_At_Ring), sim800_cmd_flow);
  return TRUE;
}


/**
 * @brief Funkcja zwracjajaca stan konta i jego waznosc
 *
 * @param stage
 * @param values
 * @return uint8_t
 */
static uint8_t AtAccount(uint8_t stage, AtCommandParametersTypedef * values)
{
  uint16_t index = 0;

  if (AT_URC_STAGE == stage)
  {
    GetNumberFromStream(values, &index);
    _DebugPrintf("Efekt parsowania: %d,", GetNumberFromStream(values, &index));
    _DebugPrintf("%d zl ", GetNumberFromStream(values, &index));
    _DebugPrintf("%d-", GetNumberFromStream(values, &index));
    _DebugPrintf("%d-", GetNumberFromStream(values, &index));
    _DebugPrintf("%d ", GetNumberFromStream(values, &index));
    _DebugPrintf("%d:", GetNumberFromStream(values, &index));
    _DebugPrintf("%d:", GetNumberFromStream(values, &index));
    _DebugPrintf("%d\n", GetNumberFromStream(values, &index));
  }
  return TRUE;
}