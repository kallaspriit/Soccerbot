#!/bin/bash

killall soccerbotd
killall soccerbot
cd /home/rx/projects/soccerbot
# git reset --hard HEAD
git pull
make debug
./soccerbotd &
exit 0
