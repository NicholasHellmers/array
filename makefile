run:
	gcc -o array array.c -lpthread
	./array
	rm -f array

array2:
	gcc -o array2 array2.c -lpthread
	./array2
	rm -f array2