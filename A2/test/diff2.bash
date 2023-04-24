make clean
make hotpotato

file1="./test/q2output_mine.txt"
file2="./test/q2output_ori.txt"
COUNT="========================finish case======================="

./hotpotato 3 6 1005 > $file1
./2/hotpotato 3 6 1005 > $file2
diff $file1 $file2 
echo $COUNT

./hotpotato 100 30 23245 > $file1
./2/hotpotato 100 30 23245 > $file2
diff $file1 $file2
echo $COUNT

./hotpotato d 30 23245 > $file1
./2/hotpotato d 30 23245 > $file2
diff $file1 $file2
echo $COUNT


./hotpotato 5 d 23245 > $file1
./2/hotpotato 5 d 23245 > $file2
diff $file1 $file2
echo $COUNT

./hotpotato d d 23245 > $file1
./2/hotpotato d d 23245 > $file2
diff $file1 $file2
echo $COUNT

./hotpotato 1000 100 2423423 > $file1
./2/hotpotato 1000 100 2423423 > $file2
diff $file1 $file2
echo $COUNT