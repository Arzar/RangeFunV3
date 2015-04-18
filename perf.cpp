#include <range/v3/range.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/group_by.hpp>


#include <vector>
#include <iostream>

#include "perf.hpp"
#include "stat_iterators.hpp"

using namespace ranges;

struct EqualTo
{
   static int count;

   bool operator() (int a, int b)const{count++; return a== b;}

   bool operator()(forward_iterator<std::vector<int>::iterator> a,
                   forward_iterator<std::vector<int>::iterator> b)const
   {count++; return *a == *b;}

   bool operator()(random_access_iterator<std::vector<int>::iterator> a,
                   random_access_iterator<std::vector<int>::iterator> b)const
   {count++; return *a == *b;}
};

int EqualTo::count = 0;

int perf()
{
   int NBGROUPMAX = 11;

   std::vector<int> v;
   for(int i = 0 ; i < NBGROUPMAX; i++)
      v.push_back(i);

   for(int i = 0; i< NBGROUPMAX; i++)
   {
       forward_iterator<std::vector<int>::iterator> b{v.begin()};
       forward_iterator<std::vector<int>::iterator> e{next(v.begin(), i)};

       auto r = make_range(b, e);
       EqualTo equalTo;
       auto g = r | view::group_by(equalTo);
       std::cout << g << std::endl;

       std::cout << "cnt_deref " << forward_iterator<std::vector<int>::iterator>::cnt_deref << std::endl;
       std::cout << "cnt_inc " << forward_iterator<std::vector<int>::iterator>::cnt_inc << std::endl;
       std::cout << "cnt_eq " << forward_iterator<std::vector<int>::iterator>::cnt_eq << std::endl;
       std::cout << "cnt_eq_to " << equalTo.count << std::endl;

       forward_iterator<std::vector<int>::iterator>::cnt_deref = 0;
       forward_iterator<std::vector<int>::iterator>::cnt_inc = 0;
       forward_iterator<std::vector<int>::iterator>::cnt_eq = 0;
       EqualTo::count = 0;

   }

      for(int i = 0; i< NBGROUPMAX; i++)
   {
       EqualTo::count = 0;
       random_access_iterator<std::vector<int>::iterator> b{v.begin()};
       random_access_iterator<std::vector<int>::iterator> e{next(v.begin(), i)};

       auto r = make_range(b, e);
       EqualTo equalTo;
       auto g = r | view::group_by(equalTo);
       std::cout << g << std::endl;

       std::cout << "cnt_deref " << random_access_iterator<std::vector<int>::iterator>::cnt_deref << std::endl;
       std::cout << "cnt_inc " << random_access_iterator<std::vector<int>::iterator>::cnt_inc << std::endl;
       std::cout << "cnt_eq " << random_access_iterator<std::vector<int>::iterator>::cnt_eq << std::endl;
       std::cout << "cnt_eq_to " << equalTo.count << std::endl;

       random_access_iterator<std::vector<int>::iterator>::cnt_deref = 0;
       random_access_iterator<std::vector<int>::iterator>::cnt_inc = 0;
       random_access_iterator<std::vector<int>::iterator>::cnt_eq = 0;
       EqualTo::count = 0;

   }
   return 0;
}
