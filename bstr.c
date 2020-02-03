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
buninit(bstr_t **bstrptr)
{
	if(!bstrptr || !*bstrptr)
		return;

	if((*bstrptr)->bs_str)
		free((*bstrptr)->bs_str);

	free(*bstrptr);
	*bstrptr = NULL;

}


void
buninit_(bstr_t *bstr)
{
	/* Same as buninit(), but does not free (or alter) the pointer itself.
	 * In most cases, your bstr will be allocated on the heap, and you will
	 * want to use buninit(), and NOT this function. Only use if you know
	 * what you're doing! (One example when you'd use buninit_() would be
	 * when cleaning up a barr of bstr's. */

	if(!bstr)
		return;

	if(bstr->bs_str)
		free(bstr->bs_str);
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
bputc(bstr_t *bstr, const char ch)
{
	if(!bstr)
		return EINVAL;

	return bmemcat(bstr, &ch, sizeof(char));
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


int
bstrlen_utf8(bstr_t *bstr)
{
	int	i;
	int	len;

	if(bstr == NULL)
		return 0;

	len = 0;
	for(i = 0; i < bstr->bs_len; ++i) {

		/* Ignore continuation characters. */
		if((bstr->bs_str[i] & 0xc0) != 0x80)
			++len;

	}

	return len;
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
bstrcasebeginswith(bstr_t *bstr, const char *substr)
{
	if(bstr == NULL)
		return 0;
	return xstrcasebeginswith(bget(bstr), substr);
}


int
xstrcmp(const char *str1, const char *str2)
{
	if(str1 == NULL || str2 == NULL)
		return 0;
	return strcmp(str1, str2);
}


int
xstrncmp(const char *str1, const char *str2, size_t n)
{
	if(str1 == NULL || str2 == NULL)
		return 0;
	return strncmp(str1, str2, n);
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
xstrcasebeginswith(const char *str, const char *substr)
{
	if(xstrempty(str) || xstrempty(substr))
		return 0;

	if(strcasestr(str, substr) == str)
		return 1;
	else
		return 0;
}


int
xstrtomaxlen(const char *str, char *buf, int size, int ellipsis)
{
	int	tocopy;
	int	cut;

	if(xstrempty(str) || buf == NULL || (ellipsis && size < 7) ||
	    (!ellipsis && size < 2))
		return EINVAL;

	cut = 0;
	if(ellipsis)
		tocopy = size - 6;
	else
		tocopy = size - 1;
	if(strlen(str) < tocopy)
		tocopy = strlen(str);
	else
		cut++;

	memset(buf, ' ', size);
	strncpy(buf, str, tocopy);
	if(cut && ellipsis)
		sprintf(buf + tocopy, "[...]");

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
		return errno;

	ret = bfromfilep(bstr, f);
	
	fclose(f);

	return ret;

}


int
bfromfile(bstr_t *bstr, const char *filen)
{
	FILE	*f;
	int	ret;

	if(xstrempty(filen) || bstr == NULL)
		return EINVAL;

	f = fopen(filen, "r");
	if(f == NULL)
		return errno;

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
xstcaserstr(const char *haystack, const char *needle)
{
	/* libc's strcasestr returns a char * containing either a pointer to the
	 * first occurrence of needle, or NULL if needle wasn't found.
	 *
	 * IMO it's safer to return an offset into haystack, or -1 if not found.
	 */

	char	*res;

	if(xstrempty(haystack) || xstrempty(needle))
		return -1;

	res = strcasestr(haystack, needle);

	if(res == NULL)
		return -1;
	else
		return res - haystack;
}


char *
xstrchr(const char *str, int ch)
{
	if(xstrempty(str))
		return NULL;

	return strchr(str, ch);
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

	arr = barr_init(sizeof(bstr_t));
	if(arr == NULL) {
		err = ENOENT;
		goto end_label;
	}

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
			barr_add(arr, elem);
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
				barr_add(arr, elem);
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
			barr_add(arr, elem);
			cur += idx + xstrlen(sep);
			continue;
		}
	}

end_label:

	if(err != 0) {
		if(arr != NULL) {
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

	while(bstrlen_utf8(bstr) < nlen)
		bprintf(bstr, "%c", padc);

	return 0;
}


int
bstrremhtml(bstr_t *text, bstr_t *res)
{
	/* Removes HTML tags from the string. Converts a few tags like
	 * <br> etc, but mostly just removes tags. */
	
	char	*cur;
	int	intag;

	if(bstrempty(text) || res == NULL)
		return EINVAL;

	cur = bget(text);

	intag = 0;

	while(*cur) {

		if(!intag) {
			if(xstrcasebeginswith(cur, "<br>")) {
				bstrcat(res, "\n");
				cur += 4;
			} else
			if(xstrcasebeginswith(cur, "<p>")) {
				bstrcat(res, "\n\n");
				cur += 3;
			} else
			if(xstrcasebeginswith(cur, "&#x27;")) {
				bstrcat(res, "'");
				cur += 6;
			} else
			if(xstrcasebeginswith(cur, "<a ")) {
				bstrcat(res, "[");
				cur += 3;
				++intag;
			} else
			if(xstrcasebeginswith(cur, "</a>")) {
				bstrcat(res, "]");
				cur += 4;
			} else
			if(xstrcasebeginswith(cur, "<img ")) {
				bstrcat(res, "<IMAGE>");
				cur += 5;
				++intag;
			} else
			if(xstrcasebeginswith(cur, "<")) {
				cur += 1;
				++intag;
			} else {
				bprintf(res, "%c", *cur);
				++cur;
			}
		} else {
			if(xstrcasebeginswith(cur, ">"))
				intag = 0;
			++cur;
		}
	}

	return 0;
}


int
bstrtomaxlen(bstr_t *old, bstr_t *new, int size, int ellipsis)
{
	int	tocopy;
	int	cut;

	if(old == NULL || new == NULL || (ellipsis && size < 6) ||
	    (!ellipsis && size < 1))
		return EINVAL;

	cut = 0;
	if(ellipsis)
		tocopy = size - 5;
	else
		tocopy = size;
	
	if(bstrlen(old) < tocopy)
		tocopy = bstrlen(old);
	else
		cut++;

	bclear(new);
	bmemcat(new, bget(old), tocopy);

	if(cut && ellipsis)
		bstrcat(new, "[...]");

	return 0;
}


int
bstrtomaxlen_utf8(bstr_t *old, bstr_t *new, int size, int ellipsis)
{
	int	tocopy;
	int	tocopy_bytes;
	int	charcnt;
	int	cut;
	char	*beg;
	char	*ch;
	char	*end;

	if(old == NULL || new == NULL || (ellipsis && size < 6) ||
	    (!ellipsis && size < 1))
		return EINVAL;

	cut = 0;
	if(ellipsis)
		tocopy = size - 5;
	else
		tocopy = size;
	
	if(bstrlen_utf8(old) < tocopy)
		tocopy = bstrlen_utf8(old);
	else
		cut++;

	/* Count how many actual bytes we have to copy. */
	beg = bget(old);
	end = beg + old->bs_len;
	charcnt = 0;
	for(ch = beg; ch < end; ++ch) {

		/* Note: It's important to compare here and not at the end of
		 * the loop because we need to do it after ch was incremented
		 * by the loop so the byte counting math is correct. */
		if(charcnt >= tocopy)
			break;

		/* If it's a continuation character, don't increment char
		 * count */
                if((*ch & 0xc0) != 0x80)
                        ++charcnt;
	}

	/* In case we're in the middle of a multi-byte character, walk to the
	 * end */
	while(ch < end && ((*ch & 0xc0) == 0xc0 || (*ch & 0xc0) == 0x80))
		++ch;

	tocopy_bytes = ch - beg;

	bclear(new);
	bmemcat(new, bget(old), tocopy_bytes);

	if(cut && ellipsis)
		bstrcat(new, "[...]");

#if 0
printf("len=%d, tocopy=%d, tocopy_bytes=%d\n", bstrlen_utf8(old), tocopy, tocopy_bytes);
#endif


	return 0;
}


int
bstrlimitlines(bstr_t *old, bstr_t *new, int maxlines)
{
	int	lcnt;
	char	*ch;

	if(old == NULL || new == NULL || maxlines <= 1)
		return EINVAL;

	bclear(new);

	lcnt = 0;
	for(ch = bget(old); *ch; ch++) {
		if(*ch == '\n') {
			lcnt++;
			if(lcnt >= maxlines) {
				bprintf(new, "\n[...]\n");
				return 0;
			}
		}
		bprintf(new, "%c", *ch);
	}

	return 0;
}

#define XSPA_BETWEEN_ARGS	0
#define XSPA_IN_QUOTED_ARG	1
#define XSPA_IN_NONQUOTED_ARG	2

int
xstrparseargs(const char *str, barr_t **res)
{
/*
 * Parses a string of arguments (quoted or unquoted) into an array of bstr_t.
 */
	int		err;
	barr_t		*arr;
	bstr_t		*elem;
	int		state;
	const char	*ch;

	if(xstrempty(str))
		return EINVAL;

	elem = NULL;
	err = 0;

        arr = barr_init(sizeof(bstr_t));
        if(arr == NULL) {
                err = ENOENT;
                goto end_label;
        }

	state = XSPA_BETWEEN_ARGS;

	for(ch = str; *ch != 0; ++ch) {
		switch(state) {

		case XSPA_BETWEEN_ARGS:
			if(isspace(*ch))
				continue;

			/* It's not a space char, which means there's a new
			 * argument starting. */
			elem = binit();
			if(!elem) {
				err = ENOMEM;
				goto end_label;
			}

			if(*ch == '"') 
				state = XSPA_IN_QUOTED_ARG;
			else {
				state = XSPA_IN_NONQUOTED_ARG;
				bputc(elem, *ch);
			}

			continue;

		case XSPA_IN_NONQUOTED_ARG:
			if(isspace(*ch)) {
				/* Argument ending. */
				barr_add(arr, elem);
				elem = NULL;
				state = XSPA_BETWEEN_ARGS;
				continue;
			}
			bputc(elem, *ch);
			continue;

		case XSPA_IN_QUOTED_ARG:
			if(*ch == '"') {
				/* Argument ending. */
				barr_add(arr, elem);
				elem = NULL;
				state = XSPA_BETWEEN_ARGS;
				continue;
			}

			if(*ch == '\\') {
				/* Deal with escaped chars. */
				++ch;
				if(!*ch) {
					/* String ends too soon. */
					err = ENODATA;
					goto end_label;
				}
				
				if(*ch == '"') { 
					bputc(elem, '"');
					continue;
				} else
				if(*ch == '\\') { 
					bputc(elem, '\\');
					continue;
				} else {
					err = ENOENT;
					goto end_label;
				}
				
			} else {
				bputc(elem, *ch);
				continue;
			}
		}
	}
	
	if(state == XSPA_IN_QUOTED_ARG) {
		/* String ended in the middle of a quoted argument. */
		err = ESRCH;
		goto end_label;
	} else if(state == XSPA_IN_NONQUOTED_ARG) {
		/* Add last argument. */
		barr_add(arr, elem);
		elem = NULL;
	}

end_label:

	if(err != 0) {
		buninit(&elem);
		if(arr != NULL) {
			for(elem = (bstr_t *) barr_begin(arr);
			    elem < (bstr_t *) barr_end(arr);
			    ++elem) {
				buninit_(elem);
			}
			barr_uninit(&arr);
		}
	} else {
		*res = arr;
	}

	return err;

}
