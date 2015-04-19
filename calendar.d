/*
Folowwing code come from the article "Component programming with ranges" 
By H. S. Teoh
http://wiki.dlang.org/Component_programming_with_ranges
https://github.com/quickfur/dcal
*/

/*
* D calendar
*
* An example of how using component-style programming with ranges simplifies a
* complex task into manageable pieces. The task is, given a year, to produce a
* range of lines representing a nicely laid-out calendar of that year.
*
* This example shows how something is complex as calendar layout can be
* written in a clear, readable way that allows individual components to be
* reused.
*/

int counterDateEq = 0;
int counterDateInc = 0;
int counterDateSave = 0;

import std.algorithm;
import std.conv;
import std.datetime;
import std.functional;
import std.range;
import std.stdio : writeln, writefln, stderr;
import std.string;

/**
* Returns: A string containing exactly n spaces.
*/
string spaces(size_t n) pure nothrow {
   return std.array.replicate(" ", n);
}
/**
* Returns: A range of dates in a given year.
*/
auto datesInYear(int year) pure {
   return Date(year, 1, 1)
   .recurrence!((a,n) => a[n-1] + 1.days)
   .until!(a => a.year > year);
}

auto datesInYear2(int year) {

   static struct DatesYear {
      private Date current_;
      private Date end_;
      this(Date b, Date e) {
         current_ = b;
         end_ = e;
      }
      @property bool empty() { counterDateEq++ ; return current_ == end_; }
      @property auto front() { return current_; }
      void popFront() { counterDateInc++; current_ = current_ + 1.days; }
      @property DatesYear save() {
         counterDateSave++;
         DatesYear copy = this;
         copy.current_ = current_;
         copy.end_ = end_;
         return copy;
      }
   }
   
   return DatesYear( Date(year, 1, 1),  Date(year + 1, 1, 1) );
}

/**
* Convenience template for verifying that a given range is an input range of
* Dates.
*/
template isDateRange(R) {
   enum isDateRange = isInputRange!R && is(ElementType!R : Date);
}
static assert(isDateRange!(typeof(datesInYear(1))));
/**
* Chunks an input range by equivalent elements.
*
* This function takes an input range, and splits it up into subranges that
* contain equivalent adjacent elements, where equivalence is defined by having
* the same value of attrFun(e) for each element e.
*
* Note that equivalent elements separated by an intervening non-equivalent
* element will appear in separate subranges; this function only considers
* adjacent equivalence.
*
* This is similar to std.algorithm.group, but the latter can't be used when
* the individual elements in each group must be iterable in the result.
*
* Parameters:
* attrFun = The function for determining equivalence. The return value must
* be comparable using ==.
* r = The range to be chunked.
*
* Returns: A range of ranges in which all elements in a given subrange share
* the same attribute with each other.
*/

int counterChunkByAttrEq = 0;


auto chunkBy(alias attrFun, Range)(Range r)
if (isInputRange!Range && is(typeof(attrFun(ElementType!Range.init) == attrFun(ElementType!Range.init))))
{
   alias attr = unaryFun!attrFun;
   alias AttrType = typeof(attr(r.front));
   static struct Chunk {
      private Range r;
      private AttrType curAttr;
      @property bool empty() {
         counterChunkByAttrEq++;
         return r.empty || !(curAttr == attr(r.front));
      }
      @property ElementType!Range front() { return r.front; }
      void popFront() {
         assert(!r.empty);
      r.popFront();
      }
   }
   static struct ChunkBy {
      private Range r;
      private AttrType lastAttr;
      this(Range _r) {
         r = _r;
         if (!empty)
            lastAttr = attr(r.front);
      }
      @property bool empty() { return r.empty; }
      @property auto front() {
      assert(!r.empty);
      return Chunk(r, lastAttr);
   }
   void popFront() {
      assert(!r.empty);
      while (!r.empty && attr(r.front) == lastAttr) {
         counterChunkByAttrEq++;
         r.popFront();
      }
      if (!r.empty)
         lastAttr = attr(r.front);
   }
   static if (isForwardRange!Range) {
      @property ChunkBy save() {
         ChunkBy copy = this;
         copy.r = r.save;
         return copy;
      }
   }
}
return ChunkBy(r);
}
///
/**
* Chunks a given input range of dates by month.
* Returns: A range of ranges, each subrange of which contains dates for the
* same month.
*/
auto byMonth(InputRange)(InputRange dates)
if (isDateRange!InputRange)
{
   return dates.chunkBy!(a => a.month());
}


/**
* Chunks a given input range of dates by week.
* Returns: A range of ranges, each subrange of which contains dates for the
* same week. Note that weeks begin on Sunday and end on Saturday.
*/
auto byWeek(InputRange)(InputRange dates) pure nothrow if (isDateRange!InputRange)
{
   static struct ByWeek {
      InputRange r;
      @property bool empty() { return r.empty; }
      @property auto front() {
         return until!((Date a) => a.dayOfWeek == DayOfWeek.sat) (r, OpenRight.no);
      }
      void popFront() {
         assert(!r.empty);
         r.popFront();
         while (!r.empty && r.front.dayOfWeek != DayOfWeek.sun)
             r.popFront();
      }
   }
   return ByWeek(dates);
}



/// The number of columns per day in the formatted output.
enum ColsPerDay = 3;
/// The number of columns per week in the formatted output.
enum ColsPerWeek = 7 * ColsPerDay;
/**
* Formats a range of weeks into a range of strings.
*
* Each day is formatted into the digit representation of the day of the month,
* padded with spaces to fill up 3 characters.
*
* Parameters:
* weeks = A range of ranges of Dates, each inner range representing
* consecutive dates in a week.
*/
auto formatWeek(Range)(Range weeks) pure nothrow
if (isInputRange!Range && isInputRange!(ElementType!Range) &&
is(ElementType!(ElementType!Range) == Date))
{
   struct WeekStrings {
   Range r;
   @property bool empty() { return r.empty; }
   string front()
   out(s) { assert(s.length == ColsPerWeek); }
   body
   {
      auto buf = appender!string();
      // Insert enough filler to align the first day with its respective
      // day-of-week.
      assert(!r.front.empty);
      auto startDay = r.front.front.dayOfWeek;
      buf.put(spaces(ColsPerDay * startDay));
      // Format each day into its own cell and append to target string.
      string[] days = map!((Date d) => " %2d".format(d.day))(r.front)
                     .array;
      assert(days.length <= 7 - startDay);
      days.copy(buf);
      // Insert more filler at the end to fill up the remainder of the
      // week, if it's a short week (e.g. at the end of the month).
      if (days.length < 7 - startDay)
         buf.put(spaces(ColsPerDay * (7 - startDay - days.length)));
      return buf.data;
   }
   void popFront() {
      r.popFront();
   }
}
return WeekStrings(weeks);
}

/**
* Formats the name of a month centered on ColsPerWeek.
*/
string monthTitle(Month month) pure nothrow {
   static immutable string[] monthNames = [
   "January", "February", "March", "April", "May", "June",
   "July", "August", "September", "October", "November", "December"];
   static assert(monthNames.length == 12);
   // Determine how many spaces before and after the month name we need to
   // center it over the formatted weeks in the month
   auto name = monthNames[month - 1];
   assert(name.length < ColsPerWeek);
   auto before = (ColsPerWeek - name.length) / 2;
   auto after = ColsPerWeek - name.length - before;
   return spaces(before) ~ name ~ spaces(after);
}

/**
* Formats a month.
* Parameters:
* monthDays = A range of Dates representing consecutive days in a month.
* Returns: A range of strings representing each line of the formatted month.
*/
auto formatMonth(Range)(Range monthDays)
if (isInputRange!Range && is(ElementType!Range == Date))
   in {
      assert(!monthDays.empty);
      assert(monthDays.front.day == 1);
   } body {
      return chain( [ monthTitle(monthDays.front.month) ], monthDays.byWeek().formatWeek());
}

/**
* Formats a range of months.
* Parameters:
* months = A range of ranges, each inner range is a range of Dates in a
* month.
* Returns:
* A range of ranges of formatted lines for each month.
*/
auto formatMonths(Range)(Range months) pure nothrow
if (isInputRange!Range && is(ElementType!(ElementType!Range) == Date))
{
   return months.map!formatMonth;
}

int main() 
{
   auto s = datesInYear2(2014).byMonth().formatMonths();
   writeln(s);
   writeln(counterDateEq);
   writeln(counterDateInc);
   writeln(counterDateSave);
   writeln(counterChunkByAttrEq);
   
   return 0;
}
