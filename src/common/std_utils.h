#ifndef _STD_UTILS_H_
#define _STD_UTILS_H_

#include "std.h"
#include <chrono>

///@file common/std_utils.h Utilities based on std. @ingroup common
///@defgroup std_utils Utilities based on std
///@ingroup common
///@{

struct timer {
    using _clock = std::chrono::high_resolution_clock;
    using _time_point = _clock::time_point;
    
    _time_point _start_time;

    timer() { start(); }
    
    void start() { _start_time = _now(); }

    double operator()() { return elapsed(); }
    double elapsed() { return std::chrono::duration_cast<std::chrono::microseconds>(_now()-_start_time).count() / 1000000.0f; }
    
    static _time_point _now() { return _clock::now(); }
};

struct timer_avg {
    timer _t;
    double _elapsed = 0;
    unsigned long int _count = 0;

    void start() { _t.start(); }
    void stop() { _elapsed += _t.elapsed(); _count ++; }
    void reset() { _elapsed = 0; _count = 0; }
    
    double elapsed() { return (_count) ? _elapsed / _count : 0; }
};

// TODO: this is only for small strings!!!
template<typename T>
inline static string _to_string(const char* fmt, const T& value) {
    char buf[1024];
    sprintf(buf, fmt, value);
    return buf;
}

inline string to_string(const char* fmt, int value) { return _to_string(fmt, value); }
inline string to_string(const char* fmt, float value) { return _to_string(fmt, value); }
inline string to_string(const char* fmt, double value) { return _to_string(fmt, value); }

inline bool string_endswith(const string& s, const string& end) {
    auto offset = s.length() - end.length();
    for (int i = 0; i < end.size(); i ++) {
        if(s[offset+i] != end[i]) return false;
    }
    return true;
}

///@}

#endif
