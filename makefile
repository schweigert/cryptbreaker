
all: clear build deploy

build:
	mpicc cryptbreaker.c -o cryptbreaker.run -lcrypt -fopenmp -O2

clear:
	rm -rf cryptbreaker.run

run:
	mpirun --mca -machinefile ip_list_to_run cryptbreaker.run 2 2

deploy:
	cp cryptbreaker ~/cryptbreaker.run
	for i in `cat ip_list_to_copy`; do scp cryptbreaker.run $i:~/; done
	cp imput ~/imput
	mpirun --mca -machinefile ip_list_to_run ~/cryptbreaker.run 2 2
