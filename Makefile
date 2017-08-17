CFLAGS = -g -Wall

libb: bstr.o barr.o
	ar rc libb.a bstr.o barr.o && ranlib libb.a

clean:
	rm *o; rm libb.a

