
all: clear build

build:
	mpicc cryptbreaker.c -o cryptbreaker.run -lcrypt -fopenmp -O2

clear:
	rm -rf cryptbreaker.run

run:
	mpirun --mca -machinefile ip_list_to_run cryptbreaker.run

deploy:
	sh deploy.sh
