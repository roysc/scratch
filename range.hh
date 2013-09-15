#include <iterator>

#include "cxx14.hh"

#ifndef _SCRATCH_RANGE
#define _SCRATCH_RANGE

namespace range
{
    template <class T>
    struct is_forward_range
    {
        template <class T_ = std::decay_t<T>,
                  class EltRef = decltype(std::declval<T_>().front()),
                  class HasNext = decltype(std::declval<T_>().has_next()),
                  class PopFront = decltype(std::declval<T_>().pop_front())>
        static std::enable_if_t<
            std::is_lvalue_reference<EltRef>::value &&
            std::is_same<HasNext, bool>::value,
            std::true_type> test(int);
        static std::false_type test(...);
        static const bool value = decltype(test(0))::value;
    };

    template <class T>
    using is_range = std::integral_constant<
        bool,
        is_forward_range<T>::value
        >;

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

        RangeIterator& operator++() { m_range.pop_front(); }

        bool operator!=(const RangeIterator& that) const
        { return m_range.has_next(); }

      private:
        R m_range;
    };
    
    
    template <class Predicate, class It>
    struct FilterRange
    {
        using BaseTraits = std::iterator_traits<It>;
        using ElementType = typename BaseTraits::value_type;
        using Reference = ElementType&;

        FilterRange(const Predicate& f, const It& it, const It& end)
            : m_pred(f), m_cursor(it), m_end(end)
        {
            satisfy_predicate();
        }
        
        Reference front() const { return *m_cursor; }

        void pop_front()
        {
            ++m_cursor;
            satisfy_predicate();
        }

        bool has_next() const { return m_cursor != m_end; }

      private:
        void satisfy_predicate()
        {
            while (m_cursor != m_end && !m_pred(*m_cursor))
                ++m_cursor;
        }
        
      private:
        Predicate m_pred;
        It m_cursor, m_end;
    };

    template <class Predicate, class R>
    FilterRange<Predicate, RangeIterator<R> >
    filter(Predicate&& f, const R& r)
    {
        return FilterRange<Predicate, RangeIterator<R> >(
            std::forward<Predicate>(f),
            range_begin(r), range_end(r));
    }
    
    template <class Predicate, class It>
    FilterRange<Predicate, It>
    filter(Predicate&& f, It it, It end)
    {
        return FilterRange<Predicate, It>(
            std::forward<Predicate>(f), it, end);
    }


    template <class It>
    struct AsRange
    {
        using BaseTraits = typename std::iterator_traits<It>;
        using ElementType = typename BaseTraits::value_type;
        using Reference = ElementType&;

        AsRange(It beg, It end)
            : m_cursor(beg), m_end(end)
        { }
        
        Reference front() const { return *m_cursor; }

        void pop_front() { ++m_cursor; }

        bool has_next() const { return m_cursor != m_end; }
        
      private:
        It m_cursor, m_end;
    };
    
    template <class It>
    AsRange<It> as_range(It beg, It end)
    { return AsRange<It>(beg, end); }


    template <class R>
    RangeIterator<R> range_begin(const R& r)
    { return RangeIterator<R>(r); }

    template <class R>
    RangeIterator<R> range_end(const R& r)
    { return RangeIterator<R>(r); }
    
};

namespace std
{
    template <class R,
              class = std::enable_if_t<range::is_range<R>::value> >
    range::RangeIterator<R> begin(const R& r)
    { return range::range_begin(r); }

    template <class R,
              class = std::enable_if_t<range::is_range<R>::value> >
    range::RangeIterator<R> end(const R& r)
    { return range::range_end(r); }
}


#ifdef _BUILD_TEST

using namespace range;

struct P { bool operator()(int x) { return x > 0; } };
static_assert(is_forward_range<FilterRange<P, int*> >::value, "");

#endif

#endif
