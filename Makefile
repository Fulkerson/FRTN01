

LIB_PATH=$(PWD)/lib
BIN_PATH=$(PWD)/bin
INCLUDE_PATH=$(PWD)/include
export LIB_PATH
export BIN_PATH
export INCLUDE_PATH

# Add your sub dirs here
SUB_DIRS = control client

all:
	mkdir -p $(LIB_PATH)
	mkdir -p $(BIN_PATH)
	mkdir -p $(INCLUDE_PATH)
	for DIR in $(SUB_DIRS); do \
		cd $(PWD)/$$DIR && make && make install; \
	done

clean:
	for DIR in $(SUB_DIRS); do \
		cd $(PWD)/$$DIR && make clean && make uninstall; \
	done
