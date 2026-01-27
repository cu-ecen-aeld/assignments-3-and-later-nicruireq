#!/bin/bash

filedir=$1
searchstr=$2

if [ -z $filedir ] || [ -z $searchstr ]
then
	echo 'Arguments are empty'
	exit 1
elif [ ! -d $filedir ]
then
	echo "$filedir is not an existing directory"
	exit 1
else
	filesnumber=$(find $filedir -type f | wc -l)
	strmatches=$(grep -r $searchstr $filedir/* | wc -l)
	echo "The number of files are $filesnumber and the number of matching lines are $strmatches"
fi
