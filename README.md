# ic - intiger calculator

Small command line calculator with C-like syntax


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
  * q  exit ic
  * d  decimal output format
  * o  octal format
  * x  hexadecimal format
  * r  print contents of all registers

-----------------------------------------------

```
    # shell like comments
    # There are 10 registers (0-9).
    0 = 5       # assign values to registers
    3 = $0 * 3  # get register values with '$'
    1 << 63     # long int  values
    ~ _         # '_' returns previous result
```

```
all valid operators:
	( )  ~  *  /  %
	-  +  <<  >>  &  |  ^
```
