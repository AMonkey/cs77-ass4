#include "primitive.h"

///@file igl/primitive.cpp Primitives. @ingroup igl

mat4f transformed_matrix(TransformedSurface* transformed, float time) {
    auto m = identity_mat4f;
    
    m = frame_to_matrix_inverse(transformed->pivot) * m;
    
    auto scale = transformed->scale * ((transformed->anim_scale) ? keyframed_value(transformed->anim_scale,time) : one3f);
    m = scaling_matrix(scale) * m;
    
    auto rotation_euler = transformed->rotation_euler + ((transformed->anim_rotation_euler) ? keyframed_value(transformed->anim_rotation_euler,time) : zero3f);
    m = rotation_matrix(rotation_euler.x,x3f) * m;
    m = rotation_matrix(rotation_euler.y,y3f) * m;
    m = rotation_matrix(rotation_euler.z,z3f) * m;
    
    auto translation = transformed->translation + ((transformed->anim_translation) ? keyframed_value(transformed->anim_translation,time) : zero3f);
    m = translation_matrix(translation) * m;

    m = frame_to_matrix(transformed->pivot) * m;

    return m;
}

mat4f transformed_matrix_inv(TransformedSurface* transformed, float time) {
    auto mi = identity_mat4f;
    
    mi = mi * frame_to_matrix_inverse(transformed->pivot);
    
    auto scale = transformed->scale * ((transformed->anim_scale) ? keyframed_value(transformed->anim_scale,time) : one3f);
    mi = mi * scaling_matrix(1/scale);
    
    auto rotation_euler = transformed->rotation_euler + ((transformed->anim_rotation_euler) ? keyframed_value(transformed->anim_rotation_euler,time) : zero3f);
    mi = mi * rotation_matrix(-rotation_euler.x,x3f);
    mi = mi * rotation_matrix(-rotation_euler.y,y3f);
    mi = mi * rotation_matrix(-rotation_euler.z,z3f);
    
    auto translation = transformed->translation + ((transformed->anim_translation) ? keyframed_value(transformed->anim_translation,time) : zero3f);
    mi = mi * translation_matrix(-translation);
    
    mi = mi * frame_to_matrix(transformed->pivot);
    
    return mi;
}

range1f primitive_animation_interval(Primitive* prim) {
    if(is<TransformedSurface>(prim)) return transformed_animation_interval(cast<TransformedSurface>(prim));
    else return range1f();
}

void primitive_animation_snapshot(Primitive* prim, float time) {
    if(is<TransformedSurface>(prim)) {
        auto transformed = cast<TransformedSurface>(prim);
        if(not transformed_animated(transformed)) return;
        transformed->translation += (transformed->anim_translation) ? keyframed_value(transformed->anim_translation,time): zero3f;
        transformed->rotation_euler += (transformed->anim_rotation_euler) ? keyframed_value(transformed->anim_rotation_euler,time): zero3f;
        transformed->scale *= (transformed->anim_scale) ? keyframed_value(transformed->anim_scale,time): one3f;
        // TODO: free memory
        transformed->anim_translation = nullptr;
        transformed->anim_rotation_euler = nullptr;
        transformed->anim_scale = nullptr;
    }
    else return;
}

