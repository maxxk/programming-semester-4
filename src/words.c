#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "words.h"

void words_add(struct forth *forth)
{
    int status = 0;
    static const char* square[] = { "dup", "*", "exit", 0 };

    forth_add_codeword(forth, "interpret", interpreter_stub);
    forth->stopword = forth->latest;
    forth->executing = &forth->stopword;
    forth_add_codeword(forth, "drop", drop);
    forth_add_codeword(forth, "dup", _dup);
    forth_add_codeword(forth, "+", add);
    forth_add_codeword(forth, "-", sub);
    forth_add_codeword(forth, "*", mul);
    forth_add_codeword(forth, "/", _div);
    forth_add_codeword(forth, "%", mod);
    forth_add_codeword(forth, "swap", swap);
    forth_add_codeword(forth, "rot", rot);
    forth_add_codeword(forth, "-rot", rot_back);
    forth_add_codeword(forth, "show", show);
    forth_add_codeword(forth, "over", over);
    forth_add_codeword(forth, "true", _true);
    forth_add_codeword(forth, "false", _false);
    forth_add_codeword(forth, "xor", _xor);
    forth_add_codeword(forth, "or", _or);
    forth_add_codeword(forth, "and", _and);
    forth_add_codeword(forth, "not", _not);
    forth_add_codeword(forth, "=", _eq);
    forth_add_codeword(forth, "<", lt);
    forth_add_codeword(forth, "within", within);

    forth_add_codeword(forth, "exit", forth_exit);
    forth_add_codeword(forth, "lit", literal);
    forth_add_codeword(forth, ":", compile_start);
    forth_add_codeword(forth, ";", compile_end);
    forth->latest->immediate = true;
    forth_add_codeword(forth, "'", literal);

    forth_add_codeword(forth, ">r", rpush);
    forth_add_codeword(forth, "r>", rpop);
    forth_add_codeword(forth, "i", rtop);
    forth_add_codeword(forth, "rshow", rshow);
    forth_add_codeword(forth, "@", memory_read);
    forth_add_codeword(forth, "!", memory_write);
    forth_add_codeword(forth, "here", here);
    forth_add_codeword(forth, "branch", branch);
    forth_add_codeword(forth, "0branch", branch0);
    forth_add_codeword(forth, "immediate", immediate);
    forth->latest->immediate = true;

    forth_add_codeword(forth, "word", next_word);
    forth_add_codeword(forth, ">cfa", _word_code);
    forth_add_codeword(forth, "find", find);
    forth_add_codeword(forth, ",", comma);
    forth_add_codeword(forth, "next", next);

    forth_add_codeword(forth, "save", save_words);
    forth_add_codeword(forth, "load", load_words);

    forth_add_codeword(forth, "see", decompile_words);

    status = forth_add_compileword(forth, "square", square);
    assert(!status);
}

void decompile_words(struct forth* forth) {
    char buffer[MAX_WORD+1];
    size_t length;
    char* piece_of_complex_word;
    int i = 0;

    read_word(forth->input, MAX_WORD, buffer, &length);
    uint8_t u_length = (uint8_t)length;

    const struct word *x_word = word_find(forth->latest, u_length, buffer);
    if (x_word == NULL){
        printf("unknown word %s \n", buffer);
    } else {
        printf("%s : ", x_word->name);
        if(!x_word->compiled) {
            printf("It is codeword, its pointer is %ld", *(cell*)(x_word));
        } else {
            piece_of_complex_word = ((struct word**)word_code(x_word))[0]->name;

            while(strcmp(piece_of_complex_word, "exit") != 0){
                i++;
                if(strcmp(piece_of_complex_word, "lit") == 0){
                    printf("%ld ", ((cell*)word_code(x_word))[i]);
                    i++;
                } else {
                    printf("%s ", piece_of_complex_word);
                }
                piece_of_complex_word = ((struct word**)word_code(x_word))[i]->name;
            }
        }
        printf("\n");
    }

}

void save_words(struct forth *forth) {
    char buffer[MAX_WORD+1];
    size_t length;

    read_word(forth->input, MAX_WORD, buffer, &length);

    FILE *file = fopen(buffer, "w");
    int i;

    struct word *last_word = forth->latest;
    struct word *cur;

    while (  strcmp(last_word->name, "square") != 0  ) {
        if (!last_word->compiled) {
            last_word = last_word->next;
            continue;
        }

        fprintf(file, "%s :  ", last_word->name);
        i = 0;
        cur = (struct word*)(((cell*)word_code(last_word))+i);
        while( strcmp( cur->next->name, "exit") != 0 ) {
            fprintf(file, "%s ", cur->next->name);
            i++;
            cur = (struct word*)(((cell*)word_code(last_word))+i);
        }
        fprintf(file, ";\n");
        last_word = last_word->next;
    }
    fclose(file);
}

void load_words(struct forth *forth) {
    char buffer[MAX_WORD+1];
    size_t length;
    FILE *file;

    int i;
    int status = 0;
    int words_number = 0;
    bool search_for_new_word = true;

    char *name;
    char **words;


    read_word(forth->input, MAX_WORD, buffer, &length);

    file = fopen(buffer, "r");

    while (fscanf(file, "%s", buffer) != EOF) {
        if (!strcmp(buffer, ":")) {
            //Ничего не делаем, идем дальше
        } else {
            if (search_for_new_word) {
                name = (char*)malloc(MAX_WORD*sizeof(char));
                words = (char**)malloc(150*sizeof(char*));
                for (i = 0; i < 150; i++) {
                    words[i] = (char*)malloc(MAX_WORD*sizeof(char));
                }

                strncpy( name, buffer, MAX_WORD);
                name[MAX_WORD-1]=0;
                printf("name: %s\n", name);
                words_number = 0;
                search_for_new_word = false;

            } else {
                if (!strcmp(buffer, ";")) {
                    //В конец ставим exit, остальные зануляем
                    words[words_number] = "exit";
                    for (i = (words_number+1); i < 150; i++) {
                        words[i] = 0;
                    }
                    status = forth_add_compileword(forth, (const char*)name, (const char**)words);
                    assert(!status);
                    free(name);
                    free(words);

                    search_for_new_word = true;
                } else {
                    for (i = 0; i < (int)strlen(buffer); i++) {
                        words[words_number][i] = buffer[i];
                    }
                    printf("\t\t%s\n", words[words_number]);
                    words_number++;
                }
            }
        }

    }

}


void drop(struct forth *forth) {
    forth_pop(forth);
}

void _dup(struct forth *forth) {
    forth_push(forth, *forth_top(forth));
}

void add(struct forth *forth) {
    cell a, b;
    b = forth_pop(forth);
    a = forth_pop(forth);
    forth_push(forth, a + b);
}

void sub(struct forth *forth) {
    cell a, b;
    b = forth_pop(forth);
    a = forth_pop(forth);
    forth_push(forth, a - b);
}

void mul(struct forth *forth) {
    cell a, b;
    b = forth_pop(forth);
    a = forth_pop(forth);
    forth_push(forth, a * b);
}

void _div(struct forth *forth) {
    cell a, b;
    b = forth_pop(forth);
    a = forth_pop(forth);
    forth_push(forth, a / b);
}

void mod(struct forth *forth) {
    cell a, b;
    b = forth_pop(forth);
    a = forth_pop(forth);
    forth_push(forth, a % b);
}

void swap(struct forth *forth) {
    cell a, b;
    b = forth_pop(forth);
    a = forth_pop(forth);
    forth_push(forth, b);
    forth_push(forth, a);
}

void rot_back(struct forth *forth) {
    cell a, b, c;
    c = forth_pop(forth);
    b = forth_pop(forth);
    a = forth_pop(forth);
    forth_push(forth, c);
    forth_push(forth, a);
    forth_push(forth, b);
}

void rot(struct forth *forth) {
    cell a, b, c;
    c = forth_pop(forth);
    b = forth_pop(forth);
    a = forth_pop(forth);
    forth_push(forth, b);
    forth_push(forth, c);
    forth_push(forth, a);
}

void show(struct forth *forth) {
    const cell *c = forth->sp0;
    while (c <= forth_top(forth)) {
        cell_print(*c);
        c += 1;
    }
    printf("(top)\n");
}

void over(struct forth *forth) {
    assert(forth_top(forth) - 1 >= forth->sp0);
    forth_push(forth, *(forth_top(forth)-1));
}

void _true(struct forth *forth) {
    forth_push(forth, -1);
}

void _false(struct forth *forth) {
    forth_push(forth, 0);
}

void _xor(struct forth *forth) {
    cell a, b;
    b = forth_pop(forth);
    a = forth_pop(forth);
    forth_push(forth, a ^ b);
}

void _or(struct forth *forth) {
    cell a, b;
    b = forth_pop(forth);
    a = forth_pop(forth);
    forth_push(forth, a | b);
}

void _and(struct forth *forth) {
    cell a, b;
    b = forth_pop(forth);
    a = forth_pop(forth);
    forth_push(forth, a & b);
}

void _not(struct forth *forth) {
    cell c = forth_pop(forth);
    forth_push(forth, ~c);
}

void _eq(struct forth *forth) {
    cell a, b;
    b = forth_pop(forth);
    a = forth_pop(forth);
    forth_push(forth, a == b ? -1 : 0);
}

void lt(struct forth *forth) {
    cell a, b;
    b = forth_pop(forth);
    a = forth_pop(forth);
    forth_push(forth, a < b ? -1 : 0);
}

void within(struct forth *forth) {
    cell a, l, r;
    r = forth_pop(forth);
    l = forth_pop(forth);
    a = forth_pop(forth);
    forth_push(forth, l <= a && a < r ? -1 : 0);
}

void forth_exit(struct forth *forth) {
    cell return_address = forth_pop_return(forth);
    forth->executing = (struct word**)return_address;
}

void literal(struct forth *forth)
{
    cell value = *(cell*)forth->executing;
    forth->executing += 1;
    forth_push(forth, value);
}

void compile_start(struct forth *forth)
{
    char buffer[MAX_WORD+1];
    size_t length;
    struct word* word;

    read_word(forth->input, MAX_WORD, buffer, &length);
    assert(length > 0);

    word = word_add(forth, (uint8_t)length, buffer);
    forth->is_compiling = true;
    word->hidden = true;
    word->compiled = true;
}

void compile_end(struct forth *forth)
{
    const struct word *exit = word_find(forth->latest, strlen("exit"), "exit");
    assert(exit);
    forth_emit(forth, (cell)exit);
    forth->is_compiling = false;
    forth->latest->hidden = false;
}

void rpush(struct forth *forth) {
    cell a = forth_pop(forth);
    forth_push_return(forth, a);
}

void rpop(struct forth *forth) {
    forth_push(forth, forth_pop_return(forth));
}

void rtop(struct forth  *forth) {
    assert(forth->rp > forth->rp0+1);
    forth_push(forth, forth->rp[-2]);
}

void rshow(struct forth *forth) {
    const cell *c = forth->rp0;
    while (c < forth->rp) {
        cell_print(*c);
        c += 1;
    }
    printf("(r-top)\n");
}


void memory_read(struct forth *forth)
{
    cell address = forth_pop(forth);
    forth_push(forth, *(cell*)address);
}

void memory_write(struct forth *forth)
{
    cell address = forth_pop(forth);
    cell* address_as_pointer = (cell*)address;
    cell value = forth_pop(forth);
    *address_as_pointer = value;
}

void here(struct forth *forth)
{
    forth_push(forth, (cell)&forth->memory_free);
}

void branch(struct forth *forth)
{
    forth->executing += ((size_t)forth->executing[0])/sizeof(cell);
}

void branch0(struct forth *forth)
{
    cell offset = *(cell*)(forth->executing);
    cell value = forth_pop(forth);
    if (!value) {
        forth->executing += offset / sizeof(cell);
    } else {
        forth->executing += 1;
    }
}

void immediate(struct forth *forth)
{
    forth->latest->immediate = !forth->latest->immediate;
}

void next_word(struct forth *forth)
{
    size_t length;
    static char buffer[MAX_WORD+1];
    read_word(forth->input, MAX_WORD+1, buffer, &length);
    forth_push(forth, (cell)buffer);
    forth_push(forth, (cell)length);
}

void find(struct forth *forth)
{
    cell length = forth_pop(forth);
    cell name_address = forth_pop(forth);
    const char *name = (const char *)name_address;
    const struct word *word = word_find(forth->latest, length, name);
    forth_push(forth, (cell)word);
}

void _word_code(struct forth *forth)
{
    cell word_address = forth_pop(forth);
    const struct word* word = (const struct word*)word_address;
    const void *code = word_code(word);
    forth_push(forth, (cell)code);
}

void comma(struct forth *forth)
{
    forth_emit(forth, forth_pop(forth));
}

void next(struct forth *forth)
{
    forth->executing += 1;
}

void interpreter_stub(struct forth *forth)
{
    (void)forth;
    printf("ERROR: return stack underflow (must exit to interpreter)\n");
    exit(2);
}
