#ifndef _MONTECARLO_H_
#define _MONTECARLO_H_

#include "vec.h"

#include <algorithm>
#include <cmath>

struct DirectionSample {
    vec3f           dir = zero3f;
    float           pdf = 0;
};

struct IndexSample {
    int             idx = -1;
    float           pdf = 0;
};

inline DirectionSample sample_direction_hemispherical(const vec2f& ruv) {
    auto z = ruv.y;
    auto r = sqrt(1-z*z);
    auto phi = 2 * pi * ruv.x;
    DirectionSample ds;
    ds.dir = vec3f(r*cos(phi), r*sin(phi), z);
    ds.pdf = 1/(2*pi);
    return ds;
}

inline float sample_direction_hemispherical_pdf(const vec3f& w) {
    return (w.z <= 0) ? 0 : 1/(2*pi);
}

inline DirectionSample sample_direction_spherical(const vec2f ruv) {
    auto z = 2*ruv.y-1;
    auto r = sqrt(1-z*z);
    auto phi = 2 * pi * ruv.x;
    DirectionSample ds;
    ds.dir = vec3f(r*cos(phi), r*sin(phi), z);
    ds.pdf = 1/(4*pi);
    return ds;
}

inline float sample_direction_spherical_pdf(const vec3f& w) {
    return 1/(4*pi);
}

inline DirectionSample sample_direction_hemisphericalcos(const vec2f& ruv) {
    auto z = sqrt(ruv.y);
    auto r = sqrt(1-z*z);
    auto phi = 2 * pi * ruv.x;
    DirectionSample ds;
    ds.dir = vec3f(r*cos(phi), r*sin(phi), z);
    ds.pdf = z/pi;
    return ds;
}

inline float sample_direction_hemisphericalcos_pdf(const vec3f& w) {
    return (w.z <= 0) ? 0 : w.z/pi;
}

inline float sample_direction_hemisphericalcospower_pdf(const vec3f& w, float n) {
    return (w.z <= 0) ? 0 : pow(w.z,n) * (n+1) / (2*pi);
}

inline DirectionSample sample_direction_hemisphericalcospower(const vec2f& ruv, float n) {
    auto z = pow(ruv.y,1/(n+1));
    auto r = sqrt(1-z*z);
    auto phi = 2 * pi * ruv.x;
    DirectionSample ds;
    ds.dir = vec3f(r*cos(phi), r*sin(phi), z);
    ds.pdf = pow(z,n) * (n+1) / (2*pi);
    return ds;
}

inline IndexSample sample_index_uniform(float r, int size) {
    int l = clamp((int)(r * size), 0, size-1);
    IndexSample is;
    is.idx = l;
    is.pdf = 1.0f / size;
    return is;
}

inline float sample_uniform_pdf(int size) {
    return 1.0f / size;
}

inline int sample_stratify_samplesnumber(int samples) {
    return (int)round(sqrt(samples));
}

inline vec2f sample_stratify_sample(const vec2f& uv, int sample, int samples_x, int samples_y) {
    int sample_x = sample % samples_x;
    int sample_y = sample / samples_x;
    return vec2f((sample_x + uv.x / samples_x), (sample_y + uv.y / samples_y));
}

#if 0
// from pbrt
inline vec2<R> sampleConcentricDisk(const vec2<R> &ruv) {
    R r, theta;
    // Map uniform random numbers to $[-1,1]^2$
    R sx = 2 * ruv.x - 1;
    R sy = 2 * ruv.y - 1;

    // Map square to $(r,\theta)$
    // Handle degeneracy at the origin
    if (sx == 0 && sy == 0)
        return zero2f;
    if (sx >= -sy) {
        if (sx > sy) {
            // Handle first region of disk
            r = sx;
            if (sy > 0) theta = sy/r;
            else        theta = 8 + sy/r;
        }
        else {
            // Handle second region of disk
            r = sy;
            theta = 2 - sx/r;
        }
    }
    else {
        if (sx <= sy) {
            // Handle third region of disk
            r = -sx;
            theta = 4 - sy/r;
        }
        else {
            // Handle fourth region of disk
            r = -sy;
            theta = 6 + sx/r;
        }
    }
    theta *= consts<R>::pi / 4;
    return makevec2<R>(r * cos(theta), r * sin(theta));
}
#endif

// from pbrt
struct DistrubutionSample1D {
    float   value;
    int     index;
    float   pdf;
};

struct DistrubutionSample2D {
    vec2f   value;
    float   pdf;
};

struct Distribution1D {
    vector<float>           values;
    vector<float>           cdf;
    float                   integral;
};

struct Distribution2D {
    vector<Distribution1D>  conditionalV;
    Distribution1D          marginal;
};

inline Distribution1D sample_init_distribution1d(const vector<float>& values) {
    Distribution1D dist;
    
    dist.values = values;
    dist.cdf.resize(values.size()+1);
    
    dist.cdf[0] = 0;
    for (int i = 1; i < dist.cdf.size(); ++i) {
        // TODO: why the division by size?
        dist.cdf[i] = dist.cdf[i-1] + dist.values[i-1] / dist.values.size();
    }
    
    dist.integral = dist.cdf[dist.values.size()];
    if (dist.integral == 0.f) {
        for (int i = 1; i < dist.cdf.size(); ++i) dist.cdf[i] = float(i) / float(dist.values.size());
    } else {
        for (int i = 1; i < dist.cdf.size(); ++i) dist.cdf[i] /= dist.integral;
    }
    
    return dist;
}

inline Distribution2D sample_init_distribution2d(const vector<vector<float>>& values) {
    Distribution2D dist;
    dist.conditionalV.resize(values.size());
    vector<float> condBuffer(values[0].size());
    for (int v = 0; v < values.size(); v++) {
        for(int u = 0; u < values[v].size(); u++)
            condBuffer[u] = values[v][u];
        dist.conditionalV[v] = sample_init_distribution1d(condBuffer);
    }
    // Compute marginal sampling distribution $p[\tilde{v}]$
    vector<float> marginalFunc(values.size());
    for (int v = 0; v < values.size(); ++v)
        marginalFunc[v] = dist.conditionalV[v].integral;
    dist.marginal = sample_init_distribution1d(marginalFunc);
    return dist;
}

inline DistrubutionSample1D sample_distribution1d(Distribution1D* dist, float u) {
    DistrubutionSample1D ret;
    
    // TODO: why is this needed?
    // HACK: this should not be here
    u = clamp(u, 0.0f, 0.999999f);
    
    auto cdf = &dist->cdf[0];
    auto ptr = std::upper_bound(cdf, cdf+dist->cdf.size(), u);
    ret.index = max(0, int(ptr-cdf-1));
    ERROR_IF_NOT(ret.index < dist->values.size(), "incorrect cdf sampling");
    ERROR_IF_NOT(u >= cdf[ret.index] && u < cdf[ret.index+1], "incorrect cdf sampling");
    
    auto du = (u - cdf[ret.index]) / (cdf[ret.index+1] - cdf[ret.index]);
    ERROR_IF_NOT(not std::isnan(du), "problem with du");
    
    ret.pdf = dist->values[ret.index] / dist->integral;
    ret.value =  (ret.index + du) / dist->values.size();
    
    return ret;
}

inline DistrubutionSample2D sample_distribution2d(Distribution2D* dist, const vec2f& uv) {
    DistrubutionSample2D ret;
    auto sampleY = sample_distribution1d(&dist->marginal, uv.y);
    auto sampleX = sample_distribution1d(&dist->conditionalV[sampleY.index], uv.x);
    ret.value = vec2f(sampleX.value,sampleY.value);
    ret.pdf = sampleX.pdf * sampleY.pdf;
    return ret;
}
// end - from pbrt

#endif