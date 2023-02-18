#!/bin/bash

PROJECT="test"
FRAMERATE=30

if [ $# -eq 1 ]
  then
    PROJECT=$1
fi

if [ $# -eq 2 ]
  then
    PROJECT=$1
    FRAMERATE=$2
fi



echo -e "\nGenerating video file for project: $PROJECT\n"
cd capture_$PROJECT
ffmpeg -r $FRAMERATE -i "fbo_%d.jpg" -pix_fmt yuv420p -movflags +faststart video.mp4 -y 
