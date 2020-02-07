#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

struct forth;
typedef intptr_t cell;

#define MAX_WORD 32

struct word {
    struct word *next;
    bool compiled;
    bool hidden;
    bool immediate;
    uint8_t length;
    char name[];
};

struct forth {
    struct word **executing;
    cell *sp;
    cell *rp;
    cell *memory;
    struct word *latest;
    struct word *stopword;
    bool is_compiling;

    FILE* input;

    cell *memory_free;
    cell *sp0;
    cell *rp0;
    size_t memory_size;
    size_t return_size;
    size_t data_size;
};


struct word* word_add(struct forth *forth,
    uint8_t length, const char name[length]);
const void* word_code(const struct word *word);
const struct word* word_find(const struct word* first,
    uint8_t length, const char name[length]);

typedef void (*function)(struct forth *forth);

int forth_init(struct forth *forth, FILE* input,
    size_t memory, size_t stack, size_t ret);
void forth_free(struct forth *forth);

void forth_push(struct forth *forth, cell value);
cell forth_pop(struct forth *forth);
cell* forth_top(struct forth *forth);
void forth_push_return(struct forth *forth, cell value);
cell forth_pop_return(struct forth *forth);
void forth_emit(struct forth *forth, cell value);
void forth_add_codeword(struct forth *forth,
    const char* name, const function handler);
int forth_add_compileword(struct forth *forth,
    const char* name, const char** words);

void cell_print(cell c);

enum forth_result {
    FORTH_OK,
    FORTH_EOF,
    FORTH_WORD_NOT_FOUND,
    FORTH_BUFFER_OVERFLOW,
};

enum forth_result read_word(FILE* source,
    size_t buffer_size, char buffer[buffer_size], size_t *length);

enum forth_result forth_run(struct forth* forth);
