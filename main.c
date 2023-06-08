#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "at_engine.h"

int main(void)
{
  ModemInit();

  while(1)
  {
    ModemGsmModule();
    if (ModemGsmReady())
    {
      printf("Modem GSM ready\n");
      ModemClosePort();
      return 0;
    }
    usleep(1000);
  }
  return 0;
}
