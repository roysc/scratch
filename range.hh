#include <iterator>

#ifndef _SCRATCH_RANGE
#define _SCRATCH_RANGE

namespace range
{
    template <class Predicate, class IterBase>
    struct FilterRange
    {
        using BaseTraits = typename std::iterator_traits<IterBase>;

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
        
            typename BaseTraits::reference
            operator*() const { return *m_base; }

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
            : m_pred(f), m_iter(it), m_end(end)
        { }

        Iterator begin() const
        { return Iterator(m_pred, m_iter, m_end); }
        Iterator end() const
        { return Iterator(m_pred, m_end, m_end); }

        Predicate m_pred;
        IterBase m_iter, m_end;
    };

    template <class Predicate, class It>
    FilterRange<Predicate, It>
    filter(Predicate&& f, It it, It end)
    {
        return FilterRange<Predicate, It>(
            std::forward<Predicate>(f),
            std::forward<It>(it), std::forward<It>(end));
    }
    
}

#endif
