#    Thomas Burch (1314305)
#    tburch1@ucsc.edu
#    CMPS104A-ASG3


MKFILE    = Makefile
DEPSFILE  = ${MKFILE}.deps
NOINCLUDE = ci clean spotless
NEEDINCL  = ${filter ${NOINCLUDE}, ${MAKECMDGOALS}}
VALGRIND  = valgrind --leak-check=full --show-reachable=yes
PROJECT   = cmps104a-wm.s15 asg2

#
# Definitions of list of files:
#
HSOURCES  = auxlib.h stringset.h lyutils.h astree.h
CSOURCES  = auxlib.cpp main.cpp stringset.cpp lyutils.cpp astree.cpp
LSOURCES  = scanner.l
YSOURCES  = parser.y
ETCSRC    = README PARTNER ${MKFILE}
CLGEN     = yylex.cc
HYGEN     = yyparse.h
CYGEN     = yyparse.cc
CGENS     = ${CLGEN} ${CYGEN}
ALLGENS   = ${HYGEN} ${CGENS}
EXECBIN   = oc
ALLCSRC   = ${CSOURCES} ${CGENS}
OBJECTS   = ${CSOURCES:.cpp=.o} ${CYGEN:.cc=.o}
LREPORT   = yylex.output
YREPORT   = yyparse.output
IREPORT   = ident.output
REPORTS   = ${LREPORT} ${YREPORT} ${IREPORT}
ALLSRC    = ${ETCSRC} ${YSOURCES} ${LSOURCES} ${HSOURCES} ${CSOURCES}
TESTINS   = ${wildcard test?.in}
LISTSRC   = ${ALLSRC} ${HYGEN}

#
# Definitions of the compiler and compilation options:
#
GCC       = g++ -g -O0 -Wall -Wextra -std=gnu++11
MKDEPS    = g++ -MM -std=gnu++11

#
# The first target is always ``all'', and hence the default,
# and builds the executable images
#
all: ${EXECBIN}

#
# Build the executable image from the object files.
#
${EXECBIN}: ${OBJECTS} ${CLGEN} ${CYGEN} ${HYGEN}
	${GCC} -o${EXECBIN} ${OBJECTS}
	ident ${OBJECTS} ${EXECBIN} >${IREPORT}


%.o: %.cpp
	${GCC} -c $<


#
# Build the scanner.
#
${CLGEN}: ${LSOURCES}
	flex --outfile=${CLGEN} ${LSOURCES} 2>${LREPORT}
	- grep -v '^  ' ${LREPORT}

#
# Build the parser.
#
${CYGEN} ${HYGEN}: ${YSOURCES}
	bison --defines=${HYGEN} --output=${CYGEN} ${YSOURCES}

#
# Check directory into git repository.
#
ci:
	git add ${ALLSRC} ${TESTINS}
	git commit -m "$m"

#
# Clean and spotless remove generated files.
#
clean:
	- rm -f ${OBJECTS} ${ALLGENS} ${REPORTS} ${DEPSFILE} core

spotless: clean
	- rm -f ${EXECBIN}


#
# Build the dependencies file using the C preprocessor
#
deps: ${ALLCSRC}
	@ echo "# ${DEPSFILE} created `date` by ${MAKE}" >${DEPSFILE}
	${MKDEPS} ${ALLCSRC} >>${DEPSFILE}

${DEPSFILE}:
	@ touch ${DEPSFILE}
	${MAKE} --no-print-directory deps

#
# TEST
#
tests: ${EXECBIN}
	touch ${TESTINS}
	make --no-print-directory ${TESTINS:.in=.out}

%.out %.err: %.in ${EXECBIN}
	( ${VALGRIND} ${EXECBIN} -ly -@@ $< \
	;  echo EXIT STATUS $$? 1>&2 \
	) 1>$*.out 2>$*.err

#
# Runs checksource on files to submit
#
check:
	checksource ${ALLSRC}

#
# Submits project files into submit directory
#
submit: ${ALLSRC}
	submit ${PROJECT} ${ALLSRC}

#
# Everything
#
again:
	gmake --no-print-directory spotless deps all lis

ifeq "${NEEDINCL}" ""
include ${DEPSFILE}
endif
