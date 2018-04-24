.PHONY: all
all: clean md2

.PHONY: md2
md2:
	gcc -g -Wall md2.c -lm -o md2

.PHONY: clean
clean:
	rm -rf md2
