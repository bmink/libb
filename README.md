
A collection of helpful things for the C programming language that I wrote
over the years and use in most of my projects.

* bstr.\[ch\] provides bstr_t, a convenient and safe "string" entity that manages
  its own memory growth. Most conveniently, it can be appended to via a convenient
  bprintf() call which supports the usual format string specifications.

* bstr.\[ch\] also provides safe versions of many standard string manipulation
  functions. These are prepended with an 'x'. So there's xstrlen, xstrcmp, etc.

* barr.\[ch\] is an array library. It manages its own memory and growth. In addition
  to adding and accessing elements, it can be sorted and searched easily.

* btime.[ch] contains utilities for time stucture (time_t, struct timeval,
  struct timespec) manipulation, comparison, addition/substraction etc.

* blog.\[ch\] is a logging utility. Uses syslog most anywhere, but on Mac OS X, it
  uses the new ASL services.

* bcurl.\[ch\] is a wrapper around libcurl. Documents can be fetched via a single call
  returning the response in a bstr_t.
  
* bfs.\[ch\] contains file system related utilities.

* bint.\[ch\] provides integer utilities.


## barr/bstr example

It can get confusing to use a barr of bstr's (or any other struct-type that's allocated on the heap), so here's a boilerplate example:

```
barr_t  *arr;
bstr_t  *elem;

arr = barr_init(sizeof(bstr_t));

/* Adding */
elem = binit();
barr_add(arr, elem);
elem = binit();
barr_add(arr, elem);
elem = binit();
barr_add(arr, elem);
elem = binit();
barr_add(arr, elem);

/* Reading */
for(elem = (bstr_t *) barr_begin(arr);
    elem < (bstr_t *) barr_end(arr);
    ++elem) {
        printf("(%d) %s\n", idx, bstrlen(elem), bget(elem));
    }
}

/* Cleanup */
for(elem = (bstr_t *) barr_begin(arr);
    elem < (bstr_t *) barr_end(arr);
    ++elem) {
        buninit_(elem); /* Use the "_" form of buninit that doesn't free/reset the pointer */
}
barr_uninit(&arr);
```
