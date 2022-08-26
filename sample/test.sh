cru hello.cru
lli hello.ll

cru -C hello.cru
clang -o hello hello.c
./hello

cru -P hello.cru
python3 hello.py

echo ====== $status
echo hello.cru OK.

echo 
cru fibonacci.cru
lli fibonacci.ll

cru -C fibonacci.cru
clang -o fibonacci fibonacci.c
./fibonacci

cru -P fibonacci.cru
python3 fibonacci.py

echo ====== $status
echo fibonacci.cru OK.

echo 
cru recursive.cru
lli recursive.ll

cru -P recursive.cru
python3 recursive.py

cru -C recursive.cru
clang -o recursive recursive.c
./recursive

echo ====== $status
echo recursive.cru OK.
