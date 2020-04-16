CC = clang

calc: parser.c config.h
	$(CC) -o $@ $<

#scanner.o: y.tab.o
#y.tab.c: parser.y
#	yacc -d parser.y
#y.tab.h: y.tab.c
#y.tab.o: y.tab.c y.tab.h
#
.PHONY: clear install test


test: calc
	cat test.txt | ./calc

clean:
	rm -f *.o
	rm -f scanner.c

install: calc
	mkdir -p /usr/local/bin/
	cp -f calc /usr/local/bin/
	chmod 775 /usr/local/bin/calc
