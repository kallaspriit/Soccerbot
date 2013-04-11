ECHO "Post-building"

set HOST=%COMPUTERNAME%

if "%HOST%" NEQ "ZBOX" call "%CD%\push.bat"