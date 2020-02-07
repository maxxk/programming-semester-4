#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "words.h"

void words_add(struct forth *forth)
{
    int status = 0;
    static const char* square[] = { "dup", "*", "exit" };

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
    
    status = forth_add_compileword(forth, "square", square);
    assert(!status);
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
    forth->executing = (struct word**)forth_pop_return(forth);
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

void interpreter_stub(struct forth *forth)
{
    (void)forth;
    printf("ERROR: return stack underflow (must exit to interpreter)\n");
    exit(2);
}
