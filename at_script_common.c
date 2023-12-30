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
 * @brief Lista komend od wyslania SMS-a
 *
 */
static AtCommandLineTypedef const At_Sms_Send[] =
{
  {0, 0, 60, "+CMGS=\"%s\""          , "+CMGS: %d"            , &AtSmsSend   , IGNORE, DEFAULT,   0, 0, NULL},  // wyslanie SMS-a
};

AtCommandListTypedef const At_Sms_Send_List = {At_Sms_Send, _NumOfRows(At_Sms_Send)};

static AtCommandLineTypedef const At_Sms_Recieve[] =
{
  {0, 0, 60, "+CMGR=1" , "+CMGR: \"%s\",\"%s\",\"\",\"%s,%s\"", &AtSmsRecieve, IGNORE, DEFAULT,   0, 0, NULL},  // odczytanie SMS-a
  {0, 0, 60, "+CMGD=1,4", NULL                                , NULL         , IGNORE, DEFAULT,   0, 0, NULL},  // usuniecie SMS-a
};

static AtCommandLineTypedef const At_Ring[] =
{
  {       0, 0, 60, "H"                     , NULL            , NULL      , IGNORE, DEFAULT,      0, 0, NULL},  // odrzuc polaczenie
};
/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje zmiennych
*/

static AtCmdFlowTypedef * common_cmd_flow;
static ModemStatusTypedef * common_modem_status;
static AtScriptInitTypedef * common_ops;

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Funkcje
*/

void AtModemCommonInitScript(AtScriptInitTypedef * ops, AtCmdFlowTypedef * cmd_flow, ModemStatusTypedef * status)
{
  common_cmd_flow = cmd_flow;
  common_modem_status = status;
  common_ops = ops;
}

/**
 * @brief
 *
 * @param stage
 * @param values
 * @return uint8_t
 */
uint8_t AtIpr(uint8_t stage, AtCommandParametersTypedef * values, uint_t * data)
{
  ModemStatusTypedef * modem_params = (ModemStatusTypedef *) (* data);

  switch (stage)
  {
    case AT_SEND_STAGE:
      if (modem_params->baudrate)
      {
        PutNumberToStream(modem_params->baudrate, values);
      }
      else
      {
        PutNumberToStream(DEFAULT_BAUDRATE, values);
      }
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

/**
 * @brief
 *
 * @param stage
 * @param values
 * @return uint8_t
 */
uint8_t AtIpAddr(uint8_t stage, AtCommandParametersTypedef * values, uint_t * data)
{
  uint16_t index = 0;
  ModemStatusTypedef * modem_params = (ModemStatusTypedef *) (* data);

  switch (stage)
  {
    case AT_SEND_STAGE:

      break;

    case AT_ITR_STAGE:
      modem_params->ip.a = GetNumberFromStream(values, &index);
      modem_params->ip.b = GetNumberFromStream(values, &index);
      modem_params->ip.c = GetNumberFromStream(values, &index);
      modem_params->ip.d = GetNumberFromStream(values, &index);
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
uint8_t AtApn(uint8_t stage, AtCommandParametersTypedef * values, uint_t * data)
{
  ModemStatusTypedef * modem_params = (ModemStatusTypedef *) (* data);

  switch (stage)
  {
    case AT_SEND_STAGE:
      if (modem_params->apn != NULL)
      {
        PutStringToStream(modem_params->apn, values);
      }
      else
      {
        PutStringToStream(DEFAULT_APN, values);
      }
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
uint8_t AtCsq(uint8_t stage, AtCommandParametersTypedef * values, uint_t * data)
{
  uint16_t index = 0;
  ModemStatusTypedef * modem_params = (ModemStatusTypedef *) (* data);

  switch (stage)
  {
    case AT_SEND_STAGE:

      break;

    case AT_ITR_STAGE:
      modem_params->csq = GetNumberFromStream(values, &index);
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
uint8_t AtReg(uint8_t stage, AtCommandParametersTypedef * values, uint_t * data)
{
  uint16_t index = 0;

  ModemStatusTypedef * modem_params = (ModemStatusTypedef *) (* data);

  switch (stage)
  {
    case AT_SEND_STAGE:

      break;

    case AT_ITR_STAGE:
      modem_params->reg = GetNumberFromStream(values, &index);
      modem_params->reg = GetNumberFromStream(values, &index);
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
uint8_t AtRing(uint8_t stage, AtCommandParametersTypedef * values, uint_t * data)
{
  if (AT_URC_STAGE == stage)
    PutAtCmdListToFlow((AtCommandLineTypedef *)At_Ring, _NumOfRows(At_Ring), common_cmd_flow);
  return TRUE;
}



/**
 * @brief  * @brief  Funkcja pomocnicza od wyslania SMS-a
 *
 * @param stage     - etap dla ktorego wykonac akcje
 * @param values    - parametr do przetworzenia
 * @return uint8_t  - TRUE lub FALSE
 */
uint8_t AtSmsSend(uint8_t stage, AtCommandParametersTypedef * values, uint_t * data)
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
uint8_t AtSmsRecieve(uint8_t stage, AtCommandParametersTypedef * values, uint_t * data)
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
      common_modem_status->sms_num = GetNumberFromStream(values, &index);
      PutAtCmdListToFlow((AtCommandLineTypedef *)At_Sms_Recieve, _NumOfRows(At_Sms_Recieve), common_cmd_flow);
      break;

    case AT_OK_STAGE:
      if (common_ops->SmsRecieveCallback != NULL)
      {
        common_ops->SmsRecieveCallback();
      }
      break;

    case AT_ERROR_STAGE:

      break;

    default:
      break;
  }
  return TRUE;
}

