#!/bin/sh

if [ -e ./vsfs ]; then 
    ./fs rmfs
fi

./fs mkfs

rm ./plikiwsadowe/*.txt

cp ./plikitestowe/*.txt ./plikiwsadowe

    ./fs fsinfo
    echo "\n"

    mv ./plikiwsadowe/*.txt ./

    ./fs cp a.txt
    ./fs cp b.txt
    ./fs cp c.txt
    ./fs cp d.txt
    ./fs cp e.txt
    ./fs cp f.txt
    ./fs cp g.txt

    rm *.txt

    ./fs fsinfo
    echo "\n"

    ./fs load a.txt
    ./fs load b.txt
    ./fs load c.txt
    ./fs load d.txt
    ./fs load e.txt
    ./fs load f.txt
    ./fs load g.txt

    ./fs rm a.txt
    ./fs rm b.txt
    ./fs rm c.txt
    ./fs rm d.txt
    ./fs rm e.txt
    ./fs rm f.txt
    ./fs rm g.txt

    ./fs fsinfo
    echo "\n"

    mv ./*.txt ./plikiwynikowe
    mv ./plikiwynikowe/*.txt ./plikiwsadowe