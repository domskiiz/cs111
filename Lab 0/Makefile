#NAME: 	ALISSA NIEWIADOMSKI
#EMAIL: aniewiadomski@ucla.edu
#UID:	*********

default:
	gcc -o lab0 -g -Wall -Wextra lab0.c

check:
	./testScripts.sh

clean:
	rm -f lab0 lab0-*********.tar.gz
	find ./ -name "*.txt" -not -name "input.txt" -exec rm {} \;

dist:
	tar -cvzf lab0-*********.tar.gz lab0.c README Makefile \
	 	 backtrace.png breakpoint.png testScripts.sh input.txt
