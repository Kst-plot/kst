#!/bin/sh
#

VALGRIND=
GCOV=

if [ "x$1" == "x-g" ]; then
	GCOV=1
fi

if [ "x$1" == "x-v" ]; then
	VALGRIND="valgrind -v --num-callers=25";
fi

if [ "x$1" == "x-t" ]; then
	VALGRIND="valgrind -v --skin=helgrind --num-callers=25";
fi


######### All settings are above this line

CONSOLELOG=`mktemp /tmp/Kst_regressionlog.XXXXXX` || exit 4
echo Logging to $CONSOLELOG

make check
if [ $? -ne 0 ]; then
	echo "ERROR BUILDING TESTCASES"
	exit -1;
fi

TESTS="testeqparser testhistogram testscalars testlabelparser testrvector testvector healpix/testhealpix"
testeqparser_FILES="eparse.y escan.l eparse.c escan.c enodes.cpp enodefactory.cpp"
testhistogram_FILES="ksthistogram.cpp"
testscalars_FILES="kstscalar.cpp"
testlabelparser_FILES="labelparser.cpp"
testrvector_FILES="kstrvector.cpp"
testvector_FILES="kstvector.cpp"

for i in $TESTS; do
	if [ ! -e $i ]; then
		echo "ERROR: Couldn't find test $i.  Not running."
		continue;
	fi
	echo "-----------------------------------------------------------" >> $CONSOLELOG
	echo "Running test: $i" >> $CONSOLELOG
	output=`$VALGRIND ./$i $* 2>&1`
	FAILED=$?
	echo "$output" | grep -v QPixmap >>$CONSOLELOG
	if [ $FAILED -gt 0 ]; then
		echo "$FAILED testcases failed in $i" | tee -a $CONSOLELOG
	fi
	echo >>$CONSOLELOG
	echo Code coverage: >>$CONSOLELOG
	pushd ../kst >/dev/null 2>&1
	filenames='$'"$i"_FILES;
	list=`eval echo $filenames`
	for j in $list; do
		gcov -o .libs -n $j 2>&1 |
		while true; do
			read k
			if test $? -ne 0; then
				break;
			fi
			echo "$k" | grep "File \`$j'" >/dev/null 2>&1
			if test $? -eq 0; then
				read l
				echo $j: $l >>$CONSOLELOG
				break
			fi
		done
	done
	popd >/dev/null 2>&1
	echo >>$CONSOLELOG
	echo >>$CONSOLELOG
done

exit 0

