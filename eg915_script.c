/*
-------------------------------------------------------------------------------------------------------------------------------------------
*/
#include "at_script_common.h"
#include "eg915_script.h"
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
Definicje zmiennych
*/

static AtCmdFlowTypedef * eg915_cmd_flow;
static uint_t modem_status;
static AtScriptInitTypedef * eg915_ops;

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje stalych
*/

static StringListTypedef const At_Error[] =
{
  {NULL, "ERROR"         },
  {NULL, "+CME ERROR: %d"},
  {NULL, "+CMS ERROR: %d"}
};

static AtCommandStrListTypedef const At_Error_List = {At_Error, _NumOfRows(At_Error)};

static StringListTypedef const At_Urc[] =
{
  {&AtSmsRecieve, "+CMTI: \"SM\",%d"},
  {&AtRing      , "RING"            },
  {&AtReg       , "+CREG: %d"       },
};

static AtCommandStrListTypedef const At_Urc_List   = {At_Urc   , _NumOfRows(At_Urc)};


/**
 * @brief Lista komend AT do wyslania
 *
 */
static AtCommandLineTypedef const At_Commands[] =
{
  {LB_EMPTY, 5,   3, ""                         , NULL                          , NULL      , IGNORE, DEFAULT,      0, 0, NULL},
  {LB_ECHO,  0,   3, "E0"                       , NULL                          , NULL      , IGNORE, DEFAULT,      0, 0, NULL},
  {LB_IPR,   0,   3, "+IPR=%d"                  , NULL                          , &AtIpr    , IGNORE, DEFAULT,      0, 0, NULL},
  {LB_CMEE,  0,   3, "+CMEE=1"                  , NULL                          , NULL      , IGNORE, DEFAULT,      0, 0, NULL},
  {LB_CFUN,  5, 180, "+CFUN=1"                  , NULL                          , NULL      , IGNORE, DEFAULT,      0, 0, NULL},
  {LB_APN,   0,   3, "+CGDCONT=1,\"IP\",\"%s\"" , NULL                          , &AtApn    , IGNORE, DEFAULT,      0, 0, NULL},
  {LB_CGAT,  0, 180, "+CGATT=1"                 , NULL                          , NULL      , REBOOT, DEFAULT,      0, 0, NULL},
  {LB_CGAT,  0, 180, "+CGACT=1,1"               , NULL                          , NULL      , REBOOT, DEFAULT,      0, 0, NULL},
  {LB_CSQ,   0,   3, "+CSQ"                     , "+CSQ: %d,%d"                 , &AtCsq    , IGNORE, DEFAULT,      0, 0, &modem_status},
  {LB_IP,    0,   3, "+CGPADDR=1"               , "+CGPADDR: 1,\"%d,%d,%d,%d\"" , &AtIpAddr , IGNORE, DEFAULT,      0, 0, &modem_status},
  {LB_CREG,  5,   3, "+CREG?"                   , "+CREG: %d,%d"                , &AtReg    , JUMP  , JUMP   , LB_CSQ, 0, &modem_status}
};

static AtCommandListTypedef const At_Commands_List = {At_Commands, _NumOfRows(At_Commands)};


/*
-------------------------------------------------------------------------------------------------------------------------------------------
Funkcje
*/

/**
 * @brief 
 * 
 * @param ops 
 * @param cmd_flow 
 * @param status 
 */
void AtModemInitScript(AtScriptInitTypedef * ops, AtCmdFlowTypedef * cmd_flow, ModemStatusTypedef * status)
{
  eg915_cmd_flow = cmd_flow;
  modem_status = (uint_t)status;

  if (NULL == ops || NULL == cmd_flow)
    return;

  memset(ops, 0, sizeof(AtScriptInitTypedef));

  ops->base = (AtCommandListTypedef *)&At_Commands_List;
  ops->sms_send = (AtCommandListTypedef *)&At_Sms_Send_List;
  ops->error = (AtCommandStrListTypedef *)&At_Error_List;
  ops->urc = (AtCommandStrListTypedef *)&At_Urc_List;
  eg915_ops = ops;

  AtModemCommonInitScript(ops, cmd_flow, status);
}