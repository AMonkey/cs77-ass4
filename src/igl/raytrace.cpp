#include "raytrace.h"

#include "vmath/random.h"
#include "intersect.h"

///@file igl/raytrace.cpp Raytracing. @ingroup igl

vec3f _raytrace_scene_ray(Scene* scene, const ray3f& ray, const RaytraceOptions& opts, int depth) {
    // intersect
    intersection3f intersection;
    if(not intersect_scene_first(scene,ray,intersection)) return opts.background;
    
    // set up variables
    auto frame = intersection.frame;
    auto texcoord = intersection.texcoord;
    auto wo = -ray.d;
    auto material = intersection.material;
    
    // shading frame
    if(opts.doublesided) frame = faceforward(frame,ray.d);
    frame = material_shading_frame(material, frame, texcoord);

    // brdf
    auto brdf = material_shading_textures(intersection.material, intersection.texcoord);

    // compute ambient
    vec3f c = zero3f;
    c += opts.ambient * material_diffuse_albedo(brdf);
    
    // compute emission
    c += material_emission(brdf, frame, wo);
    
    // compute direct
    auto& ll = (opts.cameralights) ? scene->_cameralights : scene->lights;
    for(auto l : ll->lights) {
        auto ss = light_shadow_sample(l,frame.o);
        auto wi = ss.dir;
        if(ss.radiance == zero3f) continue;
        vec3f cl = ss.radiance * material_brdfcos(brdf,frame,wi,wo) / ss.pdf;
        if(cl == zero3f) continue;
        if(opts.shadows) {
            if(not intersect_scene_any(scene,ray3f::segment(frame.o,frame.o+ss.dir*ss.dist))) c += cl;
        } else c += cl;
    }
    
    // recursively compute reflections
    if(opts.reflections and depth < opts.max_depth) {
        auto bs = material_sample_reflection(brdf, frame, wo);
        if(not (bs.brdfcos == zero3f)) {
            auto refl_ray = ray3f(frame.o,bs.wi);
            c += _raytrace_scene_ray(scene, refl_ray, opts, depth+1) * bs.brdfcos;
        }
    }
    
    // cleanup
    delete brdf;
    
    // done
    return c;
}

void raytrace_scene_progressive(ImageBuffer& buffer, Scene* scene, const RaytraceOptions& opts) {
    auto w = buffer.width();
    auto h = buffer.height();
    
    int s2 = max(1,(int)sqrt(opts.samples));
    for(int j = 0; j < h; j ++) {
        for(int i = 0; i < w; i ++) {
            auto cs = buffer.samples.at(i,h-1-j);
            auto ii = cs % s2; auto jj = cs / s2;
            float u = (i+(ii+0.5)/s2)/w;
            float v = (j+(jj+0.5)/s2)/h;
            ray3f ray = camera_ray(scene->camera,vec2f(u,v));
            buffer.accum.at(i,h-1-j) += _raytrace_scene_ray(scene,ray,opts,0);
            buffer.samples.at(i,h-1-j) += 1;
        }
    }
}

