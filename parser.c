/*
 * date: 12.04 2020
 *
 * tiny parser for simple C-like syntax
 *
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>

#include "config.h"

long prev_val = 0;
long reg_val[10] = { 0 };

long parse_f();
long parse_a();

long
parse_f()
{
	long val;
	char c;
	c = getc(stdin);
	while (c == ' ' || c == '\t') {
		c = getc(stdin);
	};
	if (c == '_') {
		val = prev_val;
	} else if (c >='0' && c <='9') {
		val = 0;
		for (;;) {
			val = val*10 + c - '0';
			c = getc(stdin);
			if (c <'0' || c >'9') {
				ungetc(c, stdin);
				break;
			};
		};
	} else switch (c) {
	case '-':  val = -parse_f(); break;
	case '+':  val = +parse_f(); break;
	case '#':
		ungetc(c, stdin);
		return 0;
	case '$':
		c = getc(stdin);
		if (c < '0' || c > '9')
			errx(1, "digit expected");
		val = reg_val[c -'0'];
		break;
	case '(':  val = +parse_a();
		if (getc(stdin) != ')')
			errx(1, "')' expected");
		break;
	case ')':  errx(1, "')' unexpected");
	default:   errx(1, "unknown token");
	};
	c = getc(stdin);
	while (c == ' ' || c == '\t') {
		c = getc(stdin);
	};
	switch (c) {
	case '*': return val * parse_f();
	case '/': return val / parse_f();
	case '%': return val % parse_f();
	default:
		ungetc(c, stdin);
		return val;
	};
}


long
parse_a()
{
	int c;
	long val;
	c = getc(stdin);
	if (c == '\n' || c <= 0) return 0;
	ungetc(c, stdin);
	val = parse_f();
	for (;;) {
		c = getc(stdin);
		switch (c) {
		case '\t': // -->
		case ' ':  break;
		case '+': val += parse_f(); break;
		case '-': val -= parse_f(); break;
		case '=':
			if (val < 10)
				return reg_val[val] = parse_a();
			else
				errx(1, "register inde too high");
		case '#':
			ungetc(c, stdin);
			return prev_val;
		case ')':
		case '\n':
			ungetc(c, stdin);
		case -1:  return val;
		default:  errx(1, "unknown token");
		};
	};
}


int
main(int argc, char *argv[])
{
	int prompt = show_prompt;
	int c;
	char *form = "%d\n";
	// TODO: exit_on_err (longjmp)
	if (argc > 1 && !strcmp(argv[1], "-p")) {
		prompt = 1;
	};
	for (;;)  {
		if (prompt) fputs(ps, stderr);
		switch (c = getc(stdin)) {
		case  0:
		case -1:
		case 'q':
			return 0;
		case '\n': ungetc(c, stdin); goto endline;
		case 'x': form = "%x\n"; goto endline;
		case 'o': form = "%o\n"; goto endline;
		case 'd': form = "%d\n"; goto endline;
		default: ungetc(c, stdin); break;
		};
		prev_val = parse_a();
	endline:
		c = getc(stdin);
		if (c == '#') {
			while (c > 0 && c != '\n')
				c = getc(stdin);
		} else if (c != '\n') {
			ungetc(c, stdin);
		};
		printf(form, prev_val);
	};
	return 0;
}

// PAD



























// EOF
