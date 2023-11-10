#!/bin/bash 
# make-arduino.sh

read -p "Enter version: " version

if [ -z "$version" ]
then
    echo "Error please add a version"
else
    echo "Make arduino ide version $version"

    projectName="heat"
    # can replace strings in your project like passwords or settings, this is an array can add more
    stringReplacements=("s/string to be replaced/replacement string/g")

    # makes a folder called build for the results of this script
    mkdir build

    # main program
    mainName="$projectName-$version"
    mkdir build/$mainName
    # copying your custom libraries to the output folder
    cp ./lib/**/* build/$mainName
    # copying the source files
    cp ./src/* build/$mainName
    # rename the main.cpp to ino so it opens in arduino ide
    mv build/$mainName/main.cpp build/$mainName/$mainName.ino
    # replacing any strings like passwords and things
    for ((i = 0; i < ${#stringReplacements[@]}; i++))
    do
        sed -i '' "${stringReplacements[$i]}" build/$mainName/*.cpp build/$mainName/*.h build/$mainName/*.ino
    done

    # copy the libraries from pio folder to a libs folder (paste into the /Arduino/libraries folder)
    mkdir -p build/$mainName/libs
    cp -R ./.pio/libdeps/**/* build/$mainName/libs

    # rename if there are spaces 
    for i in build/$mainName/libs/*
    do
        mv "$i" ${i// /_}
    done

    # copying your flash data folder to the output folder
    mkdir -p build/$mainName/data
    cp -R ./data/* build/$mainName/data

    # zip it so you can send
    zip -r build/$mainName.zip build/$mainName
    echo "Made build/$mainName"
fi