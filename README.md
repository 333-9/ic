# ic - intiger calculator

Small command line calculator


## arguments
``` sh
ic [-hv]
ic [-peq] [file]
```
  * -p  enable prompt
  * -e  exit on error
  * -q  quiet (no error messages)
  * -h  help
  * -v  print version and exit
  * file   use file instead of `stdin`

## usage
Commands:
  * .q  exit ic
  * .d  decimal output format
  * .o  octal format
  * .x  hexadecimal format
  * .r  print contents of all registers
  * .c  clear the screen

-----------------------------------------------

```
1+2*3  # infix expressions
1 < 2  # bit shifting
1 | _  # _ represents previous result
a += _ # registers: a-z
.o     # changing output format
```
