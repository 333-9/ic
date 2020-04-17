/*
 * date: 12.04 2020
 *
 * tiny parser for simple C-like syntax
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include <err.h>

#include "config.h"


long prev_val = 0;
long reg_val[10] = { 0 };
jmp_buf jmp;
FILE *file;



static long parse_term();
static long parse_expr();


void
p_error(char *s, char c)
{
	if (exit_on_err) {
		if (quiet) exit(1);
		if (c != 0)
			errx(1, s, c);
		else
			errx(1, s);
	} else {
		if (quiet) longjmp(jmp, 1);
		if (c != 0)
			fprintf(stderr, s, c);
		else
			fputs(s, stderr);
		putc('\n', stdout);
		longjmp(jmp, 1);
	};
}


char
parse_reg()
{
	char c;
	c = getc(file);
	if (c < '0' || c > '9')
		p_error("digit expected, not %c", c);
	return c - '0';
}


long
parse_paren()
{
	long val;
	val = parse_expr();
	if (getc(file) != ')')
		p_error("')' expected", 0);
	return val;
}


char
parse_space()
{
	char c;
	do  c = getc(file);
	while (c > 0 && strchr(" \t", c));
	return c;
}


static long
parse_term()
{
	long val;
	char c;
	c = parse_space();
	ungetc(c, file);
	// scanf consumes '+' & '-'
	if (fscanf(file, "%li", &val) < 1) {
		if (!strchr("+-", c))
			c = parse_space();
		switch (c) {
		case '~':  val = ~ parse_term();  break;
		case '-':  val = - parse_term();  break;
		case '+':  val = + parse_term();  break;
		case '_':  val = prev_val;  break;
		case '$':  val = reg_val[parse_reg()];  break;
		case '(':  val = parse_paren();  break;
		case ')':  p_error("')' unexpected", 0);
		case '#':
			ungetc(c, file);
			return prev_val;
		default:
			ungetc(c, file);
			p_error("unknown token `%c`", c? c:'?');
		};
	};
	c = parse_space();
	switch (c) {
	case '*': return val * parse_term();
	case '/': return val / parse_term();
	case '%': return val % parse_term();
	default:
		ungetc(c, file);
		return val;
	};
}


static long
parse_expr()
{
	int c;
	long val;
	c = getc(file);
	if (c == '\n' || c <= 0) return 0;
	ungetc(c, file);
	val = parse_term();
	for (;;) {
		c = getc(file);
		switch (c) {
		case '\t':
		case ' ':
			continue;
		case '+': val += parse_term(); break;
		case '-': val -= parse_term(); break;
		case '&': val &= parse_term(); break;
		case '|': val |= parse_term(); break;
		case '^': val ^= parse_term(); break;
		case '<':
		case '>':
			if (getc(file) != c)
				p_error("expected %c", c);
			if (c == '<')
				val <<= parse_term();
			else
				val >>= parse_term();
			break;
		case '=':
			if (val < 10)
				return reg_val[val] = parse_expr();
			else
				p_error("register index too high", 0);
		case '#':
		case '\n':
		case ')':
			ungetc(c, file);
		case -1:
			return val;
		default:
			p_error("unknown token '%c'", c? c:'?');
		};
	};
}


int
main(int argc, char *argv[])
{
	int c, i;
	char *form = "%d\n";
	file = stdin;
	for (i = 1; i < argc; i++) {
		if (*argv[i] != '-') {
			if (file != stdin)
				err(1, "usage: ic [-peq] [-hv] file");
			file = fopen(argv[i], "r");
			if (file == NULL) err(1, "");
			continue;
		};
		for (c = 0; argv[i][c]; c++) {
			switch (argv[i][c]) {
			case 'p': prompt = 1;       break;
			case 'e': exit_on_err = 1;  break;
			case 'q': quiet = 1;        break;
			case 'h':
				puts("usage: ic [-peq] [-hv] file");
				return 0;
			case 'v':
				puts("ic version 0");
				return 0;
			};
		};
	};
	if (setjmp(jmp)) {
		do  c = getc(file);
		while (c > 0 && c != '\n');
		if (c <= 0) return 0;
	};
	for (;;)  {
		if (prompt) fputs(ps, stderr);
		switch (c = getc(file)) {
		case  0:
		case -1:
			if (prompt) putc('\n', stdout);
			return 0;
		case 'q':
			return 0;
		case '\n': ungetc(c, file); break;
		case 'x':  form = "%x\n";    break;
		case 'o':  form = "%o\n";    break;
		case 'd':  form = "%d\n";    break;
		case 'r':
			for (i = 0; i < 10; i++) {
				printf("%d = ", i);
				printf(form, reg_val[i]);
			};
			fputs("_ = ", stdout);
			break;
		default:
			ungetc(c, file);
			prev_val = parse_expr();
			break;
		};
		c = parse_space();
		if (c == '#') {
			while (c > 0 && c != '\n')
				c = getc(file);
		} else if (c <= 0) {
			printf(form, prev_val);
			return 0;
		} else if (c != '\n') {
			ungetc(c, file);
		};
		printf(form, prev_val);
	};
	return 0;
}
