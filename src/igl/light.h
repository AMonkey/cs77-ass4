#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "node.h"
#include "shape.h"
#include "texture.h"
#include "vmath/montecarlo.h"

///@file igl/light.h Lights. @ingroup igl
///@defgroup light Lights
///@ingroup igl
///@{

/// Abstract Light
struct Light : Node {
    REGISTER_FAST_RTTI(Node,Light,6)
    
    frame3f             frame = identity_frame3f; ///< frame
};

/// Point Light at the origin
struct PointLight : Light {
    REGISTER_FAST_RTTI(Light,PointLight,1)
    
    vec3f               intensity = one3f; ///< intensity
};

/// Directional Light along z
struct DirectionalLight : Light {
    REGISTER_FAST_RTTI(Light,DirectionalLight,2)
    
    vec3f               intensity = one3f; ///< intensity
};

/// Area Light based on a shape
struct AreaLight : Light {
    REGISTER_FAST_RTTI(Light,AreaLight,3)
    
    vec3f               intensity = one3f; ///< intensity
    Shape*              shape = nullptr; ///< light shape (should support sampling)
    int                 shadow_samples = 16; ///< number of shadow rays
};

/// Environment Light based on an infinite sphere
struct EnvLight : Light {
    REGISTER_FAST_RTTI(Light,EnvLight,4)
    
    vec3f               intensity = one3f; ///< intensity
    Texture*            envmap = nullptr; ///< envmap
    bool                hemisphere = false; ///< hemisphere only
    int                 shadow_samples = 16; ///< number of shadow rays
    bool                importance_sampling = true; ///< whether to use importance sampling for the texture
    
    Distribution2D*     _importance_distribution = nullptr; ///< whether to use importance sampling
};

/// Group of Lights
struct LightGroup : Node {
    REGISTER_FAST_RTTI(Node,LightGroup,7)
    
    vector<Light*>      lights;
};

///@name sample interface
/// requsted number of shadow rays
inline int light_shadow_nsamples(Light* light) {
    if(is<AreaLight>(light)) return cast<AreaLight>(light)->shadow_samples;
    else if(is<EnvLight>(light)) return cast<EnvLight>(light)->shadow_samples;
    else return 1;
}

/// Shadow Sample
struct ShadowSample {
    vec3f           radiance; ///< light radiance
    vec3f           dir; ///< light direction
    float           dist; ///< light distance
    float           pdf; ///< sample pdf
};

/// shadow ray and radiance for light center
inline ShadowSample light_shadow_sample(Light* light, const vec3f& p) {
    auto pl = transform_point_inverse(light->frame, p);
    ShadowSample ss;
    if(is<PointLight>(light)) {
        ss.dir = normalize(-pl);
        ss.dist = length(pl);
        ss.radiance = cast<PointLight>(light)->intensity / lengthSqr(pl);
        ss.pdf = 1;
    }
    else if(is<DirectionalLight>(light)) {
        ss.dir = -z3f;
        ss.dist = ray3f::rayinf;
        ss.radiance = cast<DirectionalLight>(light)->intensity;
        ss.pdf = 1;
    }
    else if(is<AreaLight>(light)) {
        ss.dir = normalize(-pl);
        ss.dist = length(pl);
        ss.radiance = cast<AreaLight>(light)->intensity / lengthSqr(pl);
        ss.pdf = 1;
    }
    else if(is<EnvLight>(light)) {
        ss.dir = normalize(-pl);
        ss.dist = ray3f::rayinf;
        ss.radiance = cast<EnvLight>(light)->intensity * pif;
        ss.pdf = 1;
    }
    else { NOT_IMPLEMENTED_ERROR(); }
    ss.dir = transform_direction(light->frame, ss.dir);
    return ss;
}

inline ShadowSample rand_light_shadow_sample(Light* light,
                                             const vec3f& p,
                                             float u_rand,
                                             float v_rand)
{
    // Handles soft shadows
    ShadowSample ss;
    auto rand_point = light->frame;
    if(is<AreaLight>(light)) {
        // Shift light frame by a random amount within the shape
        auto sh = cast<Quad>(cast<AreaLight>(light)->shape);
        auto u = (0.5 - u_rand) * sh->width;
        auto v = (0.5 - v_rand) * sh->height;
        vec3f shift = vec3f(u,v,0);
        // Transform to object pov
        shift = transform_vector(identity_frame3f, shift); 
        rand_point.o += shift;
        
        auto pl = transform_point_inverse(rand_point, p);

        ss.dir = normalize(-pl);
        ss.dist = length(pl);
        ss.radiance = cast<AreaLight>(light)->intensity / lengthSqr(pl);
        ss.radiance *= dot(vec3f(0.0, 0.0, 1.0), -ss.dir);
        ss.pdf = 1 / (sh->width * sh->height); //cast<AreaLight>(light)->shadow_samples;

    }
    else {
        //NOT_IMPLEMENTED_ERROR();
        //message_va("Warning: not an area light, skipping soft shadowing.");
        return light_shadow_sample(light, p);

    }
    ss.dir = transform_direction(rand_point, ss.dir);
    return ss;
}

/// sample light background if needed (only userful for envlights)
inline vec3f light_sample_background(Light* light, const vec3f& wo) {
    auto wol = transform_direction_inverse(light->frame, wo);
    if(is<EnvLight>(light)) {
        auto sample = cast<EnvLight>(light)->intensity;
        return sample;
    } else return zero3f;
}

/// init light sampling
inline void sample_light_init(Light* light) {
    if(is<EnvLight>(light)) {
        if(not cast<EnvLight>(light)->importance_sampling or not cast<EnvLight>(light)->envmap) return;
        if(cast<EnvLight>(light)->_importance_distribution) delete cast<EnvLight>(light)->_importance_distribution;
        const image<vec3f>& txt = cast<EnvLight>(light)->envmap->image;
        image<float> values(txt.width(),txt.height());
        for (auto v : range(txt.height())) {
            float sinTheta = sin(pif * float(v+.5f)/float(txt.height()));
            for (auto u : range(txt.width())) values.at(u,v) = mean_component(txt.at(u,v)) * sinTheta;
        }
        vector<vector<float>> _values(values.height());
        for (auto v : range(values.height())) {
            _values[v].resize(values.width());
            for (auto u : range(values.width())) _values[v][u] = values.at(u,v);
        }
        cast<EnvLight>(light)->_importance_distribution = new Distribution2D();
        *cast<EnvLight>(light)->_importance_distribution = sample_init_distribution2d(_values);
    } else {}
}

/// init light sampling
inline void sample_lights_init(LightGroup* lights) { for(auto light : lights->lights) sample_light_init(light); }

///@}

///@name frame manipulation
/// orients a light
inline void light_lookat(Light* light, const vec3f& eye, const vec3f& center, const vec3f& up) {
    light->frame.o = eye;
    light->frame.z = normalize(center-eye);
    light->frame.y = up;
    light->frame = orthonormalize(light->frame);
}

//inline ray3f rand_reflected_ray(ray3f r, )
///@}

///@}

#endif
