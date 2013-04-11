ECHO ! POST-BUILD

set HOST=%COMPUTERNAME%

if "%HOST%" NEQ "ZBOX" call "%CD%\push.bat"