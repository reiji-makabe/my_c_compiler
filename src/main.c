#include "9cc.h"

Token	*token;
char	*user_input;

int	main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "引数の数が正しくありません\n");
		return 1;
	}

	user_input = argv[1];
	token = tokenize(user_input);
	program();

	// assembly template
	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");

	// prologue
	printf("\tpush rbp\n");
	printf("\tmov rbp, rsp\n");
	printf("\tsub rsp, 208\n");

	// code gen
	for (int i=0; code[i]; ++i) {
		gen(code[i]);

		// 式の評価結果としてスタックに1つ値が残っているらしい
		// 正直よくわかっていない
		printf("\tpop rax\n");
	}

	// epilogue
	// The end of culc result
	printf("\tmov rsp, rbp\n");
	printf("\tpop rbp\n");
	printf("\tret\n");
	return 0;
}
