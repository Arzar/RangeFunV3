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


import std.algorithm;
import std.conv;
import std.datetime;
import std.functional;
import std.range;
import std.stdio : writeln, writefln, stderr;
import std.string;



// Used by groupBy.
/**
* Specifies whether a predicate is an equivalence relation.
*/
import std.typecons : Flag;
alias EquivRelation = Flag!"equivRelation";
// Used by implementation of groupBy.
private struct GroupByChunkImpl(alias pred, EquivRelation equivRelation, Range)
{
alias fun = binaryFun!pred;
private Range r;
static if (!equivRelation)
private bool first = true;
else
private enum first = false;
/* For forward ranges, using .save is more reliable than blindly assuming
* that the current value of .front will persist past a .popFront. However,
* if Range is only an input range, then we have no choice but to save the
* value of .front. */
static if (isForwardRange!Range)
{
private Range prev;
this(Range _r, Range _prev)
{
r = _r.save;
prev = _prev.save;
}
private void savePrev() { prev = r.save; }
@property bool empty()
{
return r.empty || (!first && !fun(prev.front, r.front));
}
}
else
{
private ElementType!Range prev;
this(Range _r, ElementType!Range _prev)
{
r = _r;
prev = _prev;
}
private void savePrev() { prev = r.front; }
@property bool empty()
{
return r.empty || (!first && !fun(prev, r.front));
}
}
@property ElementType!Range front() { return r.front; }
void popFront()
in
{
import core.exception : RangeError;
if (r.empty) throw new RangeError();
}
body
{
// If this is a non-equivalence relation, we cannot assume transitivity
// so we have to update .prev at every step.
static if (!equivRelation)
{
savePrev();
first = false;
}
r.popFront();
}
static if (isForwardRange!Range)
{
@property typeof(this) save()
{
typeof(this) copy;
copy.r = r.save;
copy.prev = prev.save;
return copy;
}
}
}
// Implementation of groupBy.
private struct GroupByImpl(alias pred, EquivRelation equivRelation, Range)
{
alias fun = binaryFun!pred;
private Range r;
/* For forward ranges, using .save is more reliable than blindly assuming
* that the current value of .front will persist past a .popFront. However,
* if Range is only an input range, then we have no choice but to save the
* value of .front. */
static if (isForwardRange!Range)
{
private Range _prev;
private void savePrev() { _prev = r.save; }
private @property ElementType!Range prev() { return _prev.front; }
}
else
{
private ElementType!Range _prev;
private void savePrev() { _prev = r.front; }
private alias prev = _prev;
}
this(Range _r)
{
r = _r;
if (!empty)
{
// Check reflexivity if predicate is claimed to be an equivalence
// relation.
assert(!equivRelation || pred(r.front, r.front),
"predicate " ~ pred.stringof ~ " is claimed to be "~
"equivalence relation yet isn't reflexive");
savePrev();
}
}
@property bool empty() { return r.empty; }
@property auto front()
in
{
import core.exception : RangeError;
if (r.empty) throw new RangeError();
}
body
{
return GroupByChunkImpl!(pred, equivRelation, Range)(r, _prev);
}
void popFront()
{
while (!r.empty)
{
static if (equivRelation)
{
if (!fun(prev, r.front))
{
savePrev();
break;
}
r.popFront();
}
else
{
// For non-equivalence relations, we cannot assume transitivity
// so we must update prev each time.
savePrev();
r.popFront();
if (!r.empty && !fun(prev, r.front))
break;
}
}
}
static if (isForwardRange!Range)
{
@property typeof(this) save()
{
typeof(this) copy;
copy.r = r.save;
copy._prev = _prev.save;
return copy;
}
}
}
/**
* Chunks an input range into subranges of equivalent adjacent elements.
*
* Equivalence is defined by the predicate $(D pred), which can be either
* binary or unary. In the binary form, two _range elements $(D a) and $(D b)
* are considered equivalent if $(D pred(a,b)) is true. In unary form, two
* elements are considered equivalent if $(D pred(a) == pred(b)) is true.
*
* The optional parameter $(D equivRelation), which defaults to
* $(D EquivRelation.no) for binary predicates if not specified, specifies
* whether $(D pred) is an equivalence relation, that is, whether it is
* reflexive ($(D pred(x,x)) is always true), symmetric ($(D pred(x,y) ==
* pred(y,x))), and transitive ($(D pred(x,y) && pred(y,z)) implies
* $(D pred(x,z))). When this is the case, $(D groupBy) can take advantage of
* these three properties for a slight performance improvement.
*
* Note that it is not an error to specify $(D EquivRelation.no) even when
* $(D pred) is an equivalence relation; the resulting range will just be
* slightly slower than it could be. However, if $(D EquivRelation.yes) is
* specified yet $(D pred) is actually $(I not) an equivalence relation, the
* behaviour of the resulting range is undefined.
*
* Unary predicates always imply $(D equivRelation.yes), since they are
* internally converted to the binary equivalence relation $(D pred(a) ==
* pred(b)).
*
* Params:
* pred = Predicate for determining equivalence.
* r = The range to be chunked.
*
* Returns: A range of ranges in which all elements in a given subrange are
* equivalent under the given predicate.
*
* Notes:
*
* Equivalent elements separated by an intervening non-equivalent element will
* appear in separate subranges; this function only considers adjacent
* equivalence. Elements in the subranges will always appear in the same order
* they appear in the original range.
*
* See_also:
* $(XREF algorithm,group), which collapses adjacent equivalent elements into a
* single element.
*/
auto groupBy(alias pred, Range)(Range r)
if (isInputRange!Range)
{
return groupBy!(pred, EquivRelation.no, Range)(r);
}
/// ditto
auto groupBy(alias pred, EquivRelation equivRelation, Range)(Range r)
if (isInputRange!Range)
{
static if (is(typeof(binaryFun!pred(ElementType!Range.init,
ElementType!Range.init)) : bool))
return GroupByImpl!(pred, equivRelation, Range)(r);
else static if (is(typeof(
unaryFun!pred(ElementType!Range.init) ==
unaryFun!pred(ElementType!Range.init))))
return GroupByImpl!((a,b) => pred(a) == pred(b), EquivRelation.yes, Range)(r);
else
static assert(0, "groupBy expects either a binary predicate or "~
"a unary predicate on range elements of type: "~
ElementType!Range.stringof);
}
















int cnt_empty = 0;
int cnt_front = 0;
int cnt_popfront = 0;
int cnt_save = 0;

auto statRange(Range)(Range r)
{
   static struct StatRange {
      private Range r_;
      this(Range r) {
         r_ = r;
      }
      @property bool empty() { cnt_empty++ ; return r_.empty; }
      @property auto front() { cnt_front++; return r_.front; }
      @property void popFront() { cnt_popfront++; r_.popFront; }
      @property StatRange save() {
         cnt_save++;
         StatRange copy = this;
         copy.r_ = r_;
         return copy;
      }
   }

   return StatRange( r );
}

int cnt_eq = 0;

bool equal_stat(truc)(truc a, truc b)
{
  cnt_eq++;
  return a == b;
}

int main() 
{
   int[] a = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];

   

   auto s = statRange(a).groupBy!((a, b) =>  equal_stat(a, b));
   writeln(s);
   writeln("empty ", cnt_empty);
   writeln("front ", cnt_front);
   writeln("popFront ", cnt_popfront);
   writeln("save ", cnt_save);
   writeln("equal ", cnt_eq);

cnt_empty = 0;
cnt_front = 0;
cnt_popfront = 0;
cnt_save = 0;
cnt_eq = 0;

  auto ss = statRange(a).groupBy!((a, b) =>  equal_stat(a, b), EquivRelation.yes);
   writeln(ss);
   writeln("empty ", cnt_empty);
   writeln("front ", cnt_front);
   writeln("popFront ", cnt_popfront);
   writeln("save ", cnt_save);
   writeln("equal ", cnt_eq);
 
   return 0;
}
