#!/bin/bash

#killall soccerbotd
#killall soccerbot
cd /home/rx/projects/soccerbot
# git reset --hard HEAD
git pull
make release
./soccerbot > /home/rx/projects/soccerbot/log.txt 2> /home/rx/projects/soccerbot/error.txt &

echo -e "\e[00;32m-- soccerbot started, run kill.sh to stop, logs in log.txt and error.txt --\e[00m"

exit 0
