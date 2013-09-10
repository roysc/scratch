CC		= g++

# OPT     = -O3
CFLAGS	= -g -std=c++11 $(OPT) -w -D_DEBUG
LFLAGS  = 
OBJ_FILES = common.o sfinae.o util.o entity.o component.o entity_space.o

BIN = test

all: $(BIN)

%.o: %.cc %.hh
	$(CC) -c $< -o $@

# test: $(OBJ_FILES) test.cc
test: $(OBJ_FILES:.o=.hh) test.cc
	$(CC) $^ -o $@ $(CFLAGS) $(LFLAGS)

test-%: %.hh
	$(CC) $^ -o $@ $(CFLAGS) $(LFLAGS) -D_BUILD_TEST

clean:
	rm $(BIN) $(OBJ_FILES) *.gch
