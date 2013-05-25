#ifndef _DRAW_H_
#define _DRAW_H_

#include "scene.h"

///@file igl/draw.h Interactive Drawing. @ingroup igl
///@defgroup draw Interactive Drawing
///@ingroup igl
///@{

/// Draw and shade options
struct DrawOptions : Node {
    REGISTER_FAST_RTTI(Node,DrawOptions,16)
    
    int res = 512; ///< image resolution
    int samples = 4; ///< anti-aliasing samples
    
    bool doublesided = true; ///< double sided rendering
    float time = 0; ///< time at which to draw
    
    vec3f   background = vec3f(0.25,0.25,0.25); ///< image background
    
    bool    cameralights = false; ///< whether to use camera lights
    vec3f   ambient = {0.1,0.1,0.1}; ///< ambient light value
    vector<vec3f> cameralights_dir = { {1,-1,-1}, {-1,-1,-1}, {-1,1,0} }; ///< camera light directions
    vector<vec3f> cameralights_col = { {1,1,1}, {0.5,0.5,0.5}, {0.25,0.25,0.25} }; ///< camera light colors
    
    bool faces = true; ///< whether to draw shape faces
    bool edges = true; ///< whether to draw shape edges
    bool lines = true; ///< whether to draw shape hightled lines (used for tesselation)
    bool control = true; ///< whether to draw shape control points
    bool control_no_depth = false; ///< whether to darw control without depth testing
    bool gizmos = true; ///< whether to draw gizmos
};

///@name interactive draw interface
///@{
void draw_scene(Scene* scene, const DrawOptions& opts, bool clear);
void draw_scene_decorations(Scene* scene, const DrawOptions& opts, bool clear);

void draw_shape(Shape* shape);
void draw_shape_decorations(Shape* shape, bool edges, bool lines, bool control);

void draw_gizmo(Gizmo* gizmo);
///@}

///@}

#endif
