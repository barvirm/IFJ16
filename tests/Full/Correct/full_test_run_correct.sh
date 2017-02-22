#!/bin/bash
#Script for testing on merlin
#ŃOT WORKING, !DONT USE IT!

FILES=*.ifj16

Test=0
Return=0
for f in $FILES
do
	echo "Processing: $f"
	((Test++))
	echo "TEST: $Test" > Correct/$f.output.correct
	echo "Processing: $f" >> Correct/$f.output.correct
	echo "Output:" >> Correct/$f.output.correct
	cat $f > tmp.java
	javac tmp.java
	Return=$?
	echo "Javac $Return"
	if (($Return==0)); then
		echo "Running"
		java ifj16 >> Correct/$f.output.correct
	fi
	#./run_ifj16 $f >> Correct/${f:11}.output.correct
	echo "Return: $?" >> Correct/$f.output.correct
done

cat Correct/*.output.correct > Correct/merged_outputs