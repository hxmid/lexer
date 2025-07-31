#include "lexer.h"

int main( void ) {
    lexer_t lexer = lexer_create_from_string(
        "0 "
        "1 "
        "10 "
        "100\n"
        "00 "
        "01 "
        "010 "
        "0100\n"
        "0h "
        "1h "
        "10h "
        "100h\n"
        "0x0 "
        "0x1 "
        "0x10 "
        "0x100\n"
        "0X0 "
        "0X1 "
        "0X10 "
        "0X100\n"
        "0b0 "
        "0b1 "
        "0b10 "
        "0b100"
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
