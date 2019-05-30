
all: clear main

main:
	mpicc main.c -o main -lcrypt

clear:
	rm -rf main

run:
	mpirun --mca btl_tcp_if_include wlp1s0 -n 25 -machinefile world.txt main 2 2