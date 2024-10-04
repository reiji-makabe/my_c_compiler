#!/bin/bash
assert() {
	expected="$1"
	input="$2"

	./9cc "$input" > tmp.s
	cc -o tmp tmp.s
	./tmp
	actual="$?"

	if [ "$actual" = "$expected" ]; then
		echo "$input -> $actual"
	else
		echo "$input -> $expected expected, but got $actual"
		exit 1
	fi
}

error() {
	expected="$1"
	input="$2"

	error=$(./9cc "$input" 2>&1 > tmp.s)
	if [ "$error" = "$expected" ]; then
		echo "$expected:$input"
	else
		echo "error message $expected is expected, but got $error"
	fi
}

assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
assert 41 " 12 + 34 - 5 ;"
assert 42 "20*2 + 4 / 2;"
assert 20 "-10+30;"
assert 20 "10 + (2 * 5);"
assert 42 "48 + 2 * -3;"
assert 4 "7 + (-3);"
assert 1 "42 == 42;"
assert 1 "42 != 0;"
assert 1 "0 < 42;"
assert 1 "0 <= 42;"
assert 1 "42 <= 42;"
assert 1 "42 > 0;"
assert 1 "42 >= 0;"
assert 1 "42 >= 42;"
assert 1 "a=0; b=1; a+b;"
error "代入の左辺値が変数ではありません" "a + b = 3;"

echo ok
