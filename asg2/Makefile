CPP  		= g++ -g -O0 -Wall -Wextra -std=gnu++11
CPPHEADER   = bigint.h   scanner.h   debug.h   util.h   iterstack.h
CPPSOURCE   = bigint.cpp scanner.cpp debug.cpp util.cpp main.cpp
EXECBIN     = ydc
OBJECTS     = ${CPPSOURCE:.cpp=.o}
OTHERS      = Makefile README
ALLSOURCES  = ${CPPHEADER} ${CPPSOURCE} ${OTHERS}

all : ${EXECBIN}
	- checksource ${ALLSOURCES}

${EXECBIN} : ${OBJECTS}
	${CPP} -o $@ ${OBJECTS}

%.o : %.cpp
	${CPP} -c $<

submit:
	submit cmps109-wm.s15 asg2 ${ALLSOURCES}

OBJSNOMAIN  = $(filter-out main.o ,${OBJECTS})
TESTSRCS 	= test/test_*.h
TESTMAIN 	= test/test_main.cpp
TESTBIN 	= test/test_main

autotest:
	nodemon --exec "make test_" --ext cpp,h

test_: ${TESTBIN}
	- ${TESTBIN}

test: ${TESTBIN}
	${TESTBIN}

${TESTBIN}: ${TESTMAIN} ${TESTSRCS} ${OBJSNOMAIN}
	${CPP} -o ${TESTBIN} ${TESTMAIN} ${OBJSNOMAIN}

clean :
	-@ rm ${OBJECTS}\
		${EXECBIN}.dSYM ${TESTBIN}.dSYM\
		2> /dev/null

spotless : clean
	-@ rm ${EXECBIN} ${TESTBIN}\
		2> /dev/null

.PHONY: test autotest
