#include "9cc.h"
#include "my_conv.h"

/* program    = stmt*
 * stmt       = expr ";"
 *              | "if" "(" expr ")" stmt ("else" stmt)?
 *              | "while" "(" expr ")" stmt
 *              | "for" "(" expr? ";" expr? ";" expr? ")" stmt
 *              | "return" expr ";"
 * expr       = assign
 * assign     = equality ("=" assign)?
 * equality   = relational ("==" relational | "!=" relational)*
 * relational = add ("<" add | "<=" add | ">" add | ">=" add)*
 * add        = mul ("+" mul | "-" mul)*
 * mul        = unary ("*" unary | "/" unary)*
 * unary      = ("+" | "-")? primary
 * primary    = num | ident | "(" expr ")"
 */

Node	*code[100];

Node	*expr(void);

// searches for lvar array by name
// if not found, return NULL
LVar	*find_lvar(Token *tok) {
	for (LVar *var = locals; var; var = var->next) {
		if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
			return var;
		}
	}
	return NULL;
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

// return whether the end of token
bool	at_eof(void) {
	return token->kind == TK_EOF;
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

		LVar *lvar = find_lvar(tok);
		if (lvar) {
			node->offset = lvar->offset;
		} else {
			lvar = (LVar *)calloc(1, sizeof(LVar));
			lvar->next = locals;
			lvar->name = tok->str;
			lvar->len = tok->len;
			if (locals == NULL) {
				lvar->offset = 8;
			} else {
				lvar->offset = locals->offset + 8;
			}
			node->offset = lvar->offset;
			locals = lvar;
		}
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
	Node	*node;

	if (consume("return")) {
		node = calloc(1, sizeof(Node));
		node->kind = ND_RETURN;
		node->lhs = expr();
	} else {
		node = expr();
	}
	if (!consume(";")) {
		error_at(token->str, "';'ではないトークンです");
	}
	return node;
}

Node	*program(void) {
	int	i = 0;

	while (!at_eof()) {
		code[i++] = stmt();
	}
	code[i] = NULL;
}
