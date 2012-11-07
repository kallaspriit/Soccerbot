hostname > tmp
SET /p HOST= < tmp
DEL tmp

if "%HOST%" NEQ "zbox" call "C:\soccerbot\push.bat"