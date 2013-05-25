#ifndef _INTERSECT_H_
#define _INTERSECT_H_

#include "vmath/vmath.h"
#include "common/std.h"

///@file igl/intersect.h Intersection. @ingroup igl
///@defgroup intersect Intersection
///@ingroup igl
///@{

struct Material;
struct Scene;
struct Shape;

/// intersection record
struct intersection3f {
	float                   ray_t; ///< ray parameter
	frame3f                 frame; ///< intersection frame
	vec3f                   geom_norm; ///< intersection geometric normal
	vec2f                   uv; ///< intersection shape uv
	vec2f                   texcoord; ///< intersection texcoord
	Material*               material; ///< intersection material
};

/// transform an intersection elements by a frame
inline intersection3f transform_intersection(const frame3f& frame, const intersection3f& intersection) {
    auto ret = intersection;
    ret.frame = transform_frame(frame,intersection.frame);
    ret.geom_norm = transform_normal(frame,intersection.geom_norm);
    return ret;
}

/// transform an intersection elements by a matrix (and its invere)
inline intersection3f transform_intersection(const mat4f& m, const mat4f& mi, const intersection3f& intersection) {
    auto ret = intersection;
    ret.frame = transform_frame(m,intersection.frame);
    ret.geom_norm = transform_normal(transpose(mi),intersection.geom_norm);
    return ret;
}

///@name intersection interface
///@{
void intersect_scene_accelerate(Scene* scene);
range3f intersect_scene_bounds(Scene* scene);

bool intersect_scene_first(Scene* scene, const ray3f& ray, intersection3f& intersection);
bool intersect_scene_any(Scene* scene, const ray3f& ray);

bool intersect_shape_first(Shape* shape, const ray3f& ray, intersection3f& intersection);
void intersect_shape_accelerate(Shape* shape);


///@}

///@}

#endif
