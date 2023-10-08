run:
	gcc -o array array.c -pthread
	./array
	rm -f array