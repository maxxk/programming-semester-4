#include <stdio.h>
#include <assert.h>
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
        printf(";\n");
    }

}

struct word_stack{
    struct word * wordptr;
    struct word_stack * next;
};

void save_words(struct forth *forth) {
    char buffer[MAX_WORD+1];
    size_t length;

    read_word(forth->input, MAX_WORD, buffer, &length);

    FILE *file = fopen(buffer, "w");
    int i;

    struct word_stack *stack = NULL;
    struct word *last_word = forth->latest;
    struct word *cur;
    struct word_stack *newelem;

    while ( last_word->next ) {
        if ( last_word->compiled ) {
            if ( stack ) {
                newelem = malloc( sizeof( struct word_stack ) );
                newelem->wordptr = last_word;
                newelem->next = stack;
                stack = newelem;
            } else {
                stack = malloc( sizeof( struct word_stack ) );
                stack->wordptr = last_word;
                stack->next = NULL;
            }
        }
        last_word = last_word->next;
    }
    while ( stack ) {
        last_word = stack->wordptr; 
        fprintf(file, "%s :  ", last_word->name);
        i = 0;
        cur = (struct word*)(((cell*)word_code(last_word))+i);
        while( strcmp( cur->next->name, "exit") != 0 ) {
            fprintf(file, "%s ", cur->next->name);
            i++;
            cur = (struct word*)(((cell*)word_code(last_word))+i);
        }
        fprintf(file, ";\n");
        newelem = stack;
        stack = stack->next;
        free( newelem );
    }
    fclose(file);
}

struct word_list {
        char * wordptr;
        struct word_list * next;
};

static
struct word_list *
word_list_push_end(
    struct word_list * end,
    char * new_elem ) {
        if ( !end ) {
            end = malloc( sizeof( struct word_list ) );
            end->next = NULL;
            end->wordptr = new_elem;
            return end;
        } else {
            end->next = malloc( sizeof( struct word_list ) );
            end->next->wordptr = new_elem;
            end->next->next = NULL;
            return end->next;
        }
}

static
struct word_list *
word_list_pop_begin(
    struct word_list * head ) {
        struct word_list *copy;

        if ( !head ) return NULL;
        copy = head;
        head = head->next;
        free(copy);
        return head;
}


void load_words(struct forth *forth) {
    char buffer[MAX_WORD+1];
    size_t length;
    char format[12];
    FILE *file;
    char *wordbuf;

    int status = 0;
    int words_number = 0;
    bool search_for_new_word = true;
    int i;

    char *name;
    char **words;

    struct word_list *list_of_words = NULL;
    struct word_list * words_last = NULL;


    read_word(forth->input, MAX_WORD, buffer, &length);

    file = fopen(buffer, "r");
    snprintf( format, sizeof(format), "%%%ds", MAX_WORD+1 );
    while ( fscanf( file, format, buffer ) != EOF ) {
        if (!strcmp(buffer, ":")) {
            //Ничего не делаем, идем дальше
        } else {
            if (search_for_new_word) {
                name = (char*)malloc(MAX_WORD*sizeof(char));
                list_of_words = NULL;
                words_last = NULL;
                //words = (char**)malloc(150*sizeof(char*));
                strncpy( name, buffer, MAX_WORD);
                name[MAX_WORD-1]=0;
                printf("name: %s\n", name);
                words_number = 0;
                search_for_new_word = false;

            } else {
                if (!strcmp(buffer, ";")) {
                    //В конец ставим exit, остальные зануляем
                    wordbuf = malloc( MAX_WORD * sizeof(char) );
                    snprintf( wordbuf, MAX_WORD, "exit" );
                    words_last = word_list_push_end( words_last, wordbuf );
                    if ( !list_of_words ) {
                        list_of_words = words_last;
                    }
                    words_number++;
                    words = malloc( (words_number+1) * sizeof( char* ) );
                    i = 0;
                    while ( ( list_of_words ) && ( i < words_number ) ) {
                        words[i] = list_of_words->wordptr;
                        i++;
                        list_of_words = word_list_pop_begin( list_of_words );
                        if ( !list_of_words ) {
                            words_last = NULL;
                        }
                    }
                    words[ words_number ] = 0;
                    status = forth_add_compileword(forth, (const char*)name, (const char**)words);
                    assert(!status);
                    free(name);
                    free(words);

                    search_for_new_word = true;
                } else {
                    //printf("%s", buffer );
                    wordbuf = malloc( MAX_WORD * sizeof(char) );
                    strncpy( wordbuf, buffer, MAX_WORD );
                    words_last = word_list_push_end( words_last, wordbuf );
                    if ( !list_of_words ) {
                        list_of_words = words_last;
                    }
                    printf("\t\t%s\n", wordbuf);
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
