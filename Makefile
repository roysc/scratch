CC		= g++

# OPT     = -O3
INCLUDE = -I./
CFLAGS	= -g -std=c++11 $(OPT) -w -D_DEBUG $(INCLUDE)
LFLAGS  = 
# OBJ_FILES = 
HEADERS = \
	common.hh sfinae.hh range.hh util.hh \
	entity.hh component.hh entity_space.hh system.hh

BIN = test

all: $(BIN)

%.o: %.cc %.hh
	$(CC) -c $< -o $@

# test: $(OBJ_FILES:.o=.hh) test.cc
test: $(HEADERS) test.cc
	$(CC) $^ -o $@ $(CFLAGS) $(LFLAGS)

# util.hh: $(shell find util -name '*.hh')
util.hh: util/traits_impl.hh util/common.hh util/mp.hh util/traits.hh \
	 util/io.hh


# range.hh: $(shell find range -name '*.hh')
range.hh: range/common.hh range/filter.hh range/traits.hh range/adapt.hh


test-%: %.hh
	$(CC) $^ -o $@ $(CFLAGS) $(LFLAGS) -D_BUILD_TEST

clean:
	rm $(BIN) $(OBJ_FILES) *.gch
