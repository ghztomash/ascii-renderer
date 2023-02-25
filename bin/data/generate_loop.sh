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
cd captures
cd $PROJECT
ffmpeg -r $FRAMERATE -i $PROJECT"_%d.jpg" -pix_fmt yuv420p -movflags +faststart $PROJECT.mp4 -y
