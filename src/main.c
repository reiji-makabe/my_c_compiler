#include "9cc.h"

Token	*token;
char	*user_input;

int	main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "引数の数が正しくありません\n");
		return 1;
	}

	user_input = argv[1];
	token = tokenize(argv[1]);
	Node *node = expr();

	// assembly template
	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");

	// code gen
	gen(node);

	// The end of culc result
	printf("\tpop rax\n");
	printf("\tret\n");
	return 0;
}
