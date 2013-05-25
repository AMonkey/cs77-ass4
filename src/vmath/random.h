#ifndef _RANDOM_H_
#define _RANDOM_H_

#include "stdmath.h"
#include "vec.h"

#include <random>
#include <vector>

///@file vmath/random.h Random number generation. @ingroup vmath
///@defgroup random Random number generation
///@ingroup vmath
///@{

/// Random number generator
struct Rng {
    /// C++11 random number engine
    std::minstd_rand                        engine;
    
    /// Seed the generator
    void seed(unsigned int seed) { engine.seed(seed); }
	
    /// Generate a float in [0,1)
	float next_float() { return std::uniform_real_distribution<float>(0,1)(engine); }
    /// Generate a float in [a,b)
	float next_float(float a, float b) { return std::uniform_real_distribution<float>(a,b)(engine); }
    /// Generate a float in [v.x,v.y)
	float next_float(const vec2f& v) { return next_float(v.x,v.y); }
    /// Generate a float in [r.min,r.max)
	float next_float(const range1f& r) { return next_float(r.min,r.max); }

	/// Generate 2 floats in [0,1)^2
    vec2f next_vec2f() { return vec2f(next_float(),next_float()); }
	/// Generate 3 floats in [0,1)^3
    vec3f next_vec3f() { return vec3f(next_float(),next_float(),next_float()); }
    
    /// Generator an int in [a,b)
	int next_int(int a, int b) { return std::uniform_int_distribution<int>(a,b)(engine); }
    /// Generator an int in [v.x,v.y)
    int next_int(const vec2i& v) { return next_int(v.x,v.y); }
    /// Generator an int in [r.min,r.max)
    int next_int(const range1i& r) { return next_int(r.min,r.max); }
};

/// Create and seed nrngs generators
inline std::vector<Rng> rng_generate_seeded(int nrngs) {
    std::seed_seq sseq{0,1,2,3,4,5,6,7,8,9};
    auto seeds = std::vector<int>(nrngs);
    sseq.generate(seeds.begin(), seeds.end());
    auto rngs = std::vector<Rng>(nrngs);
    for(int i = 0; i < nrngs; i ++) {
        rngs[i].seed(seeds[i]);
    }
    return rngs;
}

///@}

#endif
