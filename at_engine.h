/*
 * modem_gsm.h
 *
 *  Created on: 9 lip 2019
 *      Author: msk
 */

#ifndef __AT_ENGINE_H
#define __AT_ENGINE_H

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje prprocesora
*/

#define SMS_QUEUE_DEPTH                    20

#define EXPECTED_SMS                       gsm_flags.flag.f0
#define MODEM_GSM_SMS_SEND_OK              gsm_flags.flag.f1
#define MODEM_GSM_SMS_SEND_ER              gsm_flags.flag.f2

#define _IsVisibleChar(x)                  ((x >= ' ') && (x <= '~'))
#define _NumOfRows(x)                      (sizeof(x)/sizeof(x[0]))
/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje typow
*/

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje Funkcje
*/
void ModemRsInit(void);
void ModemGsmModule(void);
void ModemGsmSendSmsRequest(char * phone_number, char * sms, uint16_t len);
uint8_t ModemGsmSmsGet(char * phone_number, char * date, char * text);
void IncrementIndex(uint16_t * index_ptr, uint16_t offset, uint16_t range);

#endif /* MODEM_GSM_H_ */
