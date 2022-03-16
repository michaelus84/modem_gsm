#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "at_engine.h"

int main()
{
  ModemRsInit();

  while(1)
  {
    ModemGsmModule();
    usleep(1000);
  }
  return 0;
}
