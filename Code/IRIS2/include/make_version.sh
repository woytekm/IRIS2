#!/bin/sh

echo "// version and compile time information"
echo "// "
echo "   "
echo "#define IRIS_COMPILE_TIME \"`date +%D@%H:%M`\""
echo "#define IRIS_VER_MAJOR `cat ../version | awk '{print $1}'`"
echo "#define IRIS_VER_MINOR `cat ../version | awk '{print $2}'`"
echo " "


