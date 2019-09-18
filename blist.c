#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "blist.h"

blist_t *
blist_init(void)
{
	blist_t	*bl;

	bl = malloc(sizeof(blist_t));
	if(bl == NULL)
		return NULL;

	memset(bl, 0, sizeof(blist_t));

	return bl;
}


void
blist_uninit(blist_t **blist, int freedata)
{
	/* This function should be either called when the list has been
	 * emtpied by the caller by freeing the elements' payload and the
	 * freedata argument set to 0. Or, if the payload is simple and can be
	 * freed with a single free(), then set freedata to nonzero, and
	 * this function takes care of it. */

	if(blist == NULL || *blist == NULL)
		return;

	blist_clear(*blist, freedata);

	free(*blist);
	*blist = NULL;
}


void
blist_clear(blist_t *blist, int freedata)
{
	blelem_t	*elem;

	if(blist == NULL)
		return;

	while((elem = blist_lpop(blist))) {
		if(elem->be_data != NULL && freedata)
			free(elem->be_data);
		free(elem);	
	}
}


int
blist_lpush(blist_t *blist, void *data)
{
	blelem_t	*elem;

	if(blist == NULL || data == NULL)
		return EINVAL;

	elem = malloc(sizeof(elem));
	memset(elem, 0, sizeof(blelem_t));

	elem->be_data = data;

	/* This will be the new first element. */
	if(blist->bl_first)
		blist->bl_first->be_prev = elem;
	elem->be_next = blist->bl_first;
	blist->bl_first = elem;

	
	/* If the list was empty, then this will also be the new last
	 * element. */
	if(blist->bl_last == NULL)
		blist->bl_last = elem;

	++blist->bl_cnt;

	return 0;
}


int
blist_rpush(blist_t *blist, void *data)
{
	blelem_t	*elem;

	if(blist == NULL || data == NULL)
		return EINVAL;

	elem = malloc(sizeof(elem));
	memset(elem, 0, sizeof(blelem_t));

	elem->be_data = data;

	/* This will be the new last element. */
	if(blist->bl_last)
		blist->bl_last->be_next = elem;
	elem->be_prev = blist->bl_last;
	blist->bl_last = elem;


	/* If the list was empty, then this will also be the new first
	 * element. */
	if(blist->bl_first == NULL)
		blist->bl_first = elem;
		

	++blist->bl_cnt;

	return 0;
}


blelem_t *
blist_lpop(blist_t *blist)
{
	blelem_t	*elem;

	if(blist == NULL)
		return NULL;

	if(blist->bl_first == NULL)
		return NULL;

	elem = blist->bl_first;
	blist->bl_first = elem->be_next;
	if(blist->bl_first)
		blist->bl_first->be_prev = NULL;
	
	if(elem == blist->bl_last)
		blist->bl_last = NULL;

	elem->be_prev = NULL;
	elem->be_next = NULL;

	--blist->bl_cnt;

	return elem;
}


blelem_t *
blist_rpop(blist_t *blist)
{
	blelem_t	*elem;

	if(blist == NULL)
		return NULL;

	if(blist->bl_last == NULL)
		return NULL;

	elem = blist->bl_last;
	blist->bl_last = elem->be_prev;
	if(blist->bl_last)
		blist->bl_last->be_next = NULL;

	if(elem == blist->bl_first)
		blist->bl_first = NULL;

	elem->be_prev = NULL;
	elem->be_next = NULL;

	--blist->bl_cnt;

	return elem;
}


blelem_t *
blist_getidx(blist_t *bl, int idx)
{
	int		i;
	blelem_t	*elem;

	if(bl == NULL)
		return NULL;

	i = 0;
	for(elem = bl->bl_first; elem != NULL; elem = elem->be_next) {
		if(i == idx)
			return elem;
		++i;
	}

	return NULL;
}
