#include "y.tab.h"
#include "stdio.h"

int
main()
{
	int ret = 0;
	do {
		fputs("\x1b[32m> \x1b[m", stderr);
	} while (!(ret = yyparse()));
	return (ret == 3 ? 0 : 1); /* 3 - normal exit */
}
