/*
-------------------------------------------------------------------------------------------------------------------------------------------
*/
#include "at_script_common.h"
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


/**
 * @brief Lista URC do sprawdzenia
 *
 */
static StringListTypedef const UrcList[] =
{
  {&AtSmsRecieve, "+CMTI: \"SM\",%d"                                                        },
  {&AtRing      , "RING"                                                                    },
  {&AtReg       , "+CREG: %d"                                                               },
};

static AtCommandStrListTypedef Sim800_Urc_List   = {UrcList   , _NumOfRows(UrcList)   };

/**
 * @brief Lista komend od wyslania SMS-a
 *
 */
static AtCommandLineTypedef const At_Sms_Send[] =
{
  {0, 0, 60, "+CMGS=\"%s\""          , "+CMGS: %d"   , &AtSmsSend, IGNORE, DEFAULT,      0, 0},  // wyslanie SMS-a
};

static AtCommandLineTypedef const At_Sms_Recieve[] =
{
  {0, 0, 60, "+CMGR=1" , "+CMGR: \"%s\",\"%s\",\"\",\"%s,%s\"", &AtSmsRecieve, IGNORE, DEFAULT,   0, 0},  // odczytanie SMS-a
  {0, 0, 60, "+CMGD=1,4", NULL                                , NULL         , IGNORE, DEFAULT,   0, 0},  // usuniecie SMS-a
};

static AtCommandLineTypedef const At_Ring[] =
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
 * @brief
 *
 * @param stage
 * @param values
 * @return uint8_t
 */
uint8_t AtIpr(uint8_t stage, AtCommandParametersTypedef * values, void * data)
{
  switch (stage)
  {
    case AT_SEND_STAGE:

      break;

    case AT_ITR_STAGE:

      break;

    case AT_OK_STAGE:
      BASE_CONFIG_DONE = 1;
      break;

    case AT_ERROR_STAGE:

      break;

    default:
      break;
  }
  return TRUE;
}

#if NOT_PRESENT == ONLY_MODEM_START
/**
 * @brief
 *
 * @param stage
 * @param values
 * @return uint8_t
 */
uint8_t AtIpAddr(uint8_t stage, AtCommandParametersTypedef * values, void * data)
{
  uint16_t index = 0;
  ModemStatusTypedef * modem_status = (ModemStatusTypedef *) data;

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
uint8_t AtApn(uint8_t stage, AtCommandParametersTypedef * values, void * data)
{
  char * str =  (char *) data;

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
uint8_t AtCsq(uint8_t stage, AtCommandParametersTypedef * values, void * data)
{
  uint16_t index = 0;
  ModemStatusTypedef * modem_status = (ModemStatusTypedef *) data;

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
uint8_t AtReg(uint8_t stage, AtCommandParametersTypedef * values, void * data)
{
  uint16_t index = 0;

  ModemStatusTypedef * modem_status = (ModemStatusTypedef *) data;

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
uint8_t AtRing(uint8_t stage, AtCommandParametersTypedef * values, void * data)
{
  if (AT_URC_STAGE == stage)
    PutAtCmdListToFlow((AtCommandLineTypedef *)At_Ring, _NumOfRows(At_Ring), eg915_cmd_flow);
  return TRUE;
}



/**
 * @brief  * @brief  Funkcja pomocnicza od wyslania SMS-a
 *
 * @param stage     - etap dla ktorego wykonac akcje
 * @param values    - parametr do przetworzenia
 * @return uint8_t  - TRUE lub FALSE
 */
uint8_t AtSmsSend(uint8_t stage, AtCommandParametersTypedef * values)
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
uint8_t AtSmsRecieve(uint8_t stage, AtCommandParametersTypedef * values)
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
      PutAtCmdListToFlow((AtCommandLineTypedef *)At_Sms_Recieve, _NumOfRows(At_Sms_Recieve), sim800_cmd_flow);
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

