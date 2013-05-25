#ifndef _MAT_H_
#define _MAT_H_

#include "vec.h"

///@file vmath/mat.h Matrix math. @ingroup vmath
///@defgroup mat Matrix math
///@ingroup vmath
///@{

/// 2x2 Matrix
template<typename T>
struct mat2 {
    vec2<T> x; ///< 1st row
    vec2<T> y; ///< 2nd row

    /// Default constructor (identity matrix)
    mat2<T>() : x(1, 0), y(0, 1) { }
    /// Row-setting constrution
    mat2<T>(const vec2<T>& x, const vec2<T>& y) : x(x), y(y) { }
    /// Element-setting constrution
    mat2<T>(const T& x_x, const T& x_y , const T& y_x, const T& y_y ) : x(x_x, x_y), y(y_x, y_y) { }

    /// Row access
    vec2<T>& operator[](int i) { return (&x)[i]; }
    /// Row access
    const vec2<T>& operator[](int i) const { return (&x)[i]; }

    /// Row size
    static int size() { return 2; }

    /// Iterator support
    vec2<T>* begin() { return &x; }
    /// Iterator support
    vec2<T>* end() { return (&x)+2; }
    /// Iterator support
    const vec2<T>* begin() const { return &x; }
    /// Iterator support
    const vec2<T>* end() const { return (&x)+2; }

    /// Data access
    vec2<T>* data() { return &x; }
    /// Data access
    const vec2<T>* data() const { return &x; }

    /// Raw data access
    T* raw_data() { return x.raw_data(); }
    /// Raw data access
    const T* raw_data() const { return x.raw_data(); }
    /// Raw data access
    static int raw_size() { return 2*vec2<T>::raw_size(); }
};

///@name 2x2 Matrix Typedefs
///@{
using mat2f = mat2<float>;
using mat2d = mat2<double>;
using mat2i = mat2<int>;
///@}

///@name 2x2 Matrix Constants
///@{
const mat2f identity_mat2f = mat2f();
const mat2d identity_mat2d = mat2d();
const mat2i identity_mat2i = mat2i();
///@}

///@name 2x2 Component-wise equality
///@{
template<typename T> inline bool operator==(const mat2<T>& a, const mat2<T>& b) { return a.x==b.x && a.y==b.y; }
///@}

///@name 2x2 Linear Algebra operations
///@{
template<typename T> inline mat2<T> operator-(const mat2<T>& a) { return mat2<T>(-a.x, -a.y); }
template<typename T> inline mat2<T> operator+(const mat2<T>& a, const mat2<T>& b) { return mat2<T>(a.x+b.x, a.y+b.y); }
template<typename T> inline mat2<T>& operator+=(mat2<T>& a, const mat2<T>& b) { a.x+=b.x; a.y+=b.y; return a; }
template<typename T> inline mat2<T> operator-(const mat2<T>& a, const mat2<T>& b) { return mat2<T>(a.x-b.x, a.y-b.y); }
template<typename T> inline mat2<T>& operator-=(mat2<T>& a, const mat2<T>& b) { a.x-=b.x; a.y-=b.y; return a; }
template<typename T> inline mat2<T> operator*(const mat2<T>& a, const T& b) { return mat2<T>(a.x*b, a.y*b); }
template<typename T> inline mat2<T> operator*(const T& a, const mat2<T>& b) { return mat2<T>(a*b.x, a*b.y); }
template<typename T> inline mat2<T>& operator*=(mat2<T>& a, const T& b) { a.x*=b; a.y*=b; return a; }
template<typename T> inline mat2<T> operator/(const mat2<T>& a, const T& b) { return mat2<T>(a.x/b, a.y/b); }
template<typename T> inline mat2<T>& operator/=(mat2<T>& a, const T& b) { a.x/=b; a.y/=b; return a; }

template<typename T> inline mat2<T> operator*(const mat2<T>& a, const mat2<T>& b) { return mat2<T>(a.x.x*b.x.x+a.x.y*b.y.x , a.x.x*b.x.y+a.x.y*b.y.y  , a.y.x*b.x.x+a.y.y*b.y.x , a.y.x*b.x.y+a.y.y*b.y.y  ); }
template<typename T> inline mat2<T>& operator*=(mat2<T>& a, const mat2<T>& b) { a = a*b; return a; }
template<typename T> inline vec2<T> operator*(const mat2<T>& a, const vec2<T>& b) { return vec2<T>(dot(a.x,b), dot(a.y,b)); }

template<typename T> inline mat2<T> transpose(const mat2<T>& a) { return mat2<T>(a.x.x, a.y.x , a.x.y, a.y.y ); }
template<typename T> inline T determinant(const mat2<T>& a) { return a.x.x*a.y.y-a.x.y*a.y.x; }
template<typename T> inline mat2<T> inverse(const mat2<T>& a) { T d = determinant(a); T di = 1/d; return mat2<T>(a.y.y,-a.x.y,-a.x.y,a.x.x)*di; }
///@}

/// 3x3 Matrix
template<typename T>
struct mat3 {
    vec3<T> x, y, z;

    mat3<T>() : x(1, 0, 0), y(0, 1, 0), z(0, 0, 1) { }
    mat3<T>(const vec3<T>& x, const vec3<T>& y, const vec3<T>& z) : x(x), y(y), z(z) { }
    mat3<T>(const T& x_x, const T& x_y, const T& x_z , const T& y_x, const T& y_y, const T& y_z , const T& z_x, const T& z_y, const T& z_z ) : x(x_x, x_y, x_z), y(y_x, y_y, y_z), z(z_x, z_y, z_z) { }

    vec3<T>& operator[](int i) { return (&x)[i]; }
    const vec3<T>& operator[](int i) const { return (&x)[i]; }

    static int size() { return 3; }

    vec3<T>* begin() { return &x; }
    vec3<T>* end() { return (&x)+3; }
    const vec3<T>* begin() const { return &x; }
    const vec3<T>* end() const { return (&x)+3; }

    vec3<T>* data() { return &x; }
    const vec3<T>* data() const { return &x; }

    T* raw_data() { return x.raw_data(); }
    const T* raw_data() const { return x.raw_data(); }
    static int raw_size() { return 3*vec3<T>::raw_size(); }
};

///@name 3x3 Matrix Typedefs
///@{
using mat3f = mat3<float>;
using mat3d = mat3<double>;
using mat3i = mat3<int>;
///@}

///@name 3x3 Matrix Constants
///@{
const mat3f identity_mat3f = mat3f();
const mat3d identity_mat3d = mat3d();
const mat3i identity_mat3i = mat3i();
///@}

///@name 3x3 Component-wise equality
///@{
template<typename T> inline bool operator==(const mat3<T>& a, const mat3<T>& b) { return a.x==b.x && a.y==b.y && a.z==b.z; }
///@}

///@name 3x3 Linear Algebra operations
///@{
template<typename T> inline mat3<T> operator-(const mat3<T>& a) { return mat3<T>(-a.x, -a.y, -a.z); }
template<typename T> inline mat3<T> operator+(const mat3<T>& a, const mat3<T>& b) { return mat3<T>(a.x+b.x, a.y+b.y, a.z+b.z); }
template<typename T> inline mat3<T>& operator+=(mat3<T>& a, const mat3<T>& b) { a.x+=b.x; a.y+=b.y; a.z+=b.z; return a; }
template<typename T> inline mat3<T> operator-(const mat3<T>& a, const mat3<T>& b) { return mat3<T>(a.x-b.x, a.y-b.y, a.z-b.z); }
template<typename T> inline mat3<T>& operator-=(mat3<T>& a, const mat3<T>& b) { a.x-=b.x; a.y-=b.y; a.z-=b.z; return a; }
template<typename T> inline mat3<T> operator*(const mat3<T>& a, const T& b) { return mat3<T>(a.x*b, a.y*b, a.z*b); }
template<typename T> inline mat3<T> operator*(const T& a, const mat3<T>& b) { return mat3<T>(a*b.x, a*b.y, a*b.z); }
template<typename T> inline mat3<T>& operator*=(mat3<T>& a, const T& b) { a.x*=b; a.y*=b; a.z*=b; return a; }
template<typename T> inline mat3<T> operator/(const mat3<T>& a, const T& b) { return mat3<T>(a.x/b, a.y/b, a.z/b); }
template<typename T> inline mat3<T>& operator/=(mat3<T>& a, const T& b) { a.x/=b; a.y/=b; a.z/=b; return a; }

template<typename T> inline mat3<T> operator*(const mat3<T>& a, const mat3<T>& b) { return mat3<T>(a.x.x*b.x.x+a.x.y*b.y.x+a.x.z*b.z.x , a.x.x*b.x.y+a.x.y*b.y.y+a.x.z*b.z.y , a.x.x*b.x.z+a.x.y*b.y.z+a.x.z*b.z.z  , a.y.x*b.x.x+a.y.y*b.y.x+a.y.z*b.z.x , a.y.x*b.x.y+a.y.y*b.y.y+a.y.z*b.z.y , a.y.x*b.x.z+a.y.y*b.y.z+a.y.z*b.z.z  , a.z.x*b.x.x+a.z.y*b.y.x+a.z.z*b.z.x , a.z.x*b.x.y+a.z.y*b.y.y+a.z.z*b.z.y , a.z.x*b.x.z+a.z.y*b.y.z+a.z.z*b.z.z  ); }
template<typename T> inline mat3<T>& operator*=(mat3<T>& a, const mat3<T>& b) { a = a*b; return a; }
template<typename T> inline vec3<T> operator*(const mat3<T>& a, const vec3<T>& b) { return vec3<T>(dot(a.x,b), dot(a.y,b), dot(a.z,b)); }

template<typename T> inline mat3<T> transpose(const mat3<T>& a) { return mat3<T>(a.x.x, a.y.x, a.z.x , a.x.y, a.y.y, a.z.y , a.x.z, a.y.z, a.z.z ); }
template<typename T> inline T determinant(const mat3<T>& a) { return +a.x.x*(a.y.y*a.z.z-a.y.z*a.z.y)-a.x.y*(a.y.x*a.z.z-a.y.z*a.z.x)+a.x.z*(a.y.x*a.z.y-a.y.y*a.z.x); }
template<typename T> inline mat3<T> inverse(const mat3<T>& a) { T d = determinant(a); T di = 1/d; return mat3<T>(+(a.y.y*a.z.z-a.y.z*a.z.y), -(a.x.y*a.z.z-a.x.z*a.z.y), +(a.x.y*a.y.z-a.x.z*a.y.y), -(a.y.x*a.z.z-a.y.z*a.z.x), +(a.x.x*a.z.z-a.x.z*a.z.x), -(a.x.x*a.y.z-a.x.z*a.y.x), +(a.y.x*a.z.y-a.y.y*a.z.x), -(a.x.x*a.z.y-a.x.y*a.z.x), +(a.x.x*a.y.y-a.x.y*a.y.x))*di; }
///@}

/// 4x4 Matrix
template<typename T>
struct mat4 {
    vec4<T> x, y, z, w;

    mat4<T>() : x(1, 0, 0, 0), y(0, 1, 0, 0), z(0, 0, 1, 0), w(0, 0, 0, 1) { }
    mat4<T>(const vec4<T>& x, const vec4<T>& y, const vec4<T>& z, const vec4<T>& w) : x(x), y(y), z(z), w(w) { }
    mat4<T>(const T& x_x, const T& x_y, const T& x_z, const T& x_w , const T& y_x, const T& y_y, const T& y_z, const T& y_w , const T& z_x, const T& z_y, const T& z_z, const T& z_w , const T& w_x, const T& w_y, const T& w_z, const T& w_w ) : x(x_x, x_y, x_z, x_w), y(y_x, y_y, y_z, y_w), z(z_x, z_y, z_z, z_w), w(w_x, w_y, w_z, w_w) { }

    vec4<T>& operator[](int i) { return (&x)[i]; }
    const vec4<T>& operator[](int i) const { return (&x)[i]; }

    static int size() { return 4; }

    vec4<T>* begin() { return &x; }
    vec4<T>* end() { return (&x)+4; }
    const vec4<T>* begin() const { return &x; }
    const vec4<T>* end() const { return (&x)+4; }

    vec4<T>* data() { return &x; }
    const vec4<T>* data() const { return &x; }

    T* raw_data() { return x.raw_data(); }
    const T* raw_data() const { return x.raw_data(); }
    static int raw_size() { return 4*vec4<T>::raw_size(); }
};

///@name 4x4 Matrix Typedefs
///@{
using mat4f = mat4<float>;
using mat4d = mat4<double>;
using mat4i = mat4<int>;
///@}

///@name 4x4 Matrix Constants
///@{
const mat4f identity_mat4f = mat4f();
const mat4d identity_mat4d = mat4d();
const mat4i identity_mat4i = mat4i();
///@}

///@name 4x4 Component-wise equality
///@{
template<typename T> inline bool operator==(const mat4<T>& a, const mat4<T>& b) { return a.x==b.x && a.y==b.y && a.z==b.z && a.w==b.w; }
///@}

///@name 4x4 Linear Algebra operations
///@{
template<typename T> inline mat4<T> operator-(const mat4<T>& a) { return mat4<T>(-a.x, -a.y, -a.z, -a.w); }
template<typename T> inline mat4<T> operator+(const mat4<T>& a, const mat4<T>& b) { return mat4<T>(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w); }
template<typename T> inline mat4<T>& operator+=(mat4<T>& a, const mat4<T>& b) { a.x+=b.x; a.y+=b.y; a.z+=b.z; a.w+=b.w; return a; }
template<typename T> inline mat4<T> operator-(const mat4<T>& a, const mat4<T>& b) { return mat4<T>(a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w); }
template<typename T> inline mat4<T>& operator-=(mat4<T>& a, const mat4<T>& b) { a.x-=b.x; a.y-=b.y; a.z-=b.z; a.w-=b.w; return a; }
template<typename T> inline mat4<T> operator*(const mat4<T>& a, const T& b) { return mat4<T>(a.x*b, a.y*b, a.z*b, a.w*b); }
template<typename T> inline mat4<T> operator*(const T& a, const mat4<T>& b) { return mat4<T>(a*b.x, a*b.y, a*b.z, a*b.w); }
template<typename T> inline mat4<T>& operator*=(mat4<T>& a, const T& b) { a.x*=b; a.y*=b; a.z*=b; a.w*=b; return a; }
template<typename T> inline mat4<T> operator/(const mat4<T>& a, const T& b) { return mat4<T>(a.x/b, a.y/b, a.z/b, a.w/b); }
template<typename T> inline mat4<T>& operator/=(mat4<T>& a, const T& b) { a.x/=b; a.y/=b; a.z/=b; a.w/=b; return a; }

template<typename T> inline mat4<T> operator*(const mat4<T>& a, const mat4<T>& b) { return mat4<T>(a.x.x*b.x.x+a.x.y*b.y.x+a.x.z*b.z.x+a.x.w*b.w.x , a.x.x*b.x.y+a.x.y*b.y.y+a.x.z*b.z.y+a.x.w*b.w.y , a.x.x*b.x.z+a.x.y*b.y.z+a.x.z*b.z.z+a.x.w*b.w.z , a.x.x*b.x.w+a.x.y*b.y.w+a.x.z*b.z.w+a.x.w*b.w.w  , a.y.x*b.x.x+a.y.y*b.y.x+a.y.z*b.z.x+a.y.w*b.w.x , a.y.x*b.x.y+a.y.y*b.y.y+a.y.z*b.z.y+a.y.w*b.w.y , a.y.x*b.x.z+a.y.y*b.y.z+a.y.z*b.z.z+a.y.w*b.w.z , a.y.x*b.x.w+a.y.y*b.y.w+a.y.z*b.z.w+a.y.w*b.w.w  , a.z.x*b.x.x+a.z.y*b.y.x+a.z.z*b.z.x+a.z.w*b.w.x , a.z.x*b.x.y+a.z.y*b.y.y+a.z.z*b.z.y+a.z.w*b.w.y , a.z.x*b.x.z+a.z.y*b.y.z+a.z.z*b.z.z+a.z.w*b.w.z , a.z.x*b.x.w+a.z.y*b.y.w+a.z.z*b.z.w+a.z.w*b.w.w  , a.w.x*b.x.x+a.w.y*b.y.x+a.w.z*b.z.x+a.w.w*b.w.x , a.w.x*b.x.y+a.w.y*b.y.y+a.w.z*b.z.y+a.w.w*b.w.y , a.w.x*b.x.z+a.w.y*b.y.z+a.w.z*b.z.z+a.w.w*b.w.z , a.w.x*b.x.w+a.w.y*b.y.w+a.w.z*b.z.w+a.w.w*b.w.w  ); }
template<typename T> inline mat4<T>& operator*=(mat4<T>& a, const mat4<T>& b) { a = a*b; return a; }
template<typename T> inline vec4<T> operator*(const mat4<T>& a, const vec4<T>& b) { return vec4<T>(dot(a.x,b), dot(a.y,b), dot(a.z,b), dot(a.w,b)); }

template<typename T> inline mat4<T> transpose(const mat4<T>& a) { return mat4<T>(a.x.x, a.y.x, a.z.x, a.w.x , a.x.y, a.y.y, a.z.y, a.w.y , a.x.z, a.y.z, a.z.z, a.w.z , a.x.w, a.y.w, a.z.w, a.w.w ); }
template<typename T> inline T determinant(const mat4<T>& a) { return +a.x.x*(+a.y.y*(a.z.z*a.w.w-a.z.w*a.w.z)-a.y.z*(a.z.y*a.w.w-a.z.w*a.w.y)+a.y.w*(a.z.y*a.w.z-a.z.z*a.w.y))-a.x.y*(+a.y.x*(a.z.z*a.w.w-a.z.w*a.w.z)-a.y.z*(a.z.x*a.w.w-a.z.w*a.w.x)+a.y.w*(a.z.x*a.w.z-a.z.z*a.w.x))+a.x.z*(+a.y.x*(a.z.y*a.w.w-a.z.w*a.w.y)-a.y.y*(a.z.x*a.w.w-a.z.w*a.w.x)+a.y.w*(a.z.x*a.w.y-a.z.y*a.w.x))-a.x.w*(+a.y.x*(a.z.y*a.w.z-a.z.z*a.w.y)-a.y.y*(a.z.x*a.w.z-a.z.z*a.w.x)+a.y.z*(a.z.x*a.w.y-a.z.y*a.w.x)); }
template<typename T> inline mat4<T> inverse(const mat4<T>& a) { T d = determinant(a); T di = 1/d; return mat4<T>(+(+a.y.y*(a.z.z*a.w.w-a.z.w*a.w.z)-a.y.z*(a.z.y*a.w.w-a.z.w*a.w.y)+a.y.w*(a.z.y*a.w.z-a.z.z*a.w.y)), -(+a.x.y*(a.z.z*a.w.w-a.z.w*a.w.z)-a.x.z*(a.z.y*a.w.w-a.z.w*a.w.y)+a.x.w*(a.z.y*a.w.z-a.z.z*a.w.y)), +(+a.x.y*(a.y.z*a.w.w-a.y.w*a.w.z)-a.x.z*(a.y.y*a.w.w-a.y.w*a.w.y)+a.x.w*(a.y.y*a.w.z-a.y.z*a.w.y)), -(+a.x.y*(a.y.z*a.z.w-a.y.w*a.z.z)-a.x.z*(a.y.y*a.z.w-a.y.w*a.z.y)+a.x.w*(a.y.y*a.z.z-a.y.z*a.z.y)), -(+a.y.x*(a.z.z*a.w.w-a.z.w*a.w.z)-a.y.z*(a.z.x*a.w.w-a.z.w*a.w.x)+a.y.w*(a.z.x*a.w.z-a.z.z*a.w.x)), +(+a.x.x*(a.z.z*a.w.w-a.z.w*a.w.z)-a.x.z*(a.z.x*a.w.w-a.z.w*a.w.x)+a.x.w*(a.z.x*a.w.z-a.z.z*a.w.x)), -(+a.x.x*(a.y.z*a.w.w-a.y.w*a.w.z)-a.x.z*(a.y.x*a.w.w-a.y.w*a.w.x)+a.x.w*(a.y.x*a.w.z-a.y.z*a.w.x)), +(+a.x.x*(a.y.z*a.z.w-a.y.w*a.z.z)-a.x.z*(a.y.x*a.z.w-a.y.w*a.z.x)+a.x.w*(a.y.x*a.z.z-a.y.z*a.z.x)), +(+a.y.x*(a.z.y*a.w.w-a.z.w*a.w.y)-a.y.y*(a.z.x*a.w.w-a.z.w*a.w.x)+a.y.w*(a.z.x*a.w.y-a.z.y*a.w.x)), -(+a.x.x*(a.z.y*a.w.w-a.z.w*a.w.y)-a.x.y*(a.z.x*a.w.w-a.z.w*a.w.x)+a.x.w*(a.z.x*a.w.y-a.z.y*a.w.x)), +(+a.x.x*(a.y.y*a.w.w-a.y.w*a.w.y)-a.x.y*(a.y.x*a.w.w-a.y.w*a.w.x)+a.x.w*(a.y.x*a.w.y-a.y.y*a.w.x)), -(+a.x.x*(a.y.y*a.z.w-a.y.w*a.z.y)-a.x.y*(a.y.x*a.z.w-a.y.w*a.z.x)+a.x.w*(a.y.x*a.z.y-a.y.y*a.z.x)), -(+a.y.x*(a.z.y*a.w.z-a.z.z*a.w.y)-a.y.y*(a.z.x*a.w.z-a.z.z*a.w.x)+a.y.z*(a.z.x*a.w.y-a.z.y*a.w.x)), +(+a.x.x*(a.z.y*a.w.z-a.z.z*a.w.y)-a.x.y*(a.z.x*a.w.z-a.z.z*a.w.x)+a.x.z*(a.z.x*a.w.y-a.z.y*a.w.x)), -(+a.x.x*(a.y.y*a.w.z-a.y.z*a.w.y)-a.x.y*(a.y.x*a.w.z-a.y.z*a.w.x)+a.x.z*(a.y.x*a.w.y-a.y.y*a.w.x)), +(+a.x.x*(a.y.y*a.z.z-a.y.z*a.z.y)-a.x.y*(a.y.x*a.z.z-a.y.z*a.z.x)+a.x.z*(a.y.x*a.z.y-a.y.y*a.z.x)))*di; }

template<typename T> inline bool isaffine(const mat4<T>& m) { return m.w.x==0 and m.w.y==0 and m.w.z==0 and m.w.w==1; }
///@}

///@}

#endif
