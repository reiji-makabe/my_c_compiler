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
