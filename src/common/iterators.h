#ifndef _ITERATORS_H_
#define _ITERATORS_H_

///@file common/iterators.h Iterator support @ingroup common
///@defgroup iterators Iterator support
///@ingroup common
///@{

/// Support iterator for Python-style range
template<typename T>
struct counting_iterator {
    T current;
    counting_iterator(const T& value) : current(value) { }
    T& operator*() { return current; }
    counting_iterator<T>& operator++() { ++current; return *this; }
    bool operator!=(const counting_iterator<T>& other) { return current != other.current; }
};

/// Support class for Python-style range
template<typename T>
struct counting_container {
    T min, max;
    
    counting_container(const T& min, const T& max) : min(min), max(max) { }
    
    counting_iterator<T> begin() { return counting_iterator<T>(min); }
    counting_iterator<T> end() { return counting_iterator<T>(max); }
};

/// Python-style range: iterates from 0 to max in range-based for loops
inline counting_container<int> range(int max) { return counting_container<int>(0,max); }
/// Python-style range: iterates from min to max in range-based for loops
inline counting_container<int> range(int min, int max) { return counting_container<int>(min,max); }
    
///@}
    
#endif