#ifndef _KEYFRAMED_H_
#define _KEYFRAMED_H_

#include "node.h"

///@file igl/keyframed.h Keyframed Values. @ingroup igl
///@defgroup keyframed Keyframed Values
///@ingroup igl
///@{

/// Keyframed Value (interpolated like a bezier)
struct KeyframedValue : Node {
    REGISTER_FAST_RTTI(Node,KeyframedValue,5)
    
    const float         _epsilon = 0.00000001f; ///< epsilon
    
    vector<float>       times; ///< keyframe times
    vector<vec3f>       values; ///< keyframe values
    int                 degree = 1; ///< bezier interpolation degrees
    
    int segments() const { return values.size() / (degree+1); }
};

/// keyfamed animation interval
inline range1f keyframed_interval(KeyframedValue* keyframed) {
    return range1f( keyframed->times.front(), keyframed->times.back() );
}

/// eval keyframed value
inline vec3f keyframed_value(KeyframedValue* keyframed, float time) {
    time = clamp(time,keyframed_interval(keyframed).min,keyframed_interval(keyframed).max-keyframed->_epsilon);
    int seg = 0;
    for(seg = 0; keyframed->times[seg+1] < time; seg ++);
    auto t = (time - keyframed->times[seg]) / (keyframed->times[seg+1] - keyframed->times[seg]);
    
    auto value = zero3f;
    for(int i = 0; i <= keyframed->degree; i ++) value += keyframed->values[seg*(keyframed->degree+1)+i]*bernstein(t,i,keyframed->degree);
    return value;
}

///@}

#endif
