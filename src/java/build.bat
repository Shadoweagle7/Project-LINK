REM Java Build Batch File

REM Ununsed Variant.java VariantTypeNotFoundException.java

@echo off

echo -- Java Build --

REM Dive into the directory to compile, then dive out
cd src\java\
echo | set /p temp=Java Compiler: 
javac --version
javac -Xlint:unchecked -Xdiags:verbose -d ..\..\bin\java\ Toolbox.java^
 FileToolbox.java PipeToolbox.java Test.java^
 Character.java Double.java Float.java Integer.java Long.java^
 LongLong.java Short.java UnsignedCharacter.java UnsignedInteger.java^
 UnsignedLong.java UnsignedLongLong.java UnsignedShort.java^
 WideCharacter.java ToolboxFileReadErrorException.java Primitive.java^
 Variable.java
cd ..\..\