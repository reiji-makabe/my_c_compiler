#include "9cc.h"

void	gen_lval(Node *node) {
	if (node->kind != ND_LVAR) {
		error("代入の左辺値が変数ではありません");
	}
	printf("\tmov rax, rbp\n");
	printf("\tsub rax, %d\n", node->offset);
	printf("\tpush rax\n");
}

void	gen(Node *node) {
	if (node->kind == ND_RETURN) {
		gen(node->lhs);
		printf("\tpop rax\n");
		printf("\tmov rsp, rbp\n");
		printf("\tpop rbp\n");
		printf("\tret\n");
		return;
	}
	switch (node->kind) {
	case ND_NUM:
		printf("\tpush %d\n", node->val);
		return;
	case ND_LVAR:
		gen_lval(node);
		printf("\tpop rax\n");
		printf("\tmov rax, [rax]\n");
		printf("\tpush rax\n");
		return;
	case ND_ASSIGN:
		gen_lval(node->lhs);
		gen(node->rhs);

		printf("\tpop rdi\n");
		printf("\tpop rax\n");
		printf("\tmov [rax], rdi\n");
		printf("\tpush rdi\n");
		return;
	}

	gen(node->lhs);
	gen(node->rhs);

	printf("\tpop rdi\n");
	printf("\tpop rax\n");

	switch (node->kind) {
	case ND_ADD:
		printf("\tadd rax, rdi\n");
		break;
	case ND_SUB:
		printf("\tsub rax, rdi\n");
		break;
	case ND_MUL:
		printf("\timul rax, rdi\n");
		break;
	case ND_DIV:
		printf("\tcqo\n");
		printf("\tidiv rdi\n");
		break;
	case ND_EQ:
		printf("\tcmp rax, rdi\n");
		printf("\tsete al\n");
		printf("\tmovzb rax, al\n");
		break;
	case ND_NEQ:
		printf("\tcmp rax, rdi\n");
		printf("\tsetne al\n");
		printf("\tmovzb rax, al\n");
		break;
	case ND_LT:
	case ND_GT:
		printf("\tcmp rax, rdi\n");
		printf("\tsetl al\n");
		printf("\tmovzb rax, al\n");
		break;
	case ND_LTE:
	case ND_GTE:
		printf("\tcmp rax, rdi\n");
		printf("\tsetle al\n");
		printf("\tmovzb rax, al\n");
		break;
	}
	printf("\tpush rax\n");
}
