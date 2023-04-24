#!/bin/bash

make clean
make filter

file1="./test/output.txt"
file2="./test/output_ori.txt"
COUNT=0

./filter ./test/input.txt ./test/output$COUNT.txt
./2/filter ./test/input.txt ./test/output_ori$COUNT.txt
diff ./test/output$COUNT.txt ./test/output_ori$COUNT.txt
echo $COUNT
COUNT=1

./filter -c l ./test/input.txt ./test/output$COUNT.txt
./2/filter -c l ./test/input.txt ./test/output_ori$COUNT.txt
diff ./test/output$COUNT.txt ./test/output_ori$COUNT.txt
echo $COUNT
COUNT=2

./filter -c u ./test/input.txt ./test/output$COUNT.txt
./2/filter -c u ./test/input.txt ./test/output_ori$COUNT.txt
diff ./test/output$COUNT.txt ./test/output_ori$COUNT.txt
echo $COUNT
COUNT=3


./filter -e 24 ./test/input.txt ./test/output$COUNT.txt
./2/filter -e 24 ./test/input.txt ./test/output_ori$COUNT.txt
diff ./test/output$COUNT.txt ./test/output_ori$COUNT.txt
echo $COUNT
COUNT=4


./filter -e -23 ./test/input.txt ./test/output$COUNT.txt
./2/filter -e -23 ./test/input.txt ./test/output_ori$COUNT.txt
diff ./test/output$COUNT.txt ./test/output_ori$COUNT.txt
echo $COUNT
COUNT=5


./filter -e 0 ./test/input.txt ./test/output$COUNT.txt
./2/filter -e 0 ./test/input.txt ./test/output_ori$COUNT.txt
diff ./test/output$COUNT.txt ./test/output_ori$COUNT.txt
echo $COUNT
COUNT=6

./filter -s ./test/input.txt ./test/output$COUNT.txt
./2/filter -s ./test/input.txt ./test/output_ori$COUNT.txt
diff ./test/output$COUNT.txt ./test/output_ori$COUNT.txt
echo $COUNT
COUNT=7


./filter -s -e 24 ./test/input.txt ./test/output$COUNT.txt
./2/filter -s -e 24 ./test/input.txt ./test/output_ori$COUNT.txt
diff ./test/output$COUNT.txt ./test/output_ori$COUNT.txt
echo $COUNT
COUNT=8


./filter -s -e -24 ./test/input.txt ./test/output$COUNT.txt
./2/filter -s -e -24 ./test/input.txt ./test/output_ori$COUNT.txt
diff ./test/output$COUNT.txt ./test/output_ori$COUNT.txt
echo $COUNT
COUNT=9


./filter -c l -e -24 ./test/input.txt ./test/output$COUNT.txt
./2/filter -c l -e -24 ./test/input.txt ./test/output_ori$COUNT.txt
diff ./test/output$COUNT.txt ./test/output_ori$COUNT.txt
echo $COUNT
COUNT=10

./filter -c u -e -24 ./test/input.txt ./test/output$COUNT.txt
./2/filter -c u -e -24 ./test/input.txt ./test/output_ori$COUNT.txt
diff ./test/output$COUNT.txt ./test/output_ori$COUNT.txt
echo $COUNT
COUNT=11

./filter -c u -e -24 -s -e 13 ./test/input.txt ./test/output$COUNT.txt
./2/filter -c u -e -24 -s -e 13 ./test/input.txt ./test/output_ori$COUNT.txt
diff ./test/output$COUNT.txt ./test/output_ori$COUNT.txt
echo $COUNT
COUNT=12