/*
 * 02 Nov 2019
 */


#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"

#include "y.tab.h"
#include "config.h"


#define VERSION 0
#define BUILD   1
#define HELP_STR \
"calc -[pPVh]
  -p show prompt
  -P show color prompt
  -V print version and quit 
  -h print this help and quit
"




int
main(int argc, char *argv[])
{
	int i;
	int ret = 0;
	/* --- */
	for (i = 1 ;argv[i] != NULL; i++) {
		if (argv[i][0] == '-') switch (argv[i][1]) {
		case 'p': ps1 = ps1_def;    break;
		case 'P': ps1 = ps1_color;  break;
		case 'V': printf("calc version %i.%i\n", VERSION, BUILD); exit(0);
		case 'h': puts(HELP_STR); exit(0);
		default:
			break;
		};
	};
	do {
		if (show_cursor) fputs(ps1, stderr);
		ret = yyparse();
		if (!exit_on_err && ret != 3) ret = 0;
	} while (!ret);
	return (ret == 3 ? 0 : 1); /* 3 - exit with no error */
}
