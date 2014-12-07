#pragma once

#include <iterator>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/iterator/iterator_facade.hpp>


namespace greg = boost::gregorian;

class day_iterator
{
    greg::date m_date;

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = greg::date;
    using difference_type   = ptrdiff_t;
    using pointer           = greg::date*;
    using reference         = const greg::date&;

    day_iterator() : m_date() {}
    day_iterator(const day_iterator& it) : m_date(it.m_date) {}
    day_iterator(const greg::date& d) : m_date(d) {}

    const greg::date& operator*() const {return m_date;}
    pointer operator->() {return &m_date;}

    day_iterator& operator++() {m_date = m_date + greg::date_duration(1); return *this;}
    day_iterator operator++(int)
        {day_iterator tmp(*this); ++(*this); return tmp;}

    friend bool operator==(const day_iterator& x, const day_iterator& y)
        {return x.m_date == y.m_date;}
    friend bool operator!=(const day_iterator& x, const day_iterator& y)
        {return !(x == y);}
};

using day_range = ranges::range<day_iterator>;


 struct PredByMonth
 {
    using first_argument_type = greg::date;
    using second_argument_type = greg::date;
    bool operator()(const greg::date& d1, const greg::date& d2) const
    {
      return d1.month() == d2.month();
    }
 };

 struct PredByWeek
 {
    using first_argument_type = greg::date;
    using second_argument_type = greg::date;
    bool operator()(const greg::date& d1, const greg::date& d2) const
    {
      return d1.week_number() == d2.week_number();
    }
 };

namespace ranges
{
    inline namespace v3
    {
       namespace view
       {
          auto groupByMonth = group_by(PredByMonth());
          auto groupByWeek = group_by(PredByWeek());
       }
    }
}


