#!/bin/bash


#mapfile -s 0 -n 1 line1 <  /home/midas/online/NeutronDaq/UCNAnalyzer/DCOffsetsNeededMod1.txt
#mapfile -s 1 -n 1 line2 <  /home/midas/online/NeutronDaq/UCNAnalyzer/DCOffsetsNeededMod1.txt
#mapfile -s 2 -n 1 line3 <  /home/midas/online/NeutronDaq/UCNAnalyzer/DCOffsetsNeededMod1.txt
#mapfile -s 3 -n 1 line4 <  /home/midas/online/NeutronDaq/UCNAnalyzer/DCOffsetsNeededMod1.txt
#mapfile -s 4 -n 1 line5 <  /home/midas/online/NeutronDaq/UCNAnalyzer/DCOffsetsNeededMod1.txt
#mapfile -s 5 -n 1 line6 <  /home/midas/online/NeutronDaq/UCNAnalyzer/DCOffsetsNeededMod1.txt
#mapfile -s 6 -n 1 line7 <  /home/midas/online/NeutronDaq/UCNAnalyzer/DCOffsetsNeededMod1.txt
#mapfile -s 7 -n 1 line8 <  /home/midas/online/NeutronDaq/UCNAnalyzer/DCOffsetsNeededMod1.txt
i=0
while read -r line
do
odbedit -c "set \"Equipment/FEV1720I/Settings/Module0/DC Offset[$i]\" $line" 
    echo "$line"
i=$(($i+1))

done < "$1"