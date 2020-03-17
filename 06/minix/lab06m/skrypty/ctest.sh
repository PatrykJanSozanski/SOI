#!/bin/sh

if [ -f vsfs ]; then
   rm ./vsfs
fi

./fs mkfs; 
if test $? -ne 0; then
    exit
fi

./fs rmfs
if test $? -ne 0; then
    exit
fi
