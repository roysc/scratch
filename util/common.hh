/**
 * \brief Basic utilities
 */

#ifndef SCRATCH_UTIL_COMMON_HH
#define SCRATCH_UTIL_COMMON_HH

namespace util
{
struct swallow { swallow(std::initializer_list<int>) {} };


template <class T, class... Args>
auto make(Args&&... args)
  -> decltype(T{std::forward<Args>(args)...})
{ return T{std::forward<Args>(args)...}; }
}

#endif
