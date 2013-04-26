/*
 * Copyright 2013 Kurt Van Dijck <kurt@vandijck-laurijssen.be>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Demonstrator/test for libenumif.
 */
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

	table = enumif();
	if (!table)
		return 1;
	for (iface = table; iface->if_index; ++iface) {
		/* test matching criteria */
		if (type && (iface->if_type != type))
			continue;
		if (flags && (~iface->if_flags & flags))
			continue;

		printf("%i: %s\t{%s} <%s> %s\n", iface->if_index, iface->if_name,
				if_typestr(iface->if_type),
				if_flagsstr(iface->if_flags),
				if_statestr(iface->if_state));
	}
	return 0;
}

