
#include "range/common.hh"
#include "range/traits.hh"
#include "range/adapt.hh"
#include "range/filter.hh"
#include "range/map.hh"

#ifndef _SCRATCH_RANGE
#define _SCRATCH_RANGE

namespace range
{
    
}


#ifdef _BUILD_TEST

using namespace range;

struct P { bool operator()(int x) { return x > 0; } };
static_assert(is_forward_range<FilterRange<P, AsRange<int*> > >::value, "");

#endif

#endif
