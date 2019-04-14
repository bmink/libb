/*
 * blog: Logging services (ie. wrapper around syslog)
 */
#ifndef BLOG_H
#define BLOG_H

#define BLOG_INFO	0
#define BLOG_ERR	1
#define BLOG_NUM_LEVEL	2

int blog_init(const char *);
int blog_uninit();

#define blog(...) blog_logf(__func__, __VA_ARGS__)
void blog_logf(int, const char *, ...);

#endif
