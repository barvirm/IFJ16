#!/bin/bash
#
#diff outputs prom interpret and correct outputs
#make summary

FILES=tests/Full/Outputs/*.output

difference=0
all=0
pass=0
for f in $FILES
do
	all=$((all+1))
	pass=$((pass+1))
	difference=$(diff tests/Full/Outputs/${f:19} tests/Full/Correct/${f:19}.correct | wc -l)
	if (($difference!=0)); then
		pass=$((pass-1))
		echo "OUTPUT"
		cat tests/Full/Outputs/${f:19}
		echo "CORRECT"
		cat tests/Full/Correct/${f:19}.correct
		echo " "
	fi

done

echo "SUMMARY"
echo "Number of tests:  $all"
echo "Numbet of passed: $pass"