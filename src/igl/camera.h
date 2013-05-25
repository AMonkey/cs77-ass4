#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "node.h"

///@file igl/camera.h Cameras. @ingroup igl
///@defgroup camera Cameras
///@ingroup igl
///@{

/// Camera
struct Camera : Node {
    REGISTER_FAST_RTTI(Node,Camera,1)
    
    frame3f             frame = identity_frame3f; ///< frame
    float               view_dist = 1; ///< view distance for interaction
    float               image_width = 1; ///< image plane width
    float               image_height = 1; ///< image plane height
    float               image_dist = 1; ///< image plane distance
    float               focus_dist = 1; ///< focus distance
    float               focus_aperture = 0; ///< focus aperture
    float               shutter = 1; ///< how long shutter stays open
    bool                orthographic = false; /// orthographic
};

///@name image size
///@{
inline int camera_image_width(Camera* camera, int r) {
    return (int)round(r * camera->image_width / camera->image_height);
}

inline int camera_image_height(Camera* camera, int r) {
    return r;
}

inline float camera_image_aspectratio(Camera* camera) {
    return camera->image_height/camera->image_width;
}

inline void camera_image_set_aspectratio(Camera* camera, int imw, int imh) {
    camera->image_width = (camera->image_height * imw) / imh;
}
///@}

///@name camera matrices
///@{
inline mat4f camera_projectionmatrix(Camera* camera) {
    if(not camera->orthographic)
        return frustum_matrix(-camera->image_width/2,camera->image_width/2,
                              -camera->image_height/2,camera->image_height/2,
                              camera->image_dist,1000000.0f);
    else return ortho_matrix(-camera->image_width/2, camera->image_width/2,
                             -camera->image_height/2, camera->image_height/2,
                             camera->image_dist, 1000000.0f);
}

inline mat4f camera_viewmatrix(Camera* camera) {
    return frame_to_matrix_inverse(camera->frame);
}
///@}

///@name sample interface
///@{
inline ray3f camera_ray(Camera* camera, const vec2f& uv) {
    ray3f rayl;
    if(not camera->orthographic) {
        auto q = vec3f((uv.x-0.5f)*camera->image_width, (uv.y-0.5f)*camera->image_height, -camera->image_dist);
        rayl = ray3f(zero3f,normalize(q));
    } else {
        auto l = vec3f((uv.x-0.5f)*camera->image_width, (uv.y-0.5f)*camera->image_height, 0);
        rayl = ray3f(l,-z3f);
    }
    return transform_ray(camera->frame, rayl);
}

inline ray3f camera_ray(Camera* camera, const vec2f& uv, const vec2f& auv) {
    ray3f rayl;
    if(not camera->orthographic) {
        auto e = vec3f(0.5-auv.x,0.5-auv.y,0) * camera->focus_aperture;
        auto qx = (uv.x-0.5f)*camera->image_width*camera->focus_dist/camera->image_dist;
        auto qy = (uv.y-0.5f)*camera->image_height*camera->focus_dist/camera->image_dist;
        auto q = vec3f(qx, qy, -camera->focus_dist);
        rayl = ray3f(e,normalize(q-e));
    } else {
        auto l = vec3f((uv.x-0.5f)*camera->image_width, (uv.y-0.5f)*camera->image_height, 0);
        rayl = ray3f(l,-z3f);
    }
    return transform_ray(camera->frame, rayl);
}
///@}

///@name view manipulation
///@{
inline void camera_view_lookat(Camera* camera, const vec3f& eye, const vec3f& center, const vec3f& up) {
    camera->frame = lookat_frame(eye,center,up,true);
    camera->view_dist = dist(eye,center);
}

// assume world up is z
inline void camera_view_turntable_rotate(Camera* camera, float delta_phi, float delta_theta) {
    float phi = atan2(camera->frame.z.y,camera->frame.z.x);
    float theta = acos(camera->frame.z.z);
    
    phi += delta_phi;
    theta += delta_theta;
    theta = clamp(theta,0.001f,pif-0.001f);
    
    vec3f nz = vec3f(sin(theta)*cos(phi),
                     sin(theta)*sin(phi),
                     cos(theta));
    vec3f nc = camera->frame.o-camera->frame.z*camera->view_dist;
    vec3f no = nc + nz * camera->view_dist;
    
    camera_view_lookat(camera, no, nc, z3f);
}

inline void camera_view_turntable_dolly(Camera* camera, float delta_f) {
    vec3f c = camera->frame.o - camera->frame.z * camera->view_dist;
    camera->view_dist = max(camera->view_dist+delta_f,ray3f::epsilon);
    camera->frame.o = c + camera->frame.z * camera->view_dist;
}

inline void camera_view_turntable_pan(Camera* camera, float delta_x, float delta_y) {
    camera->frame.o += camera->frame.x * delta_x + camera->frame.y * delta_y;
}

// assume world up id z
inline void camera_view_fps_move(Camera* camera, float forward, float right, float up) {
    auto up_v = z3f;
    auto forward_v = -camera->frame.z;
    auto right_v = camera->frame.x;
    orthonormalize(right_v, forward_v, up_v);
    camera->frame.o += forward*forward_v + right*right_v + up*up_v;
}

inline void camera_view_fps_rotate(Camera* camera, float delta_right, float delta_up) {
    // do it
    auto my = rotation_matrix(delta_right, y3f);
    auto mx = rotation_matrix(delta_up, camera->frame.x);
    auto m = my*mx;
    camera->frame.x = transform_direction(m, camera->frame.x);
    camera->frame.y = transform_direction(m, camera->frame.y);
    camera->frame.z = transform_direction(m, camera->frame.z);
}
///@}

///@}

#endif
