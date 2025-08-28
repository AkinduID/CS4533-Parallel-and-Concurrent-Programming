Serial program
gcc -o serial serial.c 

Case 1: ./serial 1000 10000 0.99 0.005 
Case 2: ./serial 1000 10000 0.90 0.05 
Case 3: ./serial 1000 10000 0.50 0.25 

One mutex

RW Lock
gcc -pthread -o rwlock rwlock.c -lm

Case 1: ./rwlock 1000 10000 0.99 0.005 4