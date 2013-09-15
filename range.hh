
#include "range/common.hh"
#include "range/traits.hh"
#include "range/adapt.hh"
#include "range/filter.hh"

#ifndef _SCRATCH_RANGE
#define _SCRATCH_RANGE

namespace range
{
    
}


namespace std
{
    template <class R,
              class = std::enable_if_t<range::is_range<R>::value> >
    range::RangeIterator<R> begin(const R& r)
    { return range::begin(r); }

    template <class R,
              class = std::enable_if_t<range::is_range<R>::value> >
    range::RangeIterator<R> end(const R& r)
    { return range::end(r); }
}



#ifdef _BUILD_TEST

using namespace range;

struct P { bool operator()(int x) { return x > 0; } };
static_assert(is_forward_range<FilterRange<P, AsRange<int*> > >::value, "");

#endif

#endif
