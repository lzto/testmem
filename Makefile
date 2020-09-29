.PHONY : all

all: testmem

CFLAGS=-O2 -march=native -funroll-loops

testmem:
	$(CC) testmem.c ${CFLAGS} -o testmem 
test:
	./testmem auto

.PHONY : clean
clean:
	rm -f testmem *.o

