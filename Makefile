calc: main.c y.tab.o scanner.o config.h
	tcc -o $@ main.c scanner.o y.tab.o

scanner.o:
y.tab.c: parser.y
	yacc -d parser.y
y.tab.h: y.tab.c
y.tab.o: y.tab.c y.tab.h

.PHONY: clear install

clear:
	rm -f *.o
	rm -f scanner.c

install: calc
	mv -f ./calc ~/.bin/
