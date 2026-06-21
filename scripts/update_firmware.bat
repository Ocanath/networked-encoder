@echo off
setlocal

if "%~1"=="" (
    echo Usage: %~nx0 ^<address^> 1>&2
    exit /b 1
)

set ADDR=%~1

encoder-cli "%ADDR%" --bootload
if errorlevel 1 exit /b %errorlevel%

dartt_flash "%ADDR%" ..\embedded\encoder-rs485\ReleaseBootloader\encoder-rs485.bin
if errorlevel 1 exit /b %errorlevel%

dartt_flash "%ADDR%" --start
if errorlevel 1 exit /b %errorlevel%
