
#include <range/v3/range.hpp>
#include "join.hpp"
#include "day.hpp"

using namespace std::placeholders;
using namespace ranges;

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

day_range datesInYear(int year)
{
	greg::date year_first_day = greg::date(year, 1, 1);
	greg::date next_year_first_day = greg::date(year + 1, 1, 1);
	return day_range(year_first_day, next_year_first_day);
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
	using result_type = std::string;
	std::string operator()(const greg::date& d) const
	{
		std::stringstream ss;
		ss << std::setw(2) << d.day().as_number();
		return ss.str();
	}
};


struct formatWeek
{
   using result_type = std::string;

   template <typename Range>
   std::string operator()(Range dates) const
   {
      std::stringstream ss;
      ranges::detail::decay_t<Range> r = dates;

      greg::date d = *ranges::begin(dates);
      int startDay = d.day_of_week() - 1;
      startDay = startDay < 0 ? 6 : startDay;

      ss << spaces(ColsPerDay * startDay);

      int numDays = 0;

      // goal
      //ss << (r | ranges::view::transform(formatDay()) | ranges::view::join(" "));

      std::string s;
      dates | for_each([&](const greg::date& d)
      {
        s += formatDay()(d) + " ";
      });
      ss << s;

      // Insert more filler at the end to fill up the remainder of the
      // week, if it's a short week (e.g. at the end of the month).
      if (numDays < 7 - startDay)
      {
         ss << spaces(ColsPerDay * (7 - startDay - numDays));
      }

      return ss.str();
   }
};


struct formatMonth
{
   using result_type = std::string;

   std::string operator()(day_range monthDay) const
   {
      std::stringstream ss;
      greg::date first_day_of_month = *monthDay.begin();

      ss << monthTitle(first_day_of_month.month()) << std::endl;

      ss << (monthDay | view::chunkByWeek | view::transform(formatWeek()) |  join("\n"));

      ss << std::endl;
      return ss.str();
   }
};


int main()
{
	try
	{
       std::cout << (datesInYear(2014) |
                     view::chunkByMonth |
                     view::transform(formatMonth()) |
                     join('\n'));
	}

	catch (std::exception& e)
	{
		std::cout << "Error bad date, check your entry: \n"
			<< "  Details: " << e.what() << std::endl;
	}

}




