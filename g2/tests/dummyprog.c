#include "tests/lib.h"

int main(void)
{
    //syscall_write(stdout, "Hello world! /Dummy\n", 20);
    //syscall_halt();
    int i;
    for(i = 1000000; i > 0 ; ) --i;
    return 0;
}
