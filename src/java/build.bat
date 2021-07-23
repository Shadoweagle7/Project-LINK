REM Java Build Batch File
@echo off

echo -- Java Build --

REM Dive into the directory to compile, then dive out
cd src\java\
echo | set /p temp=Java Compiler: 
javac --version
javac -d ..\..\bin\java\ Toolbox.java Test.java
cd ..\..\