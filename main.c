#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "at_engine.h"
#include "platform.h"

int main(void)
{
  ModemInit();

  while(1)
  {
    ModemGsmModule();
    usleep(1000);
  }
  GpioCloseAll();
  return 0;
}
