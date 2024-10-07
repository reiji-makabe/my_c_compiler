#include "9cc.h"
#include "my_conv.h"

int	is_token(char *c) {
	if (!memcmp(c, "<=", 2) || !memcmp(c, ">=", 2) || !memcmp(c, "==", 2) || !memcmp(c, "!=", 2)){
		return 2;
	}
	if (*c == '+' || *c == '-' || *c == '*' || *c == '/') {
		return 1;
	}
	if (*c == '(' || *c == ')' || *c == '<' || *c == '>' || *c == ';' || *c == '=') {
		return 1;
	}
	return 0;
}

// if (next == expected) {token = token->next, return true}
// else {return false }
bool	consume(char *op) {
	// token->kind != TK_RESERVED ||
	if (strlen(op) != token->len ||
		memcmp(token->str, op, token->len)) {
		return false;
	}
	token = token->next;
	return true;
}

// if (next == expected) {token = token->next}
// else {error()}
void	expect(char *op) {
	if (token->kind != TK_RESERVED ||
		strlen(op) != token->len ||
		memcmp(token->str, op, token->len)) {
		error_at(token->str, "'%s'ではありません", op);
	}
	token = token->next;
}

// if (next == number) {return(token->number), token = token->next}
// else {error()}
int	expect_number(void) {
	if(token->kind != TK_NUM) {
		error_at(token->str, "数ではありません");
	}
	int	val = token->val;
	token = token->next;
	return val;
}

// new token; cur->next = token;
Token	*new_token(TokenKind kind, Token *cur, char *start, char *end) {
	Token	*tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	cur->next = tok;
	tok->str = start;
	tok->len = end - start;
	return tok;
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
		int	len;
		len = is_token(p);
		if (len) {
			cur = new_token(TK_RESERVED, cur, p, p + len);
			p += len;
			continue;
		}
		if ('a' <= *p && *p <= 'z') {
			if (!memcmp(p, "return", 6) && !is_lval_char(p[6])) {
				cur = new_token(TK_RETURN, cur, p, p + 6);
				p += 6;
				continue;
			}
			char	*tmp_p;
			tmp_p = p;
			while (isalpha(*tmp_p) || *tmp_p == '_') {
				++tmp_p;
				}
			cur = new_token(TK_IDENT, cur, p, tmp_p);
			p += cur->len;
			continue;
		}
		if (isdigit(*p)) {
			cur = new_token(TK_NUM, cur, p, p);
			char *tmp = p;
			cur->val = strtol(p, &p, 10);
			cur->len = p - tmp;
			continue;
		}
		error_at(p, "トークナイズできません");
	}
	new_token(TK_EOF, cur, p, p);
	return head.next;
}
