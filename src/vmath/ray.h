#ifndef _RAY_H_
#define _RAY_H_

#include "vec.h"

///@file vmath/ray.h Rays. @ingroup vmath
///@defgroup ray Rays
///@ingroup vmath
///@{

/// 3D Ray
template<typename T>
struct ray3 {
    // TODO: use numeric limits
    constexpr static const T epsilon = T(0.0001); ///< ray epsilon
    constexpr static const T rayinf = T(1000000); ///< ray infinity

    vec3<T> e = vec3<T>(0,0,0); ///< origin
    vec3<T> d = vec3<T>(0,0,1); ///< direction
    T tmin = epsilon;           ///< min t value
    T tmax = rayinf;            ///< max t value

    /// Default constructor
    ray3() { }
    /// Element-wise constructor
    ray3(const vec3<T>& e, const vec3<T>& d, T tmin = epsilon, T tmax = rayinf) : 
        e(e), d(d), tmin(tmin), tmax(tmax) { }

    /// Create a ray from a segment
    static ray3 segment(const vec3<T>& a, const vec3<T>& b) { return ray3<T>(a,normalize(b-a),epsilon,dist(a,b)-2*epsilon); }

    /// Eval ray
    vec3<T> eval(T t) const { return e + d * t; }
};

///@name 3D Ray Typedefs
///@{
using ray3f = ray3<float>;
using ray3d = ray3<double>;
///@}

///@}

#endif

