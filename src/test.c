#include "minunit.h"
#include "forth.test.c"

int main(int argc, char *argv[]) {
	MU_RUN_SUITE(forth_tests);
	MU_REPORT();
	return MU_EXIT_CODE;
}
