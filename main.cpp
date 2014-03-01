
#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/algorithm/transform.hpp>
#include "range_ext.hpp"


const char* monthNames [] = { "January", "February", "March", "April", "May", "June",
"July", "August", "September", "October", "November", "December" };

/// The number of columns per day in the formatted output.
int ColsPerDay = 3;

/// The number of columns per week in the formatted output.
int ColsPerWeek = 7 * ColsPerDay;

std::string spaces(size_t n)
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

      greg::date d = *ranges::begin(dates);
      int startDay = d.day_of_week() - 1;
      startDay = startDay < 0 ? 6 : startDay;

      ss << spaces(ColsPerDay * startDay);

      int numDays = 0;
      std::string s;
      dates | ranges::for_each([&](const greg::date& d)
      {
        s += formatDay()(d) + " ";
      });


      // goal
      //std::string s = dates | view::transform(formatDay()) | join(" ");

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

std::stringstream g_ss;

struct formatMonth
{
   using result_type = std::string;

   template <typename Range>
   std::string operator()(Range monthDay) const
   {
      g_ss.clear();
      greg::date first_day_of_month = *monthDay.begin();

      g_ss << monthTitle(first_day_of_month.month()) << std::endl;

      auto weeks = ChunkByWeek(monthDay);
      weeks | ranges::for_each([](auto w)
      {
        g_ss << formatWeek()(w) << "\n";
      });

      // goal
      //ss << monthDay | ChunkByWeek | view::transform(formatWeek) |  join("\n");

      g_ss << std::endl;
      return g_ss.str();
   }
};




int main()
{

	try
	{
	    day_range year = datesInYear(2014);
	    auto months = ChunkByMonth(year);
/*
        months | ranges::for_each([](auto aMonth)
	    {
	       ChunkByWeek(aMonth) | ranges::for_each([](auto aWeek)
	       {
	          for(greg::date d : aWeek)
	             std::cout << d << "\n";
                 std::cout << "\n";
	        });

	    });*/

        months | ranges::for_each([](auto aMonth)
	    {
	       std::cout << formatMonth()(aMonth);
        });
	}


	catch (std::exception& e) {

		std::cout << "Error bad date, check your entry: \n"
			<< "  Details: " << e.what() << std::endl;
	}

}




