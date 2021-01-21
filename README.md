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
  * D  decimal output format
  * O  octal format
  * X  hexadecimal format
  * R  print contents of all registers

-----------------------------------------------

```
    # shell like comments
    # There are 10 registers (0-9).
    a = 5       # assign values to registers
    b = a * 3   # get register values with '$'
    1 << 63     # long int  values
    _ + a + b   # '_' returns previous result
```
