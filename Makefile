.PHONY : all

all: testmem

CFLAGS=-Ofast -march=native -funroll-loops -Wall -g

testmem:
	$(CC) testmem.c ${CFLAGS} -o testmem 
test:
	./testmem auto

.PHONY : clean
clean:
	rm -f testmem *.o

indent:
	clang-format -i -style=llvm testmem.c
