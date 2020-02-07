#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "words.h"

void words_add(struct forth *forth)
{
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
