/*
 * bint: Integer untilities
 */
#ifndef BINT_H
#define BINT_H

#define BINT_INCL	0
#define BINT_EXCL	1

int bint_betw(int, int, int, int, int);
int buint_betw(unsigned, unsigned, int, unsigned, int);
int blong_betw(long, long, int, long, int);
int bulong_betw(unsigned long, unsigned long, int, unsigned long, int);


#endif
