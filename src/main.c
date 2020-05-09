#include "forth.h"
#include "words.h"
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#define MAX_DATA 16384
#define MAX_STACK 16384
#define MAX_RETURN 16384

int main(int argc, char** argv)
{
    FILE *_file_;
    int i = 2;

    struct forth forth = {0};
    if(argc == 1) {
        forth_init(&forth, stdin, MAX_DATA, MAX_STACK, MAX_RETURN);
        words_add(&forth);
        forth_run(&forth);
        forth_free(&forth);
    } else {
        if((strcmp(argv[1], "-") != 0) && ( (_file_ = fopen(argv[1],"r")) == NULL) ){
                printf("Can't open a file %s\n",argv[1]);
                return 0;
        }
        forth_init(&forth, _file_, MAX_DATA, MAX_STACK, MAX_RETURN);
        words_add(&forth);
        forth_run(&forth);
        fclose(_file_);

        /* Читаем до упора все что есть, пока не закончатся аргументы. Если последний "-", то начинаем считывать с клавиатуры.
        Если встретили "-" где-то между именами файлов, то просто игнорируем его. */
        for ( i = 2; i < argc; i++ ){
            if(strcmp(argv[i], "-") != 0){
                if((_file_ = fopen(argv[i], "r") )== NULL){
                    printf("Can't open a file %s\n",argv[i]);
                    return 0;
                }
                forth.input = _file_;
                forth_run(&forth);
                fclose(_file_);
            }
        }
        if (strcmp(argv[argc - 1], "-") == 0){
            forth.input = stdin;
            forth_run(&forth);
        }
        forth_free(&forth);
    }
    return 0;
}
