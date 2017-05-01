#!/bin/bash

if [ $# -eq 0 ]
  then
    echo "Please provide a .mid.gz file name for processing"
    exit
fi


./UCNAnalyzer.exe $1 > $1.log

infile=`ls -1tr output*.root |tail -n1`
echo $infile


