/*
 * blog: Logging services
 *
 * Would love to use syslog(d) but the state of it is messed up on OS X!
 */
#ifndef BLOG_H
#define BLOG_H


int blog_init(const char *);
int blog_uninit(void);

#define blogf(...) blog_logf(__func__, __VA_ARGS__)
void blog_logf(const char *, const char *, ...);

#endif
