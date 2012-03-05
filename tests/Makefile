SRCDIR=../src/
OBJDIR=../build/

CC = clang
CPPFLAGS = 
CFLAGS = $(CPPFLAGS) -g -O2
LIBS = -lcunit
LDFLAGS =  $(LIBS)

OBJECTS=$(filter-out %/main.o,$(wildcard $(OBJDIR)/*.o))
TESTOBJECTS=$(patsubst %.c, %.o, $(wildcard *.c))

.PHONY: run_tests

run_tests: tests
	./tests

tests: $(TESTOBJECTS)

test_tempgov.o: test_tempgov.c ../src/tempgov.c
