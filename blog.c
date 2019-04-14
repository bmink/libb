#include <syslog.h>
#include <errno.h>
#include <stdarg.h>
#include "blog.h"
#include "bstr.h"
#include "bint.h"

#define BLOG_STATE_NOT_INITIALIZED	0
#define BLOG_STATE_INITIALIZED		1

int blog_state = BLOG_STATE_NOT_INITIALIZED;


int
blog_init(const char *execn)
{
	if(xstrempty(execn))
		return EINVAL;

	openlog(execn, LOG_PID, LOG_USER);

	blog_state = BLOG_STATE_INITIALIZED;
	
	return 0;
}


int
blog_uninit()
{
	if(blog_state != BLOG_STATE_INITIALIZED)
		return ENOEXEC;

	closelog();

	blog_state = BLOG_STATE_NOT_INITIALIZED;

	return 0;
}


void
blog_logf(int level, const char *fmt, ...)
{
	va_list		arglist;
	int		pri;

	if(blog_state != BLOG_STATE_INITIALIZED)
		return;

	switch(level) {
	case BLOG_INFO:
		pri = LOG_INFO;
		break;
	case BLOG_ERR:
		pri = LOG_ERR;
		break;
	default: /* Unknown level? */
		return;
	}

	va_start(arglist, fmt);
	vsyslog(pri, fmt, arglist);
        va_end(arglist);
}
