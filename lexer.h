#pragma once

#ifndef LEXER_H
#define LEXER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

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

    lexer_str_t lexer_str_init( void ) {
        lexer_str_t lexer_str;
        lexer_str.capacity = 1;
        lexer_str.str = (char*)calloc( 1, sizeof( char ) );
        if ( !lexer_str.str ) {
            fprintf( stderr, "[FATAL]: could not allocate memory for lexer_str_t.str\n" );
            exit( EXIT_FAILURE );
        }

        lexer_str.length = 0;
        lexer_str.str[lexer_str.length] = '\0';
        return lexer_str;
    }

    void lexer_str_deinit( lexer_str_t* lexer_str ) {
        free( lexer_str->str );
        lexer_str->length = 0;
        lexer_str->capacity = 0;
    }

    void lexer_str_add( lexer_str_t* lexer_str, char c ) {
        if ( lexer_str->length == lexer_str->capacity - 1 ) {
            lexer_str->capacity <<= 1;
            lexer_str->str = (char*)realloc( lexer_str->str, sizeof( char ) * lexer_str->capacity );
            if ( !lexer_str->str ) {
                fprintf( stderr, "[FATAL]: could not reallocate memory for lexer_str_t.str\n" );
                exit( EXIT_FAILURE );
            }
        }

        lexer_str->str[lexer_str->length] = c;
        lexer_str->length += 1;
        lexer_str->str[lexer_str->length] = '\0';
    }

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

    token_list_t token_list_init( void ) {
        token_list_t token_list;
        token_list.capacity = 1;
        token_list.tokens = (token_t*)calloc( 1, sizeof( token_t ) );
        if ( !token_list.tokens ) {
            fprintf( stderr, "[FATAL]: could not allocate memory for token_list_t\n" );
            exit( EXIT_FAILURE );
        }

        token_list.length = 0;
        return token_list;
    }

    void token_list_deinit( token_list_t* token_list ) {
        for ( size_t i = 0; i < token_list->length; i++ ) {
            lexer_str_deinit( &token_list->tokens[i].lexeme );
        }
        free( token_list->tokens );
        token_list->length = 0;
        token_list->capacity = 0;
    }

    void token_list_add( token_list_t* token_list, token_t* token ) {
        if ( token_list->length == token_list->capacity ) {
            token_list->capacity <<= 1;
            token_list->tokens = (token_t*)realloc( token_list->tokens, sizeof( token_t ) * token_list->capacity );
            if ( !token_list->tokens ) {
                fprintf( stderr, "[FATAL]: could not reallocate memory for token_list_t\n" );
                exit( EXIT_FAILURE );
            }
        }

        token_list->tokens[token_list->length] = *token;
        token_list->length += 1;
    }

    typedef struct {
        const char* source;
        size_t size;

        size_t start;
        size_t current;
        size_t line;

        token_list_t token_list;
    } lexer_t;

    lexer_t lexer_create_from_string( const char* string ) {
        lexer_t lexer;
        lexer.start = 0;
        lexer.current = 0;
        lexer.line = 1;
        lexer.token_list = token_list_init();
        lexer.size = strlen( string );

        lexer.source = strdup( string );
        if ( !lexer.source ) {
            fprintf( stderr, "[FATAL]: could not allocate memory for source string\n" );
            exit( EXIT_FAILURE );
        }

        return lexer;
    }

    void lexer_free( lexer_t* lexer ) {
        free( lexer->source );
        lexer->current = 0;
        lexer->line = 0;
        token_list_deinit( &lexer->token_list );
        lexer->size = 0;
    }

    char lexer_next( lexer_t* lexer ) {
        if ( lexer->current == lexer->size ) {
            return '\0';
        }
        lexer->current += 1;
        return lexer->source[lexer->current];
    }

    char lexer_lookahead( lexer_t* lexer ) {
        if ( lexer->current + 1 >= lexer->size ) {
            return '\0';
        }
        return lexer->source[lexer->current + 1];
    }

    char lexer_lookahead2( lexer_t* lexer ) {
        if ( lexer->current + 2 >= lexer->size ) {
            return '\0';
        }
        return lexer->source[lexer->current + 2];
    }


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LEXER_H
