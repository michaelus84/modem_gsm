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
static uint8_t AtCsq(uint8_t stage, AtCommandParametersTypedef * values);
static uint8_t AtReg(uint8_t stage, AtCommandParametersTypedef * values);
static uint8_t AtRing(uint8_t stage, AtCommandParametersTypedef * values);
static uint8_t AtAccount(uint8_t stage, AtCommandParametersTypedef * values);

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje stalych
*/

static StingListTypedef const Sim800_Error_List[] =
{
  {NULL, "ERROR"         },
  {NULL, "+CME ERROR: %d"},
  {NULL, "+CMS ERROR: %d"}
};

/**
 * @brief Lista URC do sprawdzenia
 *
 */
static StingListTypedef const Sim800UrcList[] =
{
  {&AtSmsRecieve, "+CMTI: \"SM\",%d"                                                        },
  {&AtRing      , "RING"                                                                    },
  {&AtReg       , "+CREG: %d"                                                               },
  {&AtAccount   , "+CUSD: %d, \"Pozostalo Ci %d,%dzl do wykorzystania do %d-%d-%d %d:%d:%d."}
};

/**
 * @brief Lista komend AT do wyslania
 *
 */
static AtCommandLineTypedef const Sim800_At_Commands_List[] =
{
  {LB_EMPTY, 5,  3, ""                      , NULL          , NULL      , IGNORE, DEFAULT,      0, 0},  // ustawiamy pelna funkconalnosc
  {LB_ECHO,  0,  3, "E0"                    , NULL          , NULL      , IGNORE, DEFAULT,      0, 0},
  {LB_CMEE,  0,  3, "+CMEE=1"               , NULL          , NULL      , IGNORE, DEFAULT,      0, 0},  // numeryczny format bledu
  {LB_CFUN,  5,180, "+CFUN=1"               , NULL          , NULL      , IGNORE, DEFAULT,      0, 0},  // ustawiamy pelna funkconalnosc
  {LB_CGAT,  0,180, "+CGATT=1"              , NULL          , NULL      , REBOOT, DEFAULT,      0, 0},  // rejestrcja do sieci
  {LB_CMGF,  0,  3, "+CMGF=1"               , NULL          , NULL      , IGNORE, DEFAULT,      0, 0},  // tekstowy format SMS-a
  {LB_CSCS,  0,  3, "+CSCS=\"GSM\""         , NULL          , NULL      , IGNORE, DEFAULT,      0, 0},
  {LB_CUSD,  0,  3, "+CUSD=1,\"*101#\""     , NULL          , NULL      , IGNORE, DEFAULT,      0, 0},
  {LB_CSQ,   0,  3, "+CSQ"                  , "+CSQ: %d,%d" , &AtCsq    , IGNORE, DEFAULT,      0, 0},
  {LB_CREG,  5,  3, "+CREG?"                , "+CREG: %d,%d", &AtReg    , JUMP  , JUMP,    LB_CSQ, 0}
};

/**
 * @brief Lista komend od wyslania SMS-a
 *
 */
static AtCommandLineTypedef const Sim800_At_Sms_Send[] =
{
  {       0, 0, 60, "+CMGS=\"%s\""          , "+CMGS: %d"   , &AtSmsSend, IGNORE, DEFAULT,      0, 0},  // wyslanie SMS-a
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

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje zmiennych
*/



/*
-------------------------------------------------------------------------------------------------------------------------------------------
Funkcje
*/

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
      PutStringToStream((char *)sms_out_queue.phone_number[sms_out_queue.start], values);
      break;

    case AT_ITR_STAGE:

      break;

    case AT_OK_STAGE:
      IncrementIndex(&sms_out_queue.start, 1, SMS_QUEUE_DEPTH);
      if (sms_out_queue.fill_status) sms_out_queue.fill_status--;
      sms_out_queue.busy = 0;
      SmsSendStatus(TRUE);
      break;

    case AT_ERROR_STAGE:
      SmsSendStatus(FALSE);
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
  uint16_t len;
  char * str_ptr;

  switch (stage)
  {
    case AT_SEND_STAGE:
      // sprawdzamy czy jest najpierw miejsce w kolejce
      if (sms_in_queue.fill_status >= SMS_QUEUE_DEPTH)  return FALSE;
      //zerujemy rzeczy od SMS-a
      EXPECTED_SMS = 0;
      break;

    case AT_ITR_STAGE:

      GetStringFromStream(&len, values, &index);
      // numer telefonu
      str_ptr = GetStringFromStream(&len, values, &index);
      memset(sms_in_queue.phone_number[sms_in_queue.end], 0, PHONE_NUMBER_STR_LEN);
      if (len > PHONE_NUMBER_LEN) len = PHONE_NUMBER_LEN;
      memcpy(sms_in_queue.phone_number[sms_in_queue.end], str_ptr, len);
      // czas
      str_ptr = GetStringFromStream(&len, values, &index);
      memset(sms_in_queue.time[sms_in_queue.end], 0, SMS_TIME_STR_LEN);
      if (len > SMS_TIME_LEN) len = SMS_TIME_LEN;
      memcpy(sms_in_queue.time[sms_in_queue.end], str_ptr, len);
      // data
      str_ptr = GetStringFromStream(&len, values, &index);
      memset(sms_in_queue.date[sms_in_queue.end], 0, SMS_DATE_STR_LEN);
      if (len > SMS_DATE_LEN) len = SMS_DATE_LEN;
      memcpy(sms_in_queue.date[sms_in_queue.end], str_ptr, len);
      EXPECTED_SMS = 1;
      break;

    case AT_URC_STAGE:
      modem_status.sms_num = GetNumberFromStream(values, &index);
      PutAtCmdListToFlow((AtCommandLineTypedef *)at_sms_recieve, _NumOfRows(at_sms_recieve), &at_cmd_flow);
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
      modem_status.csq = GetNumberFromStream(values, &index);
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
      modem_status.reg = GetNumberFromStream(values, &index);
      modem_status.reg = GetNumberFromStream(values, &index);
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
    PutAtCmdListToFlow((AtCommandLineTypedef *)at_ring, _NumOfRows(at_ring), &at_cmd_flow);
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
#endif
