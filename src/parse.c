#include "9cc.h"

/* program    = stmt*
 * stmt       = expr ";"
 * expr       = assign
 * assign     = equality ("=" assign)?
 * equality   = relational ("==" relational | "!=" relational)*
 * relational = add ("<" add | "<=" add | ">" add | ">=" add)*
 * add        = mul ("+" mul | "-" mul)*
 * mul        = unary ("*" unary | "/" unary)*
 * unary      = ("+" | "-")? primary
 * primary    = num | ident | "(" expr ")"
 */

// for error reporting
// same arguments as printf

void	error(char *fmt, ...) {
	va_list	ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}


void	error_at(char *loc, char *fmt, ...) {
	va_list	ap;
	va_start(ap, fmt);

	int	pos = loc - user_input;
	fprintf(stderr, "%s\n", user_input);
	fprintf(stderr, "%*s^", pos, " ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

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
	if (token->kind != TK_RESERVED ||
		strlen(op) != token->len ||
		memcmp(token->str, op, token->len)) {
		return false;
	}
	token = token->next;
	return true;
}

// if (now == identifier) {token = token->next, return Token(token->previous)}
Token	*consume_ident(void) {
	Token	*tok = NULL;

	if (token->kind == TK_IDENT) {
		tok = token;
		token = token->next;
	}
	return tok;
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

bool	at_eof(void) {
	return token->kind == TK_EOF;
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
			cur = new_token(TK_IDENT, cur, p, p + 1);
			p += 1;
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

Node	*new_node(NodeKind kind, Node *lhs, Node *rhs) {
	Node	*node = (Node *)calloc(1, sizeof(Node));
	node->kind = kind;
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

Node	*new_node_num(int val) {
	Node	*node = (Node *)calloc(1, sizeof(Node));
	node->kind = ND_NUM;
	node->val = val;
	return node;
}

Node	*code[100];

Node	*expr(void);

Node	*primary(void) {
	if (consume("(")) {
		Node	*node = expr();
		expect(")");
		return node;
	}
	Token	*tok = consume_ident();
	if (tok) {
		Node	*node = calloc(1, sizeof(Node));
		node->kind = ND_LVAR;
		node->offset = (tok->str[0] - 'a' + 1) * 8;
		return node;
	}
	return new_node_num(expect_number());
}

Node	*unary(void) {
	if (consume("+")) {
		return primary();
	}
	if (consume("-")) {
		return new_node(ND_SUB, new_node_num(0), primary());
	}
	return primary();
}

Node	*mul(void) {
	Node	*node = unary();

	while (1) {
		if (consume("*")) {
			node = new_node(ND_MUL, node, unary());
		} else if (consume("/")) {
			node = new_node(ND_DIV, node, unary());
		} else {
			return node;
		}
	}
}

Node	*add(void) {
	Node	*node = mul();

	while (1) {
		if (consume("+")) {
			node = new_node(ND_ADD, node, mul());
		} else if (consume("-")) {
			node = new_node(ND_SUB, node, mul());
		} else {
			return node;
		}
	}
}

Node	*relational(void) {
	Node	*node = add();

	while (1) {
		if (consume("<")) {
			node = new_node(ND_LT, node, add());
		} else if (consume("<=")) {
			node = new_node(ND_LTE, node, add());
		} else if (consume(">")) {
			node = new_node(ND_GT, add(), node);
		} else if (consume(">=")) {
			node = new_node(ND_GTE, add(), node);
		} else {
			return node;
		}
	}
}

Node	*equality(void) {
	Node	*node = relational();

	while (1) {
		if (consume("==")) {
			node = new_node(ND_EQ, node, add());
		} else if (consume("!=")) {
			node = new_node(ND_NEQ, node, add());
		} else {
			return node;
		}
	}
}

Node	*assign(void) {
	Node	*node = equality();

	while (1) {
		if (consume("=")) {
			node = new_node(ND_ASSIGN, node, assign());
		}
		return node;
	}
}

Node	*expr(void) {
	Node	*node = assign();
// I don't need this function, but I'm including it for visibility and clarity
// Don't worry, it will probably disappear after optimization
}

Node	*stmt(void) {
	Node	*node = expr();
	expect(";");
	return node;
}

Node	*program(void) {
	int	i = 0;

	while (!at_eof()) {
		code[i++] = stmt();
	}
	code[i] = NULL;
}
