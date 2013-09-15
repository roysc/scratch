
#ifndef _SCRATCH_UTIL_COMMON
#define _SCRATCH_UTIL_COMMON

namespace util
{
    struct swallow { swallow(std::initializer_list<int>) {} };


    template <class T, class... Args>
    auto make(Args&&... args)
        -> decltype(T {std::forward<Args>(args)...})
    { return T {std::forward<Args>(args)...}; }
}

#endif
