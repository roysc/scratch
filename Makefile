CC		= g++

# OPT     = -O3
INCLUDE = -I./
CFLAGS	= -g -std=c++11 $(OPT) -w -D_DEBUG $(INCLUDE)
LFLAGS  = 
# OBJ_FILES = 
HEADERS = entity.hh component.hh entity_space.hh system.hh

BIN = test

all: $(BIN)

%.o: %.cc %.hh
	$(CC) -c $< -o $@

# test: $(OBJ_FILES:.o=.hh) test.cc
test: $(HEADERS) test.cc
	$(CC) $^ -o $@ $(CFLAGS) $(LFLAGS)


test-%: %.hh
	$(CC) $^ -o $@ $(CFLAGS) $(LFLAGS) -D_BUILD_TEST

clean:
	rm $(BIN) $(OBJ_FILES) *.gch
