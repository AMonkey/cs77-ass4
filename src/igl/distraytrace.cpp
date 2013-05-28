#include "distraytrace.h"

#include "vmath/random.h"
#include "intersect.h"

///@file igl/distraytrace.cpp Distribution Raytracing. @ingroup igl

vec3f _dist_raytrace_scene_ray(Scene* scene,
                               const ray3f& ray,
                               DistributionRaytraceOptions& opts,
                               int depth)
{
    vec3f c = zero3f;
    // intersect
    intersection3f intersection;
    if(not intersect_scene_first(scene,ray,intersection)) return opts.background;
    
    // set up variables
    auto frame = intersection.frame;
    auto texcoord = intersection.texcoord;
    auto wo = -ray.d;
    auto material = intersection.material;
    auto& rng = opts.rng;

    // shading frame
    if(opts.doublesided) frame = faceforward(frame, ray.d);
    frame = material_shading_frame(material, frame, texcoord);

    // brdf
    auto brdf = material_shading_textures(intersection.material, intersection.texcoord);

    // compute ambient
    if (opts.samples_ambient != 0) {
        int visible = 0;
        for (int i = 0; i < opts.samples_ambient; i++) {
            // Make random ray along hemisphere of intersection frame
            auto hemi_dir = normalize(vec3f(0.5f - opts.rng.next_float(),
                                            0.5f - opts.rng.next_float(),
                                            abs( 0.5f - opts.rng.next_float() ) ));
            hemi_dir = transform_direction(intersection.frame, hemi_dir);
            ray3f hemi_ray = ray3f(intersection.frame.o, hemi_dir);
            if (not intersect_scene_any(scene, hemi_ray)) {
                //c += opts.ambient * material_diffuse_albedo(brdfy) / opts.samples_ambient;
                visible++;

            }
        }
        c += (opts.ambient * ((float)visible / (float)opts.samples_ambient)) * material_diffuse_albedo(brdf);
    }
    else {
        c += opts.ambient * material_diffuse_albedo(brdf);

    }

    // compute emission
    c += material_emission(brdf, frame, wo);
    
    // compute direct
    auto& ll = (opts.cameralights) ? scene->_cameralights : scene->lights;
    for(auto l : ll->lights) {
        auto ss = rand_light_shadow_sample(l, frame.o, rng.next_float(), rng.next_float());
        auto shadow_samples = (is<AreaLight>(l)) ? cast<AreaLight>(l)->shadow_samples : 1;
        for (int i = 0; i < shadow_samples; i++) {
            auto wi = ss.dir;
            if(ss.radiance == zero3f) continue;
            vec3f cl = ss.radiance * material_brdfcos(brdf,frame,wi,wo) / ss.pdf;
            if(cl == zero3f) continue;
            if(opts.shadows) {
                if(not intersect_scene_any( scene,ray3f::segment(frame.o,frame.o+ss.dir*ss.dist) )) {
                        c += cl / (float)shadow_samples;

                }
            }
            else {
                c += cl / (float)shadow_samples;

            }
        }
    }

    // recursively compute reflections
    if(opts.reflections and depth < opts.max_depth) {
        auto bs = material_sample_reflection(brdf, frame, wo);
        if(not (bs.brdfcos == zero3f)) {
            auto refl_ray = ray3f(frame.o,bs.wi);
            c += _dist_raytrace_scene_ray(scene, refl_ray, opts, depth+1) * bs.brdfcos;

        }
    }
    
    // cleanup
    delete brdf;
    
    // done
    return c; 

}

void dist_raytrace_scene_progressive(ImageBuffer& buffer,
                                     Scene* scene,
                                     DistributionRaytraceOptions& opts)
{
    auto w = buffer.width();
    auto h = buffer.height();
    auto& rng = opts.rng;
    
    int s2 = max(1,(int)sqrt(opts.samples));
    for(int j = 0; j < h; j ++) {
        for(int i = 0; i < w; i ++) {
            // Monte Carlo anti-aliasing
            for (int k = 0; k < opts.samples; k++) { 
                auto u = (i + (0.5f - rng.next_float())) / w;
                auto v = (j + (0.5f - rng.next_float())) / h;

                ray3f ray = camera_ray_dof(scene->camera, vec2f(u, v), rng); 
                buffer.accum.at(i,h-1-j) += _dist_raytrace_scene_ray(scene,ray,opts,0);
                buffer.samples.at(i,h-1-j) += 1;

            }
        }
    }
}

