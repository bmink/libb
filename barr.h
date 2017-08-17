/*
 * barr: Barna's C array library
 */
#ifndef BARR_H
#define BARR_H

#include <stdlib.h>
#include <string.h>


typedef struct barr {
	void	*ba_data;

	size_t	ba_cnt;
	size_t	ba_size;
	int	ba_growperc;

	size_t	ba_elemsiz;
	
	int	ba_sorted;
} barr_t;

#define BARR_DEFAULT_INITSIZ	1024
#define BARR_DEFAULT_GROWPERC	50

barr_t *barr_init(int);
barr_t *barr_init_siz(int, int, int);  /* Extended version with more control
					* over initial size and growth. */
int barr_uninit(barr_t **);

/* For adding simple things that need no setup. (Arrays of simple values or 
 * structs that can be simply copied. */
int barr_add(barr_t *, void *);

/* For adding things that need setup (structs that need strings duplicated etc.)
 * barr_add_begin() returns the pointer to the next element. barr_add_done()
 * finalizes the addition of the new element. On error, caller should simply
 * clean up any allocations it made on its side and not call barr_add_done(). */
int barr_add_begin(barr_t *, void **);
int barr_add_done(barr_t *);


int barr_cnt(barr_t *);

int barr_sort(barr_t *, int (*)(const void *, const void *));

void *barr_search_single(barr_t *, void *,
	int (*)(const void *, const void *));
int barr_search_multi(barr_t *, void *key, int (*)(const void *, const void *),
	void **, int **);
/* Compare functions for some common searches. */
int barr_cmp_int(const void *, const void *);
int barr_cmp_str(const void *, const void *);


void *barr_elem(barr_t *, int);

/* These two are useful for looping like such:
 * for(e = barr_begin(a); e < barr_end(a); e++) */
void *barr_begin(barr_t *);
void *barr_end(barr_t *);

#endif
