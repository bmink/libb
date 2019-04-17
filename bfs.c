#include <sys/stat.h>
#include <errno.h>
#include "bfs.h"
#include "bstr.h"


int
bfs_isdir(const char *path)
{
	int		ret;
	struct stat	ss;

	if(xstrempty(path))
		return 0;
	
	ret = stat(path, &ss);
	if(ret != 0)
		return 0;

	if(S_ISDIR(ss.st_mode))
		return 1;
	
	return 0;
}


int
bfs_isfile(const char *path)
{
	int		ret;
	struct stat	ss;

	if(xstrempty(path))
		return 0;
	
	ret = stat(path, &ss);
	if(ret != 0)
		return 0;

	if(S_ISREG(ss.st_mode))
		return 1;
	
	return 0;
}


int
bfs_mkdir(const char *path)
{
	int	ret;
	mode_t	bits;

	if(xstrempty(path))
		return EINVAL;

	if(bfs_isdir(path))
		return EEXIST;

	/* "Standard" permissions, should work for most cases. */
        bits = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP |
            S_IROTH | S_IXOTH;	

	ret = mkdir(path, bits);

	return ret;

}

