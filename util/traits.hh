#include <type_traits>
#include <iterator>

#include "cxx14.hh"
#include "util/common.hh"

#ifndef _SCRATCH_UTIL_TRAITS
#define _SCRATCH_UTIL_TRAITS

namespace util
{
    namespace traits
    {
        /** ∀ T ∈ Ts where Pred<T> */
        template <template <class> class, class...>
        struct all_satisfy;

        /** ∃ T ∈ Ts where Pred<T> */
        template <template <class> class, class...>
        struct any_satisfy;

        /// Check membership in Ts
        template <class, class...>
        struct is_member;
        
        template <class Pred>
        using neg = std::integral_constant<bool, !Pred::value>;
        
        template <class>
        struct is_iterable;
    }

    using namespace traits;
}

#endif
