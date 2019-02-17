!#/bin/sh

[ -e lab2_add.csv ] && rm lab2_add.csv
[ -e lab2_list.csv ] && rm lab2_list.csv

# Normal tests without yielding
counter=0
while [ $counter -lt 20 ]
do
    ./lab2_add --threads=1 --iterations=$((counter * 10)) >> lab2_add.csv
    ./lab2_add --threads=10 --iterations=$((counter)) >> lab2_add.csv
    ./lab2_add --threads=20 --iterations=$((counter * 5)) >> lab2_add.csv
    ./lab2_add --threads=10 --iterations=$((counter * 10)) >> lab2_add.csv
    ./lab2_add --threads=5 --iterations=100 >> lab2_add.csv
    ./lab2_add --threads=5 --iterations=1000 >> lab2_add.csv
    ./lab2_add --threads=5 --iterations=10000 >> lab2_add.csv
    ./lab2_add --threads=5 --iterations=100000 >> lab2_add.csv
    
    counter=$((counter+1));
done

# Tests with yielding
counter_yield=1
while [ $counter_yield -le 4 ]
do
    ./lab2_add --yield --threads=$((2**$counter_yield)) --iterations=10 >> lab2_add.csv
    ./lab2_add --yield --threads=$((2**$counter_yield)) --iterations=20 >> lab2_add.csv
    ./lab2_add --yield --threads=$((2**$counter_yield)) --iterations=40 >> lab2_add.csv
    ./lab2_add --yield --threads=$((2**$counter_yield)) --iterations=80 >> lab2_add.csv
    ./lab2_add --yield --threads=$((2**$counter_yield)) --iterations=100 >> lab2_add.csv
    ./lab2_add --yield --threads=$((2**$counter_yield)) --iterations=1000 >> lab2_add.csv
    ./lab2_add --yield --threads=$((2**$counter_yield)) --iterations=10000 >> lab2_add.csv
    ./lab2_add --yield --threads=$((2**$counter_yield)) --iterations=100000 >> lab2_add.csv

    # Extra tests for data plot 2, without the yields
    ./lab2_add --threads=$((2**$counter_yield)) --iterations=100 >> lab2_add.csv
    ./lab2_add --threads=$((2**$counter_yield)) --iterations=1000 >> lab2_add.csv
    ./lab2_add --threads=$((2**$counter_yield)) --iterations=10000 >> lab2_add.csv
    ./lab2_add --threads=$((2**$counter_yield)) --iterations=100000 >> lab2_add.csv

    counter_yield=$((counter_yield+1));

done

# Tests with locks
counter_locks=0
while [ $counter_locks -le 4 ]
do
    # Test that with yielding these threads still reliably work
    ./lab2_add --yield --threads=$((2**$counter_locks)) --sync=m --iterations=100 >> lab2_add.csv
    ./lab2_add --yield --threads=$((2**$counter_locks)) --sync=m --iterations=1000 >> lab2_add.csv
    ./lab2_add --yield --threads=$((2**$counter_locks)) --sync=m --iterations=10000 >> lab2_add.csv

    ./lab2_add --yield --threads=$((2**$counter_locks)) --sync=c --iterations=100 >> lab2_add.csv
    ./lab2_add --yield --threads=$((2**$counter_locks)) --sync=c --iterations=1000 >> lab2_add.csv
    ./lab2_add --yield --threads=$((2**$counter_locks)) --sync=c --iterations=10000 >> lab2_add.csv

    ./lab2_add --yield --threads=$((2**$counter_locks)) --sync=s --iterations=20 >> lab2_add.csv
    ./lab2_add --yield --threads=$((2**$counter_locks)) --sync=s --iterations=100 >> lab2_add.csv
    ./lab2_add --yield --threads=$((2**$counter_locks)) --sync=s --iterations=1000 >> lab2_add.csv

    ./lab2_add --yield --threads=$((2**$counter_locks)) --iterations=1000 >> lab2_add.csv
    ./lab2_add --yield --threads=$((2**$counter_locks)) --iterations=10000 >> lab2_add.csv


    # Test without yields for all locks and unprotected
    ./lab2_add --threads=$((2**$counter_locks)) --sync=m --iterations=1000 >> lab2_add.csv
    ./lab2_add --threads=$((2**$counter_locks)) --sync=m --iterations=10000 >> lab2_add.csv

    ./lab2_add --threads=$((2**$counter_locks)) --sync=c --iterations=1000 >> lab2_add.csv
    ./lab2_add --threads=$((2**$counter_locks)) --sync=c --iterations=10000 >> lab2_add.csv

    ./lab2_add --threads=$((2**$counter_locks)) --sync=s --iterations=1000 >> lab2_add.csv
    ./lab2_add --threads=$((2**$counter_locks)) --sync=s --iterations=10000 >> lab2_add.csv

    ./lab2_add --threads=$((2**$counter_locks)) --iterations=1000 >> lab2_add.csv
    ./lab2_add --threads=$((2**$counter_locks)) --iterations=10000 >> lab2_add.csv


    counter_locks=$((counter_locks+1));
done

# Tests with locks
counter_list=0
while [ $counter_list -le 4 ]
do
    ./lab2_list --threads=1 --iterations=10 >> lab2_list.csv
    ./lab2_list --threads=1 --iterations=100 >> lab2_list.csv
    ./lab2_list --threads=1 --iterations=1000 >> lab2_list.csv
    ./lab2_list --threads=1 --iterations=10000 >> lab2_list.csv
    ./lab2_list --threads=1 --iterations=20000 >> lab2_list.csv

    ./lab2_list --threads=$((2**$counter_list)) --iterations=1 >> lab2_list.csv
    ./lab2_list --threads=$((2**$counter_list)) --iterations=10 >> lab2_list.csv
    ./lab2_list --threads=$((2**$counter_list)) --iterations=100 >> lab2_list.csv
    ./lab2_list --threads=$((2**$counter_list)) --iterations=1000 >> lab2_list.csv
    ./lab2_list --threads=$((2**$counter_list)) --iterations=10000 >> lab2_list.csv

    counter_iterations=0
    while [ $counter_iterations -le 5 ]
    do
        ./lab2_list --yield=i --threads=$((2**$counter_list)) --iterations=$((2**$counter_iterations)) >> lab2_list.csv
        ./lab2_list --yield=id --threads=$((2**$counter_list)) --iterations=$((2**$counter_iterations)) >> lab2_list.csv
        ./lab2_list --yield=idl --threads=$((2**$counter_list)) --iterations=$((2**$counter_iterations)) >> lab2_list.csv
        ./lab2_list --yield=dl --threads=$((2**$counter_list)) --iterations=$((2**$counter_iterations)) >> lab2_list.csv
        ./lab2_list --yield=d --threads=$((2**$counter_list)) --iterations=$((2**$counter_iterations)) >> lab2_list.csv
        ./lab2_list --yield=l --threads=$((2**$counter_list)) --iterations=$((2**$counter_iterations)) >> lab2_list.csv
        ./lab2_list --yield=il --threads=$((2**$counter_list)) --iterations=$((2**$counter_iterations)) >> lab2_list.csv

        ./lab2_list --yield=i --sync=m --threads=$((2**$counter_list)) --iterations=$((2**$counter_iterations)) >> lab2_list.csv
        ./lab2_list --yield=id --sync=m --threads=$((2**$counter_list)) --iterations=$((2**$counter_iterations)) >> lab2_list.csv
        ./lab2_list --yield=idl --sync=m --threads=$((2**$counter_list)) --iterations=$((2**$counter_iterations)) >> lab2_list.csv
        ./lab2_list --yield=dl --sync=m --threads=$((2**$counter_list)) --iterations=$((2**$counter_iterations)) >> lab2_list.csv
        ./lab2_list --yield=d --sync=m --threads=$((2**$counter_list)) --iterations=$((2**$counter_iterations)) >> lab2_list.csv
        ./lab2_list --yield=l --sync=m --threads=$((2**$counter_list)) --iterations=$((2**$counter_iterations)) >> lab2_list.csv
        ./lab2_list --yield=il --sync=m --threads=$((2**$counter_list)) --iterations=$((2**$counter_iterations)) >> lab2_list.csv

        ./lab2_list --yield=i --sync=s --threads=$((2**$counter_list)) --iterations=$((2**$counter_iterations)) >> lab2_list.csv
        ./lab2_list --yield=id --sync=s --threads=$((2**$counter_list)) --iterations=$((2**$counter_iterations)) >> lab2_list.csv
        ./lab2_list --yield=idl --sync=s --threads=$((2**$counter_list)) --iterations=$((2**$counter_iterations)) >> lab2_list.csv
        ./lab2_list --yield=dl --sync=s --threads=$((2**$counter_list)) --iterations=$((2**$counter_iterations)) >> lab2_list.csv
        ./lab2_list --yield=d --sync=s --threads=$((2**$counter_list)) --iterations=$((2**$counter_iterations)) >> lab2_list.csv
        ./lab2_list --yield=l --sync=s --threads=$((2**$counter_list)) --iterations=$((2**$counter_iterations)) >> lab2_list.csv
        ./lab2_list --yield=il --sync=s --threads=$((2**$counter_list)) --iterations=$((2**$counter_iterations)) >> lab2_list.csv


        counter_iterations=$((counter_iterations+1));
    done

    # for plot 3
    ./lab2_list --yield=i --sync=m --threads=12 --iterations=32 >> lab2_list.csv
    ./lab2_list --yield=d --sync=m --threads=12 --iterations=32 >> lab2_list.csv
    ./lab2_list --yield=il --sync=m --threads=12 --iterations=32 >> lab2_list.csv
    ./lab2_list --yield=dl --sync=m --threads=12 --iterations=32 >> lab2_list.csv
    ./lab2_list --yield=i --sync=s --threads=12 --iterations=32 >> lab2_list.csv
    ./lab2_list --yield=d --sync=s --threads=12 --iterations=32 >> lab2_list.csv
    ./lab2_list --yield=il --sync=s --threads=12 --iterations=32 >> lab2_list.csv
    ./lab2_list --yield=dl --sync=s --threads=12 --iterations=32 >> lab2_list.csv

    # for plot 4
    ./lab2_list --sync=m --threads=$((2**$counter_list)) --iterations==1000 >> lab2_list.csv
    ./lab2_list --sync=m --threads=$((2**$counter_list)) --iterations=1000 >> lab2_list.csv
    ./lab2_list --sync=m --threads=$((2**$counter_list)) --iterations=1000 >> lab2_list.csv
    ./lab2_list --sync=m --threads=$((2**$counter_list)) --iterations=1000 >> lab2_list.csv
    ./lab2_list --sync=s --threads=$((2**$counter_list)) --iterations=1000 >> lab2_list.csv
    ./lab2_list --sync=s --threads=$((2**$counter_list)) --iterations=1000 >> lab2_list.csv
    ./lab2_list --sync=s --threads=$((2**$counter_list)) --iterations=1000 >> lab2_list.csv
    ./lab2_list --sync=s --threads=$((2**$counter_list)) --iterations=1000 >> lab2_list.csv

    counter_list=$((counter_list+1));
done
