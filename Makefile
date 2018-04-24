.PHONY: all
all: clean md2

.PHONY: md2
md2:
	gcc -Wall md2.c -lm -o md2

.PHONY: md2-debug
md2-debug:
	gcc -g -Wall md2.c -lm -o md2-debug

.PHONY: clean
clean:
	rm -f md2
	rm -f md2-debug
