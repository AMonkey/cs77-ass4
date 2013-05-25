#include "gl_utils.h"
#include "gls.h"

///@file igl/gl_utils.cpp Opengl Utilities. @ingroup igl

image3f glutils_read_pixels(int x, int y, int w, int h, bool front) {
    glsCheckError();
    image3f img(w,h);
    if(front) glReadBuffer(GL_FRONT); else glReadBuffer(GL_BACK);
    glReadPixels(x, y, w, h, GL_RGB, GL_FLOAT, img.data());
    glsCheckError();
    return img;
}

image3f glutils_read_pixels(int w, int h, bool front) {
    return glutils_read_pixels(0,0,w,h,front);
}

void glutils_draw_line(const vec3f& a, const vec3f& b) {
    glsCheckError();
    glBegin(GL_LINES);
    glsVertex(a);
    glsVertex(b);
    glEnd();
    glsCheckError();
}

void glutils_draw_point(const vec3f& p) {
    glsCheckError();
    glBegin(GL_POINTS);
    glsVertex(p);
    glEnd();
    glsCheckError();
}
    
void glutils_draw_grid(const vec3f& o, const vec3f& u, const vec3f& v, float size, int steps) {
    glsCheckError();
    glBegin(GL_LINES);
    for(int i = -steps/2; i <= steps/2; i ++) {
        float t = i / float(steps/2);
        glsVertex(o+(u*t+v)*(size/2));
        glsVertex(o+(u*t-v)*(size/2));
        glsVertex(o+(v*t+u)*(size/2));
        glsVertex(o+(v*t-u)*(size/2));
    }
    glEnd();
    glsCheckError();
}

void glutils_draw_grid(const frame3f& frame, float size, int steps) {
    glutils_draw_grid(frame.o, frame.x, frame.y, size, steps);
}

void glutils_draw_triangle(const vec3f& v0, const vec3f& v1, const vec3f& v2) {
    glsCheckError();
    glBegin(GL_TRIANGLES);
    glsNormal(triangle_normal(v0, v1, v2));
    glsTexCoord(zero2f);
    glsVertex(v0);
    glsTexCoord(x2f);
    glsVertex(v1);
    glsTexCoord(y2f);
    glsVertex(v2);
    glEnd();
    glsCheckError();
}

void glutils_draw_triangle_lines(const vec3f& v0, const vec3f& v1, const vec3f& v2) {
    glsCheckError();
    glBegin(GL_LINE_LOOP);
    glsNormal(triangle_normal(v0, v1, v2));
    glsTexCoord(zero2f);
    glsVertex(v0);
    glsTexCoord(x2f);
    glsVertex(v1);
    glsTexCoord(y2f);
    glsVertex(v2);
    glEnd();
    glsCheckError();
}

void glutils_draw_quad(const vec3f& o, const vec3f& u, const vec3f& v, float w, float h) {
    glsCheckError();
    glBegin(GL_QUADS);
    glsNormal(normalize(cross(u,v)));
    glsTexCoord(zero2f);
    glsVertex(o-u*w*0.5f-v*h*0.5f);
    glsTexCoord(x2f);
    glsVertex(o+u*w*0.5f-v*h*0.5f);
    glsTexCoord(one2f);
    glsVertex(o+u*w*0.5f+v*h*0.5f);
    glsTexCoord(y2f);
    glsVertex(o-u*w*0.5f+v*h*0.5f);
    glEnd();
    glsCheckError();
}

void glutils_draw_quad_lines(const vec3f& o, const vec3f& u, const vec3f& v, float w, float h) {
    glsCheckError();
    glBegin(GL_LINE_LOOP);
    glsNormal(normalize(cross(u,v)));
    glsTexCoord(zero2f);
    glsVertex(o-u*w*0.5f-v*h*0.5f);
    glsTexCoord(x2f);
    glsVertex(o+u*w*0.5f-v*h*0.5f);
    glsTexCoord(one2f);
    glsVertex(o+u*w*0.5f+v*h*0.5f);
    glsTexCoord(y2f);
    glsVertex(o-u*w*0.5f+v*h*0.5f);
    glEnd();
    glsCheckError();
}

void glutils_draw_parametric_face(int ur, int vr, bool ccw,
                                  const function<vec3f (const vec2f&)>& pos,
                                  const function<vec3f (const vec2f&)>& norm,
                                  const function<vec2f (const vec2f&)>& texcoord) {
    glsCheckError();
    int oi[] = { 0, 0, 1, 1 };
    int oj[] = { 0, 1, 1, 0 };
    if(not ccw) { swap(oi[1],oi[3]); swap(oj[1],oj[3]); }
    glBegin(GL_QUADS);
    for(int i = 0; i < ur; i ++) {
        for(int j = 0; j < vr; j ++) {
            for(int k = 0; k < 4; k ++) {
                auto uv = vec2f((i+oi[k]) / float(ur), (j+oj[k]) / float(vr));
                glsTexCoord(texcoord(uv));
                glsNormal(norm(uv));
                glsVertex(pos(uv));
            }
        }
    }
    glEnd();
    glsCheckError();
}

void glutils_draw_parametric_line(int ul, int vl, int ur, int vr,
                                 const function<vec3f (const vec2f&)>& pos,
                                 const function<vec3f (const vec2f&)>& norm) {
    glsCheckError();
    for(int i = 0; i <= ul; i ++) {
        glBegin(GL_LINE_STRIP);
        for(int j = 0; j <= vr; j ++) {
            auto uv = vec2f(i / float(ul), j / float(vr));
            glsTexCoord(uv);
            glsNormal(norm(uv));
            glsVertex(pos(uv));
        }
        glEnd();
    }
    for(int j = 0; j <= vl; j ++) {
        glBegin(GL_LINE_STRIP);
        for(int i = 0; i <= ur; i ++) {
            auto uv = vec2f(i / float(ur), j / float(vl));
            glsTexCoord(uv);
            glsNormal(norm(uv));
            glsVertex(pos(uv));
        }
        glEnd();
    }
    glsCheckError();
}

void glutils_draw_sphere(const vec3f& o, float r, int ur, int vr) {
    glsCheckError();
    glutils_draw_parametric_face(ur, vr, true,
                         [o,r](const vec2f& uv) { return o+r*vec3f(sin(pif*uv.y)*cos(2*pif*uv.x),sin(pif*uv.y)*sin(2*pif*uv.x),cos(pif*uv.y)); },
                         [o,r](const vec2f& uv) { return vec3f(sin(pif*uv.y)*cos(2*pif*uv.x),sin(pif*uv.y)*sin(2*pif*uv.x),cos(pif*uv.y)); },
                         [](const vec2f& uv) { return uv; });
    glsCheckError();
}
void glutils_draw_sphere(const vec3f& o, float r, const vec2f& uv0, int ur, int vr) {
    glsCheckError();
    glutils_draw_parametric_face(ur, vr, true,
                                 [o,r](const vec2f& uv) { return o+r*vec3f(sin(pif*uv.y)*cos(2*pif*uv.x),sin(pif*uv.y)*sin(2*pif*uv.x),cos(pif*uv.y)); },
                                 [o,r](const vec2f& uv) { return vec3f(sin(pif*uv.y)*cos(2*pif*uv.x),sin(pif*uv.y)*sin(2*pif*uv.x),cos(pif*uv.y)); },
                                 [uv0](const vec2f& uv) { return uv0; });
    glsCheckError();
}
void glutils_draw_sphere_lines(const vec3f& o, float r, int ul, int vl, int ur, int vr) {
    glsCheckError();
    glutils_draw_parametric_line(ul, vl, ur, vr,
                         [o,r](const vec2f& uv) { return o+r*vec3f(sin(pif*uv.y)*cos(2*pif*uv.x),sin(pif*uv.y)*sin(2*pif*uv.x),cos(pif*uv.y)); },
                         [o,r](const vec2f& uv) { return vec3f(sin(pif*uv.y)*cos(2*pif*uv.x),sin(pif*uv.y)*sin(2*pif*uv.x),cos(pif*uv.y)); });
    glsCheckError();
}
void glutils_draw_cylinder(float r, float h, int ur, int vr) {
    glsCheckError();
    glutils_draw_parametric_face(ur, vr, false,
                         [r,h](const vec2f& uv) { return vec3f(r*cos(2*pif*uv.x),r*sin(2*pif*uv.x),h*uv.y); },
                         [r,h](const vec2f& uv) { return vec3f(cos(2*pif*uv.x),sin(2*pif*uv.x),0); },
                         [](const vec2f& uv) { return uv; });
    glsCheckError();
}
void glutils_draw_cylinder(float r, float h, const vec2f& uv0, const vec2f& uv1, int ur, int vr) {
    glsCheckError();
    glutils_draw_parametric_face(ur, vr, false,
                                 [r,h](const vec2f& uv) { return vec3f(r*cos(2*pif*uv.x),r*sin(2*pif*uv.x),h*uv.y); },
                                 [r,h](const vec2f& uv) { return vec3f(cos(2*pif*uv.x),sin(2*pif*uv.x),0); },
                                 [uv0,uv1](const vec2f& uv) { return uv0*(1-uv.y)+uv1*uv.y; });
    glsCheckError();
}
void glutils_draw_cylinder_lines(float r, float h, int ul, int vl, int ur, int vr) {
    glsCheckError();
    glutils_draw_parametric_line(ul, vl, ur, vr,
                         [r,h](const vec2f& uv) { return vec3f(r*cos(2*pif*uv.x),r*sin(2*pif*uv.x),h*uv.y); },
                         [r,h](const vec2f& uv) { return vec3f(cos(2*pif*uv.x),sin(2*pif*uv.x),0); });
    glsCheckError();
}
void glutils_draw_cylinder(const frame3f& f, float r, float h, int ur, int vr) {
    glsCheckError();
    glutils_draw_parametric_face(ur, vr, false,
                                 [f,r,h](const vec2f& uv) { return f.o+f.x*(r*cos(2*pif*uv.x))+f.y*(r*sin(2*pif*uv.x))+f.z*(h*uv.y); },
                                 [f,r,h](const vec2f& uv) { return f.x*cos(2*pif*uv.x)+f.y*sin(2*pif*uv.x); },
                                 [](const vec2f& uv) { return uv; });
    glsCheckError();
}
void glutils_draw_cylinder(const frame3f& f, float r, float h, const vec2f& uv0, const vec2f& uv1, int ur, int vr) {
    glsCheckError();
    glutils_draw_parametric_face(ur, vr, false,
                                 [f,r,h](const vec2f& uv) { return f.o+f.x*(r*cos(2*pif*uv.x))+f.y*(r*sin(2*pif*uv.x))+f.z*(h*uv.y); },
                                 [f,r,h](const vec2f& uv) { return f.x*cos(2*pif*uv.x)+f.y*sin(2*pif*uv.x); },
                                 [uv0,uv1](const vec2f& uv) { return uv0*(1-uv.y)+uv1*uv.y; });
    glsCheckError();
}
void glutils_draw_cylinder_lines(const frame3f& f, float r, float h, int ul, int vl, int ur, int vr) {
    glsCheckError();
    glutils_draw_parametric_line(ul, vl, ur, vr,
                                 [f,r,h](const vec2f& uv) { return f.o+f.x*(r*cos(2*pif*uv.x))+f.y*(r*sin(2*pif*uv.x))+f.z*(h*uv.y); },
                                 [f,r,h](const vec2f& uv) { return f.o+f.x*cos(2*pif*uv.x)+f.y*sin(2*pif*uv.x); });
    glsCheckError();
}

void glutils_draw_axis(const frame3f& frame, float size, const vec3f& color_axis_x, const vec3f& color_axis_y, const vec3f& color_axis_z) {
    glsCheckError();
    glsColor(color_axis_x);
    glutils_draw_line(frame.o-(size/2)*frame.x,frame.o+(size/2)*frame.x);
    glsColor(color_axis_y);
    glutils_draw_line(frame.o-(size/2)*frame.y,frame.o+(size/2)*frame.y);
    glsColor(color_axis_z);
    glutils_draw_line(frame.o-(size/2)*frame.z,frame.o+(size/2)*frame.z);
    glsCheckError();
}

void glutils_draw_grids(const frame3f& frame, float size, const vec3f& color_grid, const vec3f& color_axis_x, const vec3f& color_axis_y, const vec3f& color_axis_z) {
    glsCheckError();
    glLineWidth(1);
    glsColor(vec3f(0.3,0.3,0.3));
    glutils_draw_grid(frame,size,10);
    
    glLineWidth(2);
    glutils_draw_axis(frame,size,color_axis_x,color_axis_y,color_axis_z);
    glsCheckError();
}

void draw_axis_gizmo(const frame3f& frame, int x, int y, int w, int h, const vec3f& color_axis_x, const vec3f& color_axis_y, const vec3f& color_axis_z) {
    glsCheckError();
    glPushAttrib(GL_VIEWPORT_BIT);
    
    glViewport(x, y, w, h);
    
    glLineWidth(2);
    glutils_draw_axis(frame, 1, color_axis_x, color_axis_y, color_axis_z);
    
    glPopAttrib();
    glsCheckError();
}

template<typename F, typename P, typename N, typename T>
void glutils_draw_faces(const vector<F>& face,
                       const vector<P>& pos, const vector<N>& norm,
                       const vector<T>& uv) {
    WARNING_IF_NOT(not face.empty(), "face should not be empty");
    WARNING_IF_NOT(not pos.empty(), "pos should not be empty");
    
    glsCheckError();
    
    glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
    
    if(not pos.empty()) { glEnableClientState(GL_VERTEX_ARRAY); glsVertexPointer(pos); }
    if(not norm.empty()) { glEnableClientState(GL_NORMAL_ARRAY); glsNormalPointer(norm); }
    if(not uv.empty()) { glEnableClientState(GL_TEXTURE_COORD_ARRAY); glsTexCoordPointer(uv); }
    
    glsDrawElements(face);
    
    glPopClientAttrib();
    
    glsCheckError();
}

// explicit instantiation to satisfy the compiler
template void glutils_draw_faces(const vector<vec3i> &face, const vector<vec3f> &pos, const vector<vec3f> &norm, const vector<vec2f> &uv);
template void glutils_draw_faces(const vector<vec4i> &face, const vector<vec3f> &pos, const vector<vec3f> &norm, const vector<vec2f> &uv);

void glutils_set_light(int idx, const vec4f& pos, const vec4f& ka, const vec4f& kd, const vec4f& ks) {
    glsCheckError();
    glEnable(idx);
    glLightfv(idx, GL_POSITION, &pos.x);
    glLightfv(idx, GL_AMBIENT, &ka.x);
    glLightfv(idx, GL_DIFFUSE, &kd.x);
    glLightfv(idx, GL_SPECULAR, &ks.x);
    glsCheckError();
}

void glutils_set_ambient_light(const vec4f& ka) {
    glsCheckError();
    glsCheckError();
}

void glutils_set_material(const vec4f& ke, const vec4f& ka, const vec4f& kd, const vec4f& ks, float n) {
    glsCheckError();
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,&ke.x);
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,&ka.x);
    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,&kd.x);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,&ks.x);
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,n);
    glsCheckError();
}

void glutils_set_material(const vec3f& ke, const vec3f& ka, const vec3f& kd, const vec3f& ks, float n) {
    auto _ke = vec4f(ke.x,ke.y,ke.z,1);
    auto _ka = vec4f(ka.x,ka.y,ka.z,1);
    auto _kd = vec4f(kd.x,kd.y,kd.z,1);
    auto _ks = vec4f(ks.x,ks.y,ks.z,1);
    glutils_set_material(_ke, _ka, _kd, _ks, n);
}
