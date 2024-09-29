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
	char		*str; // token string
};

// tokens of interest now
Token *token;

// for error reporting
// same arguments as printf
void	error(char *fmt, ...) {
	va_list	ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

// if (next == expected) {token = token->next, return (true)}
// else {return (false) }
bool	consume(char op) {
	if (token->kind != TK_RESERVED || token->str[0] != op) {
		return (false);
	}
	token = token->next;
	return (true);
}

// if (next == expected) {token = token->next}
// else {error()}
void	expect(char op) {
	if (token->kind != TK_RESERVED || token->str[0] != op) {
		error("'%c'ではありません", op);
	}
	token = token->next;
}

// if (next == number) {return(token->number), token = token->next}
// else {error()}
int	expect_number() {
	if(token->kind != TK_NUM) {
		error("数ではありません");
	}
	int	val = token->val;
	token = token->next;
	return (val);
}

bool	at_eof() {
	return (token->kind == TK_EOF);
}

// new token; cur->next = token;
Token	*new_token(TokenKind kind, Token *cur, char *str) {
	Token	*tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	cur->next = tok;
	return (tok);
}

// tokenize string p
Token	*tokenize(char *p) {
	Token	head;
	head.next = NULL;
	Token	*cur = &head;

	while (*p) {
		if (isspace(*p)) {
			++p;
			continue;
		}
		if (*p == '+' || *p == '-') {
			cur = new_token(TK_RESERVED, cur, p++);
			continue;
		}
		if (isdigit(*p)) {
			cur = new_token(TK_NUM, cur, p);
			cur->val = strtol(p, &p, 10);
			continue;
		}
		error("トークナイズできません");
	}
	new_token(TK_EOF, cur, p);
	return (head.next);
}

int	main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "引数の数が正しくありません\n");
		return 1;
	}

	token = tokenize(argv[1]);

	// assembly template
	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");

	// The beginning of an expression must be a number
	printf(" mov rax, %d\n", expect_number());

	// consume '+ <num>' or '- <num' token column
	// and output assembly
	while (!at_eof()) {
		if (consume('+')) {
			printf(" add rax, %d\n", expect_number());
			continue;
		}
		expect('-');
		printf(" sub rax, %d\n", expect_number());
	}
	printf(" ret\n");
	return 0;
}
