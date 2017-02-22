#!/bin/bash

FILES=*.output

for f in $FILES
do
	mv $f $f.correct
done

