#pragma once

#include <iterator>

template <class It>
class input_iterator
{
    It it_;

public:
    typedef          std::input_iterator_tag                   iterator_category;
    typedef typename std::iterator_traits<It>::value_type      value_type;
    typedef typename std::iterator_traits<It>::difference_type difference_type;
    typedef It                                                 pointer;
    typedef typename std::iterator_traits<It>::reference       reference;

    static int cnt_deref;
    static int cnt_arrow;
    static int cnt_inc;
    static int cnt_eq;

    input_iterator() : it_() {}
    explicit input_iterator(It it) : it_(it) {}

    reference operator*() const {return *it_;}
    pointer operator->() const {return it_;}

    input_iterator& operator++() {++it_; return *this;}
    input_iterator operator++(int)
        {input_iterator tmp(*this); ++(*this); return tmp;}

    friend bool operator==(const input_iterator& x, const input_iterator& y)
        {return x.it_ == y.it_;}
    friend bool operator!=(const input_iterator& x, const input_iterator& y)
        {return !(x == y);}
};

template<typename T> int input_iterator<T>::cnt_deref = 0;
template<typename T> int input_iterator<T>::cnt_arrow = 0;
template<typename T> int input_iterator<T>::cnt_inc = 0;
template<typename T> int input_iterator<T>::cnt_eq = 0;

template <class It>
class forward_iterator
{
    It it_;

public:
    typedef          std::forward_iterator_tag                 iterator_category;
    typedef typename std::iterator_traits<It>::value_type      value_type;
    typedef typename std::iterator_traits<It>::difference_type difference_type;
    typedef It                                                 pointer;
    typedef typename std::iterator_traits<It>::reference       reference;

    static int cnt_deref;
    static int cnt_arrow;
    static int cnt_inc;
    static int cnt_eq;

    forward_iterator() : it_() {}
    explicit forward_iterator(It it) : it_(it) {}

    reference operator*() const {cnt_deref++; return *it_;}
    pointer operator->() const {cnt_arrow++; return it_;}

    forward_iterator& operator++() {cnt_inc++; ++it_; return *this;}
    forward_iterator operator++(int)
        {cnt_inc++; forward_iterator tmp(*this); ++(*this); return tmp;}

    friend bool operator==(const forward_iterator& x, const forward_iterator& y)
        {cnt_eq++; return x.it_ == y.it_;}
    friend bool operator!=(const forward_iterator& x, const forward_iterator& y)
        {cnt_eq++; return !(x == y);}
};

template<typename T> int forward_iterator<T>::cnt_deref = 0;
template<typename T> int forward_iterator<T>::cnt_arrow = 0;
template<typename T> int forward_iterator<T>::cnt_inc = 0;
template<typename T> int forward_iterator<T>::cnt_eq = 0;

template <class It>
class bidirectional_iterator
{
    It it_;

public:
    typedef          std::bidirectional_iterator_tag           iterator_category;
    typedef typename std::iterator_traits<It>::value_type      value_type;
    typedef typename std::iterator_traits<It>::difference_type difference_type;
    typedef It                                                 pointer;
    typedef typename std::iterator_traits<It>::reference       reference;

    static int cnt_deref;
    static int cnt_arrow;
    static int cnt_inc;
    static int cnt_dec;
    static int cnt_eq;


    bidirectional_iterator() : it_() {}
    explicit bidirectional_iterator(It it) : it_(it) {}

    reference operator*() const {cnt_deref++; return *it_;}
    pointer operator->() const {cnt_arrow++; return it_;}

    bidirectional_iterator& operator++() {cnt_inc++; ++it_; return *this;}
    bidirectional_iterator operator++(int)
        {cnt_inc++; bidirectional_iterator tmp(*this); ++(*this); return tmp;}

    bidirectional_iterator& operator--() {cnt_dec++; --it_; return *this;}
    bidirectional_iterator operator--(int)
        {cnt_dec++; bidirectional_iterator tmp(*this); --(*this); return tmp;}

    friend bool operator==(const bidirectional_iterator& x, const bidirectional_iterator& y)
        {cnt_eq++; return x.it_ == y.it_;}
    friend bool operator!=(const bidirectional_iterator& x, const bidirectional_iterator& y)
        {cnt_eq++; return !(x == y);}
};

template<typename T> int bidirectional_iterator<T>::cnt_deref = 0;
template<typename T> int bidirectional_iterator<T>::cnt_arrow = 0;
template<typename T> int bidirectional_iterator<T>::cnt_inc = 0;
template<typename T> int bidirectional_iterator<T>::cnt_dec = 0;
template<typename T> int bidirectional_iterator<T>::cnt_eq = 0;

template <class It>
class random_access_iterator
{
    It it_;

public:
    typedef          std::random_access_iterator_tag           iterator_category;
    typedef typename std::iterator_traits<It>::value_type      value_type;
    typedef typename std::iterator_traits<It>::difference_type difference_type;
    typedef It                                                 pointer;
    typedef typename std::iterator_traits<It>::reference       reference;

    static int cnt_deref;
    static int cnt_arrow;
    static int cnt_inc;
    static int cnt_dec;
    static int cnt_eq;
    static int cnt_array;


    random_access_iterator() : it_() {}
    explicit random_access_iterator(It it) : it_(it) {}

    reference operator*() const {cnt_deref++; return *it_;}
    pointer operator->() const {cnt_arrow++; return it_;}

    random_access_iterator& operator++() {cnt_inc++; ++it_; return *this;}
    random_access_iterator operator++(int)
        {cnt_inc++; random_access_iterator tmp(*this); ++(*this); return tmp;}

    random_access_iterator& operator--() {cnt_dec++; --it_; return *this;}
    random_access_iterator operator--(int)
        {cnt_dec++; random_access_iterator tmp(*this); --(*this); return tmp;}

    random_access_iterator& operator+=(difference_type n) {cnt_inc++; it_ += n; return *this;}
    random_access_iterator operator+(difference_type n) const
        {cnt_inc++; random_access_iterator tmp(*this); tmp += n; return tmp;}
    friend random_access_iterator operator+(difference_type n, random_access_iterator x)
        {cnt_inc++; x += n; return x;}
    random_access_iterator& operator-=(difference_type n) {cnt_dec++; return *this += -n;}
    random_access_iterator operator-(difference_type n) const
        {cnt_dec++; random_access_iterator tmp(*this); tmp -= n; return tmp;}

    reference operator[](difference_type n) const {cnt_array++; return it_[n];}

    friend bool operator==(const random_access_iterator& x, const random_access_iterator& y)
        {cnt_eq++; return x.it_ == y.it_;}
    friend bool operator!=(const random_access_iterator& x, const random_access_iterator& y)
        {cnt_eq++; return !(x == y);}
};

template<typename T> int random_access_iterator<T>::cnt_deref = 0;
template<typename T> int random_access_iterator<T>::cnt_arrow = 0;
template<typename T> int random_access_iterator<T>::cnt_inc = 0;
template<typename T> int random_access_iterator<T>::cnt_dec = 0;
template<typename T> int random_access_iterator<T>::cnt_eq = 0;
template<typename T> int random_access_iterator<T>::cnt_array = 0;
