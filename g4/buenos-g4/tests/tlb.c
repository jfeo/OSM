#include "lib.h"
int a[3000];

int main() {
  int i;
  int p[10000];
  for (i = 0; i < 3000; i++) {
    a[i] = i;
  }
  for (i = 2900; i < 3000; i++) {
    printf("i = %d\n", a[i]);
  }

  return p[0];
}
