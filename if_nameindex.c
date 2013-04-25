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
