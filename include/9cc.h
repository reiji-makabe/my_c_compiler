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

typedef enum {
	ND_ADD, // +
	ND_SUB, // -
	ND_MUL, // *
	ND_DIV, // /
	ND_NUM, // number
} NodeKind;

typedef struct	Token Token;
struct	Token{
	TokenKind	kind;
	Token		*next;
	int			val;  // if (kind == num) value of number
	char		*str; // position of input string
};

typedef struct	Node Node;
struct Node {
	NodeKind	kind; // node type
	Node		*lhs; // left side
	Node		*rhs; // right side
	int			val; //if(kind==num) val=num
};

// tokens of interest now
extern Token	*token;
// user enterd programs
extern char		*user_input;

Token	*tokenize(char *p);
//bool	at_eof(void);
//int		expect_number(void);
//bool	consume(char op);
//void	expect(char op);
//void	error_at(char *loc, char *fmt, ...);
Node	*expr(void);
void	gen(Node *node);

#endif
