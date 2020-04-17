CC = clang


ic: parser.c config.h
	$(CC) -o $@ $<



.PHONY: clear install test

test: ic
	./ic -e test.txt

clean:
	rm -f *.o
	rm -f ic

install: ic
	mkdir -p  /usr/local/bin/
	cp -f $<  /usr/local/bin/
	chmod 775 /usr/local/bin/$<
