#include "barr.h"
#include "bstr.h"
#include "errno.h"

#define BARR_ELEM_PTR(b,i)	(b->ba_data + i * b->ba_elemsiz)

barr_t *
barr_init(int elemsiz)
{
	return barr_init_siz(elemsiz, BARR_DEFAULT_INITSIZ,
	    BARR_DEFAULT_GROWPERC);
}


barr_t *
barr_init_siz(int elemsiz, int initsiz, int growperc)
{
	barr_t	*barr;

	barr = malloc(sizeof(barr_t));

	if(barr == NULL)
		return NULL;

	memset(barr, 0, sizeof(barr_t));

	barr->ba_data = malloc(elemsiz * initsiz);
	if(barr->ba_data == NULL) {
		free(barr);
		return NULL;
	}
		
	barr->ba_size = initsiz;
	barr->ba_growperc = growperc;
	barr->ba_elemsiz = elemsiz;

	return barr;
}


int
barr_uninit(barr_t **barr)
{
	if(barr == NULL || *barr == NULL)
		return EINVAL;

	if((*barr)->ba_data)
		free((*barr)->ba_data);

	free(*barr);
	*barr = NULL;

	return 0;
}


static int
check_size(barr_t *barr)
{
	size_t	newsiz;
	void	*newdat;

	if(barr == NULL)
		return EINVAL;

	/* Grow if needed. */
	if(barr->ba_cnt == barr->ba_size) {
		newsiz = barr->ba_size * (100 + barr->ba_growperc) / 100;

		newdat = realloc(barr->ba_data, newsiz * barr->ba_elemsiz);
		if(newdat == NULL)
			return ENOMEM;
		barr->ba_data = newdat;
		barr->ba_size = newsiz;
	}

	return 0;
}


int
barr_add(barr_t *barr, void *elem)
{
	int	ret;
	void	*elemstart;

	if(barr == NULL || elem == NULL)
		return EINVAL;

	ret = check_size(barr);
	if(ret)
		return ret;

	elemstart = BARR_ELEM_PTR(barr, barr->ba_cnt);
	memcpy(elemstart, elem, barr->ba_elemsiz);

	barr->ba_cnt++;

	return 0;
}


int
barr_add_begin(barr_t *barr, void **elem)
{
	int	ret;
	void	*elemstart;

	if(barr == NULL || elem == NULL)
		return EINVAL;

	ret = check_size(barr);
	if(ret)
		return ret;

	elemstart = BARR_ELEM_PTR(barr, barr->ba_cnt);
	memset(elemstart, 0, barr->ba_elemsiz);

	*elem = elemstart;

	return 0;
}


int
barr_add_done(barr_t *barr)
{
	if(barr == NULL)
		return EINVAL;
	barr->ba_cnt++;
	return 0;
}


int
barr_cnt(barr_t *barr)
{
	if(barr == NULL)
		return 0;
	return barr->ba_cnt;
}


int
barr_sort(barr_t *barr, int (*cmp)(const void *, const void *))
{
	if(barr == NULL)
		return EINVAL;

	if(barr_cnt(barr) <= 1)
		return 0;

	qsort(barr->ba_data, barr->ba_cnt, barr->ba_elemsiz, cmp);

	barr->ba_sorted++;

	return 0;
}


void *
barr_elem(barr_t *barr, int idx)
{
	if(barr == NULL || idx >= barr_cnt(barr))
		return NULL;
	return BARR_ELEM_PTR(barr, idx);
}


void *
barr_begin(barr_t *barr)
{
	if(barr == NULL)
		return NULL;
	return BARR_ELEM_PTR(barr, 0);
}


void *
barr_end(barr_t *barr)
{
	if(barr == NULL)
		return NULL;
	return BARR_ELEM_PTR(barr, barr->ba_cnt);
}


void *
barr_search_single(barr_t *barr, void *key,
	int (*cmp)(const void *, const void *))
{
	if(barr == NULL || key == NULL || cmp == NULL)
		return NULL;
	return bsearch(key, barr->ba_data, barr->ba_cnt, barr->ba_elemsiz,
	    cmp);
}


int
barr_cmp_int(const void *arg1, const void *arg2)
{
	int	*i1;
	int	*i2;

	i1 = (int *) arg1;
	i2 = (int *) arg2;

	return *i1 - *i2;
}


int
barr_cmp_str(const void *arg1, const void *arg2)
{
	char	*s1;
	char	*s2;

	s1 = (char *) arg1;
	s2 = (char *) arg2;

	return xstrcmp(s1, s2);
}


