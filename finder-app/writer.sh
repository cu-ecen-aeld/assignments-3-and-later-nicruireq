#!/bin/bash

writefile=$1
writestr=$2

if [ -z $writefile ] || [ -z $writestr ]
then
	echo "Arguments must not be empty"
	exit 1
else
	pathonly=$(dirname $writefile)
	mkdir -p $pathonly
	if [ -d $pathonly ]
	then
		touch $writefile
		if [ -w $writefile ]
		then
			echo "$writestr" > $writefile
		else
			echo "the file $writefile could not be created or you have not writting permissions"
                        exit 1
		fi
	else
		echo "Directory $pathonly could not be created"
		exit 1
	fi
fi
