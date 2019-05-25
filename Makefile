CFLAGS = -g -Wall -Wstrict-prototypes

libb: bstr.o barr.o btime.o bcurl.o blog.o bint.o bfs.o blist.o
	ar rc libb.a bstr.o barr.o btime.o bcurl.o blog.o bint.o bfs.o blist.o \
		&& ranlib libb.a

install: libb
	cp libb.a ~/lib; cp *.h ~/include

clean:
	-rm *o; rm libb.a

