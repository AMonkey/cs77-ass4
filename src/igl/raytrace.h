#ifndef _RAYTRACE_H_
#define _RAYTRACE_H_

#include "scene.h"

///@file igl/raytrace.h Raytracing. @ingroup igl
///@defgroup raytrace Raytracing
///@ingroup igl
///@{

/// Raytracing options
struct RaytraceOptions : Node {
    REGISTER_FAST_RTTI(Node,RaytraceOptions,17)
    
    int res = 512; ///< image resolution
    int samples = 4; ///< antialiasing samples
    bool doublesided = true; ///< double sided rendering
    float time = 0; ///< time at which to draw
    
    vec3f   background = vec3f(0.0,0.0,0.0); ///< image background
    
    bool    cameralights = false; ///< whether to use camera lights
    vec3f   ambient = {0.1,0.1,0.1}; ///< ambient light color
    vector<vec3f> cameralights_dir = { {1,-1,-1}, {-1,-1,-1}, {-1,1,0} }; //< camera lights directions
    vector<vec3f> cameralights_col = { {1,1,1}, {0.5,0.5,0.5}, {0.25,0.25,0.25} }; ///< camera light colors
    
    bool shadows = true; ///< whether to compute shadows
    bool reflections = true; ///< whether to compute reflections
    
    int max_depth = 4; ///< maximum ray recursion for reflections
    
    Rng rng; ///< random number generator
};

///@name raytrace interface
///@{

void raytrace_scene_progressive(ImageBuffer& buffer, Scene* scene, const RaytraceOptions& opts);

///@}

///@}

#endif
