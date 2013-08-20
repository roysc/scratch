CC		= g++

# OPTIMIZE     = -O3
CFLAGS	= -std=c++11 $(OPTIMIZE)
LFLAGS  = 

BIN = test

all: $(BIN)

test: util.hh component.hh component_system.hh test.cc 
	$(CC) $^ -o $@ $(CFLAGS) $(LFLAGS)

test-%: %.hh
	$(CC) $^ -o $@ $(CFLAGS) $(LFLAGS) -D_BUILD_TEST

clean:
	rm $(BIN) *.o *.gch
