#!/bin/bash

killall soccerbotd
killall soccerbot
cd /soccerbot
# git reset --hard HEAD
git pull
make debug
./soccerbotd > /soccerbot/log.txt 2> /soccerbot/error.txt &

echo -e "\e[00;32m-- soccerbotd started, run kill.sh to stop, logs in log.txt and error.txt --\e[00m"

exit 0
