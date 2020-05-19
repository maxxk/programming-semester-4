#pragma once

#include "forth.h"

void words_add(struct forth *forth);

void drop(struct forth *forth);
void _dup(struct forth *forth);
void add(struct forth *forth);
void sub(struct forth *forth);
void mul(struct forth *forth);
void _div(struct forth *forth);
void mod(struct forth *forth);
void swap(struct forth *forth);
void rot(struct forth *forth);
void rot_back(struct forth *forth);
void show(struct forth *forth);
void over(struct forth *forth);

void _true(struct forth *forth);
void _false(struct forth *forth);
void _xor(struct forth *forth);
void _or(struct forth *forth);
void _and(struct forth *forth);
void _not(struct forth *forth);
void lt(struct forth *forth);
void _eq(struct forth *forth);
void within(struct forth *forth);

void forth_exit(struct forth *forth);
void literal(struct forth *forth);
void compile_start(struct forth *forth);
void compile_end(struct forth *forth);

void rpush(struct forth *forth);
void rpop(struct forth *forth);
void rtop(struct forth *forth);
void rshow(struct forth *forth);

void memory_read(struct forth *forth);
void memory_write(struct forth *forth);
void here(struct forth *forth);
void branch(struct forth *forth);
void branch0(struct forth *forth);
void immediate(struct forth *forth);

void next_word(struct forth *forth);
void find(struct forth *forth);
void _word_code(struct forth *forth);
void comma(struct forth *forth);

void next(struct forth *forth);
void interpreter_stub(struct forth *forth);
void decompile(struct forth *forth);
void save(struct forth *forth);
void load(struct forth *forth);
