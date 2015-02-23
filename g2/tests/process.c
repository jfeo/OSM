#include "tests/lib.h"

int main(void)
{
    syscall_exec("dummyprog");
    syscall_write(stdout, "Dummy program finished.\n", 20);
    return 0;
}
