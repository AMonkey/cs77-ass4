#ifndef _GEOM_H_
#define _GEOM_H_

#include "vec.h"
#include "ray.h"
#include "range.h"

///@file vmath/geom.h Geometric math. @ingroup vmath
///@defgroup geom Geometric math
///@ingroup vmath
///@{

///@name general utilities
///@{
/// atan2 in [0,2*pi)
inline float atan2pos(float y, float x) { auto a = atan2(y,x); return (a >= 0) ? a : 2*pi+a; }
inline vec3f unitspherical_to_direction(const vec2f& angles) { auto phi = angles.x; auto theta = angles.y; return vec3f(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta)); }
inline vec2f direction_to_unitspherical(const vec3f& w) {
    auto p = atan2(w.y, w.x);
    auto phi = (p < 0) ? p + 2*pif : p;
    auto theta = acos(clamp(w.z, -1.0f, 1.0f));
    return vec2f(phi,theta); }
inline vec3f latlong_to_direction(const vec2f& uv) { return unitspherical_to_direction(vec2f(uv.x * 2 * pif, uv.y * pif)); }
inline vec2f direction_to_latlong(const vec3f& w) { auto a = direction_to_unitspherical(w); return vec2f(a.x / (2*pif), a.y / pif); }
///@}

///@name bounds
///@{
inline range3f triangle_bounds(const vec3f& v0, const vec3f& v1, const vec3f& v2) { return range_from_values(v0,v1,v2); }
inline range3f sphere_bounds(const vec3f& pos, float r) { return range3f(pos-vec3f(r,r,r),pos+vec3f(r,r,r)); }
inline range3f cylinder_bounds(float r, float h) { return range3f(vec3f(-r,-r,0),vec3f(r,r,h)); }
inline range3f quad_bounds(float w, float h) { return range3f(vec3f(-w/2,-h/2,0),vec3f(w/2,h/2,0)); }
inline range3f quad_bounds(const vec3f& v0, const vec3f& v1, const vec3f& v2, const vec3f& v3) { return range_from_values(v0,v1,v2,v3); }
///@}

///@name normal
///@{
inline vec3f triangle_normal(const vec3f& v0, const vec3f& v1, const vec3f& v2) { return normalize(cross(v1-v0,v2-v0)); }
inline vec3f quad_normal(const vec3f& v0, const vec3f& v1, const vec3f& v2, const vec3f& v3) { return normalize(triangle_normal(v0, v1, v2)+triangle_normal(v0, v2, v3)); }
///@}

///@name volume
///@{
inline float sphere_area(float r) { return 4*pi*r*r; }
///@}

///@name volume
///@{
inline float sphere_volume(float r) { return 4*pi/3*r*r*r; }
///@}

///@name intersection
///@{
bool intersect_bbox(const ray3f& ray, const range3f& bbox, float& t0, float& t1);
bool intersect_triangle(const ray3f& ray, const vec3f& v0, const vec3f& v1, const vec3f& v2, float& t, float& ba, float& bb);
bool intersect_sphere(const ray3f& ray, const vec3f& o, float r, float& t);
bool intersect_quad(const ray3f& ray, float w, float h, float& t, float& ba, float& bb);
bool intersect_cylinder(const ray3f& ray, float r, float h, float& t);
bool intersect_point_approximate(const ray3f& ray, const vec3f& p, float r, float& t);
bool intersect_line_approximate(const ray3f& ray, const vec3f& v0, const vec3f& v1, float r0, float r1, float& t, float& s);
///@}

///@name intersection - check only
///@{
inline bool intersect_bbox(const ray3f& ray, const range3f& bbox) { float t0, t1; return intersect_bbox(ray,bbox,t0,t1); }
inline bool intersect_triangle(const ray3f& ray, const vec3f& v0, const vec3f& v1, const vec3f& v2) { float t, ba, bb; return intersect_triangle(ray, v0, v1, v2, t, ba, bb); }
inline bool intersect_sphere(const ray3f& ray, const vec3f& o, float r) { float t; return intersect_sphere(ray, o, r, t); }
inline bool intersect_quad(const ray3f& ray, float w, float h) { float t, ba, bb; return intersect_quad(ray,w,h,t,ba,bb); }
inline bool intersect_cylinder(const ray3f& ray, float r, float h) { float t; return intersect_cylinder(ray,r,h,t); }
inline bool intersect_point_approximate(const ray3f& ray, const vec3f& p, float r) { float t; return intersect_point_approximate(ray, p, r, t); }
inline bool intersect_line_approximate(const ray3f& ray, const vec3f& v0, const vec3f& v1, float r0, float r1) { float t, s; return intersect_line_approximate(ray, v0, v1, r0, r1, t, s); }
///@}

///@}

#endif
