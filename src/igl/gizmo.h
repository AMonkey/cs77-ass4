#ifndef _GIZMO_H_
#define _GIZMO_H_

#include "node.h"

///@file igl/gizmo.h Gizmos. @ingroup igl
///@defgroup gizmo Gizmos
///@ingroup igl
///@{

const int _steps = 4;

/// Abstract Gizmo
struct Gizmo : Node {
    REGISTER_FAST_RTTI(Node,Gizmo,3)
};

/// Gizmos Group
struct GizmoGroup : Node {
    REGISTER_FAST_RTTI(Node,GizmoGroup,4)
    vector<Gizmo*>  gizmos; ///< gizmos
};

/// Grid Gizmo
struct Grid : Gizmo {
    REGISTER_FAST_RTTI(Gizmo,Grid,1)
    
    frame3f     frame = identity_frame3f; ///< frame
    vec3f       color = vec3f(0.3,0.3,0.3); ///< color
    int         steps = 10; ///< number of lines
    float       size = 10; ///< grid size
    float       thickness = 1; ///< line thickness
};

/// Axes Gizmo
struct Axes : Gizmo {
    REGISTER_FAST_RTTI(Gizmo,Axes,2)
    
    frame3f     frame = identity_frame3f; ///< frame
    float       size = 2; ///< axis size
    float       thickness = 2; ///< line thickness
    vec3f       color_x = vec3f(0.5,0.125,0.125); ///< x axis color
    vec3f       color_y = vec3f(0.125,0.5,0.125); ///< y axis color
    vec3f       color_z = vec3f(0.125,0.125,0.5); ///< z axis color
};

/// Line Gizmo
struct Line : Gizmo {
    REGISTER_FAST_RTTI(Gizmo,Line,3)
    
    vec3f           pos0; ///< end point
    vec3f           pos1; ///< end point
    float           thickness = 2; ///< line thickness
    vec3f           color = one3f; ///< line color
};

/// Dot Gizmo
struct Dot : Gizmo {
    REGISTER_FAST_RTTI(Gizmo,Dot,4)
    
    vec3f       pos = zero3f; ///< position
    float       thickness = 4; ///< point size
    vec3f       color = one3f; ///< point color
};

///@}

#endif
