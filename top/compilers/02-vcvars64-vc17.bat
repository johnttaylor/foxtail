@echo off
IF "/%1"=="/name" ECHO:Visual Studio VC17 (64bit) compiler for Windows & exit /b 0
::
call "C:\compilers\vcvars64-vc17.bat"


