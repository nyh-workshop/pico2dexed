#!/bin/bash

echo "#include <Arduino.h>" > synth_dexed.h
echo "#include <Audio.h>" >> synth_dexed.h
echo "#include \"config.h\"" >> synth_dexed.h
for i in `cat includes.txt`
do
	echo "/*****************************************************" >> synth_dexed.h
	echo " * CODE; ${i}" >> synth_dexed.h
	echo " *****************************************************/" >> synth_dexed.h

	cat $i >> synth_dexed.h
	echo "" >> synth_dexed.h
	echo "//=====================================================" >> synth_dexed.h
done

echo "#include \"synth_dexed.h\"" > synth_dexed.cpp
for i in `ls orig_code/*.cpp`
do
	echo "/*****************************************************" >> synth_dexed.cpp
	echo " * CODE; ${i}" >> synth_dexed.cpp
	echo " *****************************************************/" >> synth_dexed.cpp
	cat $i >> synth_dexed.cpp
	echo "" >> synth_dexed.cpp
	echo "//=====================================================" >> synth_dexed.cpp
done
