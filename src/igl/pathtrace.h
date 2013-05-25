#ifndef _PATHTRACE_H_
#define _PATHTRACE_H_

#include "scene.h"

///@file igl/pathtrace.h Pathtracing. @ingroup igl
///@defgroup pathtrace Pathtracing
///@ingroup igl
///@{

/// Pathtracing options
struct PathtraceOptions : Node {
    REGISTER_FAST_RTTI(Node,PathtraceOptions,18)
    
    int res = 512; ///< image resolution
    int samples = 64; ///< antialiasing samples
    bool doublesided = true; ///< double sided rendering
    float time = 0; ///< time at which to draw
    
    vec3f   background = zero3f; ///< image background
    
    bool    cameralights = false; ///< whether to use camera lights
    vec3f   ambient = {0,0,0}; ///< ambient light color
    vector<vec3f> cameralights_dir = { {1,-1,-1}, {-1,-1,-1}, {-1,1,0} }; //< camera lights directions
    vector<vec3f> cameralights_col = { {1,1,1}, {0.5,0.5,0.5}, {0.25,0.25,0.25} }; ///< camera light colors

    int max_depth = 4; ///< maximum ray recursion for indirect
    bool shadows = true; ///< whether to compute shadows
    bool indirect = true; ///< whether to compute indirect
    bool reflections = true; ///< whether to compute reflections
    int indirect_samples = 16; ///< number of indirect samples
    int shadow_samples = 16; ///< max number of shadow samples
    
    float image_scale = 1; ///< scale vaalue for image pixels
    float image_gamma = 1; ///< gamma value for image pixels
    
    Rng rng; ///< random number generator
};


///@}

#endif