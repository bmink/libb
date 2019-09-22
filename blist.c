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
blist_uninit(blist_t **blist)
{
	/* This function does not know how to free the payload! Therefore it
	 * should be only called when the list has been emtpied!
	 * (Maybe later we will introduct a callback for the freeing.) */

	if(blist == NULL || *blist == NULL)
		return;

	free(*blist);
	*blist = NULL;
}


int
blist_lpush(blist_t *blist, void *data)
{
	blelem_t	*elem;

	if(blist == NULL || data == NULL)
		return EINVAL;

	elem = malloc(sizeof(blelem_t));
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

	elem = malloc(sizeof(blelem_t));
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


void *
blist_lpop(blist_t *blist)
{
	blelem_t	*elem;
	void		*retdata;

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

	retdata = elem->be_data;

	free(elem);

	return retdata;
}


void *
blist_rpop(blist_t *blist)
{
	blelem_t	*elem;
	void		*retdata;

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

	retdata = elem->be_data;

	free(elem);

	return retdata;
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
