#! /bin/sh
aclocal
automake --foreign --add-missing --copy
autoconf --output=configure configure.in
