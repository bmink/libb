libb: bstr.o barr.o
	ar rc libb.a bstr.o barr.o && ranlib libb.a

bstr.o:	bstr.h bstr.c
	cc -c bstr.c

barr.o:	barr.h barr.c
	cc -c barr.c

clean:
	rm *o; rm libb.a

