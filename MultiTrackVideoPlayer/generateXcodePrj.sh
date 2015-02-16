#!/bin/sh

DIR="xcode-build"

cd ..
if [ -d "$DIR" ]; then
  rm -rf $DIR
fi
mkdir $DIR && cd $DIR && qmake -spec macx-xcode ../MultiTrackVideoPlayer/MultiTrackVideoPlayer.pro
open MultiTrackVideoPlayer.xcodeproj
