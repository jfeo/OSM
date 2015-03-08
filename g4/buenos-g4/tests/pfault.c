/*
 * Userland helloworld
 */

#include "tests/lib.h"

int main(void)
{
  /* Allocate a program that fills > 4096 bytes (=1 page size) */
  int p[1022], i;
  i=i;
  *p=*p;
  //*((int*)44) = 1;
  //puts("Allocated 4096 bytes successfully\n");
  return 0;
}
