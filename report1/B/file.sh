#!/bin/sh

for i in `find ~/syspro/coreutils-8.9 -name "*.c"`
do
	wc -l ${i}
done | sort -n -k 1,1 |
while read get filename
do
	echo ${filename}
done > result.txt
