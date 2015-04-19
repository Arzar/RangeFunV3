// Following code use some part of Eric Niebler's range-v3 library calendar example :
// https://github.com/ericniebler/range-v3

//
// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/date_time/gregorian/gregorian.hpp>

#include <range/v3/range.hpp>
#include <range/v3/front.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/view/group_by.hpp>
#include <range/v3/view/intersperse.hpp>
#include <range/v3/view/split.hpp>
#include <range/v3/view/transform.hpp>

using namespace ranges;

namespace greg = boost::gregorian;

// Nee trick, taken from range-v3 calendar example :
// No need to create a proper iterator on greg::date, just get greg::date to satisfy
// WeaklyIncrementable concept and view:iota generate a range for us.
namespace boost { namespace gregorian {
    date &operator++(date &d) { return d = d + date_duration(1); }
    date operator++(date &d, int) { return ++d - date_duration(1); }
}}

namespace ranges {
    template<> struct difference_type<greg::date> {
        using type = greg::date::duration_type::duration_rep::int_type;
    };
}

CONCEPT_ASSERT_MSG(WeaklyIncrementable<greg::date>(),
                        "Must have pre- and post-increment operators and it must have a difference_type");

using IotaView = decltype(view::iota(std::declval<greg::date>(), std::declval<greg::date>()));
CONCEPT_ASSERT(InputIterable<IotaView>());
CONCEPT_ASSERT(Iterable<IotaView>());
CONCEPT_ASSERT(Range<IotaView>());

auto by_month() {
    return view::group_by([](greg::date a, greg::date b) {
        return a.month() == b.month();
    });
}

auto by_week() {
    return view::group_by([](greg::date a, greg::date b) {

        // BUG, the following test work in range-v3 calendar example
        // but somehow mess up the formatting here
        //return b.day_of_week() != greg::Sunday || a == b;

        return b.week_number() == a.week_number();
    });
}

const char* monthNames [] = { "January", "February", "March", "April", "May", "June",
"July", "August", "September", "October", "November", "December" };

/// The number of columns per day in the formatted output.
int ColsPerDay = 3;

/// The number of columns per week in the formatted output.
int ColsPerWeek = 7 * ColsPerDay;

std::string spaces(int n)
{
	return std::string(n, ' ');
}


//
// Formats the name of a month centered on ColsPerWeek.
//
std::string monthTitle(int month)
{
	// Determine how many spaces before and after the month name we need to
	// center it over the formatted weeks in the month
	std::string name = monthNames[month - 1];
	int before = (ColsPerWeek - name.size()) / 2;
	int after = ColsPerWeek - name.size() - before;

	return spaces(before) + name + spaces(after);
}


struct formatDay
{
	std::string operator()(greg::date d) const
	{
		std::stringstream ss;
		ss << std::setw(2) << d.day().as_number();
		return ss.str();
	}
};


struct FormatWeek
{
   template <typename Range>
   std::string operator()(Range dates) const
   {
      std::stringstream ss;

      greg::date d = ranges::front(dates);
      int startDay = d.day_of_week() - 1;
      startDay = startDay < 0 ? 6 : startDay;

      ss << spaces(ColsPerDay * startDay);

      int numDays = 0;

      auto weeks = dates | view::transform(formatDay()) | view::intersperse(" ");
      ss << ranges::accumulate(weeks, std::string());

      // Insert more filler at the end to fill up the remainder of the
      // week, if it's a short week (e.g. at the end of the month).
      if (numDays < 7 - startDay)
      {
         ss << spaces(ColsPerDay * (7 - startDay - numDays));
      }

      return ss.str();
   }
};


struct FormatMonth
{
   template <typename Range>
   std::string operator()(Range r) const
   {
      std::stringstream ss;
      greg::date first_day_of_month = *r.begin();

      ss << monthTitle(first_day_of_month.month()) << std::endl;

      auto months = r | by_week() | view::transform(FormatWeek()) | view::intersperse("\n");

      ss << accumulate(months, std::string()) << std::endl;
      return ss.str();

   }
};

auto datesInYear(int year)
{
	auto new_year_day = greg::date(year, 1, 1);
	auto last_year_day = greg::date(year, 12, 31);
	return view::iota(new_year_day, last_year_day);
}

template <typename Range>
std::string formatCalendar(Range days)
{
   auto monthsFormatted = days |
                          by_month() |
                          view::transform(FormatMonth()) |
                          view::intersperse("\n");

   return accumulate(monthsFormatted, std::string());
}

#include "perf.hpp"
int main()
{
   	try
	{
	    auto year2014 = datesInYear(2014);
	    std::cout << formatCalendar(year2014) << std::endl;
	}

	catch (std::exception& e)
	{
		std::cout << "Error bad date, check your entry: \n"
			<< "  Details: " << e.what() << std::endl;
	}

	//perf();
}




