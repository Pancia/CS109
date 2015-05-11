CPP  		= g++ -g -O0 -Wall -Wextra -std=gnu++11
CPPHEADER   = listmap.h trace.h util.h xless.h xpair.h
TEMPLATES   = util.tcc
CPPSOURCE   = trace.cpp util.cpp main.cpp
ALLCPPSRC   = ${CPPHEADER} ${TEMPLATES} ${CPPSOURCE}
OBJECTS     = ${CPPSOURCE:.cpp=.o}
EXECBIN     = keyvalue
OTHERS      = Makefile README
ALLSOURCES  = ${ALLCPPSRC} ${OTHERS}

all: ${EXECBIN}

${EXECBIN}: ${OBJECTS}
	${CPP} -o $@ ${OBJECTS}

%.o: %.cpp
	${CPP} -c $<

submit:
	submit cmps109-wm.s15 asg3 ${ALLSOURCES}

# - TESTING
OBJSNOMAIN 		= $(filter-out main.o ,${OBJECTS})
TESTSRCS		= test/test_*.h
TESTMAIN		= test/test_main.cpp
TESTBIN			= test/test_main

autotest:
	nodemon --exec "make test" --ext cpp,h,tcc

test: all test-units test-blackbox

test-units: ${TESTBIN}
	chmod u+x ${TESTBIN}
	${TESTBIN}

${TESTBIN}: ${TESTMAIN} ${TESTSRCS} ${OBJSNOMAIN}
	${CPP} -o ${TESTBIN} ${TESTMAIN} ${OBJSNOMAIN}

test-blackbox: ${EXECBIN}
	- cd test && ./run_bb_tests

# - CLEANING
clean:
	-@ rm ${OBJECTS} 2> /dev/null

spotless: clean
	-@ rm ${EXECBIN} ${TESTBIN}\
		${EXECBIN}.dSYM ${TESTBIN}.dSYM\
		2> /dev/null

.PHONY: test test-units test-blackbox autotest submit
