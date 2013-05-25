#ifndef _DISTRAYTRACE_H_
#define _DISTRAYTRACE_H_

#include "scene.h"

///@file igl/distraytrace.h Distribution Raytracing. @ingroup igl
///@defgroup distraytrace Distribution Raytracing
///@ingroup igl
///@{

/// Distribution Raytracing options
struct DistributionRaytraceOptions : Node {
    REGISTER_FAST_RTTI(Node,DistributionRaytraceOptions,19)
    
    int res = 512; ///< image resolution
    int samples = 4; ///< antialiasing samples
    bool doublesided = true; ///< double sided rendering
    float time = 0; ///< time at which to draw
    int samples_ambient = 0; ///< ambient occlusion samples (0: off)
    int samples_reflect = 1; ///< reflection samples
    
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


///@}

#endif
