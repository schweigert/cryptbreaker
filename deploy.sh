cp cryptbreaker ~/cryptbreaker.run

for i in `cat ip_list_to_copy`; do
    scp cryptbreaker.run $i:~/;
done

cp imput ~/imput
mpirun --mca -machinefile ip_list_to_run ~/cryptbreaker.run