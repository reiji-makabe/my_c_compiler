#include "my_conv.h"

int	is_lval_char(char c) {
	return ('a' <= c && c <= 'z') ||
			('A' <= c && c <= 'Z') ||
			('0' <= c && c <= '9') ||
			(c == '_');
}
