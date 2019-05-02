objects = main.o BPlusTree.o
test: $(objects)
	gcc -g -o test  $(objects) -lm
main.o: main.c BPlusTree.h
	gcc -g -c main.c BPlusTree.h -lm
BPlusTree.o: BPlusTree.c BPlusTree.h
	gcc -g -c BPlusTree.c BPlusTree.h -lm
clean:
	-rm test $(objects)
