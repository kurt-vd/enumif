default: libenumif.a enumif if_nameindex

CFLAGS	= -Wall

libenumif.a: libenumif.o tables.o
	ar crs $@ $^

enumif: libenumif.a

clean:
	rm -f $(wildcard *.o) libenumif.a enumif if_nameindex
