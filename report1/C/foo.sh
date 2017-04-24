#!/bin/sh

> 1.pdf
for i in `seq -w 0 99`
do
	wget `echo "http://pf.is.s.u-tokyo.ac.jp/jp/class/syspro/kadai1/1.pdf.${i}"`
	cat `echo "1.pdf.${i}"` >> 1.pdf
	rm `echo "1.pdf.${i}"`
done

