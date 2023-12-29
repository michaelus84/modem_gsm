#include "modem_gsm_def.h"

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
 * Funkcja od zwiekszenia indeksu bufora
 *
 * @param index_ptr - wskaznik na indeks
 * @param pin    - o ile przesunac
 * @param range     - zakres bufora
 *
 */
void IncrementIndex(uint16_t * index_ptr, uint16_t pin, uint16_t range)
{
  uint32_t new_index;

  new_index = (uint32_t) *index_ptr + pin;
  // sprawdz, czy wynik miesci sie w zakresie wielkosci bufora
  if (new_index >= range) new_index -= range;
  // zapisz wynik
  *index_ptr = new_index;
}

/**
 * @brief 
 * 
 */
uint8_t GpioInit(void)
{

  return RETURN_SUCCESS;
}

/**
 * @brief 
 * 
 * @param pin 
 * @param value 
 */
void GpioWrite(void * gpio, uint32_t pin, uint8_t value)
{

}

/**
 * @brief 
 * 
 * @param pin 
 * @return uint8_t 
 */
uint8_t GpioRead(void * gpio, uint32_t pin)
{

}

/**
 * @brief Funkcja zwracajaca licznik milisekund
 *
 * @return uint32_t
 */
uint32_t GetTick(void)
{
  uint32_t ms;

  return ms;
}
