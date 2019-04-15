CFLAGS = -g -Wall

libb: bstr.o barr.o btime.o bcurl.o blog.o bint.o bfs.o
	ar rc libb.a bstr.o barr.o btime.o bcurl.o blog.o bint.o bfs.o \
		&& ranlib libb.a

install: libb
	cp libb.a ~/lib; cp *.h ~/include

clean:
	-rm *o; rm libb.a

