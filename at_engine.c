/*
-------------------------------------------------------------------------------------------------------------------------------------------
*/
#include "modem_gsm_uart.h"
#include "at_engine.h"
#include "at_common_def.h"
#include "at_script_common.h"
#include "common.h"

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje preprocesora - lokalna
*/

#define _CTRL_Z                           0x1A

#define _CR                               '\r'
#define _LF                               '\n'

#define _PROMPT                           '>'
#define _CRLF                             _CR

#define _CMD_TIME_INTERVAL                200
#define _MS_TICK                          GetTick()

#define _PWR_KEY_LOW_STATE_TIMEOUT        2000
#define _PWR_KEY_RELEASE_TIMEOUT          10000

#define _CIRCULAR_BUFFER_LEN              4096
#define _AT_CMD_BUFFER_LEN                2048

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje typow lokalnych
*/

typedef struct
{
  uint8_t * data;
  uint16_t size;
  uint16_t search;
  uint16_t start;
  uint16_t end;
  uint16_t cr_lf;
  uint32_t prev_len;
} CircularBufferTypedef;

typedef struct
{
  uint16_t limit;
  uint16_t detected;
} SearchResultTypedef;

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje funkcji wewnetrznych
*/
static SearchResultTypedef SearchForDelimiterOrPrompt(CircularBufferTypedef *cbuf);
static uint16_t AtCmdCreateString(char * reference_string, char * buffer, uint16_t buffer_size, AtCommandParametersTypedef * parameters);
static uint8_t AtCmdParseString(char * reference_string, CircularBufferTypedef *cbuf, uint16_t limit, AtCommandParametersTypedef *values);
static uint8_t AtCheckForOk(AtCommandLineTypedef * at_cmd, AtCommandParametersTypedef * param, CircularBufferTypedef * cbuf, uint16_t limit);
static uint8_t AtCheckForErr(AtCommandLineTypedef * at_cmd, AtCommandParametersTypedef * param, CircularBufferTypedef * cbuf, uint16_t limit);
static uint8_t AtCheckForUrc(AtCommandParametersTypedef *param, CircularBufferTypedef * cbuf, uint16_t limit);
static uint8_t AtCheckForItr(AtCommandLineTypedef * at_cmd, AtCommandParametersTypedef * param, CircularBufferTypedef * cbuf, uint16_t limit);
static AtCommandLineTypedef * GetAtCmdFromFlow(AtCmdFlowTypedef * at_flow);
static uint8_t ModifyAtCmdFlow(AtCommandLineTypedef * at_line, uint8_t behaviore, AtCmdFlowTypedef * at_flow, uint8_t at_state);
static uint8_t AtCmdSequence(AtCmdFlowTypedef * at_flow, AtCommandParametersTypedef * param, CircularBufferTypedef * cbuf);
static uint16_t AtReciever(CircularBufferTypedef * cbuf);
static void NextStateWithDelay(uint8_t * state, uint8_t next_state, uint32_t delay);
static void StateWait(uint8_t * state);
static uint16_t CopyFromCircularBuffer(CircularBufferTypedef * cbuf, uint8_t * buffer, uint16_t len);
static void RestorAtFlow(AtCmdFlowTypedef * at_flow);
static void CircularBufferInit(CircularBufferTypedef * cbuf);

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje stalych
*/

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Definicje zmiennych
*/
static AtCommandParametersTypedef at_cmd_param;
static uint32_t cmd_time_interval;
static uint32_t cmd_delay_timeout;
static uint32_t cmd_delay_period;
static uint32_t cmd_timeout;
static uint8_t modem_state = MODEM_INIT;
static uint8_t modem_next_state = MODEM_INIT;
static AtCmdFlowTypedef at_cmd_flow;
static CircularBufferTypedef at_cbuf;
static char circular_buffer[_CIRCULAR_BUFFER_LEN ];
static char at_out_buf[_AT_CMD_BUFFER_LEN];
static uint32_t modem_wait_timeout[2];
static uint8_t search_bytes[2];
static uint8_t itr_status = TRUE;
static AtCommandLineTypedef * at_cmd;
static uint8_t  at_state = AT_IDLE;
static uint16_t recieve_len;
static uint8_t prev_top_flow = 0;
static UartParametersTypedef modem_handle;
static AtScriptInitTypedef scripts;
static ModemStatusTypedef modem_status;
Flags8bitTypedef gsm_flags;

#if defined(DEBUG)
uint8_t modem_running = 0;
#endif

/*
-------------------------------------------------------------------------------------------------------------------------------------------
Funkcje
*/

/**
 * @brief
 *
 */
void ModemInit(void)
{
  // konfigurujemy odbior danych
  //UartReadInit(&modem_handle, (uint8_t *)circular_buffer, sizeof(circular_buffer), 0);
  //SerialPortConfig(&modem_handle, 115200, MODEM_UART);
  //AtModemInitScript(&scripts, &at_cmd_flow, &modem_status);
  //GpioInit(PWR_KEY);
}

/**
 * @brief
 *
 */
void ModemClosePort(void)
{
  SerialPortClose(&modem_handle);
}

/**
 * @brief Inicjalizacja bufora okreznego
 *
 * @param cbuf - wskaznik na strukture opisujaca bufor okrezny
 */
static void CircularBufferInit(CircularBufferTypedef * cbuf)
{
  cbuf->data = (uint8_t *)circular_buffer;
  cbuf->size = sizeof(circular_buffer);
  cbuf->start = 0;
  cbuf->end = 0;
  cbuf->search = 0;
  cbuf->cr_lf = 0;
  cbuf->prev_len = 0;
}

/**
 * @brief Glowna obsluga modemu GSM
 *
 */
void ModemGsmModule(void)
{
  UartRecieve(&modem_handle);
  switch (modem_state)
  {
    case MODEM_REINIT:
    case MODEM_INIT:
      // zerujemy wszysto dotyczace bufora okreznego
      CircularBufferInit(&at_cbuf);

      // reset bufora na parametry
      at_cmd_param.filling = 0;
      at_cmd_param.size = MAX_PARAMETRES_BUFFER_LEN;

      // reset zmiennych od wyszukiwania delimitera
      memset(search_bytes, 0, sizeof(search_bytes));
      // resetowanie sterowania komendami AT
      memset(&at_cmd_flow, 0, sizeof(at_cmd_flow));
      // ladujemy pierwsza komende
      PutAtCmdListToFlow(scripts.base->cmd_list, scripts.base->cmd_num, &at_cmd_flow);
      modem_state = MODEM_PWR_KEY;
      break;

    case MODEM_PWR_KEY:
      // Ustwiamy power key w stan niski aby uruchomic modem
      _DP("Power Key pull down\n");
      GpioWrite(PWR_KEY_GPIO, PWR_KEY, 1);
      NextStateWithDelay(&modem_state, MODEM_PWR_KEY_RELEASE, _PWR_KEY_LOW_STATE_TIMEOUT);
      break;

    case MODEM_PWR_KEY_RELEASE:
      // zwalniamy power key
      _DP("Power Key release\n");
      GpioWrite(PWR_KEY_GPIO, PWR_KEY, 0);
      NextStateWithDelay(&modem_state, MODEM_RUN, _PWR_KEY_RELEASE_TIMEOUT);
      UartReadInit(&modem_handle, (uint8_t *)circular_buffer, sizeof(circular_buffer), 0);
      #if defined(DEBUG)
      modem_running = 0;
      #endif
      break;

    case MODEM_RUN:
      #if defined(DEBUG)
      if (!modem_running)
      {
        modem_running = 1;
        _DP("Start AT sequence\n");
      }
      #endif
      // wlasciwa obsluga modemu
      if (RETURN_FAILURE == AtCmdSequence(&at_cmd_flow, &at_cmd_param, &at_cbuf))
      {
        modem_state = MODEM_REINIT;
      }
      break;
    case MODEM_WAIT:
      StateWait(&modem_state);
      break;
  }
}

/**
 * @brief Funkcja przugotowujaca ustawienie kolejnego stanu z opoznieniem
 *
 * @param state      - stan do zmiany
 * @param next_state - jaki ma byc nastepny
 * @param delay      - z jakim opoznieniem
 */
static void NextStateWithDelay(uint8_t * state, uint8_t next_state, uint32_t delay)
{
  modem_wait_timeout[0] = _MS_TICK;
  modem_wait_timeout[1] = delay;
  *state = MODEM_WAIT;
  modem_next_state = next_state;
}

/**
 * @brief Funkcja zmieniajca stan po uplywie zadanego opoznienia
 *
 * @param state
 */
static void StateWait(uint8_t * state)
{
  if ((_MS_TICK - modem_wait_timeout[0]) > modem_wait_timeout[1])
  {
    *state = modem_next_state;
  }
}

/**
 * @brief  Funkcja pd zadania wysylajaca SMS-a
 *
 * @param sms   - tekst SMS-a
 * @param len   - dlugosc SMS-a
 */
void ModemGsmSendSmsRequest(char * phone_number, char * sms, uint16_t len)
{

}

/**
 * @brief Funkcja od wyslania SMS-a
 *
 */
void SendSms(void)
{
  PutAtCmdListToFlow(scripts.sms_send->cmd_list, scripts.sms_send->cmd_num, &at_cmd_flow);
}

/**
 * @brief Wstwiamy listę komend do wykonania do kolejki
 *
 * @param at_list    - lista komedn do wykonania
 * @param list_lines - ile komedn
 * @param at_flow    - kolejka rozkazow
 * @return uint8_t   - RETURN_SUCCESS lub RETURN_FAILURE
 */
uint8_t PutAtCmdListToFlow(const AtCommandLineTypedef * at_list, uint16_t list_lines, AtCmdFlowTypedef * at_flow)
{
  if (at_flow->queue.fill_status >= MAX_FLOW_CACHE) return RETURN_FAILURE;
  at_flow->queue.buf[at_flow->queue.end] = (AtCommandLineTypedef *) at_list;
  at_flow->queue.lines[at_flow->queue.end] = list_lines;
  at_flow->queue.fill_status++;
  IncrementIndex(&at_flow->queue.end, 1, MAX_FLOW_CACHE);

  return RETURN_SUCCESS;
}

/**
 * @brief Get the At Cmd From Flow object
 *
 * @param at_flow
 * @return AtCommandLineTypedef*
 */
static AtCommandLineTypedef * GetAtCmdFromFlow(AtCmdFlowTypedef * at_flow)
{
  AtCommandLineTypedef * at_cmd = NULL;

  if (at_flow->queue.fill_status)
  {
    if (at_flow->flow_top >= MAX_FLOW_CACHE) return NULL;

    if (at_flow->flow_top) at_flow->list.active++;

    at_flow->flow_top++;
    at_flow->queue.fill_status--;

    at_flow->list.buf[at_flow->list.active] = at_flow->queue.buf[at_flow->queue.start];
    at_flow->list.lines[at_flow->list.active] = at_flow->queue.lines[at_flow->queue.start];
    at_flow->list.active_line[at_flow->list.active] = 0;
    IncrementIndex(&at_flow->queue.start, 1, MAX_FLOW_CACHE);
  }

  if (at_flow->flow_top)
  {
    at_cmd = &at_flow->list.buf[at_flow->list.active][at_flow->list.active_line[at_flow->list.active]];
  }

  return at_cmd;
}

/**
 * @brief Funkcja przywracajaca poprzedni przeplyw komend AT
 *
 * @param at_flow
 */
static void RestorAtFlow(AtCmdFlowTypedef * at_flow)
{
  // spraedzamy czy mamy aktywna liste komend
  if (at_flow->flow_top)
  {
    //Sprawdzamy czy osiagnelismy koniec skryptu
    if (at_flow->list.active_line[at_flow->list.active] >= at_flow->list.lines[at_flow->list.active])
    {
      at_flow->flow_top--;
      if (at_flow->flow_top)
      {
        // jesli cos pozostalo to schodzmimy pietro nizej na liscie
        at_flow->list.active--;
      }
    }
  }
}

/**
 * @brief Funkcja modyfikujaca przeplyw komend AT
 *
 * @param at_line    - ostatnia wykonana komeda
 * @param behaviore  - jaka reakcja
 * @param at_flow    - kolejka rozkazow
 * @return uint8_t   - RETURN_FAILURE badz RETURN_SUCCESS
 */
static uint8_t ModifyAtCmdFlow(AtCommandLineTypedef * at_line, uint8_t behaviore, AtCmdFlowTypedef * at_flow, uint8_t at_state)
{
  uint16_t index;

  uint16_t i;

  if (!at_flow->flow_top) return RETURN_FAILURE;

  index = at_flow->list.active;

  cmd_delay_timeout = _MS_TICK;
  cmd_delay_period = at_line->delay * 1000;

  do
  {
    switch (behaviore)
    {
      // Po prostu idziemy dalej
      case DEFAULT:
      case IGNORE:
        if (at_flow->list.active_line[index] < at_flow->list.lines[index])
          at_flow->list.active_line[index]++;
      break;

      // konczymy wykonywanie zestawu komend AT
      case EXIT:
        at_flow->list.active_line[index] = at_flow->list.lines[index];
        break;
      // wracamy na poczatek zestawu komend AT
      case REBOOT:
        at_flow->list.active_line[index] = 0;
        break;
      // skok do okreslonej nazwy lini
      case JUMP:
        for (i = 0; i < at_flow->list.lines[index]; i++)
        {
          if (at_flow->list.buf[index][i].label == at_line->action_a)
            break;
        }
        if (i < at_flow->list.lines[index])
        {
          at_flow->list.active_line[index] = i;
        }
        else
        {
          at_flow->list.active_line[index] = 0;
        }
        break;
      // Powtarzanie pojednyjczej komendy
      case REPEAT_UNTIL:
        switch(at_line->action_a)
        {
          // Powtarzanie gdy dostajemy OK
          case OK:
            if (at_state != AT_OK)
            {
              behaviore = DEFAULT;
              continue;
            }
          break;
          // Powtarzanie gdy dostajemy blad
          case ER:
            if (at_state == AT_OK)
            {
              behaviore = DEFAULT;
              continue;
            }
          break;
          // Powtarzanie okreslona liczbe razy
          case NUM:
            if(++at_flow->list.repeat_cnt[index] > at_line->action_a)
            {
              at_flow->list.repeat_cnt[index] = 0;
              behaviore = DEFAULT;
              continue;
            }
          break;
        }
        break;
    }
  } while(0);

  if (at_line->action_a != REPEAT_UNTIL) at_flow->list.repeat_cnt[index] = 0;

  return RETURN_SUCCESS;
}

/**
 * @brief Funkcja kopiujaca dane z bufora okreznego
 *
 * @param cbuf        - wskaznik na bufor
 * @param buffer      - wskzanik na bufor koncowy
 * @param len         - ile danych przekopiowac
 * @return uint16_t   - ile danych skopiowano
 */
static uint16_t CopyFromCircularBuffer(CircularBufferTypedef * cbuf, uint8_t * buffer, uint16_t len)
{
  uint16_t index = cbuf->start;
  uint16_t i;

  for (i = 0; i < len; i++)
  {
    buffer[i] = cbuf->data[index];
    IncrementIndex(&index, 1, cbuf->size);
    if (index == cbuf->end) return i;
  }

  return len;
}

/**
 * @brief Funkcja na biezaco przeszukuje to co odebrano w celu znalezienia delimitera
 *
 * @param recieve_len           - dlugosc otrzymanego ciagu znakow
 * @param cbuf                  - wskaznik na bufor cykliczny
 * @return SearchResultTypedef  - wynik przeszukiwania
 */
static SearchResultTypedef SearchForDelimiterOrPrompt(CircularBufferTypedef *cbuf)
{
  uint16_t limit;

  SearchResultTypedef result;

  limit = 0;

  result.limit = 0;
  result.detected = 0;

  do
  {
    search_bytes[0] = search_bytes[1];
    search_bytes[1] = cbuf->data[cbuf->search];

    if ((_CR == search_bytes[0]) && (_LF == search_bytes[1]))
    {
      if (cbuf->cr_lf < cbuf->start)
        limit = cbuf->cr_lf + (cbuf->size - cbuf->start);
      else
        limit = cbuf->cr_lf - cbuf->start;

      result.limit = limit;
      result.detected = _CRLF;
    }
    else if (_PROMPT == search_bytes[1])
    {
      result.detected = _PROMPT;
    }
    cbuf->cr_lf = cbuf->search;
    IncrementIndex(&cbuf->search, 1, cbuf->size);
  } while ((cbuf->search != cbuf->end) && !result.detected);

  return result;
}

/**
 * @brief Tworzenie komendy AT
 *
 * @param reference_string - ciag wzorcowy
 * @param buffer           - bufor na utowrzony ciag AT
 * @param buffer_size      - zakres bufora
 * @param parameters       - parmetry do wlaczenia w ciag AT
 * @return                 - dlugosc utworzonego ciagu AT
 */
uint16_t AtCmdCreateString(char * reference_string, char * buffer, uint16_t buffer_size, AtCommandParametersTypedef * parameters)
{
  uint16_t i = 0;
  uint16_t index = 0;
  uint32_t number;
  uint32_t number_copy;
  char digit;
  uint8_t max_digits;
  uint8_t min_digits;
  uint32_t decimal_divider;

  char ref_char;

  if (buffer_size < 3) return 0;

  buffer[i++] = 'A';
  buffer[i++] = 'T';

  ref_char = *reference_string++;

  while (_IsVisibleChar(ref_char))
  {
    // sprawdzmy czy nie wyslismy ponad to co jest w buforze parametrow
    //if (index >= parameters->filling)
    //  return 0;
    if (!_IsVisibleChar(ref_char))
      return 0;
    if ('%' == ref_char)
    {
      ref_char = *reference_string++;
      if (!_IsVisibleChar(ref_char)) return 0;
      switch (ref_char)
      {
        case 'd':
          // sprawdzmy czy nie wyslismy ponad to co jest w buforze parametrow
          if ((index + 5) > parameters->filling)
            return 0;
          // Prawdzamy czy w buforze parametrow natrafilismy na liczbe
          if (TAG_NUMBER != parameters->parameters[index++])
            return 0;
          // sprawdzamy czy nie przekroczymy bufora
          if ((i + 4) >= buffer_size)
            return 0;
          // skadamy bajty na jedna liczbe uint32
          number = (uint32_t)parameters->parameters[index++] << 24;
          number |= (uint32_t)parameters->parameters[index++] << 16;
          number |= (uint32_t)parameters->parameters[index++] << 8;
          number |= (uint32_t)parameters->parameters[index++];

          number_copy = number;

          ref_char = *reference_string;

          // jesli po "%d" liczbe, liczba ta okresla minimalna liczbe cyfr
          if (_IsDigit(ref_char))
          {
            min_digits = ref_char - '0';
            reference_string++;
          }
          else
            min_digits = 1;

          max_digits = 10;              // tyle mamy cyfr dla liczby 32-bitowej
          decimal_divider = 1000000000; // maksymalan potega 10 mozliwa do zapisania na 32 bitach
          // wypisz liczbe
          do
          {
            digit = number / decimal_divider;
            number %= decimal_divider;
            max_digits--;
            if (decimal_divider <= number_copy || max_digits < min_digits)
            {
              if (i >= buffer_size) break;
              // wypisz cyfre ASCII
              buffer[i++] = digit + '0';
            }
            decimal_divider /= 10;
          } while (decimal_divider > 0);

          break;

        case 's':
          // sprawdzmy czy nie wyslismy ponad to co jest w buforze parametrow
          if ((index + 3) >= parameters->filling) return 0;
          // Prawdzamy czy w buforze parametrow natrafilismy na lancuch znakowy
          if (TAG_STRING != parameters->parameters[index++])  return 0;
          // wycigamy ile mamy znakow do skopiowania
          number = (uint32_t)parameters->parameters[index++] << 8;
          number |= (uint32_t)parameters->parameters[index++];
          // sprawdzamy czy nie przekroczymy bufora
          // sprawdzmy czy nie wyslismy ponad to co jest w buforze parametrow
          if ((index + number) > parameters->filling) return 0;
          if ((i + number) >= buffer_size) return 0;
          // kopiujemy znaki
          memcpy(&buffer[i], &parameters->parameters[index], number);
          // zwiekszamy indeksy
          i += number;
          index += number;
          break;
      }
    }
    else
    {
      buffer[i++] = ref_char;
      if (i >= buffer_size) return 0;
    }
    ref_char = *reference_string++;
  }

  buffer[i++] = '\r';
  return i;
}

/**
 * @brief Wyciaganie informacji z komendy AT
 *
 * @param reference_string  - ciag wzorcowy
 * @param cbuf              - wskaznik na bufor cyklicznu
 * @param limit             - indeks do ktorego analizowac ciag znakow
 * @param values            - bufor na wyciagniete informacje
 * @return                  - TRUE kiedy mamy zgodnosc lub FALSE przy jej braku
 */
static uint8_t AtCmdParseString(char * reference_string, CircularBufferTypedef *cbuf, uint16_t limit, AtCommandParametersTypedef *values)
{
  uint16_t i = 0;
  uint16_t index = 0;
  uint16_t len_index = 0;
  uint32_t number;

  char in_char;
  char ref_char;

  uint16_t scan_end_index;

  uint8_t comparison = TRUE;

  if (reference_string == NULL) return FALSE;
  // zerujemy wskaznik wypelnienia bufora
  values->filling = 0;

  scan_end_index = cbuf->start;
  i = cbuf->start;
  IncrementIndex(&scan_end_index, limit, cbuf->size);

  do
  {
    in_char = cbuf->data[i];
    if (!_IsVisibleChar(in_char)) return FALSE;
    // Czy mamy zwykle porownywanie
    if (comparison)
    {
      // pobieramy bajt z lancucha referencyjnego
      ref_char = *reference_string++;

      if (']' == ref_char)
      {
        ref_char = *reference_string++;
      }

      if ('\0' == ref_char)
      {
        values->filling = index;
        return TRUE; // koniec wzroca bo np. nie chcemy analizowac dalej
      }

      if (!_IsVisibleChar(ref_char))  return FALSE;

      // czy mammy cos do pobrania z lancucha
      if (ref_char != '%')
      {
        // sprawdzamy czy mamy zgodnosc
        if (ref_char != in_char) return FALSE;
        IncrementIndex(&i, 1, cbuf->size);
      }
      else
      {
        // jakieog typu dane mamy do pobrania
        ref_char = *reference_string++;
        if (!_IsVisibleChar(ref_char)) return FALSE;

        switch (ref_char)
        {
          case 'd':
            // sprawdzamy czy mamy miejsce na wpisanie liczby
            if ((index + 5) >= values->size)
              return FALSE;
            values->parameters[index++] = TAG_NUMBER;
            number = 0;
            // przelanczamy sie na pobieranie danych
            comparison = FALSE;
            break;

          case 's':
            // sprawdzamy czy mamy miejsce na wpisanie chociaz jednego znaku
            if ((index + 4) >= values->size) return FALSE;
            values->parameters[index++] = TAG_STRING;
            number = 0;
            // zachowujemy indeks na na potrzeby zapisania dlugosci
            len_index = index;
            index += 2;
            // przelanczamy sie na pobieranie danych
            comparison = FALSE;
            break;

          default:
            //return FALSE;
            break;
        }
      }
    }
    else
    {
      switch (ref_char)
      {
        case 'd':

          number *= 10;
          number += (in_char - '0');
          IncrementIndex(&i, 1, cbuf->size);

          in_char = cbuf->data[i];
          if (!_IsDigit(in_char) || (i == scan_end_index))
          {
            // gdy napotkamu inny znak niz cyfra zapisujemy przekonwertowana wartosc do bufora
            values->parameters[index++] = number >> 24;
            values->parameters[index++] = number >> 16;
            values->parameters[index++] = number >> 8;
            values->parameters[index++] = number;
            comparison = TRUE;
          }
          break;
        case 's':

          if (index >= values->size)  return FALSE;
          values->parameters[index++] = in_char;
          number++;
          IncrementIndex(&i, 1, cbuf->size);

          in_char = cbuf->data[i];
          if ((i == scan_end_index) || in_char == '"' || in_char == ',')
          {
            values->parameters[len_index++] = number >> 8;
            values->parameters[len_index] = number;
            comparison = TRUE;
          }
          break;

        default:
          return 0;
          break;
      }
    }
  } while (i != scan_end_index);
  ref_char = *reference_string;

  if ('\0' == ref_char || ']' == ref_char)
  {
    values->filling = index;
    return TRUE;
  }
  return FALSE;
}

/**
 * @brief Obsluga odbioru danych z modemu
 *
 * @param cbuf    - wskaznik na bufor cykliczny
 * @return        - ile odebrano danych
 */
static uint16_t AtReciever(CircularBufferTypedef * cbuf)
{
  uint32_t len;

  len = modem_handle.control.rx_len - cbuf->prev_len;
  if (len)
  {
    cbuf->prev_len = modem_handle.control.rx_len;
    cbuf->end = modem_handle.control.rx_index;
  }
  return len;
}

/**
 * @brief Obsluga sekwencji komend AT
 *
 * @param at_cmd  - wskaznik na komende AT
 * @param param   - bufor parametrow
 * @param cbuf    - wskaznik na bufor cykliczny
 *
 */
uint8_t AtCmdSequence(AtCmdFlowTypedef * at_flow, AtCommandParametersTypedef *param, CircularBufferTypedef *cbuf)
{
  SearchResultTypedef search_result;
  uint16_t len;

  // komende mozemy pobrac kiedy zadna komenda nie jset obslugiwana
  if (AT_IDLE == at_state)
  {
    at_cmd = GetAtCmdFromFlow(at_flow);
  }

  recieve_len = AtReciever(cbuf);

  if (recieve_len)
  {
    do
    {
      search_result = SearchForDelimiterOrPrompt(cbuf);
      if (search_result.detected)
      {
        // obsluga prompta np. wyslanie SMS
        if (_PROMPT == search_result.detected)
        {

        }
        // Cos odebralismy, sprawdzmy co
        else if (_CRLF == search_result.detected && search_result.limit)
        {
          if (AT_RESPONSE == at_state)
          {
            // Sprawdzamy czy nie otrzymalismy komunikatu o bledzie
            if (TRUE == AtCheckForErr(at_cmd, param, cbuf, search_result.limit))
            {
              at_state = AT_ERROR;
            }
            // Czy otrzymalismy OK
            else if (TRUE == AtCheckForOk(at_cmd, param, cbuf, search_result.limit))
            {
              at_state = AT_OK;
            }
            else if (EXPECTED_SMS)
            {
              EXPECTED_SMS = 0;
              //if (NULL != sms_buffer_ptr)
              //  CopyFromCircularBuffer(cbuf, sms_buffer_ptr, search_result.limit);
            }
            else if (at_cmd->ref_str_in != NULL)
            {
              // ITR jest sprawdzany jako ostani zachowujemy jego stan na potrzeby OK
              itr_status = AtCheckForItr(at_cmd, param, cbuf, search_result.limit);
            }
          }
          else
          {
            AtCheckForUrc(param, cbuf, search_result.limit);
          }
        }
        cbuf->start = cbuf->search;
      }
    } while (cbuf->search != cbuf->end);
  }

  if (NULL == at_cmd) return RETURN_SUCCESS;

  switch (at_state)
  {
    case AT_IDLE:
      if (((_MS_TICK - cmd_delay_timeout) > cmd_delay_period) || (at_flow->flow_top > prev_top_flow))
      {
        prev_top_flow = at_flow->flow_top;
        at_state = AT_SEND;
      }
      break;

    case AT_SEND:
      //Kolejna komenda moze byc wyslana dopiero po jakims czasie
      if ((_MS_TICK - cmd_time_interval) > _CMD_TIME_INTERVAL)
      {
        param->filling = 0;
        if (at_cmd->fun != NULL) at_cmd->fun(AT_SEND_STAGE, param);
        len = AtCmdCreateString(at_cmd->ref_str_out, at_out_buf, sizeof(at_out_buf), param);
        if (len > 0)
        {
          cmd_timeout = _MS_TICK;
          at_state = AT_RESPONSE;
          UartWrite(&modem_handle, (uint8_t *)at_out_buf, len);
        }
        else
        {
          _DP("AT cmommand lengh = 0\n");
          at_state = AT_IDLE;
        }
      }
      break;

    case AT_RESPONSE:
      if ((_MS_TICK - cmd_timeout) > (at_cmd->cmd_timeout * 1000))
      {
        at_state = AT_IDLE;
        return RETURN_FAILURE;
      }
      break;

    case AT_OK:
      at_state = AT_END;
      ModifyAtCmdFlow(at_cmd,at_cmd->behaviore, at_flow, AT_OK);
      break;

    case AT_ERROR:
      at_state = AT_END;
      ModifyAtCmdFlow(at_cmd,at_cmd->behaviore_err, at_flow, AT_ERROR);
      break;
  }

  if (AT_END == at_state)
  {
    RestorAtFlow(at_flow);
    cmd_time_interval = _MS_TICK;
    at_state = AT_IDLE;
  }
  return RETURN_SUCCESS;
}


/**
 * @brief  Sprawdzenie czy odebralismy "OK"
 *
 * @param at_cmd   - wskaznik na komende AT
 * @param param    - bufor parametrow
 * @param cbuf     - wskaznik na bufor cykliczny
 * @param limit    - zakres pprzeszukiwania
 * @return uint8_t - TRUE lub FALSE
 */
static uint8_t AtCheckForOk(AtCommandLineTypedef *at_cmd, AtCommandParametersTypedef *param, CircularBufferTypedef *cbuf, uint16_t limit)
{
  if (TRUE == AtCmdParseString("OK", cbuf, limit, param))
  {
    if (at_cmd->fun != NULL)
    {
      at_cmd->fun(AT_OK_STAGE, param);
    }
    return TRUE;
  }
  return FALSE;
}

/**
 * @brief  Sprawdzenie czy odebralismy blad
 *
 * @param at_cmd   - wskaznik na komende AT
 * @param param    - bufor parametrow
 * @param cbuf     - wskaznik na bufor cykliczny
 * @param limit    - zakres pprzeszukiwania
 * @return uint8_t - TRUE lub FALSE
 */
static uint8_t AtCheckForErr(AtCommandLineTypedef * at_cmd, AtCommandParametersTypedef * param, CircularBufferTypedef * cbuf, uint16_t limit)
{
  uint8_t i;

  for (i = 0; i < scripts.error->cmd_num; i++)
  {
    if (TRUE == AtCmdParseString(scripts.error->str_lis[i].string, cbuf, limit, param))
    {
      if (at_cmd->fun != NULL)
      {
        at_cmd->fun(AT_ERROR_STAGE, param);
      }
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * @brief  Sprawdzenie czy odebralismy URC
 *
 * @param at_cmd   - wskaznik na komende AT
 * @param param    - bufor parametrow
 * @param cbuf     - wskaznik na bufor cykliczny
 * @param limit    - zakres pprzeszukiwania
 * @return uint8_t - TRUE lub FALSE
 */
static uint8_t AtCheckForUrc(AtCommandParametersTypedef * param, CircularBufferTypedef *cbuf, uint16_t limit)
{
  uint8_t i;
  for (i = 0; i < scripts.urc->cmd_num; i++)
  {
    if (TRUE == AtCmdParseString(scripts.urc->str_lis[i].string, cbuf, limit, param))
    {
      if (scripts.urc->str_lis[i].fun != NULL)
      {
        scripts.urc->str_lis[i].fun(AT_URC_STAGE, param);
      }
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * @brief  Sprawdzenie czy odebralismy ITR
 *
 * @param at_cmd   - wskaznik na komende AT
 * @param param    - bufor parametrow
 * @param cbuf     - wskaznik na bufor cykliczny
 * @param limit    - zakres pprzeszukiwania
 * @return uint8_t - TRUE lub FALSE
 */
static uint8_t AtCheckForItr(AtCommandLineTypedef *at_cmd, AtCommandParametersTypedef *param,
                             CircularBufferTypedef *cbuf, uint16_t limit)
{
  if (TRUE == AtCmdParseString(at_cmd->ref_str_in, cbuf, limit, param))
  {
    if (at_cmd->fun != NULL)
    {
      at_cmd->fun(AT_ITR_STAGE, param);
    }
    return TRUE;
  }
  return FALSE;
}

/**
 * @brief Funkcja informujaca ze modem jest gotowy do pracy
 *
 * @return uint8_t TRUE  - modem gotowy
 *                 FALSE - modem nie gotowy
 */
uint8_t ModemGsmReady(void)
{
  return BASE_CONFIG_DONE;
}
