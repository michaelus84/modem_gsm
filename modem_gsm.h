/*
 * modem_gsm.h
 *
 *  Created on: 9 lip 2019
 *      Author: msk
 */

#ifndef __MODEM_GSM_H
#define __MODEM_GSM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje preprocesora
*/
#define MODEM_GSM_ON              0
#define MODEM_GSM_TIMEOUT         1
#define MODEM_GSM_ERROR           2

#define MAX_MODEM_START_TIMEOUT   60000 // ms
/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje typow
*/

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje zmiennych
*/

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje Funkcje
*/
uint8_t ModemGsmStart(void);

#endif /* __MODEM_GSM_H */