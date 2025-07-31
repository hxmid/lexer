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
        size_t start;
        size_t end;
    } lexer_slice_t;

    typedef struct {
        token_type_t type;

        size_t line;
        size_t position;

        lexer_slice_t lexeme;

        union {
            op_type_t op;
            uint64_t i;
        };
    } token_t;

    void token_print_lexeme( const char* source, lexer_slice_t* slice ) {
        printf( "%.*s", (int)( slice->end - slice->start ), source + slice->start );
    }

    token_t token_create_generic( size_t line, size_t position, size_t lstart, size_t lend ) {
        token_t token;
        token.type = TOKEN_ERROR;

        token.line = line;
        token.position = position;
        token.lexeme.start = lstart;
        token.lexeme.end = lend;

        return token;
    }

    token_t token_create_op( op_type_t type, size_t line, size_t position, size_t lstart, size_t lend ) {
        token_t token = token_create_generic( line, position, lstart, lend );
        token.type = TOKEN_OPERATOR;

        token.op = type;
        return token;
    }

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
        free( (void*)token_list->tokens );
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
        size_t cursor;
        size_t line;

        token_list_t token_list;
    } lexer_inner_t, * lexer_t;

    lexer_t lexer_create_from_string( const char* string ) {
        lexer_inner_t* lexer = (lexer_inner_t*)calloc( 1, sizeof( lexer_inner_t ) );
        if ( !lexer ) {
            fprintf( stderr, "[FATAL]: could not allocate memory for lexer_t\n" );
            exit( EXIT_FAILURE );
        }

        lexer->start = 1;
        lexer->line = 1;

        lexer->cursor = 0;

        lexer->token_list = token_list_init();
        lexer->size = strlen( string );

        lexer->source = strdup( string );
        if ( !lexer->source ) {
            fprintf( stderr, "[FATAL]: could not allocate memory for lexer_t.source\n" );
            exit( EXIT_FAILURE );
        }

        return lexer;
    }

    void lexer_free( lexer_t lexer ) {
        free( (void*)lexer->source );
        lexer->cursor = 0;
        lexer->line = 0;
        token_list_deinit( &lexer->token_list );
        lexer->size = 0;
    }

    void lexer_add_token( lexer_t lexer, token_t* token ) {
        token_list_add( &lexer->token_list, token );
    }

    void lexer_add_op( lexer_t lexer, op_type_t type, size_t length ) {
        token_t token = token_create_op( type, lexer->line, lexer->start, lexer->cursor, lexer->cursor + length );
        lexer_add_token( lexer, &token );
    }

    char lexer_current( lexer_inner_t* lexer ) {
        if ( lexer->cursor == lexer->size ) {
            return '\0';
        }

        return lexer->source[lexer->cursor];
    }

    char lexer_next( lexer_inner_t* lexer ) {
        if ( lexer->cursor == lexer->size ) {
            return '\0';
        }

        lexer->cursor += 1;
        lexer->start += 1;
        return lexer->source[lexer->cursor];
    }

    char lexer_lookahead( lexer_inner_t* lexer ) {
        if ( lexer->cursor + 1 >= lexer->size ) {
            return '\0';
        }

        return lexer->source[lexer->cursor + 1];
    }

    char lexer_lookahead2( lexer_inner_t* lexer ) {
        if ( lexer->cursor + 2 >= lexer->size ) {
            return '\0';
        }

        return lexer->source[lexer->cursor + 2];
    }

    int lexer_parse( lexer_t lexer ) {
        for ( char c = lexer_current( lexer ); c != '\0'; c = lexer_next( lexer ) ) {
            switch ( c ) {
            case '\n': {
                    lexer->start = 0;
                    lexer->line += 1;
                    continue;
                }

            case '\t': {
                    continue;
                }

            case ' ': {
                    continue;
                }

            case '\r': {
                    lexer->start = 0;
                    continue;
                }

            case '+': {
                    lexer_add_op( lexer, OP_PLUS, 1 );
                }
            }
        }
    }

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LEXER_H
