#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "frame.h"
#include "mat.h"
#include "range.h"
#include "ray.h"

///@file vmath/transform.h Transforms. @ingroup vmath
///@defgroup transform Transforms
///@ingroup vmath
///@{

///@name Matrix Transforms Creation
///@{
template<typename T> inline mat4<T> translation_matrix(const vec3<T>& t) { return mat4<T>(1,0,0,t.x,  0,1,0,t.y,  0,0,1,t.z,  0,0,0,1); }
template<typename T> inline mat4<T> rotation_matrix(const T& angle, const vec3<T>& axis) { T c = cos(angle); T s = sin(angle); vec3<T> vv = normalize(axis); return mat4<T>(c + (1-c)*vv.x*vv.x, (1-c)*vv.x*vv.y - s*vv.z, (1-c)*vv.x*vv.z + s*vv.y, 0,  (1-c)*vv.x*vv.y + s*vv.z, c + (1-c)*vv.y*vv.y, (1-c)*vv.y*vv.z - s*vv.x, 0,  (1-c)*vv.x*vv.z - s*vv.y, (1-c)*vv.y*vv.z + s*vv.x, c + (1-c)*vv.z*vv.z, 0,  0,0,0,1); }
template<typename T> inline mat4<T> scaling_matrix(const vec3<T>& s) { return mat4<T>(s.x,0,0,0,  0,s.y,0,0,  0,0,s.z,0,  0,0,0,1); }
template<typename T> inline mat4<T> frustum_matrix(T l, T r, T b, T t, T n, T f) { return mat4<T>(2*n/(r-l), 0, (r+l)/(r-l), 0,  0, 2*n/(t-b), (t+b)/(t-b), 0,  0, 0, -(f+n)/(f-n), -2*f*n/(f-n), 0, 0, -1, 0); }
template<typename T> inline mat4<T> ortho_matrix(T l, T r, T b, T t, T n, T f) { return mat4<T>(2/(r-l), 0, 0, -(r+l)/(r-l),  0, 2/(t-b), 0, -(t+b)/(t-b),  0, 0, -2/(f-n), -(f+n)/(f-n), 0, 0, 0, 1); }

template<typename T> inline mat4<T> lookat_matrix(const vec3<T>& eye, const vec3<T>& center, const vec3<T>& up) { vec3<T> w = normalize(eye - center); vec3<T> u = normalize(cross(up,w)); vec3<T> v = cross(w,u); return mat4<T>(u.x, u.y, u.z, -dot(u,eye),  v.x, v.y, v.z, -dot(v,eye),  w.x, w.y, w.z, -dot(w,eye),  0, 0, 0, 1); }
template<typename T> inline mat4<T> ortho2d_matrix(T l, T r, T b, T t) { return ortho(l,r,b,t,-1,1); }
template<typename T> inline mat4<T> perspective_matrix(T fovy, T aspect, T near, T far) { T f = 1/tan(fovy/2); return mat4<T>(f/aspect,0,0,0,   0,f,0,0,    0,0,(far+near)/(near-far),2*far*near/(near-far),    0,0,-1,0); }
///@}

///@name Matrix-Element Transforms
///@{
template<typename T> inline vec3<T> transform_point(const mat4<T>& m, const vec3<T>& v) { auto tv = m * vec4<T>(v.x,v.y,v.z,1); return vec3<T>(tv.x,tv.y,tv.z) / tv.w; }
template<typename T> inline vec3<T> transform_vector(const mat4<T>& m, const vec3<T>& v) { auto tv = m * vec4<T>(v.x,v.y,v.z,0); return vec3<T>(tv.x,tv.y,tv.z); }
template<typename T> inline vec3<T> transform_direction(const mat4<T>& m, const vec3<T>& v) { return normalize(transform_vector(m,v)); }
// requires inverse transform
template<typename T> inline vec3<T> transform_normal(const mat4<T>& m, const vec3<T>& v) { return normalize(transform_vector(m,v)); }
template<typename T> inline ray3<T> transform_ray(const mat4<T>& m, const ray3<T>& v) { return ray3<T>(transform_point(m,v.e),transform_vector(m,v.d),v.tmin,v.tmax); }
template<typename T> inline range3<T> transform_bbox(const mat4<T>& m, const range3<T>& v) { range3<T> ret; for(auto vv : corners(v)) ret = runion(ret,transform_point(m,vv)); return ret; }
template<typename T> inline frame3<T> transform_frame(const mat4<T>& m, const frame3<T>& v) { frame3<T> ret; ret.o = transform_point(m,v.o); ret.x = transform_direction(m,v.x); ret.y = transform_direction(m,v.y); ret.z = cross(ret.x,ret.y); ret = orthonormalize(ret); return ret; }
///@}

///@name 2D Frame-Element Transforms
///@{
template<typename T> inline vec2<T> transform_point(const frame2<T>& f, const vec2<T>& v) { return f.o + f.x * v.x + f.y * v.y; }
template<typename T> inline vec2<T> transform_vector(const frame2<T>& f, const vec2<T>& v) { return f.x * v.x + f.y * v.y; }
template<typename T> inline vec2<T> transform_direction(const frame2<T>& f, const vec2<T>& v) { return transform_vector(f,v); }
template<typename T> inline frame2<T> transform_frame(const frame2<T>& f, const frame2<T>& v) { return frame2<T>(transform_point(f,v.o),transform_vector(f,v.x), transform_vector(f,v.y)); }
///@}

///@name 2D Frame-Element Inverse Transforms
///@{
template<typename T> inline vec2<T> transform_point_inverse(const frame2<T>& f, const vec2<T>& v) { return vec2<T>(dot(v-f.o,f.x), dot(v-f.o,f.y)); }
template<typename T> inline vec2<T> transform_vector_inverse(const frame2<T>& f, const vec2<T>& v) { return vec2<T>(dot(v,f.x), dot(v,f.y)); }
template<typename T> inline vec2<T> transform_direction_inverse(const frame2<T>& f, const vec2<T>& v) { return transform_vector_inverse(f,v); }
template<typename T> inline vec2<T> transform_normal_inverse(const frame2<T>& f, const vec2<T>& v) { return transform_vector_inverse(f,v); }
template<typename T> inline frame2<T> transform_frame_inverse(const frame2<T>& f, const frame2<T>& v) { return frame2<T>(transform_point_inverse(f,v.o),transform_vector_inverse(f,v.x), transform_vector_inverse(f,v.y)); }
///@}

///@name 3D Frame-Element Transforms
///@{
template<typename T> inline vec3<T> transform_point(const frame3<T>& f, const vec3<T>& v) { return f.o + f.x * v.x + f.y * v.y + f.z * v.z; }
template<typename T> inline vec3<T> transform_vector(const frame3<T>& f, const vec3<T>& v) { return f.x * v.x + f.y * v.y + f.z * v.z; }
template<typename T> inline vec3<T> transform_direction(const frame3<T>& f, const vec3<T>& v) { return transform_vector(f,v); }
template<typename T> inline vec3<T> transform_normal(const frame3<T>& f, const vec3<T>& v) { return transform_vector(f,v); }
template<typename T> inline frame3<T> transform_frame(const frame3<T>& f, const frame3<T>& v) { return frame3<T>(transform_point(f,v.o), transform_vector(f,v.x), transform_vector(f,v.y), transform_vector(f,v.z)); }
template<typename T> inline ray3<T> transform_ray(const frame3<T>& f, const ray3<T>& v) { return ray3<T>(transform_point(f,v.e), transform_vector(f,v.d), v.tmin, v.tmax); }
template<typename T> inline range3<T> transform_bbox(const frame3<T>& f, const range3<T>& v) { range3<T> ret; for(auto vv : corners(v)) ret = runion(ret, transform_point(f,vv)); return ret; }
///@}

///@name 3D Frame-Element Inverse Transforms
///@{
template<typename T> inline vec3<T> transform_point_inverse(const frame3<T>& f, const vec3<T>& v) { return vec3<T>(dot(v-f.o,f.x), dot(v-f.o,f.y), dot(v-f.o,f.z)); }
template<typename T> inline vec3<T> transform_vector_inverse(const frame3<T>& f, const vec3<T>& v) { return vec3<T>(dot(v,f.x), dot(v,f.y), dot(v,f.z)); }
template<typename T> inline vec3<T> transform_direction_inverse(const frame3<T>& f, const vec3<T>& v) { return transform_vector_inverse(f,v); }
template<typename T> inline vec3<T> transform_normal_inverse(const frame3<T>& f, const vec3<T>& v) { return transform_vector_inverse(f,v); }
template<typename T> inline frame3<T> transform_frame_inverse(const frame3<T>& f, const frame3<T>& v) { return frame3<T>(transform_point_inverse(f,v.o), transform_vector_inverse(f,v.x), transform_vector_inverse(f,v.y), transform_vector_inverse(f,v.z)); }
template<typename T> inline ray3<T> transform_ray_inverse(const frame3<T>& f, const ray3<T>& v) { return ray3<T>(transform_point_inverse(f,v.e),transform_vector_inverse(f,v.d),v.tmin,v.tmax); }
template<typename T> inline range3<T> transform_bbox_inverse(const frame3<T>& f, const range3<T>& v) { range3<T> ret; for(auto vv : corners(v)) ret = runion(ret,transform_point_inverse(f,vv)); return ret; }
///@}

///@name Frame-Matrix Conversion
///@{
template<typename T> inline mat4<T> frame_to_matrix(const frame3<T>& f) { return mat4<T>(f.x.x, f.y.x, f.z.x,f.o.x, f.x.y, f.y.y, f.z.y,f.o.y, f.x.z, f.y.z, f.z.z,f.o.z, 0,0,0,1); }
template<typename T> inline mat4<T> frame_to_matrix_inverse(const frame3<T>& f) { return mat4<T>(f.x.x, f.x.y, f.x.z, -dot(f.o,f.x), f.y.x, f.y.y, f.y.z, -dot(f.o,f.y), f.z.x, f.z.y, f.z.z, -dot(f.o,f.z), 0,0,0,1); }
template<typename T> inline frame3<T> matrix_to_frame(const mat4<T>& m) { WARNING_IF_NOT(isaffine(m), "not affine"); frame3<T> f; f.o = vec3<T>(m.x.w, m.y.w, m.z.w); f.x = vec3<T>(m.x.x, m.y.x, m.z.x); f.y = vec3<T>(m.x.y, m.y.y, m.z.y); f.z = vec3<T>(m.x.z, m.y.z, m.z.z); return f; }
///@}

///@}

#endif

