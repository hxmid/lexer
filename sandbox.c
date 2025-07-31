#include "lexer.h"

int main( void ) {
    lexer_t lexer = lexer_create_from_string(
        "int ( void ) {\n"
        "    'h' 'i' '\n';"
        "    return '0';\n"
        "}"
    );
    lexer_parse( lexer );

    for ( size_t i = 0; i < lexer->token_list.length; i++ ) {
        token_t* t = &lexer->token_list.tokens[i];
        printf( "line: %10d, column: %10d, ", t->line, t->column );
        printf( "type: %10d, sub: %10d, ", t->type, t->i );
        printf( "i: " );
        token_print_lexeme( lexer->source, &t->lexeme );
        printf( "\n" );
    }

    lexer_free( lexer );
}
