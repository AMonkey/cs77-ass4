#ifndef _INTERPOLATE_H_
#define _INTERPOLATE_H_

#include "vec.h"
#include "mat.h"
#include "geom.h"
#include <vector>

///@file vmath/interpolate.h Interpolation support. @ingroup vmath
///@defgroup interpolate Interpolation support
///@ingroup vmath
///@{

///@name bernstein polynomials (for Bezier)
///@{
float bernstein(float u, int i, int degree);
float bernstein_derivative(float u, int i, int degree);
///@}

///@name linear interpolation
///@{
template<typename T> inline T interpolate_linear(const T& v0, const T& v1, float t) { return v0*(1-t)+v1*t; }
template<typename T> inline T interpolate_linear(const std::vector<T>& v, const vec2i& i, float t) { return _interpolate_linear(v[i.x], v[i.y], t); }
///@}

///@name bilinear interpolation
///@{
template<typename T> inline T interpolate_bilinear(const T& v00, const T& v10, const T& v11, const T& v01, const vec2f& uv) { return v00*(1-uv.x)*(1-uv.y)+v10*uv.x*(1-uv.y)+v11*uv.x*uv.y+v01*(1-uv.x)*uv.y; }
///@}

///@name cubic bezier interpolation
///@{
template<typename T> inline T interpolate_bezier_cubic(const T& v0, const T& v1, const T& v2, const T& v3, float t) {
    return v0*bernstein(t,0,3)+v1*bernstein(t,1,3)+v2*bernstein(t,2,3)+v3*bernstein(t,3,3); 
}
template<typename T> inline T interpolate_bezier_cubic(const std::vector<T>& v, const vec4i& i, float t) { return interpolate_bezier_cubic(v[i.x], v[i.y], v[i.z], v[i.w], t); }
template<typename T> inline T interpolate_bezier_cubic_derivative(const T& v0, const T& v1, const T& v2, const T& v3, float t) { return v0*bernstein_derivative(t,0,3)+v1*bernstein_derivative(t,1,3)+v2*bernstein_derivative(t,2,3)+v3*bernstein_derivative(t,3,3); }
template<typename T> inline T interpolate_bezier_cubic_derivative(const std::vector<T>& v, const vec4i& i, float t) { return interpolate_bezier_cubic_derivative(v[i.x], v[i.y], v[i.z], v[i.w], t); }
///@}

///@name bicubic bezier interpolation
///@{
template<typename T> inline T interpolate_bezier_bicubic(const std::vector<T>& v, const mat4i& idx, const vec2f& uv) {
    auto ret = T();
    for(int i = 0; i < 4; i ++) for(int j = 0; j < 4; j ++) ret += v[idx[i][j]] * bernstein(uv.x, i, 3) * bernstein(uv.y, j, 3);
    return ret;
}
template<typename T> inline T interpolate_bezier_bicubic_derivativex(const std::vector<T>& v, const mat4i& idx, const vec2f& uv) {
    auto ret = T();
    for(int i = 0; i < 4; i ++) for(int j = 0; j < 4; j ++) ret += v[idx[i][j]] * bernstein_derivative(uv.x, i, 3) * bernstein(uv.y, j, 3);
    return ret;
}
template<typename T> inline T interpolate_bezier_bicubic_derivativey(const std::vector<T>& v, const mat4i& idx, const vec2f& uv) {
    auto ret = T();
    for(int i = 0; i < 4; i ++) for(int j = 0; j < 4; j ++) ret += v[idx[i][j]] * bernstein(uv.x, i, 3) * bernstein_derivative(uv.y, j, 3);
    return ret;
}
///@}

///@name baricentric triangle interpolation
///@{
template<typename T> inline T interpolate_baricentric_triangle(const T& v0, const T& v1, const T& v2, const vec2f& uv) { return v0*uv.x+v1*uv.y+v2*(1-uv.x-uv.y); }
template<typename T> inline T interpolate_baricentric_triangle(const std::vector<T>& v, const vec3i& i, const vec2f& uv) { return interpolate_baricentric_triangle(v[i.x], v[i.y], v[i.z], uv); }
///@}

///@name approximate quad interpolation (treated as 2 triangles)
///@{
template<typename T> inline T interpolate_approximate_quad(const T& v0, const T& v1, const T& v2, const T& v3, const vec2f& uv) {
    return (uv.x+uv.y<=1) ? interpolate_baricentric_triangle(v0,v1,v2,uv) : interpolate_baricentric_triangle(v0,v2,v3,one2f-uv);
}
template<typename T> inline T interpolate_approximate_quad(const std::vector<T>& v, const vec4i& i, const vec2f& uv) { return interpolate_approximate_quad(v[i.x], v[i.y], v[i.z], v[i.w], uv); }
///@}

///@}

#endif