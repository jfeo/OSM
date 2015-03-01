#include "tests/lib.h"

int main(void) {
	usr_sem_t *s = syscall_sem_open("simon", 0);
	printf("Semaphore should not be NULL: %p\n", s);
	
	usr_sem_t *null = syscall_sem_open("simon", 0);
	printf("Semaphore should be NULL: %p\n", null);

	usr_sem_t *s2 = syscall_sem_open("simon", -1);
	printf("Semaphore should be %p: %p\n", s, s2);

	usr_sem_t *s3 = syscall_sem_open("diku", 2);
	printf("Semaphore should not be NULL and not be %p: %p\n", s, s3);

	return 0;
}
