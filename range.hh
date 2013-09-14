#include <iterator>

#include "cxx14.hh"

#ifndef _SCRATCH_RANGE
#define _SCRATCH_RANGE

namespace range
{
    template <class Predicate, class IterBase>
    struct FilterRange
    {
        using BaseTraits = std::iterator_traits<IterBase>;
        using ElementType = typename BaseTraits::value_type;
        using Reference = ElementType&;

        struct Iterator : std::iterator<
            std::forward_iterator_tag,
            typename BaseTraits::value_type,
            typename BaseTraits::difference_type,
            typename BaseTraits::pointer,
            typename BaseTraits::reference>
        {
            Predicate m_pred;
            IterBase m_base, m_end;
            
            Iterator(const Predicate& pred,
                     const IterBase& it, const IterBase& end)
                : m_pred(pred), m_base(it), m_end(end)
            { satisfy_predicate(); }
        
            Reference operator*() const { return *m_base; }

            Iterator& operator++()
            {
                ++m_base;
                satisfy_predicate();
            }

            bool operator==(const Iterator& that)
            { return m_base == that.m_base; }

            bool operator!=(const Iterator& that)
            { return !(*this == that); }
    
            void satisfy_predicate()
            {
                while (m_base != m_end && !m_pred(*m_base))
                    ++m_base;
            }
        };
    
        FilterRange(const Predicate& f, const IterBase& it, const IterBase& end)
            : m_pred(f), m_cursor(it), m_end(end)
        { }
        
        Reference front() { return *m_cursor; }

        void pop_front() { ++m_cursor; }

        bool has_next() { return m_cursor != m_end; }

        Iterator begin() const
        { return Iterator(m_pred, m_cursor, m_end); }
        
        Iterator end() const
        { return Iterator(m_pred, m_end, m_end); }

      private:
        Predicate m_pred;
        IterBase m_cursor, m_end;
    };

    template <class Predicate, class It>
    FilterRange<Predicate, It>
    filter(Predicate&& f, It it, It end)
    {
        return FilterRange<Predicate, It>(
            std::forward<Predicate>(f),
            std::forward<It>(it), std::forward<It>(end));
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
        
        Reference front() { return *m_cursor; }

        void pop_front() { ++m_cursor; }

        bool has_next() { return m_cursor != m_end; }
        
      private:
        It m_cursor, m_end;
    };
    
    template <class It>
    AsRange<It> as_range(It beg, It end)
    { return AsRange<It>(beg, end); }


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
    using is_range = std::integral_constant<bool,
                                            is_forward_range<T>::value
                                            >;
};


#ifdef _BUILD_TEST

using namespace range;

struct P { bool operator()(int x) { return x > 0; } };
static_assert(is_forward_range<FilterRange<P, int*> >::value, "");

#endif

#endif
