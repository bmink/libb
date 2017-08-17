# libb
A collection of helpful things for the C programming language that I wrote over the years and use in most of my projects.

* bstr_t is a convenient and safe "string" entity that manages its own memory growth. It can be appended to via a convenient bprintf() cals
  which supports all usual format string specs.
* Safe versions of many standard string manipulation functions are given. These are prepended with an 'x'. So there's xstrlen, xstrcmp,
  etc.
* barr_t is an array library. It manages its own memory and growth. In addition to adding and accessing elements, it can be sorted and
  searched easily.
