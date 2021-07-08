#include <curl/curl.h>
#include <errno.h>
#include "bcurl.h"
#include "bstr.h"
#include "blog.h"

#define HTTP_RESP_OK	200

struct curl_slist	*bcurl_headers;

int
bcurl_init(void)
{
	int	ret;

	ret = curl_global_init(CURL_GLOBAL_ALL);

	if(ret != 0) {
		blogf("curl_global_init() returned error");
	} 

	return ret;
}


int
bcurl_uninit(void)
{
	bcurl_header_clearall();
	curl_global_cleanup();
		
	return 0;
}

int
bcurl_header_add(const char *hdr)
{
	struct curl_slist *ret;

	ret = curl_slist_append(bcurl_headers, hdr);
	if(ret == NULL) {
		blogf("curl_slist_append returned NULL");
		return ENOEXEC;
	}

	bcurl_headers = ret;

	return 0;
}

int
bcurl_header_clearall()
{
	if(bcurl_headers == NULL)
		return 0;

	curl_slist_free_all(bcurl_headers);
	bcurl_headers = NULL;

	return 0;	
}

static size_t
bcurl_wcallback(void *buffer, size_t size, size_t nmemb, void *userdata)
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


/* For when we need to add options to a curl call.
 * Add further option arguments to this function as needed */
int
bcurl_get_opts(const char *url, bstr_t **docp, const char *usern,
	const char *passw)
{
	CURL	*mycurl;
	long	respcode;
	bstr_t	*buf;
	int	ret;
	int	err;

	err = 0;
	mycurl = NULL;
	buf = NULL;

	if(xstrempty(url) || docp == NULL) {
		blogf("Bad arguments");
		return EINVAL;
	}

	buf = binit();
	if(buf == NULL) {
		blogf("Could not initialize buffer\n");
		err = ENOMEM;
		goto end_label;
	}

	mycurl = curl_easy_init();
	if(mycurl == NULL) {
		blogf("Could not initialize libcurl_easy\n");
		err = ENOEXEC;
		goto end_label;
	}

	ret = curl_easy_setopt(mycurl, CURLOPT_URL, url);
	if(ret != 0) {
		blogf("Could not set URL in libcurl: %s\n",
		    curl_easy_strerror(ret));
		err = ENOEXEC;
		goto end_label;
	}

	ret = curl_easy_setopt(mycurl, CURLOPT_WRITEFUNCTION, bcurl_wcallback);
	if(ret != 0) {
		blogf("Could not set libcurl write callback: %s\n",
		    curl_easy_strerror(ret));
		err = ENOEXEC;
		goto end_label;
	}

	ret = curl_easy_setopt(mycurl, CURLOPT_WRITEDATA, (void *)buf);
	if(ret != 0) {
		blogf("Could not set libcurl write data: %s\n",
		    curl_easy_strerror(ret));
		err = ENOEXEC;
		goto end_label;
	}

	ret = curl_easy_setopt(mycurl, CURLOPT_HTTPHEADER,
	    (void *)bcurl_headers);
	if(ret != 0) {
		blogf("Could not set custom headers: %s\n",
		    curl_easy_strerror(ret));
		err = ENOEXEC;
		goto end_label;
	}

	if(!xstrempty(usern)) {
		ret = curl_easy_setopt(mycurl, CURLOPT_USERNAME,
		    (void *)usern);
		if(ret != 0) {
			blogf("Could not set user name: %s\n",
			    curl_easy_strerror(ret));
			err = ENOEXEC;
			goto end_label;
		}
	}

	if(!xstrempty(passw)) {
		ret = curl_easy_setopt(mycurl, CURLOPT_PASSWORD,
		    (void *)passw);
		if(ret != 0) {
			blogf("Could not set password: %s\n",
			    curl_easy_strerror(ret));
			err = ENOEXEC;
			goto end_label;
		}
	}

	ret = curl_easy_perform(mycurl);
	if(ret != 0) {
		blogf("Could not perform libcurl call: %s\n",
		    curl_easy_strerror(ret));
		err = ENOEXEC;
		goto end_label;
	}

	ret = curl_easy_getinfo(mycurl, CURLINFO_RESPONSE_CODE, &respcode);
	if(ret != 0) {
	blogf("Could not get response code from libcurl:"
		" %s\n", curl_easy_strerror(ret));
		err = ENOEXEC;
		goto end_label;
	}

	if(respcode != HTTP_RESP_OK) {
		blogf("Error response code received: %ld\n", respcode);
		err = ENOEXEC;
		goto end_label;
	}

	*docp = buf;

end_label:

	if(mycurl != NULL) {
		curl_easy_cleanup(mycurl);
		mycurl = NULL;
	}

	if(err != 0 && buf != NULL)
		buninit(&buf);

	return err;
}

 
int
bcurl_get(const char *url, bstr_t **docp)
{
	return bcurl_get_opts(url, docp, NULL, NULL);
}


static size_t
bcurl_rcallback(char *buf, size_t size, size_t nitems, void *userdata)
{
	bstr_t	*bstr;
	size_t	tocopy;
	int	ret;

	bstr = (bstr_t *) userdata;

	/* If NULL or empty, then we're done. */
	if(bstrempty(bstr))
		return 0;

	tocopy = size * nitems;
	if(bstrlen(bstr) < tocopy)
		tocopy = bstrlen(bstr);

	memcpy(buf, bstr->bs_str, tocopy);

	ret = bstrchopl(bstr, tocopy);
	if(ret != 0) {
		blogf("Could not chopl upload data: %s", strerror(ret));
	}
	
	return tocopy;
}


int
bcurl_put_opts(const char *url, bstr_t *putdata, bstr_t **docp,
	const char *usern, const char *passw)
{
	/* NOTE: uploading non-empty putdata is untested, so expect to find
	 * issues when using for the first time. */
	CURL	*mycurl;
	long	respcode;
	bstr_t	*buf;
	int	ret;
	int	err;
	size_t	insize;

	err = 0;
	mycurl = NULL;
	buf = NULL;

	if(xstrempty(url) || docp == NULL) {
		blogf("Bad arguments");
		return EINVAL;
	}

	buf = binit();
	if(buf == NULL) {
		blogf("Could not initialize buffer\n");
		err = ENOMEM;
		goto end_label;
	}

	mycurl = curl_easy_init();
	if(mycurl == NULL) {
		blogf("Could not initialize libcurl_easy\n");
		err = ENOEXEC;
		goto end_label;
	}

	ret = curl_easy_setopt(mycurl, CURLOPT_URL, url);
	if(ret != 0) {
		blogf("Could not set URL in libcurl: %s\n",
		    curl_easy_strerror(ret));
		err = ENOEXEC;
		goto end_label;
	}

	ret = curl_easy_setopt(mycurl, CURLOPT_WRITEFUNCTION, bcurl_wcallback);
	if(ret != 0) {
		blogf("Could not set libcurl write callback: %s\n",
		    curl_easy_strerror(ret));
		err = ENOEXEC;
		goto end_label;
	}

	ret = curl_easy_setopt(mycurl, CURLOPT_WRITEDATA, (void *)buf);
	if(ret != 0) {
		blogf("Could not set libcurl write data: %s\n",
		    curl_easy_strerror(ret));
		err = ENOEXEC;
		goto end_label;
	}

	ret = curl_easy_setopt(mycurl, CURLOPT_UPLOAD, 1);
	if(ret != 0) {
		blogf("Could not set CURLOPT_UPLOAD in libcurl: %s\n",
		    curl_easy_strerror(ret));
		err = ENOEXEC;
		goto end_label;
	}

	ret = curl_easy_setopt(mycurl, CURLOPT_READFUNCTION, bcurl_rcallback);
	if(ret != 0) {
		blogf("Could not set libcurl read callback: %s\n",
		    curl_easy_strerror(ret));
		err = ENOEXEC;
		goto end_label;
	}

	ret = curl_easy_setopt(mycurl, CURLOPT_READDATA, (void *)putdata);
	if(ret != 0) {
		blogf("Could not set libcurl read data: %s\n",
		    curl_easy_strerror(ret));
		err = ENOEXEC;
		goto end_label;
	}
	
	if(bstrempty(putdata))
		insize = 0;
	else
		insize = bstrlen(putdata);

	ret = curl_easy_setopt(mycurl, CURLOPT_INFILESIZE, insize);
	if(ret != 0) {
		blogf("Could not set libcurl infilesize: %s\n",
		    curl_easy_strerror(ret));
		err = ENOEXEC;
		goto end_label;
	}
	
	ret = curl_easy_setopt(mycurl, CURLOPT_HTTPHEADER,
	    (void *)bcurl_headers);
	if(ret != 0) {
		blogf("Could not set custom headers: %s\n",
		    curl_easy_strerror(ret));
		err = ENOEXEC;
		goto end_label;
	}

	ret = curl_easy_perform(mycurl);
	if(ret != 0) {
		blogf("Could not perform libcurl call: %s\n",
		    curl_easy_strerror(ret));
		err = ENOEXEC;
		goto end_label;
	}

	ret = curl_easy_getinfo(mycurl, CURLINFO_RESPONSE_CODE, &respcode);
	if(ret != 0) {
	blogf("Could not get response code from libcurl:"
		" %s\n", curl_easy_strerror(ret));
		err = ENOEXEC;
		goto end_label;
	}

	if(respcode != 200 && respcode != 202 && respcode != 204) {
		blogf("Error response code received: %ld\n", respcode);
		err = ENOEXEC;
		goto end_label;
	}

	*docp = buf;

end_label:

	if(mycurl != NULL) {
		curl_easy_cleanup(mycurl);
		mycurl = NULL;
	}

	if(err != 0 && buf != NULL)
		buninit(&buf);

	return err;
}


int
bcurl_put(const char *url, bstr_t *putdata, bstr_t **docp)
{
	return bcurl_put_opts(url, putdata, docp, NULL, NULL);
}


int
bstrcat_urlenc(bstr_t *bstr, const char *str)
{
	CURL    *mycurl;
	char	*enc;
	int	err;

	if(bstr == NULL || str == NULL)
		return EINVAL;

	err = 0;
	mycurl = NULL;
	enc = NULL;
	
	mycurl = curl_easy_init();
	if(mycurl == NULL) {
		blogf("Could not initialize libcurl_easy\n");
		err = ENOEXEC;
		goto end_label;
	}

	enc = curl_easy_escape(mycurl, str, 0);
	
	if(enc == NULL) {
		blogf("Could not initialize libcurl_easy\n");
		err = ENOEXEC;
		goto end_label;
	}

	bstrcat(bstr, enc);

end_label:

	if(enc)
		curl_free(enc);

	if(mycurl) {
		curl_easy_cleanup(mycurl);
		mycurl = NULL;
	}

	return err;
}
