/*
 * date: 12.04 2020
 *
 * tiny parser for simple C-like syntax
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <setjmp.h>
#include <err.h>

#include "config.h"

long prev = 0;
long reg['z'-'a'] = { 0 };
jmp_buf jmp;
FILE *file;

long vstack[200];
long vsi = -1;
long rsi = 200;

/*

asign: expr ( [-+&|^<>*%/]? '=' asign )?

expr: term ( [-+&|^<>] term ...)

term: val ( [*%/] val ...)

val:
    '(' expr ')'
    ('0' 'x'?)? [0-9]+
    [a-z_]
    [-+~] val

*/


static void lex();
static void error(char *);

static unsigned parse_asign (char *, unsigned);
static unsigned parse_expr  (char *, unsigned);
static unsigned parse_term  (char *, unsigned);
static unsigned parse_val   (char *, unsigned);

static long push (long);
static long head ();
static long pop  ();

static long push2 (long);
static long head2 ();
static long pop2  ();


enum Tokens {
    Nol = 00,
    Reg  = 01,
    Val = 02,
    Typ = 03,
    BrL = 04,
    BrR = BrL+1,
    /**/
    Inv = 006, Mul = 007,
    Div = 010, Mod = Div+1,
    Add = 012, Sub = Add+1,
    Lsh = 014, Rsh = Lsh+1,
    And = 016, Or  = And+1, Xor = And+2,
    /**/
    Neg = 021, // used internally
    /**/
    Asn = 030,
};


int  tok;
long tokv;

static void
lex()
{
	int c;
	/**/
	do  c = getc(file);
	while (c > 0 && strchr(" \t", c));
	/**/
	if (islower(c)) {
		tok  = Reg;
		tokv = c -'a';
		return ;
	};
	if (isdigit(c)) {
		tok = Val;
		ungetc(c, file);
		fscanf(file, "%li", &tokv);
		return ;
	};
	if (c == '_') {
		tok  = Val;
		tokv = prev;
		return ;
	} else switch (c) {
	case '#': /*fallthrough*/
		while (c != '\n') c = getc(file);
		ungetc(c, file);
	case -1:
	case '\n': tok = Nol; return ;
	case '(': tok = BrL;  return ;
	case ')': tok = BrR;  return ;
	case '~': tok = Inv;  break ;
	case '*': tok = Mul;  break ;
	case '/': tok = Div;  break ;
	case '%': tok = Mod;  break ;
	case '+': tok = Add;  break ;
	case '-': tok = Sub;  break ;
	case '<': tok = Lsh;  break ;
	case '>': tok = Rsh;  break ;
	case '&': tok = And;  break ;
	case '|': tok = Or;   break ;
	case '^': tok = Xor;  break ;
	case '=': tok = Asn;  return ;
	default:
		ungetc(c, file);
		error("Invalid Token");
	};
	c = getc(file);
	if (c != '=') {
		ungetc(c, file);
	} else {
		tok += Asn;
	};
}

static void
error(char *s)
{
	while (tok != Nol) lex();
	if (exiterr) {
		if (quiet) exit(1);
		errx(1, s);
	} else {
		if (quiet) longjmp(jmp, 1);
		fputs(s, stderr);
		putc('\n', stderr);
		longjmp(jmp, 1);
	};
}

static unsigned
parse_asign(char *is, unsigned size)
{
	unsigned l;
	/**/
	l = parse_expr(is, size-1);
	if (tok >= Asn) {
		if (size < (tok > Asn ? 4:2))
			error("Out Of Memory");
		if (l == 1 && *is == Reg) {
			is[0] = Asn;
			if (tok > Asn) {
				is[l++] = tok - Asn;
				is[l++] = Reg;
				push(head());
			};
		} else error("Invalid Lvalue Syntax");
		l += parse_asign(is+l, size-l);
	};
	return l;
}

static unsigned
parse_expr(char *is, unsigned size)
{
	unsigned l;
	char ops[] = {
	    Add, Sub, Lsh, Rsh,
	    And, Or, Xor,
	};
	l = parse_term(is, size);
	while (memchr(ops, tok, sizeof(ops))) {
		if (size-l <= 2) error("Out Of Memory");
		memmove(is+1, is, l++); *is = tok;
		l += parse_term(is+l, size-l);
	};
	return l;
}

static unsigned
parse_term(char *is, unsigned size)
{
	unsigned l;
	char ops[] = {
	    Mul, Div, Mod,
	};
	l = parse_val(is, size);
	while (memchr(ops, tok, sizeof(ops))) {
		if (size-l <= 2) error("Out Of Memory");
		memmove(is+1, is, l++); *is = tok;
		l += parse_val(is+l, size-l);
	};
	return l;
}

static unsigned
parse_val(char *is, unsigned size)
{
	unsigned l = 0;
top:
	lex();
	if (size < 1) error("Out Of Memory");
	if (tok == BrL) {
		l = parse_asign(is, size);
		if (tok != BrR) error("Missing ')'");
	} else if (tok == Val || tok == Reg) {
		is[l++] = tok;
		push(tokv);
	} else if (tok == Inv) {
		is[0] = Inv;
		return 1+parse_val(is+1, size-1);
	} else if (tok == Sub) {
		is[0] = Neg;
		return 1+parse_val(is+1, size-1);
	} else if (tok == Add) {
		goto top;
	} else {
		error("Unexpected Token");
	};
	lex();
	return l;
}



static long
exec(char *is, unsigned size)
{
	unsigned i, o;
	long v;
	/**/
	//fprintf(stderr, "%d,\n", size);
	for (i = 1; i <= size; i++) {
		o = is[size-i];
		if ((o == Div || o == Mod) && vstack[rsi+1] == 0)
			error("Division By Zero");
		switch (o) {
		//case Typ:
		case Val: push2(pop());           break;
		case Reg: push2(reg[pop()]);      break;
		case Inv: push2(~pop2());         break;
		case Mul: push2(pop2() * pop2()); break;
		case Div: push2(pop2() / pop2()); break;
		case Mod: push2(pop2() % pop2()); break;
		case Add: push2(pop2() + pop2()); break;
		case Sub: push2(pop2() - pop2()); break;
		case Lsh: push2(pop2() <<pop2()); break;
		case Rsh: push2(pop2() >>pop2()); break;
		case And: push2(pop2() & pop2()); break;
		case  Or: push2(pop2() | pop2()); break;
		case Xor: push2(pop2() ^ pop2()); break;
		case Neg: push2(-pop2());         break;
		case Asn: push2(reg[pop()] = pop2()); break;
		};
	};
	return pop2();
}

static long push  (long v) { return vstack[++vsi] = v; }
static long head  ()       { return vstack[vsi];       }
static long pop   ()       { return vstack[vsi--];     }
static long push2 (long v) { return vstack[--rsi] = v; }
static long head2 ()       { return vstack[rsi];       }
static long pop2  ()       { return vstack[rsi++];     }



int
main(int argc, char *argv[])
{
	int c, i;
	char is[200];
	const char *form = fmt_default;
	file = stdin;
	// ---
	for (i = 1; i < argc; i++) {
		if (*argv[i] != '-') {
			if (file != stdin)
				err(1, "usage: ic [-peq] [-hv] file");
			file = fopen(argv[i], "r");
			if (file == NULL)
				err(1, "Unable to Open File");
			continue;
		};
		for (c = 0; argv[i][c]; c++) {
			switch (argv[i][c]) {
			case 'p': prompt  = 1;  break;
			case 'e': exiterr = 1;  break;
			case 'q': quiet   = 1;  break;
			case 'h':
				puts("usage: ic [-peq] [-hv] file");
				return 0;
			case 'v':
				puts("ic version 1");
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
		case '\n':
			if (prompt) printf(form, prev);
			break;
		case  0:
		case -1:
			if (prompt) putc('\n', stdout);
		case 'Q': return 0;
		case '.':
			switch ((c = getc(file))) {
			case 'x': form = fmt_hex;     break;
			case 'o': form = fmt_octal;   break;
			case 'd': form = fmt_decimal; break;
			case 'q': return 0;
			case 'c':
				fputs("\033[H\033[J", stdout);
				break;
			case 'r':
				for (i = 0; i < 'z'-'a'; i++) {
					if (reg[i] == 0) continue;
					printf("%c = ", 'a' +i);
					printf(form, reg[i]);
				};
				fputs("_ = ", stdout);
				break;
			default:
				ungetc(c, file);
				error("Invalid Format");
			};
			do c = getc(file);
			while (c != '\n' && c > 0);
			printf(form, prev);
			break;
		default:
			ungetc(c, file);
			do prev = exec(is, parse_asign(is, 200));
			while (tok != Nol);
			vsi = -1;
			rsi = 200;
			printf(form, prev);
			break;
		};
	};
	return 0;
}

/*
static long
parse_paren()
{
	long val;
	val = parse_expr();
	if (getc(file) != ')')
		error("')' expected", 0);
	return val;
}

static long
parse_term()
{
	long val;
	char c, r;
	c = parse_space();
	if (c == '\n') return 0;
	ungetc(c, file);
	// scanf consumes '+' & '-'
	if (fscanf(file, "%li", &val) < 1) {
		if (!strchr("+-", c)) {
			c = parse_space();
		};
		if (c >= 'a' && c <= 'f') {
			r = c - 'a';
			c = parse_space();
			if (c == '=') {
				val = parse_expr();
				reg_val[r] = val;
				return val;
			} else {
				ungetc(c, file);
				val = reg_val[r];
			};
		} else switch (c) {
		case '~':  val = ~ parse_term();  break;
		case '-':  val = - parse_term();  break;
		case '+':  val = + parse_term();  break;
		case '_':  val = prev_val;  break;
		case '(':  val = parse_paren();  break;
		case ')':  error("')' unexpected", 0);
		case '#':
			ungetc(c, file);
			return prev_val;
		default:
			ungetc(c, file);
			error("unknown token `%c`", c? c:'?');
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
				error("expected '%c'", c);
			if (c == '<')
				val <<= parse_term();
			else
				val >>= parse_term();
			break;
		case '=':
			if (val < 10)
				return reg_val[val] = parse_expr();
			else
				error("register index too high", 0);
		case '#':
		case ')':
		case '\n':
			ungetc(c, file);
		case -1:
			return val;
		default:
			error("unknown token '%c'", c? c:'?');
		};
	};
}
*/
