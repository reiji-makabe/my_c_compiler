CFLAGS :=-std=c11 -g -static

all:9cc tags

9cc: 9cc.c
	$(CC) $(CFLAGS) -o 9cc 9cc.c

test: 9cc
	./test.sh

tags:$(wildcard *.c *.h)
	@if [ ! -f tags ] || [ "$(shell find . -name '*.c' -newer tags -o -name '*.h' -newer tags)" ]; then \
		echo "Updating ctags..."; \
		ctags -R --exclude=Makefile; \
	fi

clean:
	rm -f 9cc *.o *~ tmp* a.out

re:clean
	make all

debug:CFLAGS += -g3 -O0
debug:clean 9cc

.PHONY: all test clean re debug tags
