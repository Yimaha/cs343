make clean
make vote VIMPL=BAR OUTPUT=NOOUTPUT
./vote 100 10 20000 1003
make vote VIMPL=SEM OUTPUT=NOOUTPUT
./vote 100 10 20000 1003
make vote VIMPL=MC OUTPUT=NOOUTPUT
./vote 100 10 20000 1003
make vote VIMPL=BAR OUTPUT=OUTPUT
./vote 8 3 2 1003 > ./test/out1.txt
make vote VIMPL=SEM OUTPUT=OUTPUT
./vote 8 3 2 1003 > ./test/out2.txt
make vote VIMPL=MC OUTPUT=OUTPUT
./vote 8 3 2 1003 > ./test/out3.txt
