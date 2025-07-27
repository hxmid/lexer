#pragma once

#ifndef LEXER_H
#define LEXER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

    typedef enum {
        TOKEN_ERROR,
        TOKEN_OPERATOR,
        TOKEN_INTEGER,
    } token_type_t;

    typedef enum {
        OP_PLUS,
        OP_MINUS,
    } op_type_t;

    typedef struct {
        char* str;

        size_t capacity;
        size_t length;
    } lexer_str_t;

    typedef struct {
        token_type_t type;

        size_t line;
        size_t position;

        lexer_str_t lexeme;

        union {
            op_type_t op;
            uint64_t i;
        };
    } token_t;

    typedef struct {
        token_t* tokens;

        size_t capacity;
        size_t length;
    } token_list_t;

    typedef struct {
        const char* source;
        size_t size;

        size_t start;
        size_t current;
        size_t line;

        token_list_t token_list;
    } lexer_t;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LEXER_H
