/*
 * bcurl: Wrapper for libcurl
 */
#ifndef BCURL_H
#define BCURL_H

#include "bstr.h"

int bcurl_init(void);
int bcurl_uninit(void);

int bcurl_header_clearall(void);
int bcurl_header_add(const char *);

int bcurl_set_useragent(const char *);

int bcurl_get_opts(const char *, bstr_t **, const char *, const char *);
int bcurl_get(const char *, bstr_t **);

int bcurl_put_opts(const char *, bstr_t *, bstr_t **, const char *,
	const char *);
int bcurl_put(const char *, bstr_t *, bstr_t **);

int bcurl_post_opts(const char *, bstr_t *, bstr_t **, const char *,
	const char *);
int bcurl_post(const char *, bstr_t *, bstr_t **);


int bstrcat_urlenc(bstr_t *, const char *);
int bstrcat_urlenc_field(bstr_t *, const char *, const char *);

#endif
