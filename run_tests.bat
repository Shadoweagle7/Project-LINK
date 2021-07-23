@echo off

REM C++
@echo C++ Test:
bin\cpp\test.exe

REM Java
@echo Java Test:
cd bin\java\
java SE7.Test ..\..\test.SE7
cd ..\..