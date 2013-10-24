// #include <tuple>
// #include <type_traits>
// #include <initializer_list>
// #include <iterator>

#include "cxx14.hh"
#include "sfinae.hh"
#include "common.hh"
#include "traits.hh"
#include "traits_impl.hh"
#include "mp.hh"
#include "io.hh"

#ifndef _SCRATCH_UTIL
#define _SCRATCH_UTIL

#ifdef BUILD_TEST

int fn1(int) { return 1; }
void fn2() {}

using namespace util;

// static_assert(false, "working?");
static_assert(any_satisfy<std::is_integral, int, float>::value, "");
static_assert(!any_satisfy<std::is_integral, double, float>::value, "");
    
static_assert(all_satisfy<std::is_integral, int, long, unsigned>::value, "");
static_assert(!all_satisfy<std::is_integral, int, float>::value, "");
static_assert(all_satisfy<std::is_function, decltype(fn1), decltype(fn2)>::value, "");

static_assert(is_member<int, long, char, int>::value, "");
static_assert(!is_member<int, double, long, float>::value, "");
    
static_assert(std::is_same<
              Filter<std::is_integral, std::tuple<int, float, long> >,
              std::tuple<int, long> >::value, "");

static_assert(std::is_same<
              Transform<std::add_pointer_t, std::tuple<int, float, char*> >,
              std::tuple<int*, float*, char**> >::value, "");


// void test_to_string()


#endif

#endif
