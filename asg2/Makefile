MKFILE      = Makefile
GMAKE       = ${MAKE} --no-print-directory

COMPILECPP  = g++ -g -O0 -Wall -Wextra -std=gnu++11
MAKEDEPCPP  = g++ -MM

CPPHEADER   = bigint.h   scanner.h   debug.h   util.h   iterstack.h
CPPSOURCE   = bigint.cpp scanner.cpp debug.cpp util.cpp main.cpp
EXECBIN     = ydc
OBJECTS     = ${CPPSOURCE:.cpp=.o}
OTHERS      = ${MKFILE} README
ALLSOURCES  = ${CPPHEADER} ${CPPSOURCE} ${OTHERS}

all : ${EXECBIN}
	- checksource ${ALLSOURCES}

${EXECBIN} : ${OBJECTS}
	${COMPILECPP} -o $@ ${OBJECTS}

%.o : %.cpp
	${COMPILECPP} -c $<

again :
	${GMAKE} spotless all

TESTBIN 	= test/test_main
TESTMAIN 	= test/test_main.cpp
TESTSRCS 	= test/test_*.h

test: ${TESTBIN}
	${TESTBIN}

${TESTBIN}: ${TESTMAIN} ${TESTSRCS} ${OBJECTS}
	${COMPILECPP} -o ${TESTBIN} ${TESTMAIN}\
		$(filter-out main.o ,${OBJECTS})

clean :
	-@ rm ${OBJECTS} ${EXECBIN}.dSYM ${TESTBIN}.dSYM\
		2> /dev/null

spotless : clean
	-@ rm ${EXECBIN} ${TESTBIN}\
		2> /dev/null

.PHONY: test
