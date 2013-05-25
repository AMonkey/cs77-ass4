#ifndef _STDMATH_H_
#define _STDMATH_H_

#include <cmath>
#include <cstdlib>

///@file vmath/stdmath.h Basic math functions. @ingroup vmath
///@defgroup stdmath Basic math functions
///@ingroup vmath
///@{

#define PIf 3.14159265f
#define PI 3.1415926535897932384626433832795

/// pi (float)
const float pif = PIf;
/// pi (double)
const double pid = PI;
/// pi
const double pi = PI;

///@name std math brought into namespace
///@{
using std::abs;
using std::exp;
using std::log;
using std::log10;
using std::sqrt;
using std::pow;
using std::sin;
using std::cos;
using std::tan;
using std::asin;
using std::acos;
using std::atan;
using std::atan2;
using std::ceil;
using std::floor;
//using std::isinf;
using std::isfinite;
///@}

///@name math utilities
///@{
inline int max(int a, int b) { return (a > b)?a:b; }
inline int min(int a, int b) { return (a < b)?a:b; }
inline int clamp(int x, int m, int M) { return min(M,max(m,x)); }
inline float max(float a, float b) { return (a > b)?a:b; }
inline float min(float a, float b) { return (a < b)?a:b; }
inline float clamp(float x, float m, float M) { return min(M,max(m,x)); }
inline double max(double a, double b) { return (a > b)?a:b; }
inline double min(double a, double b) { return (a < b)?a:b; }
inline double clamp(double x, double m, double M) { return min(M,max(m,x)); }

inline float radians(float deg) { return deg / float(180) * float(pi); }
inline float degrees(float rad) { return rad / float(pi) * float(180); }
inline double radians(double deg) { return deg / double(180) * double(pi); }
inline double degrees(double rad) { return rad / double(pi) * double(180); }

inline int pow2(int x) { int ret = 1; for(int i = 0; i < x; i ++) ret *= 2; return ret; }
inline int log2(int x) { int ret = 0; for(int v = 1; v < x; v *= 2) ret ++; return ret; }
///@}

///@}

#endif

