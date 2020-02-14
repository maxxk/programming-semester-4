#include "forth.h"
#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static uintptr_t align(uintptr_t value, uint8_t alignment);
static intptr_t strtoiptr(const char* ptr, char** endptr, int base);

int forth_init(struct forth *forth, FILE *input,
    size_t memory, size_t stack)
{
    forth->memory_size = memory;
    forth->memory = malloc(forth->memory_size * sizeof(cell));
    forth->memory_free = forth->memory;

    forth->data_size = stack;
    forth->sp0 = malloc(forth->data_size * sizeof(cell));
    forth->sp = forth->sp0;

    forth->latest = NULL;
    forth->input = input;

    return forth->memory == NULL || forth->sp0 == NULL;
}

void forth_free(struct forth *forth)
{
    free(forth->sp0);
    free(forth->memory);
    *forth = (struct forth) {0};
}

void forth_push(struct forth *forth, cell value)
{
    assert(forth->sp < forth->sp0 + forth->data_size);
    *(forth->sp) = value;
    forth->sp += 1;
}

cell forth_pop(struct forth *forth)
{
    assert(forth->sp > forth->sp0);
    forth->sp -= 1;
    return *forth->sp;
}

void forth_emit(struct forth *forth, cell value)
{
    *(forth->memory_free) = value;
    forth->memory_free += 1;
}

cell* forth_top(struct forth *forth) {
    return forth->sp-1;
}

struct word* word_add(struct forth *forth,
    uint8_t length, const char name[length])
{
    struct word* word = (struct word*)forth->memory_free;
    word->next = forth->latest;
    word->length = length;
    memcpy(word->name, name, length);
    forth->memory_free = (cell*)word_code(word);
    assert((char*)forth->memory_free >= word->name + length);
    forth->latest = word;
    return word;
}

const void* word_code(const struct word *word)
{
    uintptr_t size = align(sizeof(struct word) + 1 + word->length, sizeof(cell));
    return (const void*)((uint8_t*)word + size);
}

const struct word* word_find(const struct word* word,
    uint8_t length, const char name[length])
{
    while (word) {
        if (length == word->length
            && ! strncmp(word->name, name, length)) {
            return word;
        }
        word = word->next;
    }
    return NULL;
}

static uintptr_t align(uintptr_t value, uint8_t alignment)
{
    return ((value - 1) | (alignment - 1)) + 1;
}

void forth_add_codeword(struct forth *forth,
    const char* name, const function handler)
{
    word_add(forth, strlen(name), name);
    assert(strlen(name) <= 32);
    forth_emit(forth, (cell)handler);
}


void cell_print(cell cell) {
    printf("%"PRIdPTR" ", cell);
}

enum forth_result read_word(FILE* source,
    size_t buffer_size, char buffer[buffer_size], size_t *length)
{
    size_t l = 0;
    int c; 
    while ((c = fgetc(source)) != EOF && l < buffer_size) {
        // isspace(c) → l == 0
        if (isspace(c)) {
            if (l == 0) {
                continue;
            } else {
                break;
            }
        }
        buffer[l] = c;
        l += 1;
    }

    if (l > 0 && l < buffer_size) {
        *length = l;
        buffer[l] = 0;
        return FORTH_OK;
    }

    if (l >= buffer_size) {
        return FORTH_BUFFER_OVERFLOW;
    }
    
    return FORTH_EOF;
}

static void forth_run_number(struct forth *forth,
    size_t length, const char word_buffer[length]);

enum forth_result forth_run(struct forth* forth)
{
    size_t length;
    enum forth_result read_result;
    char word_buffer[MAX_WORD+1] = {0};

    while ((read_result = read_word(
            forth->input,
            sizeof(word_buffer),
            word_buffer,
            &length
        )) == FORTH_OK) {

        const struct word* word = word_find(forth->latest, length, word_buffer);
        if (word == NULL) {
            forth_run_number(forth, length, word_buffer);
        } else {
            // ISO C forbids conversion of object pointer to function pointer type
            const function code = ((struct { function fn; }*)word_code(word))->fn;
            code(forth);
        }
    }
    return read_result;
}

static void forth_run_number(struct forth *forth,
    size_t length, const char word_buffer[length])
{
    char* end;
    intptr_t number = strtoiptr(word_buffer, &end, 10); // FIXME: BASE can be internal variable
    if (end - word_buffer < (int)length) {
        fprintf(stderr, "Unknown word: '%.*s'\n", (int)length, word_buffer);
    } else {
        forth_push(forth, number);
    }
}

static intptr_t strtoiptr(const char* ptr, char** endptr, int base) {
    if (sizeof(intptr_t) <= sizeof(long)) {
        return (intptr_t)strtol(ptr, endptr, base);
    } else {
        return (intptr_t)strtoll(ptr, endptr, base);
    }
}
