REM C++ Build Batch File
@echo off

REM Dive into the directory to compile, then dive out
cd src\cpp\
cl /std:c++latest /Zi /EHsc /Fe: ..\..\bin\cpp\test.exe test.cpp
cd ..\..