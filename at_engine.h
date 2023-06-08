/*
 * modem_gsm.h
 *
 *  Created on: 9 lip 2019
 *      Author: msk
 */

#ifndef __AT_ENGINE_H
#define __AT_ENGINE_H

#include <stdint.h>

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje prprocesora
*/

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje typow
*/

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje Funkcje
*/
void ModemInit(void);
void ModemClosePort(void);
void ModemGsmModule(void);
void ModemGsmSendSmsRequest(char * phone_number, char * sms, uint16_t len);
uint8_t ModemGsmSmsGet(char * phone_number, char * date, char * text);
uint8_t ModemGsmReady(void);


#endif /* MODEM_GSM_H_ */
