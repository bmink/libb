#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include "blog.h"
#include "bstr.h"
#include "bfs.h"

#define BLOG_STATE_NOT_INITIALIZED	0
#define BLOG_STATE_INITIALIZED		1

int blog_state = BLOG_STATE_NOT_INITIALIZED;

FILE	*blog_f = NULL;


int
blog_init(const char *execn)
{
	bstr_t	*dir;
	char	*home;
	int	err;
	int	ret;
	mode_t	bits;

	err = 0;
	dir = NULL;
	home = NULL;

	bits = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP |
	    S_IROTH | S_IXOTH;

	if(xstrempty(execn))
		return EINVAL;

	home = getenv("HOME");
	if(xstrempty(home))
		return ENOEXEC;

	if(!bfs_isdir(home))
		return ENOENT;

	dir = binit();
	if(dir == NULL)
		return ENOMEM;

	bprintf(dir, "%s/log", home);

	if(!bfs_isdir(bget(dir))) {
		ret = mkdir(bget(dir), bits);
		if(ret != 0) {
			err = errno;
			goto end_label;
		}
	}

	blog_state = BLOG_STATE_INITIALIZED;

end_label:

	if(dir)
		buninit(&dir);
	
	return err;
}


int
blog_uninit()
{
	if(blog_state != BLOG_STATE_INITIALIZED)
		return ENOEXEC;

	fclose(blog_f);

	blog_state = BLOG_STATE_NOT_INITIALIZED;

	return 0;
}


void
blog_logf(const char *func, const char *fmt, ...)
{
	va_list		arglist;
	bstr_t		*nfmt;

	if(blog_state != BLOG_STATE_INITIALIZED)
		return;

	if(xstrempty(func) || xstrempty(fmt))
		return;

	nfmt = binit();
	if(nfmt == NULL) {
	//	syslog(LOG_ERR, "Could not allocate bstr in blog!");
		return;
	}

	bprintf(nfmt, "<%s> %s", func, fmt);

	va_start(arglist, fmt);
	//vsyslog(LOG_NOTICE, bget(nfmt), arglist);
        va_end(arglist);

	printf("%s\n", bget(nfmt));	

	buninit(&nfmt);
}
