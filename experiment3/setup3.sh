#!/bin/bash

directory=$1

# this is the part doing the hashing 
#cmake ../vvpkg
#cd ../vvpkg 
#make 
#cd ..
mkdir -p ../tmp/$directory
for d in ../data/$directory/* ; do
    filename=$(basename $d)
    echo "writing to $filename.bk"
    ../vvpkg/demo/dump_blocks $d > ../tmp/$directory/$filename.bk
done

echo "../result/$directory"
#../cdmtvsmerkle/main $directory

#python3 graph3.py $directory
