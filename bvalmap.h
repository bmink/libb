/*
 * bvalmap: String/Value mapping tables
 */
#ifndef BVALMAP_H
#define BVALMAP_H


typedef struct bvalmap {
	char    *bv_str;
	int     bv_intval;
} bvalmap_t;


int bvalmap_validint(bvalmap_t *, int);
int bvalmap_validstr(bvalmap_t *, char *);

int bvalmap_getstr(bvalmap_t *, int, char **);
int bvalmap_getint(bvalmap_t *, char *, int *);


#endif
