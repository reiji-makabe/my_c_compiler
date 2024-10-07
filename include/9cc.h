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
	TK_RETURN, // return statement
	TK_IDENT, // identifier
	TK_NUM,  // number
	TK_EOF,  // EOF
} TokenKind;

typedef enum {
	ND_ADD, // +
	ND_SUB, // -
	ND_MUL, // *
	ND_DIV, // /
	ND_EQ,  // ==  equal
	ND_NEQ, // !=  not equal
	ND_LT,  // <   less than
	ND_LTE, // <=  less than equal
	ND_GT,  // >   greater than
	ND_GTE, // >=  greater than equal
	ND_NUM, // number
	ND_ASSIGN, // = assign value to variable
	ND_LVAR,   // local variable
	ND_RETURN  // return statement
} NodeKind;

typedef struct	Token Token;
struct	Token{
	TokenKind	kind;
	Token		*next;
	int			val;  // if (kind == num) value of number
	char		*str; // position of input string
	int			len; // length of token_string. if kind==num
};

typedef struct	Node Node;
struct Node {
	NodeKind	kind; // node type
	Node		*lhs; // left side
	Node		*rhs; // right side
	int			val; //if(kind==num) val=num
	int			offset; // if (kind==lvar) offset=stack address(RBP - offset = lvar)
};

// local variable type
typedef struct	LVar LVar;
struct	LVar {
	LVar	*next;
	char	*name;
	int		len;
	int		offset;
};

// tokens of interest now
extern Token	*token;
extern char		*user_input;
extern Node		*code[100];
extern LVar		*locals;

// main function
Token	*tokenize(char *p);
Node	*program(void);
void	gen(Node *node);

bool	consume(char *op);
void	expect(char *op);
int		expect_number(void);

#endif
