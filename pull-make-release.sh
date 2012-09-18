#!/bin/bash

echo "Killing anything on port 8000"
kill -2 `lsof -i :8000 | tail -n +2 | sed -e 's,[ \t][ \t]*, ,g' | cut -f2 -d' '`

echo "Killing running soccerbot if exists"
#killall soccerbotd
#killall soccerbot

echo "Switching to project directory"
cd /home/rx/projects/soccerbot

echo "Updating GIT repository"
# git reset --hard HEAD
git pull

echo "Building release"
make release

echo "Running the application"
./soccerbot > /home/rx/projects/soccerbot/log.txt 2> /home/rx/projects/soccerbot/error.txt &

echo -e "\e[00;32m-- soccerbot started, run kill.sh to stop, logs in log.txt and error.txt --\e[00m"

exit 0
