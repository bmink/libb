#include <stdlib.h>
#include <errno.h>
#include "bvalmap.h"
#include "bstr.h"


int
bvalmap_validint(bvalmap_t *vm, int val)
{
	bvalmap_t	*cur;

	if(vm == NULL)
		return 0;

	for(cur = vm; cur->bv_str != NULL; cur++) {
		if(cur->bv_intval == val)
			return 1;
	}

	return 0;
}


int
bvalmap_validstr(bvalmap_t *vm, char *str)
{
	bvalmap_t	*cur;

	if(vm == NULL)
		return 0;

	for(cur = vm; cur->bv_str != NULL; cur++) {
		if(!xstrcmp(cur->bv_str, str))
			return 1;
	}

	return 0;
}


int
bvalmap_getstr(bvalmap_t *vm, int val, char **res)
{
	bvalmap_t	*cur;

	if(vm == NULL || res == NULL)
		return EINVAL;

	for(cur = vm; cur->bv_str != NULL; cur++) {
		if(cur->bv_intval == val) {
			*res = cur->bv_str;
			return 0;
		}
	}

	return ENOENT;
}


int
bvalmap_getint(bvalmap_t *vm, char *str, int *res)
{
	bvalmap_t	*cur;

	if(vm == NULL || res == NULL)
		return EINVAL;

	for(cur = vm; cur->bv_str != NULL; cur++) {
		if(!xstrcmp(cur->bv_str, str)) {
			*res = cur->bv_intval;
			return 0;
		}
	}

	return ENOENT;
}

