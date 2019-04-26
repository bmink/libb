# libb
# https://github.com/bmink/libb

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
