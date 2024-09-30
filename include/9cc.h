#ifndef NINE_CC_H
#define NINE_CC_H

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// token type
typedef enum {
	TK_RESERVED,  // symbol
	TK_NUM,  // number
	TK_EOF,  // EOF
} TokenKind;

typedef struct Token Token;

struct	Token{
	TokenKind	kind;
	Token		*next;
	int			val;  // if (kind == num) value of number
	char		*str; // position of input string
};

// tokens of interest now
extern Token	*token;
// user enterd programs
extern char		*user_input;

Token	*tokenize(char *p);
int		expect_number(void);
bool	at_eof(void);
bool	consume(char op);
void	expect(char op);

#endif
