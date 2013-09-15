#include <iterator>

#ifndef _SCRATCH_RANGE_ADAPT
#define _SCRATCH_RANGE_ADAPT

namespace range
{
  namespace adapt
  {
    
    template <class It>
    struct AsRange
    {
      using BaseTraits = typename std::iterator_traits<It>;
      using ElementType = typename BaseTraits::value_type;
      using Reference = ElementType&;

      AsRange(It beg, It end)
          : m_cursor(beg), m_end(end)
        { }

      // template <class C,
      //           class = std::enable_if_t<util::is_iterable<C>::value> >
      // AsRange(const C& c)
      //     : AsRange(std::begin(c), std::end(c))
      // { }
        
      Reference front() const { return *m_cursor; }
      void pop_front() { ++m_cursor; }
      bool has_next() const { return m_cursor != m_end; }
        
      private:
      It m_cursor, m_end;
    };
    
    template <class It>
    AsRange<It> as_range(It beg, It end)
    { return AsRange<It>(beg, end); }


    template <class R,
              class = std::enable_if_t<is_range<R>::value> >
    struct RangeIterator : std::iterator<
        std::forward_iterator_tag,
        typename R::BaseTraits::value_type,
        typename R::BaseTraits::difference_type,
        typename R::BaseTraits::pointer,
        typename R::BaseTraits::reference>
    {
        RangeIterator(const R& r)
            : m_range(r)
        { }
            
        typename R::BaseTraits::reference
        operator*() const { return m_range.front(); }

        RangeIterator& operator++()
        { m_range.pop_front(); return *this; }

        bool operator!=(const RangeIterator&) const
        { return m_range.has_next(); }

      private:
        R m_range;
    };

    template <class R>
    RangeIterator<R> begin(const R& r)
    { return RangeIterator<R>(r); }

    template <class R>
    RangeIterator<R> end(const R& r)
    { return RangeIterator<R>(r); }
  }

  using namespace adapt;
}

#endif
