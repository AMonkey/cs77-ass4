#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "node.h"
#include "texture.h"
#include "vmath/montecarlo.h"

///@file igl/material.h Materials. @ingroup igl
///@defgroup material Materials
///@ingroup igl
///@{

/// Abstract Material
struct Material : Node {
    REGISTER_FAST_RTTI(Node,Material,8)
    
    Texture*     normal_texture = nullptr; ///< normal map
};

/// Lambert Material
struct Lambert : Material {
    REGISTER_FAST_RTTI(Material,Lambert,1)
    
    vec3f        diffuse = vec3f(0.75,0.75,0.75); ///< diffuse color
    Texture*     diffuse_texture = nullptr; ///< diffuse texture
};

/// Phong Material
struct Phong : Material {
    REGISTER_FAST_RTTI(Material,Phong,2)
    
	vec3f        diffuse = vec3f(0.75,0.75,0.75); ///< diffuse color
    vec3f        specular = vec3f(0.25,0.25,0.25); ///< specular color
    float        exponent =  10; ///< specular exponent
    vec3f        reflection = zero3f; ///< reflection color
    float        blur_size = 0.0f; ///< blurriness of reflection
	Texture*     diffuse_texture = nullptr; ///< diffuse texture
    Texture*     specular_texture = nullptr; ///< specular texture
    Texture*     exponent_texture = nullptr; ///< specular exponent texture
    Texture*     reflection_texture = nullptr; ///< reflection texture
    
    bool use_reflected = false; ///< use reflected or bisector
};

/// Lambert Emission Material
struct LambertEmission : Material {
    REGISTER_FAST_RTTI(Material,LambertEmission,5)
    
    vec3f       emission = one3f; ///< diffuse emission color
    vec3f       diffuse = one3f; ///< lambert diffuse coefficient
    Texture*    emission_texture = nullptr; ///< emission texture
    Texture*    diffuse_texture = nullptr; ///< emission texture
};

///@name eval interface
///@{

/// check whether a material has textures
inline bool material_has_textures(Material* material) {
    if(is<Lambert>(material)) {
        auto lambert = cast<Lambert>(material);
        return lambert->diffuse_texture;
    }
    else if(is<Phong>(material)) {
        auto phong = cast<Phong>(material);
        return phong->diffuse_texture or phong->specular_texture or phong->exponent_texture or phong->reflection_texture;
    }
    else if(is<LambertEmission>(material)) {
        auto emission = cast<LambertEmission>(material);
        return emission->diffuse_texture or emission->emission_texture;
    }
    else { NOT_IMPLEMENTED_ERROR(); return nullptr; }
}

/// evalute perturbed shading frame
inline frame3f material_shading_frame(Material* material, const frame3f& frame, const vec2f& texcoord) {
    return frame;
}

/// resolve texture coordinates
inline Material* material_shading_textures(Material* material, const vec2f& texcoord) {
    if(is<Lambert>(material)) {
        auto lambert = cast<Lambert>(material);
        auto ret = new Lambert();
        ret->diffuse = lambert->diffuse;
        return ret;
    }
    else if(is<Phong>(material)) {
        auto phong = cast<Phong>(material);
        auto ret = new Phong();
        ret->blur_size = phong->blur_size;
        ret->use_reflected = phong->use_reflected;
        ret->diffuse = phong->diffuse;
        ret->specular = phong->specular;
        ret->exponent = phong->exponent;
        ret->reflection = phong->reflection;
        return ret;
    }
    else if(is<LambertEmission>(material)) {
        auto emission = cast<LambertEmission>(material);
        auto ret = new LambertEmission();
        ret->diffuse = emission->diffuse;
        ret->emission = emission->emission;
        return ret;
    }
    else { NOT_IMPLEMENTED_ERROR(); return nullptr; }
}

/// evaluate the material color
inline vec3f material_diffuse_albedo(Material* material) {
    ERROR_IF_NOT(not material_has_textures(material), "cannot support textures");
    if(is<Lambert>(material)) return cast<Lambert>(material)->diffuse;
    else if(is<Phong>(material)) return cast<Phong>(material)->diffuse;
    else if(is<LambertEmission>(material)) return cast<LambertEmission>(material)->diffuse;
    else { NOT_IMPLEMENTED_ERROR(); return zero3f; }
}

/// evaluete the emission of the material
inline vec3f material_emission(Material* material, const frame3f& frame, const vec3f& wo) {
    ERROR_IF_NOT(not material_has_textures(material), "cannot support textures");
    if(is<LambertEmission>(material)) {
        auto lambert = cast<LambertEmission>(material);
        if(dot(wo,frame.z) <= 0) return zero3f;
        return lambert->emission;
    } else return zero3f;
}

/// evaluate an approximation of the fresnel model
inline vec3f _schlickFresnel(const vec3f& rhos, float iDh) {
    return rhos + (vec3f(1,1,1)-rhos) * pow(1.0f-iDh,5.0f);
}

/// evaluate an approximation of the fresnel model
inline vec3f _schlickFresnel(const vec3f& rhos, const vec3f& w, const vec3f& wh) {
    return _schlickFresnel(rhos, dot(wh,w));
}

/// evaluate product of BRDF and cosine
inline vec3f material_brdfcos(Material* material, const frame3f& frame, const vec3f& wi, const vec3f& wo) {
    ERROR_IF_NOT(not material_has_textures(material), "cannot support textures");
    if(is<Lambert>(material)) {
        auto lambert = cast<Lambert>(material);
        if(dot(wi,frame.z) <= 0 or dot(wo,frame.z) <= 0) return zero3f;
        return lambert->diffuse * abs(dot(wi,frame.z)) / pif;
    }
    else if(is<Phong>(material)) {
        auto phong = cast<Phong>(material);
        if(dot(wi,frame.z) <= 0 or dot(wo,frame.z) <= 0) return zero3f;
        if(phong->use_reflected) {
            vec3f wr = reflect(-wi,frame.z);
            return (phong->diffuse / pif + (phong->exponent + 8) * phong->specular*pow(max(dot(wo,wr),0.0f),phong->exponent) / (8*pif)) * abs(dot(wi,frame.z));
        } else {
            vec3f wh = normalize(wi+wo);
            return (phong->diffuse / pif + (phong->exponent + 8) * phong->specular*pow(max(dot(frame.z,wh),0.0f),phong->exponent) / (8*pif)) * abs(dot(wi,frame.z));
        }
    }
    else if(is<LambertEmission>(material)) {
        auto lambert = cast<LambertEmission>(material);
        if(dot(wi,frame.z) <= 0 or dot(wo,frame.z) <= 0) return zero3f;
        return lambert->diffuse * abs(dot(wi,frame.z)) / pif;
    }
    else { NOT_IMPLEMENTED_ERROR(); return zero3f; }
}

/// material average color for interactive drawing
inline vec3f material_display_color(Material* material) {
    if(is<Lambert>(material)) return cast<Lambert>(material)->diffuse;
    else if(is<Phong>(material)) return cast<Phong>(material)->diffuse;
    else if(is<LambertEmission>(material)) return cast<LambertEmission>(material)->emission;
    else { NOT_IMPLEMENTED_ERROR(); return zero3f; }
}
///@}

///@name sample interface
///@{
struct BrdfSample {
    vec3f           brdfcos = zero3f;
    vec3f           wi = zero3f;
    float           pdf = 1;
};

/// evaluate color and direction of mirror reflection (zero if not reflections)
inline BrdfSample material_sample_reflection(Material* material, const frame3f& frame, const vec3f& wo) {
    ERROR_IF_NOT(not material_has_textures(material), "no textures allowed");
    if(is<Phong>(material)) {
        auto phong = cast<Phong>(material);
        if(dot(wo,frame.z) <= 0) return BrdfSample();
        auto bs = BrdfSample();
        bs.brdfcos = phong->reflection;
        bs.wi = reflect(-wo, frame.z);
        bs.pdf = 1;
        return bs;
    }
    else { return BrdfSample(); }
}

/// evaluate color and direction of blurred mirror reflection (zero if not reflections)
inline BrdfSample material_sample_blurryreflection(Material* material, const frame3f& frame, const vec3f& wo, const vec2f& suv) {
    ERROR_IF_NOT(not material_has_textures(material), "no textures allowed");
    auto bs = BrdfSample();
    if(is<Phong>(material)) {
        auto phong = cast<Phong>(material);
        if(dot(wo,frame.z) <= 0) return BrdfSample();
        auto wi = reflect(-wo, frame.z);
        auto u = normalize(cross(wi, wo));
        auto v = normalize(cross(wi, u));
        auto sl = phong->blur_size;
        
        bs.brdfcos = phong->reflection;
        bs.wi = normalize(wi + (0.5f-suv.x)*sl*u + (0.5f-suv.y)*sl*v);
        bs.pdf = 1.0/(sl*sl);
        
        return bs;
    }
    else { return bs; }
}

/// pick a direction and sample it
inline BrdfSample material_sample_brdfcos(Material* material, const frame3f& frame, const vec3f& wo, const vec2f& suv, float sl) {
    ERROR_IF_NOT(not material_has_textures(material), "no textures allowed");
    if(is<Lambert>(material)) {
        auto lambert = cast<Lambert>(material);
        if(dot(wo,frame.z) <= 0) return BrdfSample();
        auto ds = sample_direction_hemisphericalcos(suv);
        auto wi = transform_direction(frame, ds.dir);
        BrdfSample bs;
        bs.brdfcos = material_brdfcos(lambert, frame, wi, wo);
        bs.wi = wi;
        bs.pdf = ds.pdf;
        return bs;
    }
    else if(is<Phong>(material)) {
        auto phong = cast<Phong>(material);
        if(dot(wo,frame.z) <= 0) return BrdfSample();
        auto ds = sample_direction_hemisphericalcos(suv);
        auto wi = transform_direction(frame, ds.dir);
        BrdfSample bs;
        bs.brdfcos = material_brdfcos(phong, frame, wi, wo);
        bs.wi = wi;
        bs.pdf = ds.pdf;
        return bs;
    }
    else if(is<LambertEmission>(material)) {
        auto lambert = cast<LambertEmission>(material);
        if(dot(wo,frame.z) <= 0) return BrdfSample();
        auto ds = sample_direction_hemisphericalcos(suv);
        auto wi = transform_direction(frame, ds.dir);
        BrdfSample bs;
        bs.brdfcos = material_brdfcos(lambert, frame, wi, wo);
        bs.wi = wi;
        bs.pdf = ds.pdf;
        return bs;
    }
    else { NOT_IMPLEMENTED_ERROR(); return BrdfSample(); }
}

///@}

///@}

#endif
