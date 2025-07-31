#include "lexer.h"

int main( void ) {
    lexer_t lexer = lexer_create_from_string( "int return( void ) {\n    return 0;\n}\n" );
    lexer_parse( lexer );

    for ( size_t i = 0; i < lexer->token_list.length; i++ ) {
        token_t* t = &lexer->token_list.tokens[i];
        printf( "line: %d column: %d ", t->line, t->column );
        printf( "type: %d sub: %d ", t->type, t->i );
        token_print_lexeme( lexer->source, &t->lexeme );
        printf( "\n" );
    }

    lexer_free( lexer );
}
