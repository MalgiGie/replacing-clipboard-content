@echo off

gcc -o server.exe server.c -lws2_32
IF %ERRORLEVEL% NEQ 0 (
    echo server.c compilation failed.
    PAUSE
    EXIT /B %ERRORLEVEL%
)
echo server.c compilation was successful.

REM Kompilacja pliku client.c
gcc -o client.exe client.c -lws2_32
IF %ERRORLEVEL% NEQ 0 (
    echo client.c compilation failed.
    PAUSE
    EXIT /B %ERRORLEVEL%
)
echo client.c compilation was successful.

start cmd /c server.exe
IF %ERRORLEVEL% NEQ 0 (
    echo server.exe execution failed.
    PAUSE
    EXIT /B %ERRORLEVEL%
)
echo server.exe OK

timeout /t 3 /nobreak

start cmd /c client.exe localhost
IF %ERRORLEVEL% NEQ 0 (
    echo client.exe execution failed.
    PAUSE
    EXIT /B %ERRORLEVEL%
)
echo client.exe OK

start cmd /c client.exe localhost
IF %ERRORLEVEL% NEQ 0 (
    echo client.exe execution failed.
    PAUSE
    EXIT /B %ERRORLEVEL%
)
echo client.exe OK

PAUSE
