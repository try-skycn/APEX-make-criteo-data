#!/bin/bash
if [$# -ne 3]; then
	echo "Parameters not match"
	exit 1
fi

DIR=${BASH_SOURCE[0]}/bin
DIVISION=$DIR/division
LABELINDEX=$DIR/labelindex
SORT=$DIR/sort
COUNT=$DIR/count
NUMCOUNT=$DIR/numcount
INDEX=$DIR/index
NUMINDEX=$DIR/numindex
PYTHON=python3
TRANSFER=${BASH_SOURCE[0]}/py/transfer.py

TARGET=sample
DIVDIR=$TARGET.div
INDEXDIR=$TARGET.index
LOG=log
TMP=tmp
OUT=out

TOTALFIELD=40
NUMFIELD=$(seq 1 13)
CATEFIELD=$(seq 14 39)
TRAINNUM=8000000
TESTNUM=2000000

NUMTHRESHOLD=1000
CATETHRESHOLD=5

mkdir -p $DIVDIR
echo $DIVISION $TARGET $DIVDIR
$DIVISION $TARGET $DIVDIR/$TARGET $TOTALFIELD

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

echo $PYTHON $TRANSFER $DIVDIR/$TARGET.{}.feat $TARGET.h5 --train $TRAINNUM --test $TESTNUM --fieldnum $TOTALFIELD --intervals $DIVDIR/$LOG
$PYTHON $TRANSFER $DIVDIR/$TARGET.{}.feat $TARGET.h5 --train $TRAINNUM --test $TESTNUM --fieldnum $TOTALFIELD --intervals $DIVDIR/$LOG

mkdir -p $INDEXDIR
mv $DIVDIR/$TARGET.*.index $INDEXDIR/
rm -r $DIVDIR

echo Done.
