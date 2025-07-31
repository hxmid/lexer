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
        TOKEN_PUNCTUATION,
    } token_type_t;

    typedef enum {
        // single
        OP_PLUS,        // +
        OP_MINUS,       // -
        OP_STAR,        // *
        OP_SLASH,       // /
        OP_PERCENT,     // %
        OP_AMP,         // &
        OP_PIPE,        // |
        OP_CARET,       // ^
        OP_EQUAL,       // =
        OP_BANG,        // !
        OP_LESS,        // <
        OP_GREATER,     // >
        OP_TILDE,       // ~
        OP_QUESTION,    // ?
        OP_COLON,       // :

        // double
        OP_PLUS_EQUAL,        // +=
        OP_MINUS_EQUAL,       // -=
        OP_STAR_EQUAL,        // *=
        OP_SLASH_EQUAL,       // /=
        OP_PERCENT_EQUAL,     // %=
        OP_AMP_EQUAL,         // &=
        OP_PIPE_EQUAL,        // |=
        OP_CARET_EQUAL,       // ^=
        OP_EQUAL_EQUAL,       // ==
        OP_BANG_EQUAL,        // !=
        OP_LESS_EQUAL,        // <=
        OP_GREATER_EQUAL,     // >=
        OP_TILDE_EQUAL,       // ~=

        OP_SHIFT_LEFT,        // <<
        OP_SHIFT_RIGHT,       // >>

        OP_LOGICAL_AND,       // &&
        OP_LOGICAL_OR,        // ||

        OP_ARROW,             // ->

        OP_PLUS_PLUS,         // ++
        OP_MINUS_MINUS,       // --

        // triple
        OP_SHIFT_LEFT_EQUAL,  // <<=
        OP_SHIFT_RIGHT_EQUAL, // >>=
        OP_ELLIPSIS,          // ...
    } op_type_t;

    typedef enum {
        OP_DOT,         // .
        OP_COMMA,       // ,
        OP_SEMICOLON,   // ;
        OP_LPAREN,      // (
        OP_RPAREN,      // )
        OP_LBRACKET,    // [
        OP_RBRACKET,    // ]
        OP_LBRACE,      // {
        OP_RBRACE,      // }
    } punctuation_type_t;

    typedef struct {
        size_t start;
        size_t end;
    } lexer_slice_t;

    typedef struct {
        token_type_t type;

        size_t line;
        size_t column;

        lexer_slice_t lexeme;

        union {
            op_type_t op;
            punctuation_type_t punct;
            uint64_t i;
        };
    } token_t;

    void token_print_lexeme( const char* source, const lexer_slice_t* slice ) {
        printf( "%.*s", (int)( slice->end - slice->start ), source + slice->start );
    }

    token_t token_create_generic( size_t line, size_t column, size_t lstart, size_t lend ) {
        token_t token;
        token.type = TOKEN_ERROR;

        token.line = line;
        token.column = column;
        token.lexeme.start = lstart;
        token.lexeme.end = lend;

        return token;
    }

    token_t token_create_op( op_type_t type, size_t line, size_t column, size_t lstart, size_t lend ) {
        token_t token = token_create_generic( line, column, lstart, lend );
        token.type = TOKEN_OPERATOR;

        token.op = type;
        return token;
    }

    token_t token_create_punct( punctuation_type_t type, size_t line, size_t column, size_t lstart, size_t lend ) {
        token_t token = token_create_generic( line, column, lstart, lend );
        token.type = TOKEN_PUNCTUATION;

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

        size_t cursor;

        size_t line;
        size_t column;

        token_list_t token_list;
    } lexer_inner_t, * lexer_t;

    lexer_t lexer_create_from_string( const char* string ) {
        lexer_inner_t* lexer = (lexer_inner_t*)calloc( 1, sizeof( lexer_inner_t ) );
        if ( !lexer ) {
            fprintf( stderr, "[FATAL]: could not allocate memory for lexer_t\n" );
            exit( EXIT_FAILURE );
        }

        lexer->column = 1;
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

        free( (void*)lexer );
    }

    void lexer_add_token( lexer_t lexer, token_t* token ) {
        token_list_add( &lexer->token_list, token );
    }

    void lexer_add_op( lexer_t lexer, op_type_t type, size_t length ) {
        token_t token = token_create_op( type, lexer->line, lexer->column, lexer->cursor, lexer->cursor + length );
        lexer_add_token( lexer, &token );
    }

    void lexer_add_punct( lexer_t lexer, punctuation_type_t type, size_t length ) {
        token_t token = token_create_punct( type, lexer->line, lexer->column, lexer->cursor, lexer->cursor + length );
        lexer_add_token( lexer, &token );
    }

    char lexer_current( lexer_inner_t* lexer ) {
        if ( lexer->cursor == lexer->size ) {
            return '\0';
        }

        return lexer->source[lexer->cursor];
    }

    char lexer_next( lexer_inner_t* lexer ) {
        if ( lexer->cursor >= lexer->size ) {
            return '\0';
        }

        lexer->cursor += 1;
        lexer->column += 1;
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

    void lexer_parse( lexer_t lexer ) {
        for ( char c = lexer_current( lexer ); c != '\0'; c = lexer_next( lexer ) ) {
            switch ( c ) {
            case '\n': {
                    lexer->column = 0;
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
                    lexer->column = 0;
                    continue;
                }

            case '+': {
                    if ( lexer_lookahead( lexer ) == '=' ) {
                        lexer_add_op( lexer, OP_PLUS_EQUAL, 2 );
                        lexer_next( lexer );
                    } else if ( lexer_lookahead( lexer ) == '+' ) {
                        lexer_add_op( lexer, OP_PLUS_PLUS, 2 );
                        lexer_next( lexer );
                    } else {
                        lexer_add_op( lexer, OP_PLUS, 1 );
                    }
                    break;
                }

            case '-': {
                    if ( lexer_lookahead( lexer ) == '=' ) {
                        lexer_add_op( lexer, OP_MINUS_EQUAL, 2 );
                        lexer_next( lexer );
                    } else if ( lexer_lookahead( lexer ) == '>' ) {
                        lexer_add_op( lexer, OP_ARROW, 2 );
                        lexer_next( lexer );
                    } else if ( lexer_lookahead( lexer ) == '-' ) {
                        lexer_add_op( lexer, OP_MINUS_MINUS, 2 );
                        lexer_next( lexer );
                    } else {
                        lexer_add_op( lexer, OP_MINUS, 1 );
                    }
                    break;
                }

            case '*': {
                    if ( lexer_lookahead( lexer ) == '=' ) {
                        lexer_add_op( lexer, OP_STAR_EQUAL, 2 );
                        lexer_next( lexer );
                    } else {
                        lexer_add_op( lexer, OP_STAR, 1 );
                    }
                    break;
                }

            case '/': {
                    if ( lexer_lookahead( lexer ) == '=' ) {
                        lexer_add_op( lexer, OP_SLASH_EQUAL, 2 );
                        lexer_next( lexer );
                    } else {
                        lexer_add_op( lexer, OP_SLASH, 1 );
                    }
                    break;
                }

            case '%': {
                    if ( lexer_lookahead( lexer ) == '=' ) {
                        lexer_add_op( lexer, OP_PERCENT_EQUAL, 2 );
                        lexer_next( lexer );
                    } else {
                        lexer_add_op( lexer, OP_PERCENT, 1 );
                    }
                    break;
                }

            case '&': {
                    if ( lexer_lookahead( lexer ) == '=' ) {
                        lexer_add_op( lexer, OP_AMP_EQUAL, 2 );
                        lexer_next( lexer );
                    } else if ( lexer_lookahead( lexer ) == '&' ) {
                        lexer_add_op( lexer, OP_LOGICAL_AND, 2 );
                        lexer_next( lexer );
                    } else {
                        lexer_add_op( lexer, OP_AMP, 1 );
                    }
                    break;
                }

            case '|': {
                    if ( lexer_lookahead( lexer ) == '=' ) {
                        lexer_add_op( lexer, OP_PIPE_EQUAL, 2 );
                        lexer_next( lexer );
                    } else if ( lexer_lookahead( lexer ) == '|' ) {
                        lexer_add_op( lexer, OP_LOGICAL_OR, 2 );
                        lexer_next( lexer );
                    } else {
                        lexer_add_op( lexer, OP_PIPE, 1 );
                    }
                    break;
                }

            case '^': {
                    if ( lexer_lookahead( lexer ) == '=' ) {
                        lexer_add_op( lexer, OP_CARET_EQUAL, 2 );
                        lexer_next( lexer );
                    } else {
                        lexer_add_op( lexer, OP_CARET, 1 );
                    }
                    break;
                }

            case '=': {
                    if ( lexer_lookahead( lexer ) == '=' ) {
                        lexer_add_op( lexer, OP_EQUAL_EQUAL, 2 );
                        lexer_next( lexer );
                    } else {
                        lexer_add_op( lexer, OP_EQUAL, 1 );
                    }
                    break;
                }

            case '!': {
                    if ( lexer_lookahead( lexer ) == '=' ) {
                        lexer_add_op( lexer, OP_BANG_EQUAL, 2 );
                        lexer_next( lexer );
                    } else {
                        lexer_add_op( lexer, OP_BANG, 1 );
                    }
                    break;
                }

            case '<': {
                    if ( lexer_lookahead( lexer ) == '=' ) {
                        lexer_add_op( lexer, OP_LESS_EQUAL, 2 );
                        lexer_next( lexer );
                    } else if ( lexer_lookahead( lexer ) == '<' ) {
                        if ( lexer_lookahead2( lexer ) == '=' ) {
                            lexer_add_op( lexer, OP_SHIFT_LEFT_EQUAL, 3 );
                            lexer_next( lexer );
                        } else {
                            lexer_add_op( lexer, OP_SHIFT_LEFT, 2 );
                        }
                        lexer_next( lexer );
                    } else {
                        lexer_add_op( lexer, OP_LESS, 1 );
                    }
                    break;
                }

            case '>': {
                    if ( lexer_lookahead( lexer ) == '=' ) {
                        lexer_add_op( lexer, OP_GREATER_EQUAL, 2 );
                        lexer_next( lexer );
                    } else if ( lexer_lookahead( lexer ) == '>' ) {
                        if ( lexer_lookahead2( lexer ) == '=' ) {
                            lexer_add_op( lexer, OP_SHIFT_RIGHT_EQUAL, 3 );
                            lexer_next( lexer );
                        } else {
                            lexer_add_op( lexer, OP_SHIFT_RIGHT, 2 );
                        }
                        lexer_next( lexer );
                    } else {
                        lexer_add_op( lexer, OP_GREATER, 1 );
                    }
                    break;
                }

            case '~': {
                    if ( lexer_lookahead( lexer ) == '=' ) {
                        lexer_add_op( lexer, OP_TILDE_EQUAL, 2 );
                        lexer_next( lexer );
                    } else {
                        lexer_add_op( lexer, OP_TILDE, 1 );
                    }
                    break;
                }

            case '?': {
                    lexer_add_op( lexer, OP_QUESTION, 1 );
                    break;
                }

            case ':': {
                    lexer_add_op( lexer, OP_COLON, 1 );
                    break;
                }

            case '.': {
                    if ( lexer_lookahead( lexer ) == '.' && lexer_lookahead2( lexer ) == '.' ) {
                        lexer_add_op( lexer, OP_ELLIPSIS, 3 );
                        lexer_next( lexer ); lexer_next( lexer );
                    } else {
                        lexer_add_punct( lexer, OP_DOT, 1 );
                    }
                    break;
                }

            case ',': {
                    lexer_add_punct( lexer, OP_COMMA, 1 );
                    break;
                }

            case ';': {
                    lexer_add_punct( lexer, OP_SEMICOLON, 1 );
                    break;
                }

            case '(': {
                    lexer_add_punct( lexer, OP_LPAREN, 1 );
                    break;
            }

            case ')': {
                    lexer_add_punct( lexer, OP_RPAREN, 1 );
                    break;
                }

            case '[': {
                    lexer_add_punct( lexer, OP_LBRACKET, 1 );
                    break;
                }

            case ']': {
                    lexer_add_punct( lexer, OP_RBRACKET, 1 );
                    break;
                }

            case '{': {
                    lexer_add_punct( lexer, OP_LBRACE, 1 );
                    break;
                }

            case '}': {
                    lexer_add_punct( lexer, OP_RBRACE, 1 );
                    break;
                }


            default: {
                    fprintf( stderr, "[FATAL]: unhandled token at %zu:%zu -> `%c`\n", lexer->line, lexer->column, c );
                    exit( EXIT_FAILURE );
                }
        }
    }
    }


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LEXER_H
