/*
-------------------------------------------------------------------------------------------------------------------------------------------
*/
#include "uart.h"
#include "at_engine.h"
#include "at_common_def.h"
#include "sim800_script.h"
#include "common.h"

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

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje stalych
*/

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje zmiennych
*/


/*
-------------------------------------------------------------------------------------------------------------------------------------------
Funkcje
*/

/**
 * @brief Funkcja wstawiajaca wartosc numeryczna do strumienia wartosci dla komend AT
 *
 * @param value       - wartosc to wpisania do bufora parametrow
 * @param v           - wskaznik na bufor parametrow
 * @return uint8_t    - RETURN_FAILURE badz RETURN_SUCCESS
 */
uint8_t PutNumberToStream(uint32_t number, AtCommandParametersTypedef * v)
{
  if ((v->filling + 5) >= v->size) return RETURN_FAILURE;

  v->parameters[v->filling++] = TAG_NUMBER;
  v->parameters[v->filling++] = number >> 24;
  v->parameters[v->filling++] = number >> 16;
  v->parameters[v->filling++] = number >> 8;
  v->parameters[v->filling++] = number >> 0;

  return RETURN_SUCCESS;
}


/**
 * @brief Funkcja wstawiajaca wartosc znakowa do strumienia wartosci dla komend AT
 *
 * @param value       - wartosc to wpisania do bufora parametrow
 * @param v           - wskaznik na bufor parametrow
 * @return uint8_t    - RETURN_FAILURE badz RETURN_SUCCESS
 */
uint8_t PutStringToStream(char * string, AtCommandParametersTypedef * v)
{
  uint16_t len_index;
  char src_char;
  uint32_t number;

  if ((v->filling + 3) >= v->size)
    return RETURN_FAILURE;

  v->parameters[v->filling++] = TAG_STRING;
  len_index = v->filling;
  v->filling += 2;

  number = 0;
  do
  {
    if (v->filling >= v->size) return RETURN_FAILURE;

    src_char = *string;

    if (!_IsVisibleChar(src_char)) break;

    string++;

    v->parameters[v->filling++] = src_char;
    number++;

  } while (_IsVisibleChar(src_char));

  v->parameters[len_index++] = number >> 8;
  v->parameters[len_index] = number;

  return RETURN_SUCCESS;
}

/**
 * @brief Pobranie liczby z bufora parametrow
 *
 * @param v
 * @return uint32_t
 */
uint32_t GetNumberFromStream(AtCommandParametersTypedef * v, uint16_t * index)
{
  uint32_t number = 0;
  uint16_t i;

  i = *index;

  if ((v->filling + i) < 5) return number;

  if (TAG_NUMBER != v->parameters[i++]) return number;

  number  = (uint32_t)v->parameters[i++] << 24;
  number |= (uint32_t)v->parameters[i++] << 16;
  number |= (uint32_t)v->parameters[i++] << 8;
  number |= (uint32_t)v->parameters[i++];

  *index = i;
  return number;
}

/**
 * @brief
 *
 * @param v         - wskaznik na bufor parametrow
 * @return uint32_t - odczytana liczba
 */
char * GetStringFromStream(uint16_t * string_len, AtCommandParametersTypedef * v, uint16_t * index)
{
  uint16_t i;
  uint16_t len = 0;

  i = *index;

  *string_len = 0;

  if ((v->filling + i) < 3) return NULL;
  if (TAG_STRING != v->parameters[i++]) return NULL;

  len  = (uint16_t)v->parameters[i++] << 8;
  len |= (uint16_t)v->parameters[i++];

  if ((v->filling + i) < len) return NULL;

  *index = i + len;

  *string_len = len;
  return (char *)(&v->parameters[i]);
}

