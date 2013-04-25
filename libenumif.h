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
