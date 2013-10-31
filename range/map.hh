#ifndef UTIL_RANGE_MAP_HH
#define UTIL_RANGE_MAP_HH

namespace range {

/** Lazily yields elements of an underlying range transformed by a functor. */
template <class Functor, class R>
struct MapRange : public BasicRange
{
  using BaseTraits = typename R::BaseTraits;
  using BaseElementType = typename BaseTraits::value_type;
  using ElementType = decltype(std::declval<Functor>()(std::declval<BaseElementType>()));
  using Reference = ElementType;

  template <class = std::enable_if_t<is_range<R>::value> >
  MapRange(const Functor& f, const R& r)
    : m_func(f), m_input(r)
  { }
        
  ElementType front() const { return m_func(m_input.front()); }

  void pop_front() { m_input.pop_front(); }

  bool has_next() const { return m_input.has_next(); }
        
 private:
  Functor m_func;
  R m_input;
};

template <class Functor, class R,
          class = std::enable_if_t<is_range<R>::value> >
MapRange<Functor, R>
map(Functor&& f, const R& r)
{
  return MapRange<Functor, R>(std::forward<Functor>(f), r);
}
    
template <class Functor, class It>
MapRange<Functor, AsRange<It> >
map(Functor&& f, It it, It end)
{
  return MapRange<Functor, AsRange<It> >(
    std::forward<Functor>(f), as_range(it, end));
}

// struct is_valid_functor
// {
//     template <class F = Functor,
//               class Ret = decltype(
//                   std::declval<F>()(
//                       std::declval<typename R::ElementType>()))> 
//     static std::enable_if_t<

//         std::true_type> test(int);
//     static std::false_type test(...);
//     static const bool value = decltype(test(0))::value;
// };
}

#endif
