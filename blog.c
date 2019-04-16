#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>
#include "blog.h"
#include "bstr.h"
#include "bfs.h"

#define BLOG_STATE_NOT_INITIALIZED	0
#define BLOG_STATE_INITIALIZED		1

int blog_state = BLOG_STATE_NOT_INITIALIZED;

bstr_t	*blog_fnam = NULL;
FILE *blog_f = NULL;


int
blog_init(const char *execn, int mode)
{
	bstr_t	*dir;
	char	*home;
	int	err;
	int	ret;

	err = 0;
	dir = NULL;
	home = NULL;

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

	bprintf(blog_fnam, "%s/log/%s/", home, execn, );


	blog_f = fopen();


	blog_state = BLOG_STATE_INITIALIZED;

end_label:

	if(dir)
		buninit(&dir);
	if(err != 0) {
		if(blog_fnam != NULL) {
			buninit(&blog_fnam);
		}
		if(blog_f != NULL) {
			fclose(blog_f);
			blog_f = NULL;
		}
	
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
