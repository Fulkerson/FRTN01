
CC=g++

LIB_NAME=control
OBJS= pid.o
HEAD= pid.h

all: $(OBJS)
	ar rcs lib$(LIB_NAME).a $+

install: all
	cp lib$(LIB_NAME).a $(LIB_PATH)
	mkdir -p $(INCLUDE_PATH)/$(LIB_NAME)
	cp $(HEAD) $(INCLUDE_PATH)/$(LIB_NAME)

uninstall:
	rm $(LIB_PATH)/lib$(LIB_NAME).a
	rm -r $(INCLUDE_PATH)/$(LIB_NAME)

test: all test.o 
	$(CC) -o test test.o -L. -l$(LIB_NAME)
	./test
	rm test

clean:
	rm *.o *.a

