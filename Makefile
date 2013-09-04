CC		= g++

# OPTIMIZE     = -O3
CFLAGS	= -std=c++11 $(OPTIMIZE) -w
LFLAGS  = 

BIN = test

all: $(BIN)

test: util.hh entity.hh component.hh component_space.hh test.cc 
	$(CC) $^ -o $@ $(CFLAGS) $(LFLAGS)

test-%: %.hh
	$(CC) $^ -o $@ $(CFLAGS) $(LFLAGS) -D_BUILD_TEST

clean:
	rm $(BIN) *.o *.gch
