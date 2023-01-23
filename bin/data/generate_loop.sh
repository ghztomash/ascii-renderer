#!/bin/bash

PROJECT="test"

if [ $# -eq 1 ]
  then
    PROJECT=$1
fi

echo -e "\nGenerating video file for project: $PROJECT\n"
cd capture_$PROJECT
ffmpeg -r 30 -i "fbo_%d.png" -pix_fmt yuv420p -movflags +faststart video.mp4 -y 
