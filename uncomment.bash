#!/bin/bash

sed "s_\(^//\)\(.*$1\$\)_\2_i" -i src/*.c
#uncomment lines beginning with //  and ending with //<arg1> (the c lines will be compiled on next run, not deleted.)