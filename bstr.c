#include "bstr.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>

#define BSTR_INITSIZE	4096
#define BSTR_GROWFACT	2


bstr_t *
binit(void)
{
	bstr_t	*bstr;

	bstr = malloc(sizeof(bstr_t));

	if(bstr) {
		memset(bstr, 0, sizeof(bstr_t));
		bstr->bs_str = malloc(BSTR_INITSIZE);
		if(bstr->bs_str) {
			bstr->bs_siz = BSTR_INITSIZE;
			bstr->bs_str[0] = 0;	
		} else {
			free(bstr);
			bstr = NULL;
		}
	}

	return bstr;
}


void
bclear(bstr_t *bstr)
{
	if(!bstr)
		return;

	bstr->bs_str[0] = 0;
	bstr->bs_len = 0;
}


void
buninit_(bstr_t *bstr)
{
	/* Same as buninit(), but does not alter the pointer itself. In most
	 * cases, you want to use buninit(), and NOT this function. Only
	 * use if you know what you're doing! */

	bstr_t *foo;

	if(!bstr)
		return;

	foo = bstr;
	buninit(&foo);
}


void
buninit(bstr_t **bstrptr)
{
	if(!bstrptr || !*bstrptr)
		return;

	if((*bstrptr)->bs_str)
		free((*bstrptr)->bs_str);

	free(*bstrptr);
	*bstrptr = NULL;

}


int
bmemcat(bstr_t *bstr, const char *mem, size_t len)
{
	char	*newptr;
	size_t	neededsiz;
	size_t	newsiz;

	if(bstr == NULL || mem == NULL)
		return EINVAL;
	if(len == 0)
		return 0;

	neededsiz = bstr->bs_len + len + 1;

	/* Grow buffer if needed */
	if(bstr->bs_siz < neededsiz) {

		newsiz = bstr->bs_siz;
		while(newsiz < neededsiz)
			newsiz *= BSTR_GROWFACT;

		newptr = realloc(bstr->bs_str, newsiz);
		if(newptr == NULL)
			return ENOMEM;
		bstr->bs_str = newptr;
	}

	memcpy(bstr->bs_str + bstr->bs_len, mem, len);
	bstr->bs_len += len;
	bstr->bs_str[bstr->bs_len] = 0;

	return 0;
}


int
bstrcat(bstr_t *bstr, const char *str)
{
	if(str == NULL)	
		return EINVAL;

	return bmemcat(bstr, str, strlen(str));
}


int
bvprintf(bstr_t *bstr, const char *format, va_list arglist)
{
	int		ret;
	char		*printed;

	if(bstr == NULL || format == NULL)
		return EINVAL;
	
	printed = NULL;

	ret = vasprintf(&printed, format, arglist);

	if(ret > 0 && printed) {
		bmemcat(bstr, printed, strlen(printed));
		free(printed);
		return 0;
	} else
		return ENOMEM;
}


int
bprintf(bstr_t *bstr, const char *format, ...)
{
	va_list		arglist;
	int		ret;

	if(bstr == NULL || format == NULL)
		return EINVAL;

	va_start(arglist, format);
	ret = bvprintf(bstr, format, arglist);
	va_end(arglist);

	return ret;
}


int
bstrlen(bstr_t *bstr)
{
	if(bstr == NULL)
		return 0;
	return bstr->bs_len;
}


char *bget(bstr_t *bstr)
{
	if(bstr == NULL)
		return "";

	return bstr->bs_str;
}


int
bstrempty(bstr_t *bstr)
{
	if(bstr == NULL)
		return 1;
	return xstrempty(bget(bstr));
}


int
bstrtolower(bstr_t *bstr)
{
	if(bstr == NULL)
		return EINVAL;
	return xstrtolower(bget(bstr));
}


int
bstrendswith(bstr_t *bstr, const char *substr)
{
	if(bstr == NULL)
		return 0;
	return xstrendswith(bget(bstr), substr);
}


int
bstrbeginswith(bstr_t *bstr, const char *substr)
{
	if(bstr == NULL)
		return 0;
	return xstrbeginswith(bget(bstr), substr);
}


int
xstrcmp(const char *str1, const char *str2)
{
	if(str1 == NULL || str2 == NULL)
		return 0;
	return strcmp(str1, str2);
}


int
xstrcasecmp(const char *str1, const char *str2)
{
	if(str1 == NULL || str2 == NULL)
		return -1;
	return strcasecmp(str1, str2);

}


int
xstrncasecmp(const char *str1, const char *str2, size_t n)
{
	if(str1 == NULL || str2 == NULL || n == 0)
		return -1;
	return strncasecmp(str1, str2, n);

}


int
xstrchop(char *str, int cnt)
{
	int	tochop;

	if(xstrempty(str) || cnt == 0)
		return EINVAL;

	tochop = strlen(str)<cnt?strlen(str):cnt;

	str[strlen(str) - tochop] = 0;

	return 0;
}


int
xstrlen(const char *str)
{
	if(str == NULL)
		return 0;
	return strlen(str);
}

int
xstrempty(const char *str)
{
	if(str == NULL)
		return 1;
	if(str[0] == 0)
		return 1;
	return 0;
}

int
xfree(char **str)
{
	if(*str == NULL)
		return 0;
	free(*str);
	*str = 0;
	return 0;
}


int
xstrendswith(const char *str, const char *substr)
{
	if(xstrempty(str) || xstrempty(substr))
		return 0;

	if(strstr(str, substr) == str + strlen(str) - strlen(substr))
		return 1;
	else
		return 0;
}


int
xstrbeginswith(const char *str, const char *substr)
{
	if(xstrempty(str) || xstrempty(substr))
		return 0;

	if(strstr(str, substr) == str)
		return 1;
	else
		return 0;
}


int
xstrmakefixedwidth(const char *str, char *buf, int size)
{
	int	tocopy;
	int	cut;

	if(xstrempty(str) || buf == NULL || size < 5)
		return EINVAL;

	cut = 0;
	tocopy = size - 4;
	if(strlen(str) < tocopy)
		tocopy = strlen(str);
	else
		cut++;

	memset(buf, ' ', size);
	strncpy(buf, str, tocopy);
	if(cut)
		sprintf(buf + tocopy, "...");

	buf[size - 1] = 0;

	return 0;
}


int
xstrtolower(char *str)
{
	char	*ch;

	if(str == NULL)
		return EINVAL;

	for(ch = str; *ch; ch++) {
		*ch = tolower(*ch);
	}

	return 0;
}


char *
xstrdup(const char *str)
{
	if(xstrempty(str))
		return NULL;

	return strdup(str);
}


void *
memdup(const void *mem, size_t siz)
{
	void	*memd;

	if(mem == NULL || siz == 0)
		return NULL;

	memd = malloc(siz);
	if(memd == NULL)
		return NULL;

	memcpy(memd, mem, siz);

	return memd;
}


int
btofilep(FILE *f, bstr_t *bstr)
{
	char	*buf;
	size_t	len;
	int	ret;

	if(f == NULL || bstrempty(bstr))
		return EINVAL;

	buf = bget(bstr);
	if(buf == NULL)
		return ENOENT;

	len = bstrlen(bstr);

	ret = fwrite(buf, 1, len, f);
	if(ret != len)
		return ENOEXEC;

	return 0;
}


int
btofile(const char *filen, bstr_t *bstr)
{
	FILE	*f;
	int	ret;

	if(xstrempty(filen) || bstrempty(bstr))
		return EINVAL;

	f = fopen(filen, "w");
	if(f == NULL)
		return ENOEXEC;

	ret = btofilep(f, bstr); 

	fclose(f);

	return ret;
}


int
bfromfilep(bstr_t *bstr, FILE *f)
{
	int		ret;
	unsigned char	byte;

	if(f == NULL || bstr == NULL)
		return EINVAL;

	while(1) {
		ret = fgetc(f);

		if(ret == EOF)
			break;

		byte = (unsigned char) ret;
		ret = bmemcat(bstr, (char *) &byte, 1);
		if(ret != 0)
			return ret;
	}


	return 0;
}


int
bfromfiled(bstr_t *bstr, int filedesc)
{
	FILE	*f;
	int	ret;

	f = fdopen(filedesc, "r");
	if(f == NULL)
		return EINVAL;

	ret = bfromfilep(bstr, f);
	
	fclose(f);

	return ret;

}


int
bfromfile(bstr_t *bstr, const char *filen)
{
	FILE	*f;
	int	ret;

	if(xstrempty(filen) || bstrempty(bstr))
		return EINVAL;

	f = fopen(filen, "r");
	if(f == NULL)
		return ENOEXEC;

	ret = bfromfilep(bstr, f); 

	fclose(f);

	return ret;
}


int
xatoi(const char *str)
{
	if(xstrempty(str))
		return 0;

	return atoi(str);
}


int
xstrchopnewline(char *str)
{
	if(xstrempty(str))
		return EINVAL;

	while(xstrendswith(str, "\n") || xstrendswith(str, "\r"))
		xstrchop(str, 1);

	return 0;
}


int
xstrstr(const char *haystack, const char *needle)
{
	/* libc's strstr returns a char * containing either a pointer to the
	 * first occurrence of needle, or NULL if needle wasn't found.
	 *
	 * IMO it's safer to return an offset into haystack, or -1 if not found.
	 */

	char	*res;

	if(xstrempty(haystack) || xstrempty(needle))
		return -1;

	res = strstr(haystack, needle);

	if(res == NULL)
		return -1;
	else
		return res - haystack;
}


int
xstrsplit(const char *str, const char *sep, int ignoreempty, barr_t **res)
{
	/* Unlike with libc's strtok, sep here is taken as a delimiter string,
	 * not as an array of possible delimiter characters. So the delimiter
	 * can be a string more than one character long.
	 *
	 * If ignoreempty is nonzero, then empty substrings are ignored,
	 * and repeated occurrences of the separator are ignored. This is
	 * strtok behavior. 
	 *
	 * If ignoreempty is zero, then empty elements are returned as
	 * empty bstr-s in the result array.
	 */

	barr_t	*arr;
	int	cur;
	bstr_t	*elem;
	int	err;
	int	idx;
	int	len;

	if(xstrempty(str) || xstrempty(sep) || res == NULL)
		return EINVAL;

	arr = NULL;
	elem = NULL;
	err = 0;
	len = 0;

	arr = barr_init(sizeof(barr_t *));

	len = xstrlen(str);

	cur = 0;
	while(1) {
		idx = xstrstr(str + cur, sep);
		if(idx < 0) {
			/* No more occurrences of sep, add rest of string
			 * as last element. */
			elem = binit();
			if(elem == NULL) {
				err = EINVAL;
				goto end_label;
			}

			if(cur < len) {
				bstrcat(elem, str + cur);
			}
			barr_add(arr, &elem);
			break;
		} else if(idx == 0) {
			/* This means that the separator is repeated, ie.
			 * we're dealing with an empty token. Either skip
			 * or add an empty element to result. */

			if(ignoreempty == 0) {
				elem = binit();
				if(elem == NULL) {
					err = EINVAL;
					goto end_label;
				}
				barr_add(arr, &elem);
			}
			cur += xstrlen(sep);
			continue;
		} else if(idx > 0) {
			elem = binit();
			if(elem == NULL) {
				err = EINVAL;
				goto end_label;
			}
			bmemcat(elem, str + cur, idx);
			barr_add(arr, &elem);
			cur += idx + xstrlen(sep);
			continue;
		}
	}

end_label:

	if(err != 0) {
		if(arr) {
			for(elem = (bstr_t *) barr_begin(arr);
			    elem < (bstr_t *) barr_end(arr);
			    ++elem) {
				buninit_(elem);
			}
		}
		barr_uninit(&arr);
	} else {
		*res = arr;
	}

	return 0;
}


int
bstrsplit(bstr_t *bstr, const char *sep, int ignoreempty, barr_t **res)
{

	if(bstrempty(bstr))
		return EINVAL;

	return xstrsplit(bget(bstr), sep, ignoreempty, res);
}


int
bstrpad(bstr_t *bstr, size_t nlen, char padc)
{
	if(bstr == NULL)
		return EINVAL;

	while(bstrlen(bstr) < nlen)
		bprintf(bstr, "%c", padc);

	return 0;
}


