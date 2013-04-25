#include <stdio.h>
#include <net/if.h>

int main(int argc, char *argv[])
{
	struct if_nameindex *table, *iface;

	table = if_nameindex();
	for (iface = table; iface->if_index; ++iface) {
		printf("%i: %s\n", iface->if_index, iface->if_name);
	}
	if_freenameindex(table);
	return 0;
}
