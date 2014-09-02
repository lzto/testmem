.PHONY : all

all: testmem

testmem:
	cc -O testmem.c -o testmem
test:
	./testmem auto

.PHONY : clean
clean:
	rm -f testmem *.o

