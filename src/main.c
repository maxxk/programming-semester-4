#include "forth.h"
#include "words.h"

#include <stdio.h>

#define MAX_DATA 16384
#define MAX_STACK 16384
#define MAX_RETURN 16384

int main(int argc, char** argv)
{
    FILE *input;
    int i = 2;
    struct forth forth = {0};
    forth_init(&forth, stdin, MAX_DATA, MAX_STACK, MAX_RETURN);
    words_add(&forth);    
    
    if (argc != 1){
        
        if((input = fopen(argv[1], "r")) == NULL){
            if(*argv[1] == '-'){
                printf("Can't open file ""-"", if you want to run program without input files, don't use ""'-'"" in the end\n");
            }
            else{
                printf("Can't open file %s\n", argv[1]);
            }
            return 0;
        }
        forth.input = input;
        
        forth_run(&forth);
        fclose(input);
        
        while (argv[i] != 0){
            if(*argv[i] != '-'){
                if((input = fopen(argv[i], "r")) == NULL){
                    printf("Can't open file %s\n", argv[i]);
                    return 0;
                }
                forth.input = input;
                 
                forth_run(&forth);
                fclose(input);
            }
            else{
                forth.input = stdin;
                
                forth_run(&forth);
            }
            i++;
        }
        
        
        
    } else {
        
        
        forth_run(&forth);
        
        
    }
    forth_free(&forth);
    return 0;
}