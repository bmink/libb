#include <curl/curl.h>
#include <errno.h>
#include "bstr.h"
#include "blog.h"

int
bcurl_init()
{
	int	ret;

	ret = curl_global_init(CURL_GLOBAL_ALL);

	if(ret != 0) {
		blogf("curl_global_init() returned error");
	} 

	return ret;
}


int
bcurl_uninit()
{
	curl_global_cleanup();
	return 0;
}


size_t
bcurl_callback(void *buffer, size_t size, size_t nmemb, void *userdata)
{
	bstr_t  *buf;
	size_t  bytes;

	buf = (bstr_t *) userdata;

	bytes = size * nmemb;

	if(buf == NULL || buffer == NULL || bytes == 0) {
		blogf("libcurl callback received invalid data\n");
		return 0;
	}

	bmemcat(buf, buffer, bytes);

	return bytes;
}


int
bcurl_get(const char *url, bstr_t *doc)
{
	CURL	*mycurl;
	long	respcode;
	bstr_t	*buf;
	int	ret;


	mycurl = NULL;
	buf = NULL;

	if(xstrempty(url) || doc == NULL) {
		blogf("Bad arguments");
		return EINVAL;
	}

	buf = binit();
	if(buf == NULL) {
		blogf("Could not initialize buffer\n");
		goto end_label;
	}

        mycurl = curl_easy_init();
        if(mycurl == NULL) {
                blogf("Could not initialize libcurl_easy\n");
                return ENOEXEC;
        }

	ret = curl_easy_setopt(mycurl, CURLOPT_URL, url);
	if(ret != 0) {
		blogf("Could not set URL in llibcurl: %s\n",
		    curl_easy_strerror(ret));
		goto end_label;
	}

        ret = curl_easy_setopt(mycurl, CURLOPT_WRITEFUNCTION, bcurl_callback);
        if(ret != 0) {
                blogf("Could not set libcurl callback: %s\n",
		    curl_easy_strerror(ret));
                goto end_label;
        }

	ret = curl_easy_setopt(mycurl, CURLOPT_WRITEDATA, (void *)buf);
	if(ret != 0) {
		blogf("Could not set libcurl callback: %s\n",
		    curl_easy_strerror(ret));
		goto end_label;
	}


end_label:

	if(mycurl != NULL) {
		curl_easy_cleanup(mycurl);
		mycurl = NULL;
	}

	if(buf)
		buninit(&buf);


	return 0;
}
