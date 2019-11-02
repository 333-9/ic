%{
#include <stdio.h>
#include <stdlib.h>


long vars[26] = {0};
long prev_value = 0;


#define YYSTYPE long


#define DIV_Z(a) if (!a) { yyerror("division by zero error"); return 1; }
int yylex();
void yyerror(char *);

long lpow(long, short);
void print_bit(long);
%}

%token VAR
%token INTIGER
%token EOL L_EOF
%token QUIT
%token HELP
%token CLEAR
%token F_dec F_hex F_oct F_bin F_chr
%token '(' ')'

%right '=' A_add A_sub A_mul A_div A_mod A_and A_or A_xor A_lsh A_rsh

%left '?' ':'
%left C_and C_or C_xor C_eq C_le C_ge C_ne '<' '>'
%left O_lsh O_rsh
%left '+' '-'
%left '|' '&' '^'
%left '*' '/' '%'
%left O_pow
%left O_inc O_dec
%left '~' '!'


%%


program:L_EOF              { putc('\n', stderr); return 3; }
|	EOL                { return 0; }
|	command EOL        { return 0; }
|	expr EOL           { prev_value = $1; printf("%li\n", $1); return 0; }
|	/* NOP */
;

expr:	INTIGER             { $$ = $1; }
|	assign              { $$ = $1; }
|	cmp                 { $$ = $1; }
|	func                { $$ = $1; }
|	expr O_lsh expr     { $$ = $1 << $3; }
|	expr O_rsh expr     { $$ = $1 >> $3; }
|	expr '+' expr       { $$ = $1 + $3; }
|	expr '-' expr       { $$ = $1 - $3; }
|	expr '*' expr       { $$ = $1 * $3; }
|	expr '/' expr       { DIV_Z($3);  $$ = $1 / $3; }
|	expr '%' expr       { DIV_Z($3);  $$ = $1 % $3; }
|	expr '&' expr       { $$ = $1 & $3; }
|	expr '|' expr       { $$ = $1 | $3; }
|	expr '^' expr       { $$ = $1 ^ $3; }
|	expr O_pow expr     { $$ = $3 < 0 ? 0 : lpow($1, $3); }
|	'-' expr            { $$ = - $2; }
|	'+' expr            { $$ = + $2; }
|	'~' expr            { $$ = ~ $2; }
|	expr '?' expr ':' expr    { $$ = $1 ? $3 : $5; }
|	'(' expr ')'              { $$ = $2; }
;

cmp:	'!' expr           { $$ = ! $2; }
|	expr '<' expr      { $$ = $1 < $3; }
|	expr '>' expr      { $$ = $1 > $3; }
|	expr C_eq  expr    { $$ = $1 == $3; }
|	expr C_ne  expr    { $$ = $1 != $3; }
|	expr C_le  expr    { $$ = $1 <= $3; }
|	expr C_ge  expr    { $$ = $1 >= $3; }
|	expr C_and expr    { $$ = $1 && $3; }
|	expr C_or  expr    { $$ = $1 || $3; }
|	expr C_xor expr    { $$ = !!$1 ^ !!$3; }
;

assign:	VAR              { $$ = vars[$1]; }
|	O_inc VAR        { $$ = ++(vars[$1]); }
|	O_dec VAR        { $$ = --(vars[$1]); }
|	VAR O_inc        { $$ = vars[$1]++; }
|	VAR O_dec        { $$ = vars[$1]--; }
|	VAR '=' expr     { $$ = vars[$1]  = $3; }
|	VAR A_add expr   { $$ = vars[$1] += $3; }
|	VAR A_sub expr   { $$ = vars[$1] -= $3; }
|	VAR A_mul expr   { $$ = vars[$1] *= $3; }
|	VAR A_div expr   { DIV_Z($3);  $$ = vars[$1] /= $3; }
|	VAR A_mod expr   { DIV_Z($3);  $$ = vars[$1] %= $3; }
|	VAR A_and expr   { $$ = vars[$1] &= $3; }
|	VAR A_or expr    { $$ = vars[$1] |= $3; }
|	VAR A_xor expr   { $$ = vars[$1] ^= $3; }
|	VAR A_lsh expr   { $$ = vars[$1] <<= $3; }
|	VAR A_rsh expr   { $$ = vars[$1] >>= $3; }
;

func:
	F_dec '(' expr ')'    { $$ = $3; printf("%li\n", $3); }
|	F_hex '(' expr ')'    { $$ = $3; printf("0x%lx\n", $3); }
|	F_oct '(' expr ')'    { $$ = $3; printf("0%lo\n", $3); }
|	F_bin '(' expr ')'    { $$ = $3; print_bit($3); }
|	F_chr '(' expr ')'    { $$ = $3; printf("%lc\n", $3); }
;

command: QUIT    { return 3; }
|	HELP     { return 3; }
|	CLEAR    { fputs("\033[H\033[2J", stderr); }
;


%%


long
lpow(long a, short b) {
	return (b < 1 ? 1 : a * lpow(a, --b));
}


void
print_bit(long n) {
	unsigned long bit = 1;
	int zf = 0;
	fputs("0b", stdout);
	for (bit <<= ((sizeof(long) * 8) - 1); bit > 0; bit >>= 1) {
		if (bit & n) {
			zf = 1;
			putc('1', stdout);
		} else if (zf) {
			putc('0', stdout);
		};
	};
	if (!zf) putc('0', stdout);
	putc('\n', stdout);
}


void
yyerror(char *s)
{
	int i;
	fputs(s, stderr);
	putc('\n', stderr);
	do {
		i = yylex();
		if (i == L_EOF) exit(1);
	} while (i != EOL);
}
