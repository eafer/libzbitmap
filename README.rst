============================
LZBITMAP compression library
============================

This is a C implementation of Apple's LZBITMAP compression algorithm,
reverse-engineered by myself via black box testing. For now it's much slower
than the original, but the ratios obtained are similar.

The build is simple: as long as you have ``gcc`` and ``make`` installed, you
can just run ``make``. The API is documented inside the header file
``libzbitmap.h``.

Please report any bugs you find to the author at <ernesto@corellium.com>, or
file a github issue at https://github.com/eafer/libzbitmap/issues.
