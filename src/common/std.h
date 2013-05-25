#ifndef _STD_H_
#define _STD_H_

#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <array>
#include <functional>

///@file common/std.h Brings std functionality. @ingroup common

using std::string;
using std::to_string;

using std::vector;
using std::array;

using std::map;
using std::pair;

using std::swap;

using std::function;
using std::bind;
using namespace std::placeholders;

using std::tuple;
using std::make_tuple;
using std::get;

//using std::static_pointer_cast;  //< are these used? gcc-4.7 does not recognize
//using std::dynamic_pointer_cast;

using uint = unsigned int;

#endif
