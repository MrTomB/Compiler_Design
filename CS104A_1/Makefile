#Thomas Burch
#tburch1@ucsc.edu
#CMPS104A-ASG1

GCC   = g++ -g -O0 -Wall -Wextra -std=gnu++11
GRIND = valgrind --leak-check=full --show-reachable=yes
VALGRIND   = valgrind --leak-check=full --show-reachable=yes

MKFILE = Makefile
PROJECT = cmps104a-wm.f15 asg1
CSOURCE = oc.cpp stringset.cpp auxlib.cpp
CHEADER = stringset.h auxlib.h
SUBMITS = ${CHEADER} ${CSOURCE} ${MKFILE} README

all: oc

oc: oc.o stringset.o auxlib.o
	${GCC} oc.o stringset.o auxlib.o -o oc

%.o: %.cpp
	${GCC} -c $<

ci:
	git add -A :/
	git commit -m "$m"

clean:
	-rm stringset.o oc.o auxlib.o

spotless: clean
	-rm oc test.out test.err *.str

test: oc
	${GRIND} oc * * * >test.out 2>test.err

check:
	checksource ${SUBMITS}

# Depencencies.
oc.o: oc.cpp auxlib.h stringset.h
stringset.o: stringset.cpp stringset.h
auxlib.o: auxlib.cpp auxlib.h
