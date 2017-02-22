#!/bin/bash
#run interpret for all tests in tests/Full
#
#outputs are in in tests/Full/Outputs
#outputs are merged in merged_outputs
#
#inputs are in in tests/Full/Inputs

FILES=tests/Full/full_test_*

Test=0
for f in $FILES
do
	((Test++))
	echo "TEST: $Test" > tests/Full/Outputs/${f:11}.output
	echo "Processing: $f" >> tests/Full/Outputs/${f:11}.output
	echo "Output:" >> tests/Full/Outputs/${f:11}.output
	./run_ifj16 $f < tests/Full/Inputs/${f:11}.input >> tests/Full/Outputs/${f:11}.output 2>&1
	echo "Return: $?" >> tests/Full/Outputs/${f:11}.output
done

cat tests/Full/Outputs/*.output > tests/Full/Outputs/merged_outputs