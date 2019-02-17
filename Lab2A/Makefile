# ALISSA NIEWIADOMSKI
# aniewiadomski@ucla.edu
# *********
.PHONY: tests graphs

build:
	gcc -pthread -lrt -o lab2_add -g -Wall -Wextra lab2_add.c
	gcc -pthread -lrt -o lab2_list -g -Wall -Wextra lab2_list.c SortedList.c
tests:
	make build
	./test.sh

graphs: 
	make tests
	gnuplot lab2_add.gp
	gnuplot lab2_list.gp
clean:
	rm -f lab2_add lab2_list *.tar.gz

dist:
	make graphs
	tar -czvf lab2a-*********.tar.gz lab2_add.csv lab2_list.csv lab2_add.c lab2_list.c SortedList.h SortedList.c Makefile test.sh lab2_add-1.png lab2_add-2.png lab2_add-3.png lab2_add-4.png lab2_add-5.png lab2_list-1.png lab2_list-2.png lab2_list-3.png lab2_list-4.png README lab2_add.gp lab2_list.gp