#ifndef _VEC_H_
#define _VEC_H_

#include "stdmath.h"

///@file vmath/vec.h Vector math. @ingroup vmath
///@defgroup vec Vector math
///@ingroup vmath
///@{

/// 2D Vector
template<typename T>
struct vec2 {
    T x; ///< X component
    T y; ///< Y component
    
    /// Default constructor (set to zeros)
    vec2() : x(0), y(0) { }
    /// Element-setting constructor
    vec2(const T& x, const T& y) : x(x), y(y) { }
    /// Vector conversion
    template<typename R> explicit vec2(const vec2<R>& v) : x(v.x), y(v.y) { }

    /// Element access
    T& operator[](int i) { return (&x)[i]; }
    /// Element access
    const T& operator[](int i) const { return (&x)[i]; }

    /// Element size
    static int size() { return 2; }

    /// Iterator support
    T* begin() { return &x; }
    /// Iterator support
    T* end() { return (&x)+2; }
    /// Iterator support
    const T* begin() const { return &x; }
    /// Iterator support
    const T* end() const { return (&x)+2; }

    /// Data access
    T* data() { return &x; }
    /// Data access
    const T* data() const { return &x; }

    /// Raw data access
    T* raw_data() { return &x; }
    const T* raw_data() const { return &x; }
    static int raw_size() { return 2; }
};

///@name 2D Vector Typedefs
///@{
/// 2D float vector
using vec2f = vec2<float>;
/// 2D double vector
using vec2d = vec2<double>;
/// 2D int vector
using vec2i = vec2<int>;
/// 2D unsigned byte vector
using vec2ub = vec2<unsigned char>;
///@}

///@name 2D Vector Constants
///@{
const vec2f zero2f = vec2f(0, 0);
const vec2f one2f = vec2f(1, 1);
const vec2f x2f = vec2f(1, 0);
const vec2f y2f = vec2f(0, 1);

const vec2d zero2d = vec2d(0, 0);
const vec2d one2d = vec2d(1, 1);
const vec2d x2d = vec2d(1, 0);
const vec2d y2d = vec2d(0, 1);

const vec2i zero2i = vec2i(0, 0);
const vec2i one2i = vec2i(1, 1);

const vec2ub zero2ub = vec2ub(0, 0);
const vec2ub one2ub = vec2ub(1, 1);
///@}

///@name 2D Component-wise equality
///@{
template<typename T> inline bool operator==(const vec2<T>& a, const vec2<T>& b) { return a.x==b.x and a.y==b.y; }
///@}

///@name 2D Component-wise arithmetic operators
///@{
template<typename T> inline vec2<T> operator-(const vec2<T>& a) { return vec2<T>(-a.x, -a.y); }
template<typename T> inline vec2<T> operator+(const vec2<T>& a, const vec2<T>& b) { return vec2<T>(a.x+b.x, a.y+b.y); }
template<typename T> inline vec2<T>& operator+=(vec2<T>& a, const vec2<T>& b) { a.x+=b.x; a.y+=b.y; return a; }
template<typename T> inline vec2<T> operator-(const vec2<T>& a, const vec2<T>& b) { return vec2<T>(a.x-b.x, a.y-b.y); }
template<typename T> inline vec2<T>& operator-=(vec2<T>& a, const vec2<T>& b) { a.x-=b.x; a.y-=b.y; return a; }
template<typename T> inline vec2<T> operator*(const vec2<T>& a, const vec2<T>& b) { return vec2<T>(a.x*b.x, a.y*b.y); }
template<typename T> inline vec2<T>& operator*=(vec2<T>& a, const vec2<T>& b) { a.x*=b.x; a.y*=b.y; return a; }
template<typename T> inline vec2<T> operator/(const vec2<T>& a, const vec2<T>& b) { return vec2<T>(a.x/b.x, a.y/b.y); }
template<typename T> inline vec2<T>& operator/=(vec2<T>& a, const vec2<T>& b) { a.x/=b.x; a.y/=b.y; return a; }
template<typename T> inline vec2<T> operator*(const vec2<T>& a, const T& b) { return vec2<T>(a.x*b, a.y*b); }
template<typename T> inline vec2<T> operator*(const T& a, const vec2<T>& b) { return vec2<T>(a*b.x, a*b.y); }
template<typename T> inline vec2<T>& operator*=(vec2<T>& a, const T& b) { a.x*=b; a.y*=b; return a; }
template<typename T, typename R> inline vec2<T> operator*(const vec2<T>& a, const R& b) { return vec2<T>(a.x*b, a.y*b); }
template<typename T, typename R> inline vec2<T> operator*(const R& a, const vec2<T>& b) { return vec2<T>(a*b.x, a*b.y); }
template<typename T, typename R> inline vec2<T>& operator*=(vec2<T>& a, const R& b) { a.x*=b; a.y*=b; return a; }
template<typename T> inline vec2<T> operator/(const vec2<T>& a, const T& b) { return vec2<T>(a.x/b, a.y/b); }
template<typename T> inline vec2<T> operator/(const T& a, const vec2<T>& b) { return vec2<T>(a/b.x, a/b.y); }
template<typename T> inline vec2<T>& operator/=(vec2<T>& a, const T& b) { a.x/=b; a.y/=b; return a; }
template<typename T, typename R> inline vec2<T> operator/(const vec2<T>& a, const R& b) { return vec2<T>(a.x/b, a.y/b); }
template<typename T, typename R> inline vec2<T> operator/(const R& a, const vec2<T>& b) { return vec2<T>(a/b.x, a/b.y); }
template<typename T, typename R> inline vec2<T>& operator/=(vec2<T>& a, const R& b) { a.x/=b; a.y/=b; return a; }
///@}

///@name 2D Component-wise functions
///@{
template<typename T> inline vec2<T> max(const vec2<T>& a, const vec2<T>& b) { return vec2<T>(max(a.x,b.x), max(a.y,b.y)); }
template<typename T> inline vec2<T> max(const vec2<T>& a, const T& b) { return vec2<T>(max(a.x,b), max(a.y,b)); }
template<typename T> inline vec2<T> max(const T& a, const vec2<T>& b) { return vec2<T>(max(a,b.x), max(a,b.y)); }
template<typename T> inline vec2<T> min(const vec2<T>& a, const vec2<T>& b) { return vec2<T>(min(a.x,b.x), min(a.y,b.y)); }
template<typename T> inline vec2<T> min(const vec2<T>& a, const T& b) { return vec2<T>(min(a.x,b), min(a.y,b)); }
template<typename T> inline vec2<T> min(const T& a, const vec2<T>& b) { return vec2<T>(min(a,b.x), min(a,b.y)); }
template<typename T> inline vec2<T> clamp(const vec2<T>& x, const vec2<T>& m, const vec2<T>& M) { return vec2<T>(clamp(x.x,m.x,M.x), clamp(x.y,m.y,M.y)); }
template<typename T> inline vec2<T> clamp(const vec2<T>& x, const T& m, const T& M) { return vec2<T>(clamp(x.x,m,M), clamp(x.y,m,M)); }
template<typename T> inline T max_component(const vec2<T>& a) { T r = a.x; r = max(r,a.x); r = max(r,a.y);  return r; }
template<typename T> inline T min_component(const vec2<T>& a) { T r = a.x; r = min(r,a.x); r = min(r,a.y);  return r; }
template<typename T> inline T mean_component(const vec2<T>& a) { return (a.x + a.y) / 2; }
template<typename T> inline vec2<T> pow(const vec2<T>& a, T b) { return vec2<T>(pow(a.x,b),pow(a.y,b)); }
///@}

///@name 2D Vector Operations
///@{
template<typename T> inline T dot(const vec2<T>& a, const vec2<T>& b) { return a.x*b.x+a.y*b.y; }
template<typename T> inline T length(const vec2<T>& a) { return sqrt(dot(a,a)); }
template<typename T> inline T lengthSqr(const vec2<T>& a) { return dot(a,a); }
template<typename T> inline vec2<T> normalize(const vec2<T>& a) { auto l = length(a); if (l==0) return vec2<T>(0,0); else return a * 1/l; }
template<typename T> inline T dist(const vec2<T>& a, const vec2<T>& b) { return length(a-b); }
template<typename T> inline T distSqr(const vec2<T>& a, const vec2<T>& b) { return lengthSqr(a-b); }
template<typename T> inline void orthonormalize(vec2<T>& x, vec2<T>& y) { y = normalize(y); x = normalize(x - dot(x,y)*y); }
///@}

/// 3D Vector
template<typename T>
struct vec3 {
    T x; ///< X component
    T y; ///< Y component
    T z; ///< Z component
    
    /// Default constructor (set to zeros)
    vec3() : x(0), y(0), z(0) { }
    /// Element-setting constructor
    vec3(const T& x, const T& y, const T& z) : x(x), y(y), z(z) { }
    /// Vector conversion
    template<typename R> explicit vec3(const vec3<R>& v) : x(v.x), y(v.y), z(v.z) { }

    /// Element access
    T& operator[](int i) { return (&x)[i]; }
    /// Element access
    const T& operator[](int i) const { return (&x)[i]; }

    /// Element size
    static int size() { return 3; }

    /// Iterator support
    T* begin() { return &x; }
    /// Iterator support
    T* end() { return (&x)+3; }
    /// Iterator support
    const T* begin() const { return &x; }
    /// Iterator support
    const T* end() const { return (&x)+3; }

    /// Data access
    T* data() { return &x; }
    /// Data access
    const T* data() const { return &x; }

    /// Raw data access
    T* raw_data() { return &x; }
    /// Raw data access
    const T* raw_data() const { return &x; }
    /// Raw data access
    static int raw_size() { return 3; }
};

///@name 3D Vector Typedefs
///@{
/// 3D float vector
using vec3f = vec3<float>;
/// 3D double vector
using vec3d = vec3<double>;
/// 3D int vector
using vec3i = vec3<int>;
/// 3D unsigned byte vector
using vec3ub = vec3<unsigned char>;
///@}

///@name 3D Vector Constants
///@{
const vec3f zero3f = vec3f(0, 0, 0);
const vec3f one3f = vec3f(1, 1, 1);
const vec3f x3f = vec3f(1, 0, 0);
const vec3f y3f = vec3f(0, 1, 0);
const vec3f z3f = vec3f(0, 0, 1);

const vec3d zero3d = vec3d(0, 0, 0);
const vec3d one3d = vec3d(1, 1, 1);
const vec3d x3d = vec3d(1, 0, 0);
const vec3d y3d = vec3d(0, 1, 0);
const vec3d z3d = vec3d(0, 0, 1);

const vec3i zero3i = vec3i(0, 0, 0);
const vec3i one3i = vec3i(1, 1, 1);

const vec3ub zero3ub = vec3ub(0, 0, 0);
const vec3ub one3ub = vec3ub(1, 1, 1);
///@}

///@name 3D Component-wise equality
///@{
template<typename T> inline bool operator==(const vec3<T>& a, const vec3<T>& b) { return a.x==b.x and a.y==b.y and a.z==b.z; }
///@}

///@name 3D Component-wise arithmetic operators
///@{
template<typename T> inline vec3<T> operator-(const vec3<T>& a) { return vec3<T>(-a.x, -a.y, -a.z); }
template<typename T> inline vec3<T> operator+(const vec3<T>& a, const vec3<T>& b) { return vec3<T>(a.x+b.x, a.y+b.y, a.z+b.z); }
template<typename T> inline vec3<T>& operator+=(vec3<T>& a, const vec3<T>& b) { a.x+=b.x; a.y+=b.y; a.z+=b.z; return a; }
template<typename T> inline vec3<T> operator-(const vec3<T>& a, const vec3<T>& b) { return vec3<T>(a.x-b.x, a.y-b.y, a.z-b.z); }
template<typename T> inline vec3<T>& operator-=(vec3<T>& a, const vec3<T>& b) { a.x-=b.x; a.y-=b.y; a.z-=b.z; return a; }
template<typename T> inline vec3<T> operator*(const vec3<T>& a, const vec3<T>& b) { return vec3<T>(a.x*b.x, a.y*b.y, a.z*b.z); }
template<typename T> inline vec3<T>& operator*=(vec3<T>& a, const vec3<T>& b) { a.x*=b.x; a.y*=b.y; a.z*=b.z; return a; }
template<typename T> inline vec3<T> operator/(const vec3<T>& a, const vec3<T>& b) { return vec3<T>(a.x/b.x, a.y/b.y, a.z/b.z); }
template<typename T> inline vec3<T>& operator/=(vec3<T>& a, const vec3<T>& b) { a.x/=b.x; a.y/=b.y; a.z/=b.z; return a; }
template<typename T> inline vec3<T> operator*(const vec3<T>& a, const T& b) { return vec3<T>(a.x*b, a.y*b, a.z*b); }
template<typename T> inline vec3<T> operator*(const T& a, const vec3<T>& b) { return vec3<T>(a*b.x, a*b.y, a*b.z); }
template<typename T> inline vec3<T>& operator*=(vec3<T>& a, const T& b) { a.x*=b; a.y*=b; a.z*=b; return a; }
template<typename T, typename R> inline vec3<T> operator*(const vec3<T>& a, const R& b) { return vec3<T>(a.x*b, a.y*b, a.z*b); }
template<typename T, typename R> inline vec3<T> operator*(const R& a, const vec3<T>& b) { return vec3<T>(a*b.x, a*b.y, a*b.z); }
template<typename T, typename R> inline vec3<T>& operator*=(vec3<T>& a, const R& b) { a.x*=b; a.y*=b; a.z*=b; return a; }
template<typename T> inline vec3<T> operator/(const vec3<T>& a, const T& b) { return vec3<T>(a.x/b, a.y/b, a.z/b); }
template<typename T> inline vec3<T> operator/(const T& a, const vec3<T>& b) { return vec3<T>(a/b.x, a/b.y, a/b.z); }
template<typename T> inline vec3<T>& operator/=(vec3<T>& a, const T& b) { a.x/=b; a.y/=b; a.z/=b; return a; }
template<typename T, typename R> inline vec3<T> operator/(const vec3<T>& a, const R& b) { return vec3<T>(a.x/b, a.y/b, a.z/b); }
template<typename T, typename R> inline vec3<T> operator/(const R& a, const vec3<T>& b) { return vec3<T>(a/b.x, a/b.y, a/b.z); }
template<typename T, typename R> inline vec3<T>& operator/=(vec3<T>& a, const R& b) { a.x/=b; a.y/=b; a.z/=b; return a; }
///@}

///@name 3D Component-wise functions
///@{
template<typename T> inline vec3<T> max(const vec3<T>& a, const vec3<T>& b) { return vec3<T>(max(a.x,b.x), max(a.y,b.y), max(a.z,b.z)); }
template<typename T> inline vec3<T> max(const vec3<T>& a, const T& b) { return vec3<T>(max(a.x,b), max(a.y,b), max(a.z,b)); }
template<typename T> inline vec3<T> max(const T& a, const vec3<T>& b) { return vec3<T>(max(a,b.x), max(a,b.y), max(a,b.z)); }
template<typename T> inline vec3<T> min(const vec3<T>& a, const vec3<T>& b) { return vec3<T>(min(a.x,b.x), min(a.y,b.y), min(a.z,b.z)); }
template<typename T> inline vec3<T> min(const vec3<T>& a, const T& b) { return vec3<T>(min(a.x,b), min(a.y,b), min(a.z,b)); }
template<typename T> inline vec3<T> min(const T& a, const vec3<T>& b) { return vec3<T>(min(a,b.x), min(a,b.y), min(a,b.z)); }
template<typename T> inline vec3<T> clamp(const vec3<T>& x, const vec3<T>& m, const vec3<T>& M) { return vec3<T>(clamp(x.x,m.x,M.x), clamp(x.y,m.y,M.y), clamp(x.z,m.z,M.z)); }
template<typename T> inline vec3<T> clamp(const vec3<T>& x, const T& m, const T& M) { return vec3<T>(clamp(x.x,m,M), clamp(x.y,m,M), clamp(x.z,m,M)); }
template<typename T> inline T max_component(const vec3<T>& a) { T r = a.x; r = max(r,a.x); r = max(r,a.y); r = max(r,a.z);  return r; }
template<typename T> inline T min_component(const vec3<T>& a) { T r = a.x; r = min(r,a.x); r = min(r,a.y); r = min(r,a.z);  return r; }
template<typename T> inline T mean_component(const vec3<T>& a) { return (a.x + a.y + a.z) / 3; }
template<typename T> inline vec3<T> pow(const vec3<T>& a, T b) { return vec3<T>(pow(a.x,b),pow(a.y,b),pow(a.z,b)); }
///@}

///@name 3D Vector operations
///@{
template<typename T> inline T dot(const vec3<T>& a, const vec3<T>& b) { return a.x*b.x+a.y*b.y+a.z*b.z; }
template<typename T> inline T length(const vec3<T>& a) { return sqrt(dot(a,a)); }
template<typename T> inline T lengthSqr(const vec3<T>& a) { return dot(a,a); }
template<typename T> inline vec3<T> normalize(const vec3<T>& a) { auto l = length(a); if (l==0) return vec3<T>(0,0,0); else return a * 1/l; }
template<typename T> inline T dist(const vec3<T>& a, const vec3<T>& b) { return length(a-b); }
template<typename T> inline T distSqr(const vec3<T>& a, const vec3<T>& b) { return lengthSqr(a-b); }
template<typename T> inline vec3<T> cross(const vec3<T>& a, const vec3<T>& b) { return vec3<T>(a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x); }      
template<typename T> inline void orthonormalize(vec3<T>& x, vec3<T>& y, vec3<T>& z) { z = normalize(z); x = normalize(cross(y,z)); y = normalize(cross(z,x)); }        
template<typename T> inline void orthonormalize(vec3<T>& x, vec3<T>& y) { y = normalize(y); x = normalize(x - dot(x,y)*y); }
// v is incoming
template<typename T> inline vec3<T> reflect(const vec3<T>& v, const vec3<T>& n) { return v - 2 * dot(n,v) * n; }
template<typename T> inline vec3<T> faceforward(const vec3<T>& n, const vec3<T>& i, const vec3<T>& ng) { return dot(i, ng) < 0 ? n : -n; }
template<typename T> inline vec3<T> faceforward(const vec3<T>& n, const vec3<T>& i) { return faceforward(n,i,n); }
///@}

/// 4D Vector
template<typename T>
struct vec4 {
    T x; ///< X component
    T y; ///< Y component
    T z; ///< Z component
    T w; ///< W component
    
    /// Default constructor (set to zeros)
    vec4() : x(0), y(0), z(0), w(0) { }
    /// Element-setting constructor
    vec4(const T& x, const T& y, const T& z, const T& w) : x(x), y(y), z(z), w(w) { }
    /// Vector conversion
    template<typename R> explicit vec4(const vec4<R>& v) : x(v.x), y(v.y), z(v.z), w(v.w) { }

    /// Element access
    T& operator[](int i) { return (&x)[i]; }
    /// Element access
    const T& operator[](int i) const { return (&x)[i]; }

    /// Element size
    static int size() { return 4; }

    /// Iterator support
    T* begin() { return &x; }
    /// Iterator support
    T* end() { return (&x)+4; }
    /// Iterator support
    const T* begin() const { return &x; }
    /// Iterator support
    const T* end() const { return (&x)+4; }

    /// Data access
    T* data() { return &x; }
    /// Data access
    const T* data() const { return &x; }

    /// Raw data access
    T* raw_data() { return &x; }
    /// Raw data access
    const T* raw_data() const { return &x; }
    /// Raw data access
    static int raw_size() { return 4; }
};

///@name 4D Vector Typedefs
///@{
/// 4D float vector
using vec4f = vec4<float>;
/// 4D double vector
using vec4d = vec4<double>;
/// 4D int vector
using vec4i = vec4<int>;
/// 4D unsigned byte vector
using vec4ub = vec4<unsigned char>;
///@}

///@name 4D Vector constants
///@{
const vec4f zero4f = vec4f(0, 0, 0, 0);
const vec4f one4f = vec4f(1, 1, 1, 1);
const vec4f x4f = vec4f(1, 0, 0, 0);
const vec4f y4f = vec4f(0, 1, 0, 0);
const vec4f z4f = vec4f(0, 0, 1, 0);
const vec4f w4f = vec4f(0, 0, 0, 1);

const vec4d zero4d = vec4d(0, 0, 0, 0);
const vec4d one4d = vec4d(1, 1, 1, 1);
const vec4d x4d = vec4d(1, 0, 0, 0);
const vec4d y4d = vec4d(0, 1, 0, 0);
const vec4d z4d = vec4d(0, 0, 1, 0);
const vec4d w4d = vec4d(0, 0, 0, 1);

const vec4i zero4i = vec4i(0, 0, 0, 0);
const vec4i one4i = vec4i(1, 1, 1, 1);

const vec4ub zero4ub = vec4ub(0, 0, 0, 0);
const vec4ub one4ub = vec4ub(1, 1, 1, 1);
///@}

///@name 4D Component-wise equality
///@{
template<typename T> inline bool operator==(const vec4<T>& a, const vec4<T>& b) { return a.x==b.x and a.y==b.y and a.z==b.z and a.w==b.w; }
///@}

///@name 4D Component-wise arithemtic operations
///@{
template<typename T> inline vec4<T> operator-(const vec4<T>& a) { return vec4<T>(-a.x, -a.y, -a.z, -a.w); }
template<typename T> inline vec4<T> operator+(const vec4<T>& a, const vec4<T>& b) { return vec4<T>(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w); }
template<typename T> inline vec4<T>& operator+=(vec4<T>& a, const vec4<T>& b) { a.x+=b.x; a.y+=b.y; a.z+=b.z; a.w+=b.w; return a; }
template<typename T> inline vec4<T> operator-(const vec4<T>& a, const vec4<T>& b) { return vec4<T>(a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w); }
template<typename T> inline vec4<T>& operator-=(vec4<T>& a, const vec4<T>& b) { a.x-=b.x; a.y-=b.y; a.z-=b.z; a.w-=b.w; return a; }
template<typename T> inline vec4<T> operator*(const vec4<T>& a, const vec4<T>& b) { return vec4<T>(a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w); }
template<typename T> inline vec4<T>& operator*=(vec4<T>& a, const vec4<T>& b) { a.x*=b.x; a.y*=b.y; a.z*=b.z; a.w*=b.w; return a; }
template<typename T> inline vec4<T> operator/(const vec4<T>& a, const vec4<T>& b) { return vec4<T>(a.x/b.x, a.y/b.y, a.z/b.z, a.w/b.w); }
template<typename T> inline vec4<T>& operator/=(vec4<T>& a, const vec4<T>& b) { a.x/=b.x; a.y/=b.y; a.z/=b.z; a.w/=b.w; return a; }
template<typename T> inline vec4<T> operator*(const vec4<T>& a, const T& b) { return vec4<T>(a.x*b, a.y*b, a.z*b, a.w*b); }
template<typename T> inline vec4<T> operator*(const T& a, const vec4<T>& b) { return vec4<T>(a*b.x, a*b.y, a*b.z, a*b.w); }
template<typename T> inline vec4<T>& operator*=(vec4<T>& a, const T& b) { a.x*=b; a.y*=b; a.z*=b; a.w*=b; return a; }
template<typename T, typename R> inline vec4<T> operator*(const vec4<T>& a, const R& b) { return vec4<T>(a.x*b, a.y*b, a.z*b, a.w*b); }
template<typename T, typename R> inline vec4<T> operator*(const R& a, const vec4<T>& b) { return vec4<T>(a*b.x, a*b.y, a*b.z, a*b.w); }
template<typename T, typename R> inline vec4<T>& operator*=(vec4<T>& a, const R& b) { a.x*=b; a.y*=b; a.z*=b; a.w*=b; return a; }
template<typename T> inline vec4<T> operator/(const vec4<T>& a, const T& b) { return vec4<T>(a.x/b, a.y/b, a.z/b, a.w/b); }
template<typename T> inline vec4<T> operator/(const T& a, const vec4<T>& b) { return vec4<T>(a/b.x, a/b.y, a/b.z, a/b.w); }
template<typename T> inline vec4<T>& operator/=(vec4<T>& a, const T& b) { a.x/=b; a.y/=b; a.z/=b; a.w/=b; return a; }
template<typename T, typename R> inline vec4<T> operator/(const vec4<T>& a, const R& b) { return vec4<T>(a.x/b, a.y/b, a.z/b, a.w/b); }
template<typename T, typename R> inline vec4<T> operator/(const R& a, const vec4<T>& b) { return vec4<T>(a/b.x, a/b.y, a/b.z, a/b.w); }
template<typename T, typename R> inline vec4<T>& operator/=(vec4<T>& a, const R& b) { a.x/=b; a.y/=b; a.z/=b; a.w/=b; return a; }
///@}

///@name 4D Component-wise functions
///@{
template<typename T> inline vec4<T> max(const vec4<T>& a, const vec4<T>& b) { return vec4<T>(max(a.x,b.x), max(a.y,b.y), max(a.z,b.z), max(a.w,b.w)); }
template<typename T> inline vec4<T> max(const vec4<T>& a, const T& b) { return vec4<T>(max(a.x,b), max(a.y,b), max(a.z,b), max(a.w,b)); }
template<typename T> inline vec4<T> max(const T& a, const vec4<T>& b) { return vec4<T>(max(a,b.x), max(a,b.y), max(a,b.z), max(a,b.w)); }
template<typename T> inline vec4<T> min(const vec4<T>& a, const vec4<T>& b) { return vec4<T>(min(a.x,b.x), min(a.y,b.y), min(a.z,b.z), min(a.w,b.w)); }
template<typename T> inline vec4<T> min(const vec4<T>& a, const T& b) { return vec4<T>(min(a.x,b), min(a.y,b), min(a.z,b), min(a.w,b)); }
template<typename T> inline vec4<T> min(const T& a, const vec4<T>& b) { return vec4<T>(min(a,b.x), min(a,b.y), min(a,b.z), min(a,b.w)); }
template<typename T> inline vec4<T> clamp(const vec4<T>& x, const vec4<T>& m, const vec4<T>& M) { return vec4<T>(clamp(x.x,m.x,M.x), clamp(x.y,m.y,M.y), clamp(x.z,m.z,M.z), clamp(x.w,m.w,M.w)); }
template<typename T> inline vec4<T> clamp(const vec4<T>& x, const T& m, const T& M) { return vec4<T>(clamp(x.x,m,M), clamp(x.y,m,M), clamp(x.z,m,M), clamp(x.w,m,M)); }
template<typename T> inline T max_component(const vec4<T>& a) { T r = a.x; r = max(r,a.x); r = max(r,a.y); r = max(r,a.z); r = max(r,a.w);  return r; }
template<typename T> inline T min_component(const vec4<T>& a) { T r = a.x; r = min(r,a.x); r = min(r,a.y); r = min(r,a.z); r = min(r,a.w);  return r; }
template<typename T> inline T mean_component(const vec4<T>& a) { return (a.x + a.y + a.z + a.w) / 4; }
template<typename T> inline vec4<T> pow(const vec4<T>& a, T b) { return vec4<T>(pow(a.x,b),pow(a.y,b),pow(a.z,b),pow(a.w,b)); }
///@}

///@name 4D Vector operations
///@{
template<typename T> inline T dot(const vec4<T>& a, const vec4<T>& b) { return a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w; }
template<typename T> inline T length(const vec4<T>& a) { return sqrt(dot(a,a)); }
template<typename T> inline T lengthSqr(const vec4<T>& a) { return dot(a,a); }
template<typename T> inline vec4<T> normalize(const vec4<T>& a) { auto l = length(a); if (l==0) return vec4<T>(0,0,0,0); else return a * 1/l; }
template<typename T> inline T dist(const vec4<T>& a, const vec4<T>& b) { return length(a-b); }
template<typename T> inline T distSqr(const vec4<T>& a, const vec4<T>& b) { return lengthSqr(a-b); }
///@}

///@}

#endif

