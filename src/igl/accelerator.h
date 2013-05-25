#ifndef _ACCELERATOR_H_
#define _ACCELERATOR_H_

#include "node.h"
#include "intersect.h"
#include <algorithm>

///@file igl/accelerator.h Intersection Accelerators. @ingroup igl
///@defgroup accelerator Intersection Accelerators
///@ingroup igl
///@{

/// BVH node
struct BVHNode {
    bool leaf; ///< leaf node
    range3f bbox; ///< bounding box
    union {
        struct { int start, end; }; ///< for leaves: start and end primitive
        struct { int n0, n1; }; ///< for internal: left and right node
    };
};

/// Bounding Volume Accelerator
struct BVHAccelerator {
    static const int                    min_prims = 4; ///< min primitives
    constexpr static const float        epsilon = ray3f::epsilon; ///< epsilon
    
    int                                                 _intersect_elem_num; ///< number of elements
    function<range3f (int)>                             _intersect_elem_bounds; ///< function for element bounds
    function<bool (int,const ray3f&,intersection3f&)>   _intersect_elem_first; ///< function for element first intersection
    function<bool (int,const ray3f&)>                   _intersect_elem_any; ///< function for element any intersection
    
    vector<int>                         sorted_prims; ///< sorted primitives
    vector<BVHNode>                     nodes; ///< bvh nodes
    
    /// Constructor (sets element number and functions)
    BVHAccelerator(int intersect_elem_num,
                   const function<range3f (int)> intersect_elem_bounds,
                   const function<bool (int,const ray3f&,intersection3f&)> intersect_elem_first,
                   const function<bool (int,const ray3f&)> intersect_elem_any) :
                    _intersect_elem_num(intersect_elem_num),
                    _intersect_elem_bounds(intersect_elem_bounds),
                    _intersect_elem_first(intersect_elem_first),
                    _intersect_elem_any(intersect_elem_any) { }
    /// Copy constructor
    BVHAccelerator(const BVHAccelerator& bvh) = default;    
};

///@name intersect interface
///@{
range3f intersect_bvh_bounds(BVHAccelerator* bvh);
void intersect_bvh_accelerate(BVHAccelerator* bvh);
bool intersect_bvh_first(BVHAccelerator* bvh, const ray3f& ray, intersection3f& intersection);
bool intersect_bvh_any(BVHAccelerator* bvh, const ray3f& ray);
///@}

///@}

#endif
