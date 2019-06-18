
all: clear build deploy

build:
	mpicc cryptbreaker.c -o cryptbreaker.run -lcrypt -fopenmp -O2

clear:
	rm -rf cryptbreaker.run

run:
	mpirun --mca -n 2 -machinefile ip_list cryptbreaker.run 2 2

deploy:
	cp cryptbreaker ~/cryptbreaker.run
	cp imput ~/imput
	mpirun --mca -n 2 -machinefile ip_list ~/cryptbreaker.run 2 2
