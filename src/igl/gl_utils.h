#ifndef _DRAW_UTILS_H_
#define _DRAW_UTILS_H_

#include "vmath/vmath.h"
#include "igl/image.h"

///@file igl/gl_utils.h Opengl Utilities. @ingroup igl
///@defgroup gl_utils Opengl Utilities
///@ingroup igl
///@{

///@name Pixel readback
///@{
image3f glutils_read_pixels(int x, int y, int w, int h, bool front);
image3f glutils_read_pixels(int w, int h, bool front);
///@}

///@name Draw Utilities
///@{
void glutils_draw_line(const vec3f& a, const vec3f& b);
void glutils_draw_point(const vec3f& p);
void glutils_draw_grid(const vec3f& o, const vec3f& u, const vec3f& v, float size = 2, int steps = 10);
void glutils_draw_grid(const frame3f& frame, float size = 2, int steps = 10);
void glutils_draw_triangle(const vec3f& v0, const vec3f& v1, const vec3f& v2);
void glutils_draw_triangle_lines(const vec3f& v0, const vec3f& v1, const vec3f& v2);
void glutils_draw_quad(const vec3f& o, const vec3f& u, const vec3f& v, float w, float h);
void glutils_draw_quad_lines(const vec3f& o, const vec3f& u, const vec3f& v, float w, float h);
void glutils_draw_parametric_face(int ur, int vr, bool ccw,
                          const function<vec3f (const vec2f&)>& pos,
                          const function<vec3f (const vec2f&)>& norm,
                          const function<vec2f (const vec2f&)>& texcoord);
void glutils_draw_parametric_line(int ul, int vl, int ur, int vr,
                          const function<vec3f (const vec2f&)>& pos,
                          const function<vec3f (const vec2f&)>& norm);
    
void glutils_draw_sphere(const vec3f& o, float r, int ur = 64, int vr = 32);
void glutils_draw_sphere(const vec3f& o, float r, const vec2f& uv, int ur = 64, int vr = 32);
void glutils_draw_sphere_lines(const vec3f& o, float r, int ul = 16, int vl = 8, int ur = 64, int vr = 32);
void glutils_draw_cylinder(float r, float h, int ur = 64, int vr = 32) ;
void glutils_draw_cylinder(float r, float h, const vec2f& uv0, const vec2f& uv1, int ur = 64, int vr = 32) ;
void glutils_draw_cylinder_lines(float r, float h, int ul = 16, int vl = 8, int ur = 64, int vr = 32);
void glutils_draw_cylinder(const frame3f& f, float r, float h, int ur = 64, int vr = 32) ;
void glutils_draw_cylinder(const frame3f& f, float r, float h, const vec2f& uv0, const vec2f& uv1, int ur = 64, int vr = 32) ;
void glutils_draw_cylinder_lines(const frame3f& f, float r, float h, int ul = 16, int vl = 8, int ur = 64, int vr = 32);

void glutils_draw_axis(const frame3f& frame = identity_frame3f, float size = 2,
                       const vec3f& color_axis_x = vec3f(0.5,0.125,0.125),
                       const vec3f& color_axis_y = vec3f(0.125,0.5,0.125),
                       const vec3f& color_axis_z = vec3f(0.125,0.125,0.5));
void glutils_draw_grids(const frame3f& frame = identity_frame3f, float size = 2,
                        const vec3f& color_grid = vec3f(0.3,0.3,0.3),
                        const vec3f& color_axis_x = vec3f(0.5,0.125,0.125),
                        const vec3f& color_axis_y = vec3f(0.125,0.5,0.125),
                        const vec3f& color_axis_z = vec3f(0.125,0.125,0.5));
void glutils_draw_axis_gizmo(int x, int y, int w, int h,
                             const frame3f& frame = identity_frame3f,
                             const vec3f& color_axis_x = vec3f(0.5,0.125,0.125),
                             const vec3f& color_axis_y = vec3f(0.125,0.5,0.125),
                             const vec3f& color_axis_z = vec3f(0.125,0.125,0.5));
    
template<typename F, typename P, typename N, typename T>
void glutils_draw_faces(const vector<F>& face,
                const vector<P>& pos, const vector<N>& norm,
                const vector<T>& uv);
///@}

///@name Fixed function lights/materials
///@{
void glutils_set_light(int idx, const vec4f& pos, const vec4f& ka, const vec4f& kd, const vec4f& ks);
void glutils_set_ambient_light(const vec4f& ka);
void glutils_set_material(const vec4f& ke, const vec4f& ka, const vec4f& kd, const vec4f& ks, float n);
void glutils_set_material(const vec3f& ke, const vec3f& ka, const vec3f& kd, const vec3f& ks, float n);
///@}

///@name Shading Utilities
///@{
uint glutils_init_shader(uint type, const char* code);
uint glutils_init_vert_shader(const char* code);
uint glutils_init_frag_shader(const char* code);

uint glutils_init_program(uint vert, uint frag);
void glutils_clear_program(uint prog, uint vert, uint frag);
void glutils_clear_shader(uint shader);
    
void glutils_bind_program(uint prog);
void glutils_unbind_program(uint prog);

template<typename T>
void glutils_set_uniform(uint prog, const char* name, const T& value);
///@}

///@}

#endif
