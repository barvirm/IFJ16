#!/bin/bash
#rename .output to .input

FILES=*.output

for f in $FILES
do
	mv $f ${f:0:21}
done

FILES=*.ifj16

for f in $FILES
do
	mv $f $f.input
done
