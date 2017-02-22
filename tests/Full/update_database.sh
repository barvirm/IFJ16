#!/bin/bash
#remake tests for parser to tests for interpret
#copy and rename

FILES=tests/Full/parser_test_*

rm tests/Full/*.ifj16

#copy parser tests to Full tests
cp tests/Parser/parser_test_* tests/Full

#remove first line
sed -i '1d' tests/Full/parser_test_*

#rename
for f in $FILES
do
	mv $f tests/Full/${f:23}
done


FILES=tests/Full/*.ifj16

for f in $FILES
do
	mv $f tests/Full/full_test_${f:11}
done

#make empty input file
FILES=tests/Full/*.ifj16

for f in $FILES
do
	cp -n /dev/null tests/Full/Inputs/${f:11}.input

done

#make output file
Test=1
for f in $FILES
do
	found=`ls tests/Full/Correct/${f:11}.output.correct 2> /dev/null | wc -l`
	if(($found==0));then
		cp -n /dev/null tests/Full/Correct/${f:11}.output.correct
		echo "TEST: $Test" >> tests/Full/Correct/${f:11}.output.correct
		echo "Processing: $f" >> tests/Full/Correct/${f:11}.output.correct
		echo "Output:" >> tests/Full/Correct/${f:11}.output.correct
		echo "Return:" >> tests/Full/Correct/${f:11}.output.correct
	fi
	((Test++))

done
