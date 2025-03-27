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


/* NOTE: decided to not have a blist_clear(). This is because we don't know
 * how to free up the payload. Also note that blist_uninit() should only be
 * called when the list is empty. Perhaps later if better cleanup functions
 * are really needed then we can introduce a callback for the freeing.
 *
 * The list should be cleared up by something like:
 * while(list->bl_cnt > 0) {
 *         elem = (elem_t *) blist_rpop(list);
 *         // Uninit and free elem
 * }
 * blist_uninit(&list);
 *
 * void blist_clear(blist_t *, int);
 */
void blist_uninit(blist_t **);

int blist_lpush(blist_t *, void *);
int blist_rpush(blist_t *, void *);

void *blist_lpop(blist_t *);
void *blist_rpop(blist_t *);

blelem_t *blist_getidx(blist_t *, int);
size_t blist_cnt(blist_t *);

#endif
