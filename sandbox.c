#include "lexer.h"



int main( void ) {
    lexer_t lexer = lexer_create_from_string(
        "int ( void ) {\n"
        "    'h' 'i' '$' '\\n' '\\\\' '\\x00' '\\777';"
        "    return '\\0';\n"
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
        }
        printf( "str: " );
        token_print_lexeme( lexer->source, &t->lexeme );
        printf( "\n" );
    }

    lexer_free( lexer );
}
