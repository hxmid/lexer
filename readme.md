# lexer

a c header aimed to be a highly customizable and user extensible lexer


## configuration

open `lexer.def` and make any changes you'd like. by default, it's set up to lex c files, but it's very simple to extend/modify

all you need to do is find the appropriate category, define the respective token to match and give it an alias that you can later use as an enumeration

for example, to add the scope resolution operator found in c++:

```c
// under the `#define LEXER_OPERATOR_LIST` definition:
// ...
LEXER_OP("::", OP_SCOPE_RESOLUTION) \
// ...
```


## usage

the `lexer.h` header should ideally be included within your **parser's source file** (e.g. `parser.c`). from there you should have wrappers for the functions within it. this way lexer.h acts as a blackbox of sorts

e.g.

```c
// main.c
// ...

int main( void ) {

    // ...

    parse( your_code );

    // ...

    return 0;
}

```

```c
// parser.h
// ...

void parse( const char* string );

```

```c
// parser.c
// ...

#include "lexer.h"
#include "parser.h"

void parse( const char* string ) {
    lexer_t lexer = lexer_create_from_string( string );

    lexer_parse( lexer );

    // your parser logic here
    // ...

    lexer_free( lexer );
}

```

assuming `lexer.h` is implemented properly (hopefully), you shouldn't really have a need to ever access the `token_t` struct outside of your parser

instead you just use it as a black box and parse the tokens however you'd like


## philosophy

- blackbox design: once you've configured your `lexer.def` file, `lexer.h` handles all lexing details

- separation of concerns: the `token_t` and `token_lists_t` structures should stay private to the parser's implementation, and should not leak into headers

- extensible by design: just edit `lexer.def` to support new operators, keywords or punctuation

if `lexer.h` is implemented properly, you'll rarely (if ever) need to touch tokens outside of the parser
