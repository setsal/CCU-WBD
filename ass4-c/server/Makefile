objects = main.o BPlusTree.o Block.o
test: $(objects)
	gcc -g -o server  $(objects) -lm -lwsock32
main.o: main.c BPlusTree.h Block.h
	gcc -g -c main.c BPlusTree.h Block.h -lm
BPlusTree.o: BPlusTree.c BPlusTree.h
	gcc -g -c BPlusTree.c BPlusTree.h -lm
Block.o: Block.c Block.h
	gcc -g -c Block.c Block.h -lm
clean:
	-rm server $(objects)
