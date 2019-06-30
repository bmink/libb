/*
 * bcurl: Wrapper for libcurl
 */
#ifndef BCURL_H
#define BCURL_H

#include "bstr.h"

int bcurl_init();
int bcurl_uninit();

int bcurl_get(const char *, bstr_t **);

#endif
