#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "libenumif.h"

int main(int argc, char *argv[])
{
	struct enumif *table, *iface;
	int type = 0, flags = 0;
	int j;

	if (argc >= 2)
		type = if_strtype(argv[1]);

	for (j = 2; j < argc; ++argc)
		flags |= if_strflags(argv[j]);

	table = enumif(type, flags, 0);
	if (!table)
		return 1;
	for (iface = table; iface->if_index; ++iface) {
		printf("%i: %s\t{%s} <%s> %s\n", iface->if_index, iface->if_name,
				if_typestr(iface->if_type),
				if_flagsstr(iface->if_flags),
				if_statestr(iface->if_state));
	}
	return 0;
}

