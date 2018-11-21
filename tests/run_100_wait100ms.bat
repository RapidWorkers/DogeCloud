@echo off
setlocal
:LOOP
echo Starting 100 Client
start DogeCloud
ping -n 1 localhost
set /a ClientCount+=1
if %ClientCount% GEQ 100 goto QUIT
goto LOOP
:QUIT
