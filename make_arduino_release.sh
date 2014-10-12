#! /bin/bash -e

rm -r -f jdksavdeccmcu-arduino
mkdir jdksavdeccmcu-arduino
cp include/*.h* jdksavdeccmcu-arduino/
cp jdksavdecc-c/include/*.h* jdksavdeccmcu-arduino/
cp src/*.c* jdksavdeccmcu-arduino/
cp jdksavdecc-c/src/*.c* jdksavdeccmcu-arduino/

