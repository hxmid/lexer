#include "lexer.h"

int main( void ) {
    lexer_t lexer = lexer_create_from_string(
        "int main( void ) {\n"
        "    printf( \"Hello, World!\" ); // prints hello world"
        "    return 0;\n"
        "}"
    );
    lexer_parse( lexer );

    for ( size_t i = 0; i < lexer->token_list.length; i++ ) {
        token_t* t = &lexer->token_list.tokens[i];
        printf( "line: %10zu, column: %10zu, ", t->line, t->column );
        printf( "type: %10zu, ", t->type );
        switch ( t->type ) {
        case TOKEN_OPERATOR:
            printf( "o: %5zu, ", t->op );
            break;
        case TOKEN_INTEGER:
            printf( "i: %5zu, ", t->i );
            break;
        case TOKEN_PUNCTUATION:
            printf( "p: %5zu, ", t->punct );
            break;
        case TOKEN_KEYWORD:
            printf( "k: %5zu, ", t->keyword );
            break;
        case TOKEN_CHARACTER:
            printf( "c: %5zu, ", t->i );
            break;
        case TOKEN_STRING:
            printf( "s: %5s, ", t->string->str );
            break;
        case TOKEN_DOUBLE:
            printf( "d: %2.2lf, ", t->d );
            break;
        case TOKEN_FLOAT:
            printf( "f: %3.2f, ", t->f );
            break;
        case TOKEN_IDENTIFIER:
            printf( "x: %5s, ", t->string->str );
            break;
        }
        printf( "str: " );
        token_print_lexeme( lexer->source, &t->lexeme );
        printf( "\n" );
    }

    lexer_free( lexer );
}
