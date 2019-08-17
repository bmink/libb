/*
 * btime: time and date utilities
 */
#ifndef BTIME_H
#define BTIME_H

#include "bstr.h"
#include <sys/time.h>
#include <time.h>

int btimeval_tonow(struct timeval *);
int btimeval_adds(struct timeval *, time_t);
int btimeval_addus(struct timeval *, long);
int btimeval_cmp(struct timeval *, struct timeval *);

int btimespec_tonow(struct timespec *);
int btimespec_totv(struct timespec *, struct timeval *);
int btimespec_adds(struct timespec *, time_t);
int btimespec_addus(struct timespec *, long);
int btimespec_addns(struct timespec *, long);

int bgetdate(bstr_t *);


#endif
