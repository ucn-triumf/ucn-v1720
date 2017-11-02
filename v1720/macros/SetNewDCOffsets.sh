#!/bin/bash

chmod +x GetNewDCOffsetsMod1.sh
./GetNewDCOffsetsMod1.sh /home/ucn/Andrew/ucn-v1720/UCNAnalyzer/DCOffsetsNeededMod1.txt
echo "module 0 set"

chmod +x GetNewDCOffsetsMod2.sh
./GetNewDCOffsetsMod2.sh /home/ucn/Andrew/ucn-v1720/UCNAnalyzer/DCOffsetsNeededMod2.txt
echo "module 1 set"
