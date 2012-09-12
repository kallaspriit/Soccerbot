#!/bin/bash

cd /home/rx/projects/soccerbot
# git reset --hard HEAD
git pull
make debug
./soccerbotd
