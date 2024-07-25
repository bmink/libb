#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>
#include <syslog.h>
#include "blog.h"
#include "bstr.h"
#include "bfs.h"
#include "btime.h"

#ifdef __APPLE__
#include <os/log.h>
#endif

#define BLOG_STATE_NOT_INITIALIZED	0
#define BLOG_STATE_INITIALIZED		1

int blog_state = BLOG_STATE_NOT_INITIALIZED;

bstr_t	*blog_execn = NULL;


int
blog_init(const char *execn)
{
	if(blog_state != BLOG_STATE_NOT_INITIALIZED)
		return ENOEXEC;

	if(xstrempty(execn))
		return EINVAL;

	blog_execn = binit();
	if(blog_execn == NULL)
		return ENOMEM;

	bstrcat(blog_execn, execn);

	blog_state = BLOG_STATE_INITIALIZED;

	return 0;
}


int
blog_uninit(void)
{
	if(blog_state != BLOG_STATE_INITIALIZED)
		return ENOEXEC;

	buninit(&blog_execn);

	blog_state = BLOG_STATE_NOT_INITIALIZED;

	return 0;
}


void
blog_logf(const char *func, const char *fmt, ...)
{
	va_list		arglist;
	bstr_t		*nfmt;
	bstr_t		*logmsg;

	if(blog_state != BLOG_STATE_INITIALIZED)
		return;

	if(xstrempty(func) || xstrempty(fmt))
		return;

	logmsg = NULL;

	nfmt = binit();
	if(nfmt == NULL) {
	//	syslog(LOG_ERR, "Could not allocate bstr in blog!");
		return;
	}

	bprintf(nfmt, "<%s> %s", func, fmt);

	va_start(arglist, fmt);
#ifdef __APPLE__
	/* There's no vararg version of os_log. Have to print the log message
	 * into a string ourselves. */
	logmsg = binit();
	if(logmsg != NULL) {
		bvprintf(logmsg, bget(nfmt), arglist);
		if(!bstrempty(logmsg))
			os_log(OS_LOG_DEFAULT, "%{public}s", bget(logmsg));
	}
#else
	vsyslog(LOG_NOTICE, bget(nfmt), arglist);
#endif
        va_end(arglist);

	buninit(&logmsg);
	buninit(&nfmt);
}
