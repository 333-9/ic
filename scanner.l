%{
#include <stdio.h>
#include <stdlib.h>
#include "y.tab.h"

//extern long yylval;
extern long vars[26];
extern long prev_value;
%}

%option noyywrap


%%


"quit"|"exit" { return QUIT; }
"help"        { return HELP; }
"clear"       { return CLEAR; }

"dec"         { return F_dec; }
"hex"         { return F_hex; }
"oct"         { return F_oct; }
"bin"         { return F_bin; }
"chr"|"char"  { return F_chr; }

[1-9][0-9]*   { yylval = strtol(yytext, NULL, 10);  return INTIGER; }
0[0-9]+       { yylval = strtol(yytext, NULL,  8);  return INTIGER; }
0x[0-9a-f]+   { yylval = strtol(yytext, NULL, 16);  return INTIGER; }
0b[01]+       { yylval = strtol(yytext +2, NULL,  2);  return INTIGER; }
[Ff]alse|0    { yylval = 0;  return INTIGER; }
[Tt]rue       { yylval = 1;  return INTIGER; }


"_"    { yylval = prev_value; return INTIGER; }
[a-z]  { yylval = *yytext - 'a'; return VAR; }

"++"  return O_inc;
"--"  return O_dec;
"<<"  return O_lsh;
">>"  return O_rsh;
"**"  return O_pow;

"&&"  return C_and;
"||"  return C_or;
"^^"  return C_xor;
"=="  return C_eq;
"<="  return C_le;
">="  return C_ge;
"!="  return C_ne;

"="   return '=';
"+="  return A_add;
"-="  return A_sub;
"*="  return A_mul;
"/="  return A_div;
"%="  return A_mod;
"&="  return A_and;
"|="  return A_or;
"^="  return A_xor;
"<<=" return A_lsh;
">>=" return A_rsh;

[-+*/()|&^=?:!<>~]  return *yytext;

(#.*\n|\n)    return EOL;
<<EOF>>       return L_EOF;
[ \t]+  /* - */ ;


%%
