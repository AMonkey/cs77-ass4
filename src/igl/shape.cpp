#include "shape.h"

///@file igl/shape.cpp Shapes. @ingroup igl

Shape* shape_clone(Shape* shape) {
    if(is<Sphere>(shape)) return new Sphere(*cast<Sphere>(shape));
    else if(is<Cylinder>(shape)) return new Cylinder(*cast<Cylinder>(shape));
    else if(is<Quad>(shape)) return new Quad(*cast<Quad>(shape));
    else if(is<Triangle>(shape)) return new Triangle(*cast<Triangle>(shape));
    else if(is<PointSet>(shape)) return new PointSet(*cast<PointSet>(shape));
    else if(is<LineSet>(shape)) return new LineSet(*cast<LineSet>(shape));
    else if(is<TriangleMesh>(shape)) return new TriangleMesh(*cast<TriangleMesh>(shape));
    else if(is<Mesh>(shape)) return new Mesh(*cast<Mesh>(shape));
    else if(is<FaceMesh>(shape)) return new FaceMesh(*cast<FaceMesh>(shape));
    else if(is<Spline>(shape)) return new Spline(*cast<Spline>(shape));
    else if(is<Patch>(shape)) return new Patch(*cast<Patch>(shape));
    else if(is<CatmullClarkSubdiv>(shape)) return new CatmullClarkSubdiv(*cast<CatmullClarkSubdiv>(shape));
    else if(is<Subdiv>(shape)) return new Subdiv(*cast<Subdiv>(shape));
    else return nullptr;
}

frame3f sphere_frame(Sphere* sphere, const vec2f& uv) {
    float phi = 2 * pi * uv.x;
    float theta = pi * uv.y;
    float ct = cos(theta);
    float st = sin(theta);
    float cp = cos(phi);
    float sp = sin(phi);
    frame3f frame;
    frame.o = sphere->center + sphere->radius * vec3f(st*cp,st*sp,ct);
    frame.x = vec3f(-sp,cp,0);
    frame.y = vec3f(ct*cp,ct*sp,-st);
    frame.z = vec3f(st*cp,st*sp,ct);
    return frame;
}

frame3f cylinder_frame(Cylinder* cylinder, const vec2f& uv) {
    float phi = 2 * pif * uv.x;
    float hh = cylinder->height*uv.y;
    float cp = cos(phi);
    float sp = sin(phi);
    frame3f frame;
    frame.o = vec3f(cylinder->radius*cp,cylinder->radius*sp,hh);
    frame.x = vec3f(-sp,cp,0);
    frame.y = z3f;
    frame.z = vec3f(cp,sp,0);
    return frame;
}

frame3f quad_frame(Quad* quad, const vec2f& uv) {
    auto f = identity_frame3f;
    f.o = f.x * (uv.x-0.5) * quad->width + f.y * (uv.y-0.5) * quad->height;
    return f;
}

frame3f triangle_frame(Triangle* triangle, const vec2f& uv) {
    frame3f f;
    f.x = normalize(triangle->v1-triangle->v0);
    f.y = normalize(triangle->v2-triangle->v0);
    f.z = normalize(cross(f.x,f.y));
    f.o = (triangle->v0+triangle->v1+triangle->v2)/3;
    f.o = triangle->v0*uv.x+triangle->v1*uv.y+triangle->v2*(1-uv.x-uv.y);
    return f;
}

frame3f spline_frame(Spline* spline, int elementid, float u) {
    auto s = spline->cubic[elementid];
    frame3f frame;
    frame.o = interpolate_bezier_cubic(spline->pos, s, u);
    frame.x = normalize(interpolate_bezier_cubic_derivative(spline->pos, s, u));
    frame.y = y3f;
    frame.z = z3f;
    frame = orthonormalize(frame);
    return frame;
}

float spline_radius(Spline* spline, int elementid, float u) {
    auto s = spline->cubic[elementid];
    return interpolate_bezier_cubic(spline->radius, s, u);
}

frame3f patch_frame(Patch* patch, int elementid, const vec2f& uv) {
    auto p = patch->cubic[elementid];
    frame3f frame;
    frame.o = interpolate_bezier_bicubic(patch->pos, p, uv);
    frame.x = interpolate_bezier_bicubic_derivativex(patch->pos, p, uv);
    frame.y = interpolate_bezier_bicubic_derivativey(patch->pos, p, uv);
    frame.z = normalize(cross(frame.x,frame.y));
    frame = orthonormalize(frame);
    return frame;
}

frame3f pointset_frame(PointSet* pointset, int elementid, const vec2f& uv) {
    float phi = 2 * pi * uv.x;
    float theta = pi * uv.y;
    float ct = cos(theta);
    float st = sin(theta);
    float cp = cos(phi);
    float sp = sin(phi);
    frame3f frame;
    frame.o = pointset->pos[elementid] + pointset->radius[elementid] * vec3f(st*cp,st*sp,ct);
    frame.x = vec3f(sp,cp,0);
    frame.y = vec3f(ct*cp,ct*sp,st);
    frame.z = vec3f(st*cp,st*sp,ct);
    return frame;
}

frame3f lineset_cylinder_frame(LineSet* lines, int elementid) {
    auto l = lines->line[elementid];
    frame3f f;
    f.o = lines->pos[l.x];
    f.z = normalize(lines->pos[l.y]-lines->pos[l.x]);
    f.x = x3f;
    f.y = y3f;
    f = orthonormalize(f);
    return f;
}

// TODO: check frame
frame3f lineset_frame(LineSet* lines, int elementid, const vec2f& uv) {
    auto l = lines->line[elementid];
    auto f = lineset_cylinder_frame(lines, elementid);
    auto r = (lines->radius[l.y]+lines->radius[l.x])/2;
    auto h = length(lines->pos[l.y]-lines->pos[l.x]);
    float phi = 2 * pif * uv.x;
    float hh = h*uv.y;
    float cp = cos(phi);
    float sp = sin(phi);
    frame3f frame;
    frame.o = vec3f(r*cp,r*sp,hh);
    frame.x = z3f;
    frame.y = vec3f(sp,-cp,0);
    frame.z = vec3f(cp,sp,0);
    frame = orthonormalize(frame);
    return transform_frame(f,frame);
}

frame3f trianglemesh_frame(TriangleMesh* mesh, int elementid, const vec2f& uv) {
    auto f = mesh->triangle[elementid];
    frame3f ff;
    ff.x = normalize(mesh->pos[f.y]-mesh->pos[f.x]);
    ff.y = normalize(mesh->pos[f.z]-mesh->pos[f.x]);
    if(not mesh->norm.empty()) ff.z = normalize(interpolate_baricentric_triangle(mesh->norm, f, uv));
    else ff.z = triangle_normal(mesh->pos[f.x], mesh->pos[f.y], mesh->pos[f.z]);
    ff.o = interpolate_baricentric_triangle(mesh->pos, f, uv);
    ff = orthonormalize(ff);
    return ff;
}

frame3f mesh_frame(Mesh* mesh, int elementid, const vec2f& uv) {
    auto f = mesh_triangle_face(mesh,elementid);
    frame3f ff;
    ff.x = normalize(mesh->pos[f.y]-mesh->pos[f.x]);
    ff.y = normalize(mesh->pos[f.z]-mesh->pos[f.x]);
    if(not mesh->norm.empty()) ff.z = normalize(interpolate_baricentric_triangle(mesh->norm,f,uv));
    else if(elementid < mesh->triangle.size()) ff.z = triangle_normal(mesh->pos[f.x], mesh->pos[f.y], mesh->pos[f.z]);
    else { auto f = mesh->quad[(elementid-mesh->triangle.size())/2]; ff.z = quad_normal(mesh->pos[f.x], mesh->pos[f.y], mesh->pos[f.z], mesh->pos[f.w]); }
    ff.o = interpolate_baricentric_triangle(mesh->pos,f,uv);
    ff = orthonormalize(ff);
    return ff;
}

frame3f facemesh_frame(FaceMesh* mesh, int elementid, const vec2f& uv) {
    auto f = facemesh_triangle_face(mesh,elementid);
    frame3f ff;
    ff.x = normalize(mesh->pos[mesh->vertex[f.y].x]-mesh->pos[mesh->vertex[f.x].x]);
    ff.y = normalize(mesh->pos[mesh->vertex[f.z].x]-mesh->pos[mesh->vertex[f.x].x]);
    if(not mesh->norm.empty()) ff.z = normalize(interpolate_baricentric_triangle(mesh->norm[mesh->vertex[f.x].y],mesh->norm[mesh->vertex[f.y].y],mesh->norm[mesh->vertex[f.z].y],uv));
    else if(elementid < mesh->triangle.size()) ff.z = triangle_normal(mesh->pos[mesh->vertex[f.x].x],mesh->pos[mesh->vertex[f.y].x],mesh->pos[mesh->vertex[f.z].x]);
    else { auto f = mesh->quad[(elementid-mesh->triangle.size())/2]; ff.z = quad_normal(mesh->pos[mesh->vertex[f.x].x],mesh->pos[mesh->vertex[f.y].x],mesh->pos[mesh->vertex[f.z].x],mesh->pos[mesh->vertex[f.w].x]); }
    ff.o = interpolate_baricentric_triangle(mesh->pos[mesh->vertex[f.x].x],mesh->pos[mesh->vertex[f.y].x],mesh->pos[mesh->vertex[f.z].x],uv);
    ff = orthonormalize(ff);
    return ff;
}

ShapeSample shape_sample_uniform(Shape* shape, const vec2f& uv) {
    if(is<Sphere>(shape)) {
        auto sphere = cast<Sphere>(shape);
        // see: http://mathworld.wolfram.com/SpherePointPicking.html
        float z = 1 - 2*uv.y;
        float rxy = sqrt(1-z*z);
        float phi = 2*pif*uv.x;
        vec3f pl = vec3f(rxy*cos(phi),rxy*sin(phi),z);
        auto sss = ShapeSample();
        sss.frame.o = sphere->center + pl * sphere->radius;
        sss.frame.z = normalize(pl);
        sss.frame = orthonormalize(sss.frame);
        sss.area = sphere_area(sphere->radius);
        return sss;
    }
    else if(is<Quad>(shape)) {
        auto quad = cast<Quad>(shape);
        auto sss = ShapeSample();
        sss.frame = identity_frame3f;
        sss.frame.o = (uv.x-0.5f)*quad->width*x3f + (uv.y-0.5f)*quad->height*y3f;
        sss.area = quad->width * quad->height;
        return sss;
    }
    else { NOT_IMPLEMENTED_ERROR(); return ShapeSample(); }
}

vector<vec3f>* shape_get_pos(Shape* shape) {
    if(is<PointSet>(shape)) return & cast<PointSet>(shape)->pos;
    else if(is<LineSet>(shape)) return & cast<LineSet>(shape)->pos;
    else if(is<TriangleMesh>(shape)) return & cast<TriangleMesh>(shape)->pos;
    else if(is<Mesh>(shape)) return & cast<Mesh>(shape)->pos;
    else if(is<FaceMesh>(shape)) return & cast<FaceMesh>(shape)->pos;
    else if(is<Spline>(shape)) return & cast<Spline>(shape)->pos;
    else if(is<Patch>(shape)) return & cast<Patch>(shape)->pos;
    else if(is<CatmullClarkSubdiv>(shape)) return & cast<CatmullClarkSubdiv>(shape)->pos;
    else if(is<Subdiv>(shape)) return & cast<Subdiv>(shape)->pos;
    else return nullptr;
}

vector<vec3f>* shape_get_norm(Shape* shape) {
    if(is<TriangleMesh>(shape)) return & cast<TriangleMesh>(shape)->norm;
    else if(is<Mesh>(shape)) return & cast<Mesh>(shape)->norm;
    else return nullptr;
}

vector<vec2f>* shape_get_texcoord(Shape* shape) {
    if(is<PointSet>(shape)) return & cast<PointSet>(shape)->texcoord;
    else if(is<LineSet>(shape)) return & cast<LineSet>(shape)->texcoord;
    else if(is<TriangleMesh>(shape)) return & cast<TriangleMesh>(shape)->texcoord;
    else if(is<Mesh>(shape)) return & cast<Mesh>(shape)->texcoord;
    else if(is<Spline>(shape)) return & cast<Spline>(shape)->texcoord;
    else if(is<Patch>(shape)) return & cast<Patch>(shape)->texcoord;
    else if(is<CatmullClarkSubdiv>(shape)) return & cast<CatmullClarkSubdiv>(shape)->texcoord;
    else if(is<Subdiv>(shape)) return & cast<Subdiv>(shape)->texcoord;
    else return nullptr;
}

bool shape_has_smooth_frames(Shape* shape) {
    if(is<TriangleMesh>(shape)) return not cast<TriangleMesh>(shape)->norm.empty();
    else if(is<Mesh>(shape)) return not cast<Mesh>(shape)->norm.empty();
    else if(is<FaceMesh>(shape)) return not cast<FaceMesh>(shape)->norm.empty();
    else return false;
}

void shape_smooth_frames(Shape* shape) {
    if(is<TriangleMesh>(shape)) {
        auto mesh = cast<TriangleMesh>(shape);
        mesh->norm.resize(mesh->pos.size(),zero3f);
        for(auto f : mesh->triangle) for(auto vid : f) mesh->norm[vid] += triangle_normal(mesh->pos[f.x],mesh->pos[f.y],mesh->pos[f.z]);
        for(auto &n : mesh->norm) n = normalize(n);
    }
    else if(is<Mesh>(shape)) {
        auto mesh = cast<Mesh>(shape);
        mesh->norm.resize(mesh->pos.size(),zero3f);
        for(auto f : mesh->triangle) for(auto vid : f) mesh->norm[vid] += triangle_normal(mesh->pos[f.x],mesh->pos[f.y],mesh->pos[f.z]);
        for(auto f : mesh->quad) for(auto vid : f) mesh->norm[vid] += quad_normal(mesh->pos[f.x],mesh->pos[f.y],mesh->pos[f.z],mesh->pos[f.w]);
        for(auto &n : mesh->norm) n = normalize(n);
    }
    else if(is<FaceMesh>(shape)) {
        auto mesh = cast<FaceMesh>(shape);
        mesh->norm.resize(mesh->norm.size(), zero3f);
        for(auto f : mesh->triangle) for(auto vid : f) mesh->norm[mesh->vertex[vid].y] += triangle_normal(mesh->pos[mesh->vertex[f.x].x],mesh->pos[mesh->vertex[f.y].x],mesh->pos[mesh->vertex[f.z].x]);
        for(auto f : mesh->quad) for(auto vid : f) mesh->norm[mesh->vertex[vid].y] += quad_normal(mesh->pos[mesh->vertex[f.x].x],mesh->pos[mesh->vertex[f.y].x],mesh->pos[mesh->vertex[f.z].x],mesh->pos[mesh->vertex[f.w].x]);
        for(auto &n : mesh->norm) n = normalize(n);
    }
}

void shape_clear_frames(Shape* shape) {
    if(is<TriangleMesh>(shape)) cast<TriangleMesh>(shape)->norm.clear();
    else if(is<Mesh>(shape)) cast<Mesh>(shape)->norm.clear();
    else if(is<FaceMesh>(shape)) {
        cast<FaceMesh>(shape)->norm.clear();
        for(auto &v : cast<FaceMesh>(shape)->vertex) v.y = 0;
    }
}

TriangleMesh* mesh_to_trianglemesh(Mesh* mesh) {
    ERROR_IF_NOT(mesh->quad.empty(), "mesh cannot be converted to trianglemesh: has quads");
    auto trianglemesh = new TriangleMesh();
    trianglemesh->pos = mesh->pos;
    trianglemesh->norm = mesh->norm;
    trianglemesh->texcoord = mesh->texcoord;
    trianglemesh->triangle = mesh->triangle;
    trianglemesh->_tesselation_lines = mesh->_tesselation_lines;
    return trianglemesh;
}

Mesh* trianglemesh_to_mesh(TriangleMesh* trianglemesh) {
    auto mesh = new Mesh();
    mesh->pos = trianglemesh->pos;
    mesh->norm = trianglemesh->norm;
    mesh->texcoord = trianglemesh->texcoord;
    mesh->triangle = trianglemesh->triangle;
    mesh->_tesselation_lines = trianglemesh->_tesselation_lines;
    return mesh;    
}

Subdiv* mesh_to_subdiv(Mesh* mesh, const vector<vec2i>& crease_edges, const vector<int>& crease_vertices) {
    auto subdiv = new Subdiv();
    subdiv->pos = mesh->pos;
    subdiv->texcoord = mesh->texcoord;
    subdiv->triangle = mesh->triangle;
    subdiv->quad = mesh->quad;
    subdiv->_tesselation_lines = mesh->_tesselation_lines;
    subdiv->crease_edge = crease_edges;
    subdiv->crease_vertex = crease_vertices;
    return subdiv;    
}

Mesh* subdiv_to_mesh(Subdiv* subdiv) {
    auto mesh = new Mesh();
    mesh->pos = subdiv->pos;
    mesh->texcoord = subdiv->texcoord;
    mesh->triangle = subdiv->triangle;
    mesh->quad = subdiv->quad;
    mesh->_tesselation_lines = subdiv->_tesselation_lines;
    return mesh;
}

CatmullClarkSubdiv* mesh_to_catmullclark(Mesh* mesh) {
    ERROR_IF_NOT(mesh->triangle.empty(), "mesh cannot be converted to catmullclark: has triangles");
    auto subdiv = new CatmullClarkSubdiv();
    subdiv->pos = mesh->pos;
    subdiv->texcoord = mesh->texcoord;
    subdiv->quad = mesh->quad;
    subdiv->_tesselation_lines = mesh->_tesselation_lines;
    return subdiv;
}

Mesh* catmullclark_to_mesh(CatmullClarkSubdiv* subdiv) {
    auto mesh = new Mesh();
    mesh->pos = subdiv->pos;
    mesh->texcoord = subdiv->texcoord;
    mesh->quad = subdiv->quad;
    mesh->_tesselation_lines = subdiv->_tesselation_lines;
    return mesh;    
}
