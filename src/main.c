#include "forth.h"
#include "words.h"

#include <stdio.h>

#define MAX_DATA 16384
#define MAX_STACK 16384
#define MAX_RETURN 16384

int main(void)
{
    struct forth forth = {0};
    forth_init(&forth, stdin, MAX_DATA, MAX_STACK, MAX_RETURN);
    words_add(&forth);
    forth_run(&forth);
    return 0;
}
