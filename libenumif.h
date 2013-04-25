/*
 * Copyright 2013 Kurt Van Dijck <kurt@vandijck-laurijssen.be>
 *
 * This file is part of libenumif.
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser Public License for more details.
 *
 * You should have received a copy of the GNU Lesser Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _libenumif_h_
#define _libenumif_h_

#ifdef __cplusplus
extern "C" {
#endif

struct enumif {
	unsigned int if_index;
	int if_type;
	int if_flags;
	int if_state;
	char if_name[32]; /* this is stupid, we should use IFNAMSIZ instead */
};

/* allocate an array of enumif structures, with 1 extra with if_index=0 */
extern struct enumif *enumif(int if_type, int if_flags, int if_state);

/* string representations */
extern const char *if_flagsstr(int if_flags);
extern const char *if_statestr(int if_state);
extern const char *if_typestr(int if_type);

/* string decoding */
extern int if_strtype(const char *type);
extern int if_strflags(const char *flags);
extern int if_strstate(const char *state);

/*
 * cleanup enumif resources.
 * This is called automatically on program shutdown.
 * It is exported if a program wants to free resources
 * during operation
 */
extern void enumif_cleanup(void);

#ifdef __cplusplus
}
#endif
#endif
