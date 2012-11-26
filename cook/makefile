CC	= gcc

CFLAGS	= -Wall -std=c99 -O2 -pedantic

FILE	= cook.c

TEST_FILE = term_test.c

TEST_OUT = term_test

all: 	
	$(CC) -c $(CFLAGS) $(FILE)
test:
	$(CC) -o term_test $(CFLAGS) $(FILE) $(TEST_FILE)
clean: 
	rm -rf  *.o
	rm -rf term_test
