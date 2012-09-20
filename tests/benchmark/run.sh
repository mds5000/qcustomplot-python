qmake -project "QT += testlib"
qmake
make 
./benchmark -silent  -minimumvalue 100
make clean
rm benchmark Makefile benchmark.pro benchmark.pro.user
