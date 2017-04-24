#!/bin/sh

for i in `find . -name "*.cpp"`
do
	mv ${i} `echo ${i} | sed "s/\.cpp/\.cc/"`
done

for i in `find . -name "*.cc"`
do
	sed -i "s/NEET/TAKASHI/g" ${i}
	sed -i "s/neet3/takashi/g" ${i}

	sed -i "s/[ ]\+$//" ${i}
done
