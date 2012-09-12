#!/bin/bash

killall soccerbotd
killall soccerbot
cd /home/rx/projects/soccerbot
# git reset --hard HEAD
git pull
make release
./soccerbot &
exit 0
