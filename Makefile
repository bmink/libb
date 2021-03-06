CFLAGS = -g -Wall -Wstrict-prototypes -D_GNU_SOURCE

libb: bstr.o barr.o btime.o bcurl.o blog.o bint.o bfs.o blist.o bvalmap.o 
	ar rc libb.a bstr.o barr.o btime.o bcurl.o blog.o bint.o bfs.o blist.o \
		bvalmap.o && ranlib libb.a

install: libb
	cp libb.a ~/lib; cp *.h ~/include

clean:
	-rm *o; rm libb.a

