#ifndef _SHAPE_H_
#define _SHAPE_H_

#include "node.h"

///@file igl/shape.h Shapes. @ingroup igl
///@defgroup shape Shapes
///@ingroup igl
///@{

struct BVHAccelerator;
struct Texture;

/// Abstract Shape
struct Shape : Node {
    REGISTER_FAST_RTTI(Node,Shape,14)
    
    BVHAccelerator*     _intersect_accelerator = nullptr; ///< intersection accelerator
    bool                intersect_accelerator_use = true; ///< whether to use the intersection accelerator

    Shape*              _tesselation = nullptr; ///< shape tesselation
};

/// Sphere aligned along Z axis
struct Sphere : Shape {
    REGISTER_FAST_RTTI(Shape,Sphere,1)
    
    vec3f           center = zero3f; ///< center
    float           radius = 1; ///< radius
};

/// Cylinder at the origin aligned along the Z axis
struct Cylinder : Shape {
    REGISTER_FAST_RTTI(Shape,Cylinder,2)
    
    float           radius = 1; ///< radius
    float           height = 1; ///< height
};

/// Quad at the origin in the XY plane
struct Quad : Shape {
    REGISTER_FAST_RTTI(Shape,Quad,3)
    
    float           width = 1;
    float           height = 1;
};

/// Triangle defined by vertices
struct Triangle : Shape {
    REGISTER_FAST_RTTI(Shape,Triangle,4)
    
    vec3f           v0 = vec3f(cos(radians( 90.0f)),sin(radians( 90.0f)),0.0f); ///< vertex 0
    vec3f           v1 = vec3f(cos(radians(210.0f)),sin(radians(210.0f)),0.0f); ///< vertex 1
    vec3f           v2 = vec3f(cos(radians(330.0f)),sin(radians(330.0f)),0.0f); ///< vertex 2
};

/// Sets of points with per-vertex properties (one vertex per point)
struct PointSet : Shape {
    REGISTER_FAST_RTTI(Shape,PointSet,5)
    
    vector<vec3f>       pos; ///< point position
    vector<float>       radius; ///< point radius
    vector<vec2f>       texcoord; ///< point texture coordinate
    
    bool                approximate = false; ///< whether to use approximate rendering
    float               approximate_radius = 2; ///< approximate radius/thickness
};

/// Sets of indexed lines with per-vertex properites
struct LineSet : Shape {
    REGISTER_FAST_RTTI(Shape,LineSet,6)
    
    vector<vec3f>       pos; ///< vertex position
    vector<float>       radius; ///< vertex radius
    vector<vec2f>       texcoord; ///< vertex texcoords (can be empty)
    
    vector<vec2i>       line; ///< line segment, each with two vertex indices

    bool                approximate = false; ///< whether to use approximate rendering
    float               approximate_radius = 2; ///< approximate radius/thickness
};

/// Triangle mesh with per-vertex properties
struct TriangleMesh : Shape {
    REGISTER_FAST_RTTI(Shape,TriangleMesh,7)
    
    vector<vec3f>       pos; ///< vertex position
    vector<vec3f>       norm; ///< vertex normal (can be empty, then switch to face normals)
    vector<vec2f>       texcoord; ///< vertex texcoords (can be empty)
    
    vector<vec3i>       triangle; ///< triangle list with three vertex indices per triangle
    
    vector<vec2i>       _tesselation_lines; ///< highkighted line segments (used for tesselation)
};

/// Mesh with triangles and quads with per-vertex properties
struct Mesh : Shape {
    REGISTER_FAST_RTTI(Shape,Mesh,8)
    
    vector<vec3f>       pos; ///< vertex position
    vector<vec3f>       norm; ///< vertex normal (can be empty, then switch to face normals)
    vector<vec2f>       texcoord; ///< vertex texcoords (can be empty)
    
    vector<vec3i>       triangle; ///< triangle list with three vertex indices per triangle
    vector<vec4i>       quad; ///< quad list with four vertex indices per quad

    vector<vec2i>       _tesselation_lines; ///< highkighted line segments (used for tesselation)
};

/// Mesh with triangles and quads with indirected per-vertex properties; the indirection allows vertices to share some properties (e.g. pos) but not others (i.e. norm or texcoord) 
struct FaceMesh : Shape {
    REGISTER_FAST_RTTI(Shape,FaceMesh,9)
    
    vector<vec3f>       pos; ///< vertex position
    vector<vec3f>       norm; ///< vertex normal (can be empty, then switch to face normals)
    vector<vec2f>       texcoord; ///< vertex texcoords (can be empty)
    
    vector<vec3i>       vertex; ///< vertex indirection indices: position(x), frames(y), other(z)
    vector<vec3i>       triangle; ///< triangle list with three vertex indices per triangle
    vector<vec4i>       quad; ///< quad list with four vertex indices per quad
    
    vector<vec2i>       _tesselation_lines; ///< highkighted line segments (used for tesselation)
};

/// Catmull-Clark subdivision surface on a pure quad mesh
struct CatmullClarkSubdiv : Shape {
    REGISTER_FAST_RTTI(Shape,CatmullClarkSubdiv,10)
    
    vector<vec3f>           pos; ///< vertex position
    vector<vec3f>           norm; ///< vertex normal (can be empty, then switch to subdiv normals)
    vector<vec2f>           texcoord; ///< vertex texcoords (can be empty)
    
    vector<vec4i>           quad; ///< quad list with four vertex indices per quad
    
    int                     level = 2; ///< tesselation level
    bool                    smooth = true; ///< tesselation smooth frames
    
    vector<vec2i>           _tesselation_lines; ///< highkighted line segments (used for tesselation)
};

/// Mixed quad/triangle subdivision surface with creases on a triangle and quad mesh (becomes a Loop subdiv for for triangles-only and a Catmull-Clark subdiv for quads-only)
struct Subdiv : Shape {
    REGISTER_FAST_RTTI(Shape,Subdiv,11)
    
    vector<vec3f>           pos; ///< vertex position
    vector<vec3f>           norm; ///< vertex normal (can be empty, then switch to subdiv normals)
    vector<vec2f>           texcoord; ///< vertex texcoords (can be empty)
    
    vector<vec3i>           triangle; ///< triangle list with three vertex indices per triangle
    vector<vec4i>           quad; ///< quad list with four vertex indices per quad
    
    vector<vec2i>           crease_edge; ///< creased edge list with two vertex indices per creased edge
    vector<int>             crease_vertex; ///< creased vertex list with a vertex index per crease
    
    int                     level = 2; ///< tesselation level
    bool                    smooth = true; ///< tesselation smooth frames
    
    vector<vec2i>           _tesselation_lines; ///< highkighted line segments (used for tesselation)
};

/// List of bezier spline segments with per-vertex properties
struct Spline : Shape {
    REGISTER_FAST_RTTI(Shape,Spline,12)
    
    vector<vec3f>           pos; ///< vertex position
    vector<float>           radius; ///< vertex radius
    vector<vec2f>           texcoord; ///< vertex texcoords (can be empty)
    
    vector<vec4i>           cubic; ///< list of cubic bezier segment with four vertex indices per segment
    
    bool                    continous = false; ///< whether the curve is continous 
    
    int                     level = 2; ///< tesselation level
    bool                    smooth = true; ///< tesselation smooth frames
};

/// List of bezier patches with per-vertex properties
struct Patch : Shape {
    REGISTER_FAST_RTTI(Shape,Patch,13)
    
    vector<vec3f>           pos; ///< vertex position
    vector<vec2f>           texcoord; ///< vertex texcoords (can be empty)
    
    vector<mat4i>           cubic; ///< list of cubic bezier patches with four-by-four vertex indices per patches
    
    int                     continous_stride = 0;  ///< if the patch is continous, indicates the number of cubic patches per row, otherwise 0
    
    int                     level = 2; ///< tesselation level
    bool                    smooth = true; ///< tesselation smooth frames
};

/// Forces tesselation on a base shape
struct TesselationOverride : Shape {
    REGISTER_FAST_RTTI(Shape,TesselationOverride,14)
    
    Shape*              shape = nullptr; ///< base shape
    
    int                 level = 2; ///< tesselation level
    bool                smooth = true; ///< tesselation smooth frames
};

/// Displacement-mapped shape
struct DisplacedShape : Shape {
    REGISTER_FAST_RTTI(Shape,DisplacedShape,16)
    
    Shape*              shape = nullptr; ///< base shape
    Texture*            displacement = nullptr; ///< displacement texture map
    float               height = 1; ///< displacement height (scale texture values)
    
    int                 level = 2; ///< tesselation level
    bool                smooth = true; ///< tesselation smooth frames
};

///@name bezier spline.patch utilities
///@{
inline int spline_continous_segment(Spline* spline, float u) {
    return clamp(int(u * spline->cubic.size()), 0, spline->cubic.size()-1);
}
inline float spline_continous_param(Spline* spline, float u) {
    auto s = spline_continous_segment(spline, u);
    return u * spline->cubic.size() - s;
}

inline vec2i patch_continous_segment_xy(Patch* patch, const vec2f& uv) {
    auto segments = vec2i(patch->continous_stride,patch->cubic.size()/patch->continous_stride);
    return vec2i(clamp(int(uv.x * segments.x), 0, segments.x-1),
                 clamp(int(uv.y * segments.y), 0, segments.y-1));
}
inline int patch_continous_segment(Patch* patch, const vec2f& uv) {
    auto segments = vec2i(patch->continous_stride,patch->cubic.size()/patch->continous_stride);
    auto segment_xy = patch_continous_segment_xy(patch, uv);
    return segment_xy.y * segments.x + segment_xy.x;
}
inline vec2f patch_continous_param(Patch* patch, const vec2f& uv) {
    auto segments = vec2i(patch->continous_stride,patch->cubic.size()/patch->continous_stride);
    auto segment_xy = patch_continous_segment_xy(patch, uv);
    return vec2f(uv.x * segments.x - segment_xy.x,
                 uv.y * segments.y - segment_xy.y);
}
///@}

///@name shape clone
///@{
Shape* shape_clone(Shape* shape);
///@}

///@name shape frame interface
///@{
frame3f sphere_frame(Sphere* sphere, const vec2f& uv);
frame3f cylinder_frame(Cylinder* cylinder, const vec2f& uv);
frame3f quad_frame(Quad* quad, const vec2f& uv);
frame3f triangle_frame(Triangle* triangle, const vec2f& uv);

frame3f spline_frame(Spline* spline, int elementid, float u);
float spline_radius(Spline* spline, int elementid, float u);
frame3f patch_frame(Patch* patch, int elementid, const vec2f& uv);

frame3f pointset_frame(PointSet* pointset, int elementid, const vec2f& uv);
frame3f lineset_cylinder_frame(LineSet* lines, int elementid);
frame3f lineset_frame(LineSet* lines, int elementid, const vec2f& uv);
frame3f trianglemesh_frame(TriangleMesh* mesh, int elementid, const vec2f& uv);
frame3f mesh_frame(Mesh* mesh, int elementid, const vec2f& uv);
frame3f facemesh_frame(FaceMesh* mesh, int elementid, const vec2f& uv);
///@}

///@name mesh utilities
///@{
inline vec3i mesh_triangle_face(Mesh* mesh, int elementid) {
    if(elementid < mesh->triangle.size()) return mesh->triangle[elementid];
    else {
        auto f = mesh->quad[(elementid - mesh->triangle.size())/2];
        if((elementid - mesh->triangle.size()) % 2 == 0) return vec3i(f.x,f.y,f.z);
        else return vec3i(f.x,f.z,f.w);
    }
}
inline vec3i facemesh_triangle_face(FaceMesh* mesh, int elementid) {
    if(elementid < mesh->triangle.size()) return mesh->triangle[elementid];
    else {
        auto f = mesh->quad[(elementid - mesh->triangle.size())/2];
        if((elementid - mesh->triangle.size()) % 2 == 0) return vec3i(f.x,f.y,f.z);
        else return vec3i(f.x,f.z,f.w);
    }
}
///@}

///@name vertex access interface
///@{
vector<vec3f>* shape_get_pos(Shape* shape);
vector<vec3f>* shape_get_norm(Shape* shape);
vector<vec2f>* shape_get_texcoord(Shape* shape);
///@}

///@name shape sampling interface
///@{
struct ShapeSample {
    frame3f     frame = identity_frame3f; ///< local shading frame
    float       area = 1; ///< shape area
};

ShapeSample shape_sample_uniform(Shape* shape, const vec2f& uv);
///@}

///@name shape frame smoothing interface
///@{
bool shape_has_smooth_frames(Shape* shape);
void shape_smooth_frames(Shape* shape);
void shape_clear_frames(Shape* shape);
///@}

///@name shape convertions
///@{
TriangleMesh* mesh_to_trianglemesh(Mesh* mesh);
Mesh* trianglemesh_to_mesh(TriangleMesh* mesh);
Subdiv* mesh_to_subdiv(Mesh* mesh, const vector<vec2i>& crease_edges = {}, const vector<int>& crease_vertices = {});
Mesh* subdiv_to_mesh(Subdiv* subdiv);
CatmullClarkSubdiv* mesh_to_catmullclark(Mesh* mesh);
Mesh* catmullclark_to_mesh(CatmullClarkSubdiv* subdiv);
///@}

///@}

#endif
