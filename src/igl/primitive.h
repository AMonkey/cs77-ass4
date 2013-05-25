#ifndef _PRIMITIVE_H_
#define _PRIMITIVE_H_

#include "node.h"
#include "shape.h"
#include "material.h"

#include "accelerator.h"
#include "keyframed.h"

///@file igl/primitive.h Primitives. @ingroup igl
///@defgroup primitive Primitives
///@ingroup igl
///@{

/// Abstract Primitive
struct Primitive : Node {
    REGISTER_FAST_RTTI(Node,Primitive,9)
    
    frame3f              frame; ///< frame
    Material*            material = nullptr; ///< material
};

/// Group of Primitives
struct PrimitiveGroup : Node {
    REGISTER_FAST_RTTI(Node,PrimitiveGroup,10)
    
	vector<Primitive*>       prims; ///< primitives
    
    BVHAccelerator*         _intersect_accelerator = nullptr; ///< intersection accelerator
    bool                    intersect_accelerator_use = true; ///< whether to use an intersection accelerator
};

/// Basic Surface
struct Surface : Primitive {
    REGISTER_FAST_RTTI(Primitive,Surface,1)
    
    Shape*               shape = nullptr; ///< shape
};

/// Surface Transformed with aributrary and animated transformations
struct TransformedSurface : Primitive {
    REGISTER_FAST_RTTI(Primitive,TransformedSurface,2)
    
    Shape*              shape = nullptr; ///< shape
    
    frame3f             pivot = identity_frame3f; ///< transformation center and orientation
    
    vec3f               translation = zero3f; ///< translation
    KeyframedValue*     anim_translation = nullptr; ///< translation keyframed animation
    vec3f               rotation_euler = zero3f; ///< rotation along main axis (ZYX order)
    KeyframedValue*     anim_rotation_euler = nullptr; ///< rotation keyframed animation
    vec3f               scale = one3f; ///< scaling
    KeyframedValue*     anim_scale = nullptr; ///< scaling keyframed animation
};

///@name TransformedShape animation support
///@{
inline bool transformed_animated(TransformedSurface* transformed) {
    return transformed->anim_translation or transformed->anim_rotation_euler or transformed->anim_scale;
}
inline range1f transformed_animation_interval(TransformedSurface* transformed) {
    range1f ret;
    if(transformed->anim_translation) ret = runion(ret, keyframed_interval(transformed->anim_translation));
    if(transformed->anim_rotation_euler) ret = runion(ret, keyframed_interval(transformed->anim_rotation_euler));
    if(transformed->anim_scale) ret = runion(ret, keyframed_interval(transformed->anim_scale));
    return ret;
}
mat4f transformed_matrix(TransformedSurface* transformed, float time);
mat4f transformed_matrix_inv(TransformedSurface* transformed, float time);
///@}

///@name animation interface
///@{
range1f primitive_animation_interval(Primitive* prim);
inline range1f primitives_animation_interval(PrimitiveGroup* group) {
    auto ret = range1f();
    for(auto p : group->prims) ret = runion(ret,primitive_animation_interval(p));
    return ret;
}
void primitive_animation_snapshot(Primitive* prim, float time);
inline void primitives_animation_snapshot(PrimitiveGroup* group, float time) {
    for(auto p : group->prims) primitive_animation_snapshot(p, time);
}
///@}

///@}

#endif
