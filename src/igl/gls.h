#ifndef _GLS_H_
#define _GLS_H_

#if __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include "vmath/vmath.h"
#include "igl/image.h"
#include "common/common.h"

///@file igl/gls.h Opengl typed functions. @ingroup igl
///@defgroup gls Opengl typed functions
///@ingroup igl
///@{

///@name OpenGL Error checking
///@{
#define GLS_CHECK_ERROR 1

#if GLS_CHECK_ERROR
inline void glsCheckError() {
    auto errcode = glGetError();
    ERROR_IF_NOT(errcode == GL_NO_ERROR, "gl error: %s", gluErrorString(errcode));
}
#else
inline void glsCheckError() { }
#endif
///@}

///@name OpenGL Typed Wrappers
///@{
inline void glsClearColor(const vec3f& v) { glClearColor(v.x,v.y,v.z,1); }
inline void glsClearColor(const vec4f& v) { glClearColor(v.x,v.y,v.z,v.w); }
inline void glsClearColor(const vec3d& v) { glClearColor(v.x,v.y,v.z,1); }
inline void glsClearColor(const vec4d& v) { glClearColor(v.x,v.y,v.z,v.w); }
inline void glsVertex(const vec3f& v) { glVertex3fv(&v.x); }
inline void glsVertex(const vec3d& v) { glVertex3dv(&v.x); }

inline void glsNormal(const vec3f& v) { glNormal3fv(&v.x); }
inline void glsNormal(const vec3d& v) { glNormal3dv(&v.x); }

inline void glsColor(const vec3f& v) { glColor3fv(&v.x); }
inline void glsColor(const vec3d& v) { glColor3dv(&v.x); }
inline void glsColor(const vec4f& v) { glColor4fv(&v.x); }
inline void glsColor(const vec4d& v) { glColor4dv(&v.x); }

inline void glsTexCoord(const vec2f& v) { glTexCoord2fv(&v.x); }
inline void glsTexCoord(const vec2d& v) { glTexCoord2dv(&v.x); }

inline void glsRasterPos(const vec2f& v) { glRasterPos2fv(&v.x); }
inline void glsRasterPos(const vec2d& v) { glRasterPos2dv(&v.x); }

inline void glsMultMatrix(const mat4f& m) { glsCheckError(); auto tm = transpose(m); glMultMatrixf(&tm.x.x); glsCheckError(); }
inline void glsLoadMatrix(const mat4f& m) { glsCheckError(); auto tm = transpose(m); glLoadMatrixf(&tm.x.x); glsCheckError(); } 
inline void glsMultMatrix(const mat4d& m) { glsCheckError(); auto tm = transpose(m); glMultMatrixd(&tm.x.x); glsCheckError(); }
inline void glsLoadMatrix(const mat4d& m) { glsCheckError(); auto tm = transpose(m); glLoadMatrixd(&tm.x.x); glsCheckError(); } 

inline void glsVertexPointer(const vector<vec3f>& v) { glVertexPointer(3,GL_FLOAT,0,&v[0].x); }
inline void glsVertexPointer(const vector<vec3d>& v) { glVertexPointer(3,GL_DOUBLE,0,&v[0].x); }
inline void glsNormalPointer(const vector<vec3f>& v) { glNormalPointer(GL_FLOAT,0,&v[0].x); }
inline void glsNormalPointer(const vector<vec3d>& v) { glNormalPointer(GL_DOUBLE,0,&v[0].x); }
inline void glsColorPointer(const vector<vec3f>& v) { glColorPointer(3,GL_FLOAT,0,&v[0].x); }
inline void glsColorPointer(const vector<vec3d>& v) { glColorPointer(3,GL_DOUBLE,0,&v[0].x); }
inline void glsColorPointer(const vector<vec4f>& v) { glColorPointer(4,GL_FLOAT,0,&v[0].x); }
inline void glsColorPointer(const vector<vec4d>& v) { glColorPointer(4,GL_DOUBLE,0,&v[0].x); }
inline void glsTexCoordPointer(const vector<vec2f>& v) { glTexCoordPointer(2,GL_FLOAT,0,&v[0].x); }
inline void glsTexCoordPointer(const vector<vec2d>& v) { glTexCoordPointer(2,GL_DOUBLE,0,&v[0].x); }

inline void glsDrawElements(const vector<vec3i>& v) { glDrawElements(GL_TRIANGLES,v.size()*3,GL_UNSIGNED_INT,&v[0].x); }
inline void glsDrawElements(const vector<vec4i>& v) { glDrawElements(GL_QUADS,v.size()*4,GL_UNSIGNED_INT,&v[0].x); }
///@}

///@}

#endif

