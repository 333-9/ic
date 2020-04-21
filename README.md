# ic - intiger calculator

ic is a small command line calculator with on C-like syntax


## arguments
``` sh
ic [-hv]
ic [-peq] [file]
```
  * -p  enable prompt
  * -e  exit on error
  * -q  quiet (no error messages)
  * -h  help
  * -v  paint version and exit
  * file   use file instead of `stdin`

## usage
commands are one letter followed by newline
  * q  exit ic
  * d  decimal output format
  * o  octal format
  * x  hexadecimal format
  * r  print contents of all registers


comments start with `#` and end at the end of line
there are 10 registers (0-9).
you can assign value to a "register" with `=`
`$(0-9)` expands to the register value
```
    : 0 = 5
5
    : 3 = $0 * 3
15
```
> all registers are `long int`


`_` expands to the previous result
```
    : 50 + 3
53
    : _ - 32   # 53 - 32
21
```


most C operators are includet
``` c
( )  ~  *  /  %
-  +  <<  >>  &  |  ^
```
