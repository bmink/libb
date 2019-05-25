#ifndef BLIST_H
#define BLIST_H

typedef struct blelem {
	void		*be_data;
	struct blelem	*be_next;
	struct blelem	*be_prev;
} blelem_t;


typedef struct blist {
	size_t		bl_cnt;
	blelem_t	*bl_first;
	blelem_t	*bl_last;
} blist_t;


blist_t *blist_init(void);
void blist_uninit(blist_t **, int);

int blist_lpush(blist_t *, void *);
int blist_rpush(blist_t *, void *);

blelem_t *blist_lpop(blist_t *);
blelem_t *blist_rpop(blist_t *);

int blist_insertafter(blist_t *, blelem_t *, void *);
int blist_delelem(blist_t *, blelem_t *);

#endif
