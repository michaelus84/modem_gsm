/*
 * at_scripts.h
 *
 *  Created on: 9 lip 2019
 *      Author: msk
 */

#ifndef __AT_SCRIPTS_H
#define __AT_SCRIPTS_H

#include "at_common_def.h"
/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje preprocesora
*/


/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje typow
*/

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje zmiennych
*/

extern AtCommandListTypedef const At_Sms_Send_List;

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje Funkcje
*/
void AtModemCommonInitScript(AtScriptInitTypedef * ops, AtCmdFlowTypedef * cmd_flow, ModemStatusTypedef * status);
uint8_t AtIpr(uint8_t stage, AtCommandParametersTypedef * values, uint_t * data);
uint8_t AtIpAddr(uint8_t stage, AtCommandParametersTypedef * values, uint_t * data);
uint8_t AtCsq(uint8_t stage, AtCommandParametersTypedef * values, uint_t * data);
uint8_t AtReg(uint8_t stage, AtCommandParametersTypedef * values, uint_t * data);
uint8_t AtApn(uint8_t stage, AtCommandParametersTypedef * values, uint_t * data);
uint8_t AtSmsSend(uint8_t stage, AtCommandParametersTypedef * values, uint_t * data);
uint8_t AtSmsRecieve(uint8_t stage, AtCommandParametersTypedef * values, uint_t * data);
uint8_t AtRing(uint8_t stage, AtCommandParametersTypedef * values, uint_t * data);
#endif // __AT_SCRIPTS_H
