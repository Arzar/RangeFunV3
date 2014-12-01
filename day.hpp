#pragma once

#include <iterator>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include "groupby.hpp"

namespace greg = boost::gregorian;


class day_iterator
	: public boost::iterator_facade <
	day_iterator
	, greg::date
	, boost::forward_traversal_tag
	, const greg::date&
	>
{
public:

	using iterator_category = std::forward_iterator_tag;

	day_iterator()
		: m_date()
	{}

	day_iterator(greg::date d)
		: m_date(d)
	{}

private:
	friend class boost::iterator_core_access;

	void increment() { m_date = m_date + greg::date_duration(1); }
	int distance_to(const day_iterator& other) const { return (other.m_date - m_date).days();}

	bool equal(const day_iterator& other) const
	{
		return this->m_date == other.m_date;
	}

	const greg::date& dereference() const
	{
		return m_date;
	}
	greg::date& dereference()
	{
		return m_date;
	}

	greg::date m_date;
};

struct day_range : public ranges::range<day_iterator>
{
	day_range(const greg::date& begin, const greg::date& end) :
		ranges::range<day_iterator>(day_iterator(begin), day_iterator(end))
	{}

	day_range(day_iterator begin, day_iterator end) :
		ranges::range<day_iterator>(begin, end)
	{}
};

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
          auto groupByMonth = view::groupBy(PredByMonth());
          auto groupByWeek = view::groupBy(PredByWeek());
       }
    }
}


