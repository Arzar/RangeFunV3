//#include <range/v3/utility/range.hpp>

#include <iterator>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/delimit.hpp>
#include <range/v3/view/adjacent_filter.hpp>



/*
#include <boost/range.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/copy.hpp>
*/

//#include <boost/iterator/iterator_categories.hpp>
#include <boost/algorithm/string/join.hpp>

#include "range_ext.hpp"

//using namespace ranges;

//using namespace boost::adaptors;
//using namespace boost::range::access;
//using namespace boost::range;

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


auto datesInYear2(int year)
{
   	day_iterator year_first_day = day_iterator(greg::date(year, 1, 1));
	day_iterator next_year_first_day = day_iterator(greg::date(year + 1, 1, 1));

	return ranges::view::iota(year_first_day) | ranges::view::delimit(next_year_first_day);
}


/**
* Formats the name of a month centered on ColsPerWeek.
*/
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

/*

struct formatWeek
{
	using result_type = std::string;

	std::string operator()(day_range dates) const
	{
		std::stringstream ss;

		greg::date d = front(dates);
		int startDay = d.day_of_week() - 1;
		startDay = startDay < 0 ? 6 : startDay;

		ss << spaces(ColsPerDay * startDay);

		int numDays = 0;
		ss << (dates | view::transform(formatDay()) | join(" "));

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
		greg::date first_day_of_month = monthDay | boost::range::access::front;

		ss << monthTitle(first_day_of_month.month()) << std::endl;

		ss << (monthDay | byweek | transformed(formatWeek()) | join("\n"));

		ss << std::endl;
		return ss.str();
	}
};
*/

int main()
{
	try
	{
	    greg::date year_first_day = greg::date(2014, 1, 1);



	    day_range rd = datesInYear(2014);


	    auto months = rd | ranges::view::adjacent_filter([](const greg::date& d1, const greg::date& d2){return d1.month() == d2.month();});
	    for(auto& d : months)
	    {
	        std::cout << d << "\n";
	    }



	    //auto months = datesInYear(2013);
		//auto months = datesInYear(2013)
		//	| bymonth
		//	| transformed(formatMonth());


		//boost::copy(months, std::ostream_iterator<std::string>(std::cout, "\n"));
	}

	catch (std::exception& e) {

		std::cout << "Error bad date, check your entry: \n"
			<< "  Details: " << e.what() << std::endl;
	}

}



