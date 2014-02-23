#include <range/v3/algorithm/for_each.hpp>
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


int main()
{
	try
	{
        auto rd = datesInYear(2014);

	    auto months = ChunkByMonth(rd.begin(), rd.end());
	    months | ranges::for_each([](const auto& m)
	    {
	       for(day_iterator d : m)
	          std::cout << *d << "\n";
            std::cout << "\n";
	    });

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



