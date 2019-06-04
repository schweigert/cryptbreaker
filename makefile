
all: clear main

main:
	mpicc main.c boom_file_read.c -o main -lcrypt

clear:
	rm -rf main

run:
	mpirun --mca -n 2 -machinefile ip_list main 2 2