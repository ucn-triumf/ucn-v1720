#!/bin/bash
echo "GetDCOffsets [log]"
echo "Optinal argument log to save output to ../dcoffsets/DCOffsets_date"


fname="tmp"
if [ $1=="log" ]
then
fname="../dcoffsets/DCOffsets_`date +%Y%m%d_%H%M`"
fi

echo $fname

echo "Module 0 BaselineSetValue" > $fname
odbedit -c "ls \"Equipment/Li6_Detector/Settings/Module0/Baseline Threshold\" ">>$fname
echo "Module 1 BaselineSetValue" >> $fname
odbedit -c "ls \"Equipment/Li6_Detector/Settings/Module1/Baseline Threshold\" ">>$fname
echo "Module 0 DC-Offsets" >> $fname
odbedit -c "ls \"Equipment/Li6_Detector/Settings/Module0/DC Offset\" ">>$fname
echo "Module 1 DC-Offsets" >> $fname
odbedit -c "ls \"Equipment/Li6_Detector/Settings/Module1/DC Offset\" ">>$fname

cat $fname
if [ $1=="log" ]
  then
    echo "Done"
  else
    rm $fname
fi
