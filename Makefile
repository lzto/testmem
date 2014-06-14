.PHONY : all

all: testmem

testmem:
	cc -Ofast testmem.c -o testmem
test:
	./testmem auto

.PHONY : clean
clean:
	rm -f testmem *.o

