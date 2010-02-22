#!/bin/sh

. ./configuration.sh

rm -f testcases/*.xml
find ./testcases -iname "*.xtc" | while read line; do echo $line; ${XTC2XABSL} -x ${line}; done
echo `ls testcases/*.xtc | wc -l` source files
echo "created the following files:"
ls testcases/*.xml
