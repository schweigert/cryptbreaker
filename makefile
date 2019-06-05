
all: clear main

main:
	mpicc main.c -o main -lcrypt -O3

clear:
	rm -rf main

run:
	mpirun --mca -n 2 -machinefile ip_list main 2 2