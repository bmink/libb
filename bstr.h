/* 
 * bstr: Barna's C string library
 *
 * Note: this can be used as a binary data array as well. Of course, the
 * caller then needs to be aware that the "string" can contain 0x00 bytes
 * as well, i.e. it shouldn't be treated as a zero-terminated string (duh).
 *
 */
#ifndef BSTR_H
#define BSTR_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "barr.h"


typedef struct bstr {
	char		*bs_str;
	size_t		bs_len;
	size_t		bs_siz;
} bstr_t;


bstr_t *binit(void);
void buninit(bstr_t **);
void buninit_(bstr_t *);
void bclear(bstr_t *);

int bstrcat(bstr_t *, const char *);
int bmemcat(bstr_t *, const char *, size_t);
int bprintf(bstr_t *, const char *, ...);
int bvprintf(bstr_t *, const char *, va_list);
int bputc(bstr_t *, const char);

char *bget(bstr_t *);

int bstrlen(bstr_t *);
int bstrlen_utf8(bstr_t *);
int bstrempty(bstr_t *);
int bstrtolower(bstr_t *);
int bstrendswith(bstr_t *, const char *);
int bstrbeginswith(bstr_t *, const char *);
int bstrcasebeginswith(bstr_t *, const char *);
int bstrsplit(bstr_t *, const char *, int, barr_t **);
int bstrpad(bstr_t *, size_t, char);
int bstrchop(bstr_t *, int);
int bstrchopnewline(bstr_t *);

int btofile(const char *, bstr_t *);
int btofilep(FILE *, bstr_t *);
int bfromfile(bstr_t *, const char *);
int bfromfilep(bstr_t *, FILE *);
int bfromfiled(bstr_t *, int);

int bstrtomaxlen(bstr_t *, bstr_t *, int, int);
int bstrtomaxlen_utf8(bstr_t *, bstr_t *, int, int);
int bstrlimitlines(bstr_t *, bstr_t *, int);
int bstrremhtml(bstr_t *, bstr_t *);

int xfree(char **);
int xstrcmp(const char *, const char *);
int xstrncmp(const char *, const char *, size_t);
int xstrcasecmp(const char *, const char *);
int xstrncasecmp(const char *, const char *, size_t);
int xstrlen(const char *);
int xstrempty(const char *);
int xstrchop(char *, int);
int xstrchopnewline(char *);
int xstrtolower(char *);
int xstrendswith(const char *, const char *);
int xstrbeginswith(const char *, const char *);
int xstrcasebeginswith(const char *, const char *);
int xstrbeginswithn(const char *, const char *);
int xstrtomaxlen(const char *, char *, int, int);
/* NOTE: xstrtomaxlen_utf8 is not provided. That's hard (and risky) to do with
 * straight up buffers. Use bstrtomaxlen_utf8 instead. */
char *xstrchr(const char *, int);
int xstrstr(const char *, const char *);
int xstrcasestr(const char *, const char *);
char *xstrdup(const char *);
int xatoi(const char *);
void *memdup(const void *, size_t);
int xstrsplit(const char *, const char *, int, barr_t **);

int xstrparseargs(const char *, barr_t **);

#endif
