#include "lexer.h"

int main( void ) {
    lexer_t lexer = lexer_create_from_string( " +\n+" );
    lexer_parse( lexer );

    for ( size_t i = 0; i < lexer->token_list.length; i++ ) {
        token_t* t = &lexer->token_list.tokens[i];
        printf( "line: %d position: %d ", t->line, t->position );
        printf( "type: %d op: %d ", t->type, t->op );
        token_print_lexeme( lexer->source, &t->lexeme );
        printf( "\n" );
    }

    lexer_free( lexer );
}
