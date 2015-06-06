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
#include <sstream>

#include <boost/date_time/gregorian/gregorian.hpp>

#include <range/v3/core.hpp>
#include <range/v3/size.hpp>
#include <range/v3/range.hpp>
#include <range/v3/front.hpp>
#include <range/v3/getlines.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/view/group_by.hpp>
#include <range/v3/view/intersperse.hpp>
#include <range/v3/view/chunk.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/single.hpp>
#include <range/v3/view/concat.hpp>
#include <range/v3/view/repeat_n.hpp>
#include <range/v3/view/iota.hpp>

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

auto by_month() {
    return view::group_by([](greg::date a, greg::date b) {
        return a.month() == b.month();
    });
}

auto by_week() {
    return view::group_by([](greg::date a, greg::date b) {
        return b.week_number() == a.week_number();
    });
}

const char* monthNames [] = { "January", "February", "March", "April", "May", "June",
"July", "August", "September", "October", "November", "December" };

/// The number of columns per day in the formatted output.
int ColsPerDay = 3;

/// The number of columns per week in the formatted output.
int ColsPerWeek = 7 * ColsPerDay;

std::string MonthSep = "   ";

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

      // cope with american barbaric convention on starting week on sunday
      int startDay = d.day_of_week() - 1;
      startDay = startDay < 0 ? 6 : startDay;

      ss << spaces(ColsPerDay * startDay);

      auto weeks = dates | view::transform(formatDay())
                         | view::intersperse(" ");

      ss << ranges::accumulate(weeks, std::string());

      // Insert more filler at the end to fill up the remainder of the
      // week, if it's a short week (e.g. at the end of the month).
      int numDays = ranges::distance(dates);
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
   auto operator()(Range month) const
   {
      // Required for last line padding, really inefficient though
      int week_count = distance(month | by_week());

       return view::concat(
            view::single(monthTitle(front(month).month())),
            month | by_week() | view::transform(FormatWeek()),
            view::repeat_n(std::string(ColsPerWeek-1,' '), 6-week_count));
   }
};

auto datesInYear(int year)
{
	return view::iota(greg::date(year, 1, 1), greg::date(year+1, 1, 1));
}

template <typename Range>
auto formatMonths(Range days)
{
   return days |
          by_month() |
          view::transform(FormatMonth());
}

int main()
{
   	try
	{
	    auto year = datesInYear(2015);
	    const int monthsByLine = 5;

	    auto months = formatMonths(year) | view::chunk(monthsByLine);
	    RANGES_FOR(auto monthsLine, months)
	    {
	       auto monthsLineVec = to_<std::vector<std::vector<std::string>>>(monthsLine);

	       for(int j = 0; j < 7; j++)
	       {
	          for(int i = 0; i < monthsLineVec.size(); i++)
	          {
	             std::cout << monthsLineVec[i][j] << MonthSep;
	          }
	          std::cout << std::endl;
	       }
	       std::cout << std::endl;
	    }
	}

	catch (std::exception& e)
	{
		std::cout << "Error bad date, check your entry: \n"
			<< "  Details: " << e.what() << std::endl;
	}
}




