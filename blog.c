#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>
#include "blog.h"
#include "bstr.h"
#include "bfs.h"
#include "btime.h"

#define BLOG_STATE_NOT_INITIALIZED	0
#define BLOG_STATE_INITIALIZED		1

int blog_state = BLOG_STATE_NOT_INITIALIZED;
int blog_mode = BLOG_MODE_SINGLE;

bstr_t	*blog_fnam = NULL;
bstr_t	*blog_execn = NULL;
FILE *blog_f = NULL;


int
blog_init(const char *execn, int mode)
{
	bstr_t	*dir;
	char	*home;
	int	err;
	int	ret;
	bstr_t	*date;

	err = 0;
	dir = NULL;
	home = NULL;
	date = NULL;

	if(xstrempty(execn))
		return EINVAL;

	if(mode != BLOG_MODE_SINGLE && mode != BLOG_MODE_MULTI)
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

	ret = bfs_mkdir(bget(dir));
	if(ret != 0 && ret != EEXIST) {
		err = errno;
		goto end_label;
	}

	bclear(dir);
	bprintf(dir, "%s/log/%s", home, execn);

	ret = bfs_mkdir(bget(dir));
	if(ret != 0 && ret != EEXIST) {
		err = errno;
		goto end_label;
	}

	blog_fnam = binit();
	if(blog_fnam == NULL) {
		err = ENOMEM;
		goto end_label;
	}

	date = binit();
	if(date == NULL) {
		err = ENOMEM;
		goto end_label;
	}

	ret = bgetdate(date);
	if(ret != 0) {
		err = ret;
		goto end_label;
	}

	if(mode == BLOG_MODE_SINGLE) {
		bprintf(blog_fnam, "%s/log/%s/%s", home, execn, bget(date));
	} else {
		bprintf(blog_fnam, "%s/log/%s/%s.%d", home, execn, bget(date),
		    getpid());
	}

	printf("%s\n", bget(blog_fnam));
	exit(0);

	if(bstrempty(blog_fnam)) {
		err = ENOEXEC;
		goto end_label;
	}

	blog_f = fopen(bget(blog_fnam), "a");
	if(blog_f == NULL) {
		err = errno;
		goto end_label;
	}

	blog_execn = binit();
	if(blog_execn == NULL) {
		err = ENOMEM;
		goto end_label;
	}

	bstrcat(blog_execn, execn);

	blog_state = BLOG_STATE_INITIALIZED;

end_label:

	if(dir)
		buninit(&dir);
	if(err != 0) {
		if(blog_fnam != NULL) {
			buninit(&blog_fnam);
		}
		if(blog_execn != NULL) {
			buninit(&blog_execn);
		}
		if(blog_f != NULL) {
			fclose(blog_f);
			blog_f = NULL;
		}
	}
	if(date)
		buninit(&date);
	
	return err;
}


int
blog_uninit()
{
	if(blog_state != BLOG_STATE_INITIALIZED)
		return ENOEXEC;

	fclose(blog_f);
	blog_f = NULL;

	buninit(&blog_fnam);
	buninit(&blog_execn);

	blog_state = BLOG_STATE_NOT_INITIALIZED;

	return 0;
}


void
blog_logf(const char *func, const char *fmt, ...)
{
	va_list		arglist;
	bstr_t		*nfmt;


/* TODO
   check if we need to open new file *
   fflush if needed
*/

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
