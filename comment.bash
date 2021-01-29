#!/bin/bash

sed "s_^[^/][^/].*//$1\$_//&_i" -i src/*.c
#Comment lines not beginning with // and ending with //<arg1> (the c lines will be compiled on next run, not deleted.)