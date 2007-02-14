#!/bin/sh 
# -xvf
#

VALGRIND=

if [ "x$1" == "x-v" ]; then
	VALGRIND="valgrind -v --tool=memcheck --num-callers=25";
fi

if [ "x$1" == "x-t" ]; then
	VALGRIND="valgrind -v --tool=helgrind --num-callers=25";
fi


######### All settings are above this line

CONSOLELOG=`mktemp /tmp/Kst_ConsoleLog.XXXXXX` || exit 4
echo Logging to $CONSOLELOG

$VALGRIND kst --nq >$CONSOLELOG 2>&1 &

DCOP="dcop kst-$! KstIface"
QUIT="dcop kst-$! MainApplication-Interface quit"

TMPFILE=`mktemp /tmp/Kst_TestSuite.XXXXXX` || exit 3


doExit() {
	$QUIT
	rm $TMPFILE
	rm $TESTFILE
	rm -f $KSTFILE
	exit $1;
}

checkEmptyResponse() {
	if [ ! -s "$TMPFILE" ]; then
		echo "Test $1 failed.  Response was non-empty. [" `cat $TMPFILE` "]"
		return;
	fi

	echo -ne "Test $1 passed.\r"
}

checkArraySize() {
	LEN=`wc -l < $TMPFILE`
	if test ! "x$LEN" = "x$2"; then
		echo "Test $1 failed.  Expected [$2] entries, received [$LEN]."
		return;
	fi

	echo -ne "Test $1 passed.\r"
}

checkStringResponse() {
	TESTSTR=`cat $TMPFILE | tr -d '\n'`
	if test ! "x$2" = "x$TESTSTR"; then
		echo "Test $1 failed.  Expected [$2], received [$TESTSTR]."
		return;
	fi

	echo -ne "Test $1 passed.\r"
}


echo -n "Waiting for Kst launch completely."
while [ 0 ]; do
	$DCOP >/dev/null 2>&1
	if [ $? -eq 0 ]; then
		break;
	fi
	echo -n "."
	sleep 1;
done
echo ""


declare -i cnt=0

#####  All lists are empty to start
$DCOP curveList >$TMPFILE
checkEmptyResponse 1

$DCOP plotList >$TMPFILE
checkEmptyResponse 2

$DCOP objectList >$TMPFILE
checkEmptyResponse 3

$DCOP vectorList >$TMPFILE
checkEmptyResponse 4

$DCOP scalarList >$TMPFILE
checkEmptyResponse 5

#####  Scalars
$DCOP generateScalar "Scalar Test 1" 3.14159265358979 >$TMPFILE
checkStringResponse 30 "Scalar Test 1"

$DCOP scalar "Scalar Test 1" >$TMPFILE
checkStringResponse 31 "3.141593"

$DCOP generateScalar "Scalar Test 1" 0.000000000 >$TMPFILE
checkStringResponse 32 "Scalar Test 1'"

$DCOP scalar "Scalar Test 1'" >$TMPFILE
checkStringResponse 33 "0.000000"

$DCOP generateScalar "Scalar Test 1" "-33.5e+23" >$TMPFILE
checkStringResponse 34 "Scalar Test 1''"

$DCOP scalar "Scalar Test 1''" >$TMPFILE
checkStringResponse 35 "-3349999999999999970639872.000000"

$DCOP scalarList >$TMPFILE
checkArraySize 36 25 

#####  Vectors
$DCOP generateVector "Vector Test 1" -10 10 21 >$TMPFILE
checkStringResponse 100 "Vector Test 1"

$DCOP vectorSize "Vector Test 1" >$TMPFILE
checkStringResponse 101 "21"

# tests 102-122
cnt=0
while [ $cnt -lt 21 ]; do
	$DCOP vector "Vector Test 1" $cnt >$TMPFILE
	declare -i j=-10+$cnt
	declare -i n=102+$cnt
	checkStringResponse $n "$j.000000"
	cnt=$cnt+1;
done

$DCOP vector "Vector Test 1" -1 >$TMPFILE
checkStringResponse 123 "0.000000"

$DCOP vector "Vector Test 1" 21 >$TMPFILE
checkStringResponse 124 "0.000000"

$DCOP generateVector "Vector Test 2" -10 10 1 >$TMPFILE
checkStringResponse 125 "Vector Test 2"

# vectors must have at least 2 entries
$DCOP vectorSize "Vector Test 2" >$TMPFILE
checkStringResponse 126 "2"

$DCOP vector "Vector Test 2" 0 >$TMPFILE
checkStringResponse 127 "-10.000000"

$DCOP vector "Vector Test 2" 1 >$TMPFILE
checkStringResponse 128 "10.000000"

$DCOP generateVector "Vector Test 3" -10 10 0 >$TMPFILE
checkStringResponse 129 "Vector Test 3"

$DCOP vectorSize "Vector Test 3" >$TMPFILE
checkStringResponse 130 "2"

$DCOP vector "Vector Test 3" 0 >$TMPFILE
checkStringResponse 131 "-10.000000"

$DCOP vector "Vector Test 3" 1 >$TMPFILE
checkStringResponse 132 "10.000000"

$DCOP generateVector "Vector Test 4" -10 10 -987 >$TMPFILE
checkStringResponse 133 "Vector Test 4"

$DCOP vectorSize "Vector Test 4" >$TMPFILE
checkStringResponse 134 "2"

$DCOP vector "Vector Test 4" 0 >$TMPFILE
checkStringResponse 135 "-10.000000"

$DCOP vector "Vector Test 4" 1 >$TMPFILE
checkStringResponse 136 "10.000000"

$DCOP generateVector "Vector Test 5" -1 -1 44 >$TMPFILE
checkStringResponse 137 "Vector Test 5"

$DCOP vectorSize "Vector Test 5" >$TMPFILE
checkStringResponse 138 "44"

$DCOP vector "Vector Test 5" 33 >$TMPFILE
checkStringResponse 139 "-0.923256"

$DCOP vector "Vector Test 5" 2 >$TMPFILE
checkStringResponse 140 "-0.995349"

$DCOP vectorList >$TMPFILE
checkArraySize 141 5

$DCOP clearVector "Vector Test 1" >$TMPFILE
checkStringResponse 142 "true"

$DCOP vectorSize "Vector Test 1" >$TMPFILE
checkStringResponse 143 "21"

# tests 144-164
cnt=0
while [ $cnt -lt 21 ]; do
	$DCOP vector "Vector Test 1" $cnt >$TMPFILE
	declare -i n=144+$cnt
	checkStringResponse $n "0.000000"
	cnt=$cnt+1;
done

$DCOP resizeVector "Vector Test 1" 5 >$TMPFILE
checkStringResponse 165 "true"

$DCOP vectorSize "Vector Test 1" >$TMPFILE
checkStringResponse 166 "5"

$DCOP resizeVector "Vector Test 1" 2 >$TMPFILE
checkStringResponse 167 "true"

$DCOP vectorSize "Vector Test 1" >$TMPFILE
checkStringResponse 168 "2"

$DCOP resizeVector "Vector Test 1" 1 >$TMPFILE
checkStringResponse 169 "true"

$DCOP vectorSize "Vector Test 1" >$TMPFILE
checkStringResponse 170 "1"

$DCOP resizeVector "Vector Test 1" 0 >$TMPFILE
checkStringResponse 171 "false"

$DCOP vectorSize "Vector Test 1" >$TMPFILE
checkStringResponse 172 "1"

$DCOP resizeVector "Vector Test 1" -4 >$TMPFILE
checkStringResponse 173 "false"

$DCOP vectorSize "Vector Test 1" >$TMPFILE
checkStringResponse 174 "1"

$DCOP resizeVector "Vector Test 1" 3 >$TMPFILE
checkStringResponse 175 "true"

$DCOP setVector "Vector Test 1" -1 42 >$TMPFILE
checkStringResponse 176 "false"

$DCOP setVector "Vector Test 1" -2 42 >$TMPFILE
checkStringResponse 177 "false"

$DCOP setVector "Vector Test 1" 0 42 >$TMPFILE
checkStringResponse 178 "true"

$DCOP setVector "Vector Test 1" 1 42.42 >$TMPFILE
checkStringResponse 179 "true"

$DCOP setVector "Vector Test 1" 2 42.4242 >$TMPFILE
checkStringResponse 180 "true"

$DCOP vector "Vector Test 1" 0 >$TMPFILE
checkStringResponse 181 "42.000000"

$DCOP vector "Vector Test 1" 1 >$TMPFILE
checkStringResponse 182 "42.420000"

$DCOP vector "Vector Test 1" 2 >$TMPFILE
checkStringResponse 183 "42.424200"

$DCOP setVector "Vector Test 1" 3 42.424242 >$TMPFILE
checkStringResponse 184 "false"

$DCOP vectorSize "Vector Test 1" >$TMPFILE
checkStringResponse 185 "3"

$DCOP resizeVector "Vector Test 1" 5 >$TMPFILE
checkStringResponse 186 "true"

$DCOP vector "Vector Test 1" 0 >$TMPFILE
checkStringResponse 187 "42.000000"

$DCOP vector "Vector Test 1" 1 >$TMPFILE
checkStringResponse 188 "42.420000"

$DCOP vector "Vector Test 1" 2 >$TMPFILE
checkStringResponse 189 "42.424200"

$DCOP vector "Vector Test 1" 3 >$TMPFILE
checkStringResponse 190 "0.000000"

$DCOP vector "Vector Test 1" 4 >$TMPFILE
checkStringResponse 191 "0.000000"

$DCOP setVector "Vector Test 1" 3 42.424242 >$TMPFILE
checkStringResponse 192 "true"

$DCOP setVector "Vector Test 1" 4 42424242.42 >$TMPFILE
checkStringResponse 193 "true"

$DCOP vector "Vector Test 1" 3 >$TMPFILE
checkStringResponse 194 "42.424242"

$DCOP vector "Vector Test 1" 4 >$TMPFILE
checkStringResponse 195 "42424242.420000"

$DCOP saveVector "Vector Test 1" "" >$TMPFILE
checkStringResponse 196 "false"

VTMPFILE=`mktemp /tmp/Kst_Vector.XXXXXX` || exit 3
$DCOP saveVector "Vector Test 1" $VTMPFILE >$TMPFILE
checkStringResponse 197 "true"

	LEN=`wc -l < $VTMPFILE`
	if test ! "x$LEN" = "x7"; then
		echo "Test 198 failed.  Expected [7] entries, received [$LEN].";
	fi

	echo -ne "Test 198 passed.\r"
	rm $VTMPFILE

#####  Plots

$DCOP generateVector "plotinput1" 0 1000 10000 >$TMPFILE
checkStringResponse 500 "plotinput1"

$DCOP generateVector "plotinput2" 0 100 10000 >$TMPFILE
checkStringResponse 501 "plotinput2"

$DCOP createCurve "curve-ramp" "plotinput1" "plotinput2" "" "" >$TMPFILE
checkStringResponse 502 "curve-ramp"

$DCOP createCurve "curve-ramp2" "plotinput2" "plotinput1" "" "" >$TMPFILE
checkStringResponse 503 "curve-ramp2"

$DCOP createPlot "P0" >$TMPFILE
checkStringResponse 504 "P0"

$DCOP addCurveToPlot "P0" "curve-ramp" >$TMPFILE
checkStringResponse 505 "true"

$DCOP createPlot "P1" >$TMPFILE
checkStringResponse 506 "P1"

$DCOP addCurveToPlot "P1" "curve-ramp2" >$TMPFILE
checkStringResponse 507 "true"

$DCOP createPlot "P2" >$TMPFILE
checkStringResponse 508 "P2"

$DCOP plotEquation "plotinput1" "2*sin(x)*cos(x^2)" "P2" >$TMPFILE
checkStringResponse 509 "true"

VTMPFILE=`mktemp /tmp/Kst_TestPrintOut1.png.XXXXXX` || exit 3
echo "Writing printout image [P0,P1,P2] to $VTMPFILE"
$DCOP printImage $VTMPFILE >$TMPFILE
checkStringResponse 510 "true"

VTMPFILE=`mktemp /tmp/Kst_TestPrintOut1.ps.XXXXXX` || exit 3
echo "Writing printout postscript [P0,P1,P2] to $VTMPFILE"
$DCOP printPostScript $VTMPFILE >$TMPFILE
checkStringResponse 511 "true"

$DCOP deletePlot "P1" >$TMPFILE
checkStringResponse 512 "true"

$DCOP deletePlot "P0" >$TMPFILE
checkStringResponse 513 "true"

$DCOP deletePlot "P2" >$TMPFILE
checkStringResponse 514 "true"

VTMPFILE=`mktemp /tmp/Kst_TestPrintOut1.png.XXXXXX` || exit 3
echo "Writing printout image [blank] to $VTMPFILE"
$DCOP printImage $VTMPFILE >$TMPFILE
checkStringResponse 515 "true"

VTMPFILE=`mktemp /tmp/Kst_TestPrintOut1.ps.XXXXXX` || exit 3
echo "Writing printout postscript [blank] to $VTMPFILE"
$DCOP printPostScript $VTMPFILE >$TMPFILE
checkStringResponse 516 "true"

echo -ne "Generating a test file.\r"

TESTFILE=`mktemp /tmp/Kst_TestData.XXXXXX` || exit 3
declare -i ln=0
while [ $ln -lt 1000 ]; do
  echo $ln | awk "{print $ln, sin($ln/10), ($ln/100)^2, cos($ln/10)}" >>$TESTFILE
  declare -i ln=1+$ln;
done

echo -ne "                       \r"

# Must clear out the generated vectors because they don't save properly FIXME
# 599
$DCOP newFile >$TMPFILE

# Read in the vectors
$DCOP loadVector $TESTFILE INDEX >$TMPFILE
VINDEX=`cat $TMPFILE | tr -d '\n'`
checkEmptyResponse 600

$DCOP loadVector $TESTFILE 2 >$TMPFILE
V2=`cat $TMPFILE | tr -d '\n'`
checkEmptyResponse 601

$DCOP loadVector $TESTFILE 3 >$TMPFILE
V3=`cat $TMPFILE | tr -d '\n'`
checkEmptyResponse 602

$DCOP loadVector $TESTFILE 4 >$TMPFILE
V4=`cat $TMPFILE | tr -d '\n'`
checkEmptyResponse 603

$DCOP createCurve "firstCurve" $VINDEX $V2 "" "" >$TMPFILE
CURVE1=`cat $TMPFILE | tr -d '\n'`
checkEmptyResponse 604

$DCOP createPlot "firstPlot" >$TMPFILE
PLOT1=`cat $TMPFILE | tr -d '\n'`
checkEmptyResponse 605

$DCOP addCurveToPlot $PLOT1 $CURVE1 >$TMPFILE
checkStringResponse 606 "true"

$DCOP createCurve "secondCurve" $VINDEX $V4 "" "" >$TMPFILE
CURVE2=`cat $TMPFILE | tr -d '\n'`
checkEmptyResponse 607

$DCOP addCurveToPlot $PLOT1 $CURVE2 >$TMPFILE
checkStringResponse 608 "true"

$DCOP createPlot "secondPlot" >$TMPFILE
PLOT2=`cat $TMPFILE | tr -d '\n'`
checkEmptyResponse 609

$DCOP createCurve "thirdCurve" $VINDEX $V3 "" "" >$TMPFILE
CURVE3=`cat $TMPFILE | tr -d '\n'`
checkEmptyResponse 610

$DCOP addCurveToPlot $PLOT2 $CURVE3 >$TMPFILE
checkStringResponse 611 "true"

KSTFILE=`mktemp /tmp/Kst_TestFile.XXXXXX` || exit 3
rm $KSTFILE

$DCOP saveAs $KSTFILE >$TMPFILE
checkStringResponse 612 "true"

# 613
$DCOP newFile >$TMPFILE

$DCOP curveList >$TMPFILE
checkEmptyResponse 614

# crashes Kst generally, bypassed with #599
$DCOP open $KSTFILE >$TMPFILE
checkStringResponse 620 "true"

rm -f $KSTFILE

$DCOP save >$TMPFILE
checkStringResponse 621 "true"

$DCOP open $KSTFILE >$TMPFILE
checkStringResponse 622 "true"

$DCOP save >$TMPFILE
checkStringResponse 623 "true"

echo ""
doExit 0


#
# Things to test still
# --------------------
#
# Vector/scalar namespace
# Object namespace
# Plot content manipulations
# Error vectors
# PSDs
# Histograms
# Equations with variables
# Plugins
# Labels
# Data vectors
# Plot scales
# Saving and loading
# Scalars generated for vectors
#


