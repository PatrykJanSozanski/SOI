#!/bin/sh

if [ -e ./vsfs ]; then 
    ./fs rmfs
fi

./fs mkfs

x=0;
while [ $x -le 56 ]; do
    touch $x.txt
    echo "12345678" > $x.txt
    ./fs cp $x.txt
    x=$((x+1))
done

rm *.txt
