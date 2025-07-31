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
#include <stdbool.h>
#include <ctype.h>

#include "lexer.def"

    static size_t match_any( const char* str, const char* options ) {
        const char* p = options;
        while ( *p ) {
            while ( *p == ' ' ) { p++; }
            const char* start = p;

            while ( *p && *p != ' ' ) { p++; }
            size_t length = p - start;

            if ( length > 0 && strncmp( str, start, length ) == 0 )
                return length;
        }
        return 0;
    }

    typedef enum {
        TOKEN_ERROR,
        TOKEN_OPERATOR,
        TOKEN_INTEGER,
        TOKEN_PUNCTUATION,
        TOKEN_KEYWORD,
        TOKEN_CHARACTER,
    } token_type_t;

    typedef enum {
    # define LEXER_OP(sym, name) name,
        LEXER_OPERATOR_LIST
    # undef LEXER_OP
    } operator_type_t;

    typedef struct {
        const char* symbol;
        operator_type_t type;
        size_t length;
    } operator_def_t;

    static operator_def_t operator_defs[] = {
        # define LEXER_OP(sym, name) { sym, name, sizeof(sym) - 1 },
        LEXER_OPERATOR_LIST
        # undef LEXER_OP
    };

    typedef enum {
    # define LEXER_PUNCT(sym, name) name,
        LEXER_PUNCTUATION_LIST
    # undef LEXER_PUNCT
    } punctuation_type_t;

    typedef struct {
        const char* symbol;
        punctuation_type_t type;
        size_t length;
    } punctuation_def_t;

    static punctuation_def_t punctuation_defs[] = {
        # define LEXER_PUNCT(sym, name) { sym, name, sizeof(sym) - 1 },
        LEXER_PUNCTUATION_LIST
        # undef LEXER_PUNCT
    };

    typedef enum {
    # define LEXER_KEYWORD(sym, name) name,
        LEXER_KEYWORD_LIST
    # undef LEXER_KEYWORD
    } keyword_type_t;

    typedef struct {
        const char* symbol;
        keyword_type_t type;
        size_t length;
    } keyword_def_t;

    static keyword_def_t keyword_defs[] = {
        # define LEXER_KEYWORD(sym, name) { sym, name, sizeof(sym) - 1 },
        LEXER_KEYWORD_LIST
        # undef LEXER_KEYWORD
    };

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
            operator_type_t op;
            punctuation_type_t punct;
            keyword_type_t keyword;
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

    token_t token_create_operator( operator_type_t type, size_t line, size_t column, size_t lstart, size_t lend ) {
        token_t token = token_create_generic( line, column, lstart, lend );
        token.type = TOKEN_OPERATOR;

        token.op = type;
        return token;
    }

    token_t token_create_punct( punctuation_type_t type, size_t line, size_t column, size_t lstart, size_t lend ) {
        token_t token = token_create_generic( line, column, lstart, lend );
        token.type = TOKEN_PUNCTUATION;

        token.punct = type;
        return token;
    }

    token_t token_create_keyword( punctuation_type_t type, size_t line, size_t column, size_t lstart, size_t lend ) {
        token_t token = token_create_generic( line, column, lstart, lend );
        token.type = TOKEN_KEYWORD;

        token.keyword = type;
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

    void lexer_add_operator( lexer_t lexer, operator_type_t type, size_t length ) {
        token_t token = token_create_operator( type, lexer->line, lexer->column, lexer->cursor, lexer->cursor + length );
        lexer_add_token( lexer, &token );
    }

    void lexer_add_punct( lexer_t lexer, punctuation_type_t type, size_t length ) {
        token_t token = token_create_punct( type, lexer->line, lexer->column, lexer->cursor, lexer->cursor + length );
        lexer_add_token( lexer, &token );
    }

    void lexer_add_keyword( lexer_t lexer, keyword_type_t type, size_t length ) {
        token_t token = token_create_keyword( type, lexer->line, lexer->column, lexer->cursor, lexer->cursor + length );
        lexer_add_token( lexer, &token );
    }

    void lexer_advance( lexer_inner_t* lexer, size_t x ) {
        lexer->cursor += x;
        lexer->column += x;
    }

    char lexer_next( lexer_inner_t* lexer ) {
        if ( lexer->cursor >= lexer->size ) {
            return '\0';
        }

        lexer_advance( lexer, 1 );
        return lexer->source[lexer->cursor];
    }

    char lexer_lookaheadx( lexer_inner_t* lexer, size_t x ) {
        if ( lexer->cursor + x >= lexer->size ) {
            return '\0';
        }

        return lexer->source[lexer->cursor + x];
    }

    char lexer_current( lexer_inner_t* lexer ) {
        return lexer_lookaheadx( lexer, 0 );
    }

    char lexer_lookahead( lexer_inner_t* lexer ) {
        return lexer_lookaheadx( lexer, 1 );
    }

    bool lexer_parse_operator( lexer_t lexer ) {
        static size_t max_len = 0;
        if ( max_len == 0 ) {
            for ( size_t i = 0; i < sizeof( operator_defs ) / sizeof( operator_defs[0] ); i++ ) {
                if ( operator_defs[i].length > max_len )
                    max_len = operator_defs[i].length;
            }
        }

        for ( size_t pass = max_len; pass > 0; pass-- ) {
            for ( size_t i = 0; i < sizeof( operator_defs ) / sizeof( operator_defs[0] ); i++ ) {
                operator_def_t* operator = &operator_defs[i];

                if ( operator->length != pass ) {
                    continue;
                }

                if ( lexer->cursor + operator->length <= lexer->size ) {
                    if ( strncmp( lexer->source + lexer->cursor, operator->symbol, operator->length ) == 0 ) {
                        lexer_add_operator( lexer, operator->type, operator->length );
                        for ( size_t j = 0; j < operator->length - 1; j++ ) lexer_next( lexer );
                        return true;
                    }
                }
            }
        }

        return false;
    }

    bool lexer_parse_punctuation( lexer_t lexer ) {
        static size_t max_len = 0;
        if ( max_len == 0 ) {
            for ( size_t i = 0; i < sizeof( punctuation_defs ) / sizeof( punctuation_defs[0] ); i++ ) {
                if ( punctuation_defs[i].length > max_len )
                    max_len = punctuation_defs[i].length;
            }
        }

        for ( size_t pass = max_len; pass > 0; pass-- ) {
            for ( size_t i = 0; i < sizeof( punctuation_defs ) / sizeof( punctuation_defs[0] ); i++ ) {
                punctuation_def_t* punctuation = &punctuation_defs[i];

                if ( punctuation->length != pass ) {
                    continue;
                }

                if ( lexer->cursor + punctuation->length <= lexer->size ) {
                    if ( strncmp( lexer->source + lexer->cursor, punctuation->symbol, punctuation->length ) == 0 ) {
                        lexer_add_punct( lexer, punctuation->type, punctuation->length );
                        for ( size_t j = 0; j < punctuation->length - 1; j++ ) lexer_next( lexer );
                        return true;
                    }
                }
            }
        }

        return false;
    }


    bool lexer_parse_keyword( lexer_t lexer ) {
        static size_t max_len = 0;
        if ( max_len == 0 ) {
            for ( size_t i = 0; i < sizeof( keyword_defs ) / sizeof( keyword_defs[0] ); i++ ) {
                if ( keyword_defs[i].length > max_len )
                    max_len = keyword_defs[i].length;
            }
        }

        for ( size_t pass = max_len; pass > 0; pass-- ) {
            for ( size_t i = 0; i < sizeof( keyword_defs ) / sizeof( keyword_defs[0] ); i++ ) {
                keyword_def_t* keyword = &keyword_defs[i];

                if ( keyword->length != pass ) {
                    continue;
                }

                if ( lexer->cursor + keyword->length <= lexer->size ) {
                    if ( strncmp( lexer->source + lexer->cursor, keyword->symbol, keyword->length ) == 0 ) {
                        lexer_add_keyword( lexer, keyword->type, keyword->length );
                        for ( size_t j = 0; j < keyword->length - 1; j++ ) lexer_next( lexer );
                        return true;
                    }
                }
            }
        }

        return false;
    }

    bool lexer_parse_integer( lexer_t lexer ) {
        size_t start = lexer->cursor;
        size_t column = lexer->column;
        if ( !isdigit( lexer_current( lexer ) ) ) return false;

        const char* str = lexer->source + start;
        size_t hex_pre = match_any( str, LEXER_HEX_PREFIXES );
        size_t oct_pre = match_any( str, LEXER_OCT_PREFIXES );
        size_t bin_pre = match_any( str, LEXER_BIN_PREFIXES );

        int base = 10;
        size_t prefix_len = 0;
        if ( hex_pre > oct_pre && hex_pre > bin_pre ) {
            base = 16;
            prefix_len = hex_pre;
        } else if ( oct_pre > hex_pre && oct_pre > bin_pre ) {
            base = 8;
            prefix_len = oct_pre;
        } else if ( bin_pre > hex_pre && bin_pre > oct_pre ) {
            base = 2;  prefix_len = bin_pre;
        } else if ( ( hex_pre && ( hex_pre == oct_pre || hex_pre == bin_pre ) ) || ( oct_pre && ( oct_pre == bin_pre ) ) ) {
            fprintf( stderr, "[FATAL]: integer literal prefix overlap at %zu:%zu\n", lexer->line, lexer->column );
            exit( EXIT_FAILURE );
        }

        if ( !isalnum( lexer_lookahead( lexer ) ) ) {
            token_t t = token_create_generic( lexer->line, column, start, lexer->cursor + 1 );
            t.type = TOKEN_INTEGER;
            t.i = strtoull( &lexer->source[lexer->cursor], NULL, 10 );
            lexer_add_token( lexer, &t );
            return true;
        }

        lexer_advance( lexer, prefix_len );

        while ( 1 ) {
            char la = lexer_lookahead( lexer );
            bool valid = false;

            if ( base == 16 ) {
                valid = isxdigit( la );
            } else if ( base == 10 ) {
                valid = isdigit( la );
            } else if ( base == 8 ) {
                valid = ( la >= '0' && la <= '7' );
            } else if ( base == 2 ) {
                valid = ( la == '0' || la == '1' );
            }

            if ( !valid ) {
                break;
            }

            lexer_next( lexer );
        }

        const char* suffix_str = lexer->source + lexer->cursor + 1;
        size_t hex_suf = match_any( suffix_str, LEXER_HEX_SUFFIXES );
        size_t oct_suf = match_any( suffix_str, LEXER_OCT_SUFFIXES );
        size_t bin_suf = match_any( suffix_str, LEXER_BIN_SUFFIXES );

        size_t suffix_len = 0;

        if ( hex_suf > oct_suf && hex_suf > bin_suf ) {
            if ( base != 16 && base != 10 ) {
                fprintf( stderr, "[FATAL]: invalid hex suffix on base %d at %zu:%zu\n", base, lexer->line, lexer->column );
                exit( EXIT_FAILURE );
            }

            base = 16;
            suffix_len = hex_suf;

        } else if ( oct_suf > hex_suf && oct_suf > bin_suf ) {
            if ( base != 8 && base != 10 ) {
                fprintf( stderr, "[FATAL]: invalid oct suffix on base %d at %zu:%zu\n", base, lexer->line, lexer->column );
                exit( EXIT_FAILURE );
            }

            base = 8;
            suffix_len = oct_suf;

        } else if ( bin_suf > hex_suf && bin_suf > oct_suf ) {
            if ( base != 2 && base != 10 ) {
                fprintf( stderr, "[FATAL]: invalid bin suffix on base %d at %zu:%zu\n", base, lexer->line, lexer->column );
                exit( EXIT_FAILURE );
            }

            base = 2;
            suffix_len = bin_suf;

        } else if (
            ( hex_suf && ( hex_suf == oct_suf || hex_suf == bin_suf ) )
            || ( oct_suf && ( oct_suf == bin_suf ) )
            ) {
            fprintf( stderr, "[FATAL]: integer literal suffix overlap at %zu:%zu\n", lexer->line, lexer->column );
            exit( EXIT_FAILURE );
        }

        lexer_advance( lexer, suffix_len );

        if ( isalnum( lexer_lookahead( lexer ) ) ) {
            fprintf( stderr, "[FATAL]: unexpected token in integer literal at %zu:%zu\n",
                lexer->line, lexer->column );
            exit( EXIT_FAILURE );
        }

        size_t length = ( lexer->cursor - start + 1 ) - suffix_len;
        if ( length - prefix_len >= 128 ) {
            fprintf( stderr, "[FATAL]: integer literal too large at %zu:%zu\n",
                lexer->line, lexer->column );
            exit( EXIT_FAILURE );
        }

        char buffer[128];
        memcpy( buffer, lexer->source + start + prefix_len, length - prefix_len );
        buffer[length - prefix_len] = '\0';

        token_t t = token_create_generic( lexer->line, column, start, lexer->cursor + 1 );
        t.type = TOKEN_INTEGER;
        t.i = strtoull( buffer, NULL, base );
        lexer_add_token( lexer, &t );
        return true;
    }

# define LEXER_ESC(c, value) case c: return value;
    char lexer_unescape_character( lexer_t lexer, char esc ) {
        switch ( esc ) {
            LEXER_ESCAPE_CHAR_LIST
        default:
            fprintf( stderr, "[FATAL]: unknown escape sequence at %zu:%zu\n", lexer->line, lexer->column );
            exit( EXIT_FAILURE );
        }
    }
#undef LEXER_ESC

    bool lexer_parse_character( lexer_t lexer ) {
        size_t start = lexer->cursor;
        size_t column = lexer->column;

        if ( lexer_current( lexer ) != LEXER_CHAR_DELIMITER ) return false;
        lexer_next( lexer );

        char c = lexer_current( lexer );
        if ( c == LEXER_ESCAPE_CHAR ) {
            lexer_next( lexer );
            c = lexer_unescape_character( lexer, c );
        }

        lexer_next( lexer );

        if ( lexer_current( lexer ) != LEXER_CHAR_DELIMITER ) {
            fprintf( stderr, "[FATAL]: unterminated char literal at %zu:%zu\n",
                lexer->line, lexer->column );
            exit( EXIT_FAILURE );
        }
        lexer_next( lexer );

        token_t t = token_create_generic( lexer->line, column, start, lexer->cursor );
        t.type = TOKEN_CHARACTER;
        t.i = (uint64_t)c;
        lexer_add_token( lexer, &t );

        return true;
    }


    void lexer_parse( lexer_t lexer ) {
        for ( char c = lexer_current( lexer ); c != '\0'; c = lexer_next( lexer ) ) {
            if ( c == '\n' ) {
                lexer->column = 0;
                lexer->line += 1;
                continue;
            } else if ( c == '\t' ) {
                continue;
            } else if ( c == ' ' ) {
                continue;
            } else if ( c == '\r' ) {
                lexer->column = 0;
                continue;
            }

            // TODO(hamid): look into a trie data structure for these in the future
            bool matched = lexer_parse_character( lexer )
                || lexer_parse_integer( lexer )
                || lexer_parse_operator( lexer )
                || lexer_parse_punctuation( lexer )
                || lexer_parse_keyword( lexer );

            if ( !matched ) {
                fprintf( stderr, "[FATAL]: unhandled token at %zu:%zu -> `%c`\n", lexer->line, lexer->column, c );
                exit( EXIT_FAILURE );
            }
        }
    }


#undef LEXER_OPERATOR_LIST
#undef LEXER_PUNCTUATION_LIST
#undef LEXER_KEYWORD_LIST

#ifdef __cplusplus
    }
#endif // __cplusplus

#endif // LEXER_H
