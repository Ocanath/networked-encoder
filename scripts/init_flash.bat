@echo off
setlocal

if "%~1"=="" (
    echo Usage: %~nx0 ^<address^> 1>&2
    exit /b 1
)

set ADDR=%~1

dartt_flash 255 ..\embedded\encoder-rs485\ReleaseBootloader\encoder-rs485.bin
if errorlevel 1 exit /b %errorlevel%

dartt_flash 255 --enable-autoboot
if errorlevel 1 exit /b %errorlevel%

dartt_flash 255 --start
if errorlevel 1 exit /b %errorlevel%

encoder-cli 0 --set-address "%ADDR%" --current-bootloader-address 255
if errorlevel 1 exit /b %errorlevel%
