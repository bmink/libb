#include <errno.h>
#include "bint.h"


int
blong_betw(long num, long left, int left_incl, long right, int right_incl)
{
	int	between;

	between = 1;

	switch(left_incl) {
	case BINT_INCL:
		if(num < left)
			between = 0;
		break;
	case BINT_EXCL:
		if(num <= left)
			between = 0;
		break;
	default: /* Unknown value? */
		between = 0;
		break;
	}

	if(between == 0)
		return between;

	switch(right_incl) {
	case BINT_INCL:
		if(num > right)
			between = 0;
		break;
	case BINT_EXCL:
		if(num >= right)
			between = 0;
		break;
	default: /* Unknown value? */
		between = 0;
		break;
	}

	return between;
}


int
bint_betw(int num, int left, int left_incl, int right, int right_incl)
{
	return blong_betw((long) num, (long) left, left_incl,
	    (long) right, right_incl);
}


int
bulong_betw(unsigned long num, unsigned long left, int left_incl,
    unsigned long right, int right_incl)
{
	int	between;

	between = 1;

	switch(left_incl) {
	case BINT_INCL:
		if(num < left)
			between = 0;
		break;
	case BINT_EXCL:
		if(num <= left)
			between = 0;
		break;
	default: /* Unknown value? */
		between = 0;
		break;
	}

	if(between == 0)
		return between;

	switch(right_incl) {
	case BINT_INCL:
		if(num > right)
			between = 0;
		break;
	case BINT_EXCL:
		if(num >= right)
			between = 0;
		break;
	default: /* Unknown value? */
		between = 0;
		break;
	}

	return between;
}


int
buint_betw(unsigned num, unsigned left, int left_incl,
    unsigned int right, int right_incl)
{
	return blong_betw((unsigned long) num, (unsigned long) left, left_incl,
	    (unsigned long) right, right_incl);
}
