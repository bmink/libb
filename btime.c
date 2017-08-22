#include "btime.h"
#include <errno.h>

#define ONE_MILLION	1000000
#define ONE_BILLION	1000000000

int
btimeval_tonow(struct timeval *tv)
{
	if(tv == NULL)
		return EINVAL;

	return gettimeofday(tv, NULL);
}


int
btimeval_adds(struct timeval *tv, time_t sec)
{
	/* Adds the specified number of seconds to timeval. */

	if(tv == NULL)
		return EINVAL;

	tv->tv_sec += sec;

	return 0;
}


int
btimeval_addus(struct timeval *tv, long us)
{
	/* Adds the specified number of microseconds to timeval. */

	if(tv == NULL)
		return EINVAL;

	tv->tv_sec += us / ONE_MILLION;
	tv->tv_usec += us % ONE_MILLION;
	if(tv->tv_usec >= ONE_MILLION) {
		tv->tv_sec++;
		tv->tv_usec -= ONE_MILLION;
	}

	return 0;
}


int
btimeval_cmp(struct timeval *tv1, struct timeval *tv2)
{
	long diff;

	diff = tv1->tv_sec - tv2->tv_sec;
	if(diff)
		return diff;

	return tv1->tv_usec - tv2->tv_usec;

	return 0;
}


int
btimespec_tonow(struct timespec *ts)
{
	struct timeval	tv;

	if(ts == NULL)
		return EINVAL;

	gettimeofday(&tv, NULL);

	return btimespec_totv(ts, &tv);
}


int
btimespec_totv(struct timespec *ts, struct timeval *tv)
{
	/* Sets ts to the time specified in tv */

	if(ts == NULL || tv == NULL)
		return EINVAL;

	ts->tv_sec = tv->tv_sec;
	ts->tv_nsec = tv->tv_usec * 1000;

	return 0;
}


int
btimespec_adds(struct timespec *ts, time_t sec)
{
	if(ts == NULL)
		return EINVAL;

	ts->tv_sec += sec;

	return 0;
}


int
btimespec_addus(struct timespec *ts, long usec)
{
	return btimespec_addns(ts, usec * 1000); 
}


int
btimespec_addns(struct timespec *ts, long ns)
{
	/* Adds the specified number of nanoseconds to timespec. */

	if(ts == NULL)
		return EINVAL;

	ts->tv_sec += ns / ONE_BILLION;
	ts->tv_nsec += ns % ONE_BILLION;
	if(ts->tv_nsec >= ONE_BILLION) {
		ts->tv_sec++;
		ts->tv_nsec -= ONE_BILLION;
	}

	return 0;
}


