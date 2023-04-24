#!/bin/bash

make clean
make 
make exception
make throwcatch

./exception d d 1001 > "./sample/auto_1_1.txt"
./returnglobal d d 1001 > "./sample/auto_1_2.txt"
./returntype d d 1001 > "./sample/auto_1_3.txt"
./returntypec d d 1001 > "./sample/auto_1_4.txt"
./throwcatch d d 1001 d > "./sample/auto_2_1.txt"
./longjmp d d 1001 d > "./sample/auto_2_2.txt"
./sample/fmtcode < sample/q3input1.txt > sample/q3out1_ori.txt
./fmtcode < sample/q3input1.txt > sample/q3out1_mine.txt

file1="./sample/auto_1_1.txt"
file2="./sample/auto_1_2.txt"
file3="./sample/auto_1_3.txt"
file4="./sample/auto_1_4.txt"
file5="./sample/auto_2_1.txt"
file6="./sample/auto_2_2.txt"
file7="./sample/q3out1_ori.txt"
file8="./sample/q3out1_mine.txt"
file2="./out2.txt"

cmp -s "$file1" "$file2";
cmp -s "$file1" "$file3";
cmp -s "$file1" "$file4";
cmp -s "$file5" "$file6";
cmp -s "$file7" "$file8";

