#!/bin/bash

if test -e "./BaselineSetValue"; then rm ./BaselineSetValue;fi

fname="./BaselineSetValue"


echo $fname
echo "Module 0 BaselineSetValue" > $fname
odbedit -c "ls \"Equipment/Li6_Detector/Settings/Module0/Baseline Threshold\" ">>$fname
echo "Module 1 BaselineSetValue" >> $fname
odbedit -c "ls \"Equipment/Li6_Detector/Settings/Module1/Baseline Threshold\" ">>$fname
