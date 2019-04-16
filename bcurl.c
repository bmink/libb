#include <curl/curl.h>
#include <errno.h>
#include "bstr.h"
#include "blog.h"

int
bcurl_init()
{
	return curl_global_init(CURL_GLOBAL_ALL);
}


int
bcurl_uninit()
{
	curl_global_cleanup();
	return 0;
}


int
bcurl_url_fetch(const char *url, bstr_t *doc)
{
	CURL	*mycurl;
	long	respcode;

	mycurl = NULL;

	if(xstrempty(url) || doc == NULL) {
		blogf("Bad arguments");
		return EINVAL;
	}


        mycurl = curl_easy_init();
        if(mycurl == NULL) {
                blogf("Could not initialize libcurl_easy\n");
                return ENOEXEC;
        }








	return 0;
}
