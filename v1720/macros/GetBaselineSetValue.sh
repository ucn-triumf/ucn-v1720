#!/bin/bash

if test -e "/home/midas/online/NeutronDaq/UCNAnalyzer/BaselineSetValue"; then rm /home/midas/online/NeutronDaq/UCNAnalyzer/BaselineSetValue;fi

fname="/home/midas/online/NeutronDaq/UCNAnalyzer/BaselineSetValue"


echo $fname
#echo "BaselineSetValue" > $fname

odbedit -c "ls \"Equipment/FEV1720I/Settings/Module0/Baseline Threshold\" ">>$fname