#!/bin/bash

echo "Killing anything on port 8000"
kill -2 `lsof -i :8000 | tail -n +2 | sed -e 's,[ \t][ \t]*, ,g' | cut -f2 -d' '`

echo "Killing running soccerbot if exists"
taskkill /F /IM soccerbot.exe
taskkill /F /IM soccerbotd.exe

echo "Switching to project directory"
cd C:/soccerbot

echo "Updating GIT repository"
# git reset --hard HEAD
git pull

echo "Building release"
call "C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\vcvarsall.bat"
msbuild soccerbot.sln /p:Configuration=Release

echo "Running the application"
soccerbot.exe > C:/soccerbot/log.txt 2> C:/soccerbot/error.txt &

echo -e "\e[00;32m-- soccerbot started, run kill.sh to stop, logs in log.txt and error.txt --\e[00m"

exit 0
