default: libenumif.a enumif if_nameindex

-include config.mk

libenumif.a: libenumif.o tables.o
	$(AR) crs $@ $^

enumif: libenumif.a

clean:
	rm -f $(wildcard *.o) libenumif.a enumif if_nameindex
