#! /bin/sh
echo "Bootstrapping: "

echo " -=> aclocal"
aclocal

echo " -=> autoheader"
autoheader ./configure.ac

echo " -=> automake"
automake --foreign --add-missing --copy

echo " -=> autoconf"
autoconf --output=configure configure.ac

echo
