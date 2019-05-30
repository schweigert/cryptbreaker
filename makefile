
all: clear main

main:
	gcc main.c -o main -lcrypt

clear:
	rm -rf main