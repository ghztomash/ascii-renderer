#!/bin/bash

ADDONS="../../../../addons"
LIBS="../../../../libs"
PROJECT="waveforms"

echo -e "\nGenerating lua bindings for $PROJECT\n"
swig -v -Wall -c++ -lua -fcompact -fvirtual -I$LIBS/openFrameworks -I$ADDONS/ofxWaveforms/src WaveformsBindings.i
swig -c++ -lua -fcompact -fvirtual -I$LIBS/openFrameworks -I$ADDONS/ofxWaveforms/src -debug-lsymbols WaveformsBindings.i > ${PROJECT}_symbols.txt
$ADDONS/ofxLua/scripts/lua_syntax.py $PROJECT ${PROJECT}_symbols.txt
rm -v ${PROJECT}_symbols.txt
