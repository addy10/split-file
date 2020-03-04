all: reassemble.c 
	gcc reassemble.c -ljson-c -o reassemble
	./reassemble
clean: 
	$(RM) reassemble