#include "lexer.h"

int main( void ) {
    lexer_t lexer = lexer_create_from_string(
        "int main( void ) {\n"
        "    /*\n"
        "     * multiline comment\n"
        "     */this\n"
        "    printf( \"Hello, World!\" ); // prints \"Hello, World\" to stdout\n"
        "    return 0;\n"
        "}"
    );
    lexer_parse( lexer );

    for ( size_t i = 0; i < lexer->token_list.length; i++ ) {
        token_t* t = &lexer->token_list.tokens[i];
        printf( "line: %10zu, column: %10zu, ", t->line, t->column );
        printf( "type: (%2zu) ", t->type );
        switch ( t->type ) {
        case TOKEN_OPERATOR:
            printf( "%10s", "operator, " );
            printf( "o: %5zu, ", t->op );
            break;
        case TOKEN_INTEGER:
            printf( "%14s", "integer, " );
            printf( "i: %5zu, ", t->i );
            break;
        case TOKEN_PUNCTUATION:
            printf( "%14s", "punctuation, " );
            printf( "p: %5zu, ", t->punct );
            break;
        case TOKEN_KEYWORD:
            printf( "%14s", "keyword, " );
            printf( "k: %5zu, ", t->keyword );
            break;
        case TOKEN_CHARACTER:
            printf( "%14s", "character, " );
            printf( "c: %5zu, ", t->i );
            break;
        case TOKEN_STRING:
            printf( "%14s", "string, " );
            printf( "s: %5s, ", t->string->str );
            break;
        case TOKEN_DOUBLE:
            printf( "%14s", "double, " );
            printf( "d: %2.2lf, ", t->d );
            break;
        case TOKEN_FLOAT:
            printf( "%14s", "float, " );
            printf( "f: %3.2f, ", t->f );
            break;
        case TOKEN_IDENTIFIER:
            printf( "%14s", "identifier, " );
            printf( "x: %5s, ", t->string->str );
            break;
        }
        printf( "str: " );
        token_print_lexeme( lexer->source, &t->lexeme );
        printf( "\n" );
    }

    lexer_free( lexer );
}
