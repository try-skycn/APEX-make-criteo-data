#!/bin/bash
if [[ $# -ne 4 ]]; then
	echo "Parameters not match"
	exit 1
fi

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

BIN=$DIR/bin
DIVISION=$BIN/division
LABELINDEX=$BIN/labelindex
SORT=$BIN/sort
COUNT=$BIN/count
NUMCOUNT=$BIN/numcount
INDEX=$BIN/index
NUMINDEX=$BIN/numindex
PYTHON=python3
TRANSFER=$DIR/py/transfer.py

TARGET=$1
DIVDIR=$TARGET.div
INDEXDIR=$TARGET.index
LOG=log
TMP=tmp
OUT=out

TOTALFIELD=40
NUMFIELD=$(seq 1 13)
CATEFIELD=$(seq 14 39)
TRAINNUM=$2

echo "target: $TARGET"

NUMTHRESHOLD=$3
CATETHRESHOLD=$4

echo "numerical field threshold: $NUMTHRESHOLD"
echo "categorical field threshold: $CATETHRESHOLD"

mkdir -p $DIVDIR
echo $DIVISION $TARGET $DIVDIR
$DIVISION $TARGET $DIVDIR/$TARGET $TOTALFIELD 1> $OUT
TOTAL=$(cat $OUT)
rm $OUT

TESTNUM=$((TOTAL-TRAINNUM))
echo "train-set: $TRAINNUM"
echo "test-set: $TESTNUM"
echo "total: $TOTAL"

cd $DIVDIR

$LABELINDEX $TARGET.0 $TARGET.0.feat

indexhead=1
indextail=1

for i in $NUMFIELD; do
	FIELD=$TARGET.$i
	FIELDCOUNT=$FIELD.count
	FIELDINDEX=$FIELD.index
	FIELDFEAT=$FIELD.feat

	echo $NUMCOUNT $FIELD $FIELDCOUNT
	$NUMCOUNT $FIELD $FIELDCOUNT

	indexhead=$indextail
	echo $NUMINDEX $FIELD $FIELDCOUNT $indexhead $NUMTHRESHOLD $FIELDINDEX $FIELDFEAT
	$NUMINDEX $FIELD $FIELDCOUNT $indexhead $NUMTHRESHOLD $FIELDINDEX $FIELDFEAT 1> $OUT
	indextail=$(cat $OUT)
	rm $OUT

	echo $indexhead $indextail >> $LOG
	rm $FIELD $FIELDCOUNT
done

for i in $CATEFIELD; do
	FIELD=$TARGET.$i
	FIELDCOUNT=$FIELD.count
	FIELDINDEX=$FIELD.index
	FIELDFEAT=$FIELD.feat

	mkdir -p $TMP
	echo $SORT $FIELD $TMP/$FIELD
	$SORT $FIELD $TMP/$FIELD 1> $OUT
	filenum=$(cat $OUT)
	rm $OUT

	echo $COUNT $TMP/$FIELD $filenum $FIELDCOUNT
	$COUNT $TMP/$FIELD $filenum $FIELDCOUNT
	rm -r $TMP

	indexhead=$indextail
	echo $INDEX $FIELD $FIELDCOUNT $indexhead $NUMTHRESHOLD $FIELDINDEX $FIELDFEAT
	$INDEX $FIELD $FIELDCOUNT $indexhead $NUMTHRESHOLD $FIELDINDEX $FIELDFEAT 1> $OUT
	indextail=$(cat $OUT)
	rm $OUT

	echo $indexhead $indextail >> $LOG
	rm $FIELD $FIELDCOUNT
done

cd ..

echo $PYTHON $TRANSFER $DIVDIR/$TARGET.{}.feat $TARGET.h5 --train $TRAINNUM --test $TESTNUM --fieldnum $TOTALFIELD --intervals $DIVDIR/$LOG --index $DIVDIR/$TARGET.{}.index
$PYTHON $TRANSFER $DIVDIR/$TARGET.{}.feat $TARGET.h5 --train $TRAINNUM --test $TESTNUM --fieldnum $TOTALFIELD --intervals $DIVDIR/$LOG --index $DIVDIR/$TARGET.{}.index

rm -r $DIVDIR

echo Done.
