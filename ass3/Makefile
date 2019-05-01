objects = main.o BPlusTree.o
test: $(objects)
	gcc -g -o test  $(objects)
main.o: main.c BPlusTree.h
	gcc -g -c main.c BPlusTree.h
BPlusTree.o: BPlusTree.c BPlusTree.h
	gcc -g -c BPlusTree.c BPlusTree.h
clean:
	-rm test $(objects)
