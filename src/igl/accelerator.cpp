#include "accelerator.h"

///@file igl/accelerator.cpp Intersection Accelerators. @ingroup igl

struct _BVHBoxedPrim { int i; range3f bbox; vec3f center; };

bool intersect_bvhnode_first(BVHAccelerator* bvh, int nodeid, const ray3f& ray, intersection3f& intersection) {
    auto& node = bvh->nodes[nodeid];
    if(not intersect_bbox(ray, node.bbox)) return false;
    bool hit = false; float mint = ray3f::rayinf;
    ray3f sray = ray;
    if(node.leaf) {
        for(auto idx : range(node.start,node.end)) {
            auto i = bvh->sorted_prims[idx];
            intersection3f sintersection;
            if(bvh->_intersect_elem_first(i, sray, sintersection)) {
                if(mint > sintersection.ray_t) {
                    hit = true;
                    mint = sintersection.ray_t;
                    sray.tmax = mint;
                    intersection = sintersection;
                }
            }
        }
    } else {
        for(auto n : { node.n0, node.n1 }) {
            intersection3f sintersection;
            if(intersect_bvhnode_first(bvh, n, sray, sintersection)) {
                if(mint > sintersection.ray_t) {
                    hit = true;
                    mint = sintersection.ray_t;
                    sray.tmax = mint;
                    intersection = sintersection;
                }
            }
        }
    }
    return hit;
}

bool intersect_bvh_first(BVHAccelerator* bvh, const ray3f& ray, intersection3f& intersection) {
    return intersect_bvhnode_first(bvh, 0, ray, intersection);
}

bool intersect_bvhnode_any(BVHAccelerator* bvh, int nodeid, const ray3f& ray) {
    auto& node = bvh->nodes[nodeid];
    if(not intersect_bbox(ray, node.bbox)) return false;
    if(node.leaf) {
        for(auto idx : range(node.start,node.end)) {
            auto i = bvh->sorted_prims[idx];
            if(bvh->_intersect_elem_any(i,ray)) return true;
        }
    } else {
        if(intersect_bvhnode_any(bvh,node.n0,ray)) return true;
        if(intersect_bvhnode_any(bvh,node.n1,ray)) return true;
    }
    return false;
}

bool intersect_bvh_any(BVHAccelerator* bvh, const ray3f& ray) {
    return intersect_bvhnode_any(bvh, 0, ray);
}

int intersect_bvh_build_split(BVHAccelerator* bvh, vector<_BVHBoxedPrim>& prim, int start, int end, const range3f& bbox) {
    vec3f d = size(bbox);
    if(d.x > d.y and d.x > d.z) {
        std::sort(prim.begin()+start,prim.begin()+end,
                  [](const _BVHBoxedPrim& i, const _BVHBoxedPrim& j) { return i.center.x < j.center.x; });
    } else if(d.y > d.z) {
        std::sort(prim.begin()+start,prim.begin()+end,
                  [](const _BVHBoxedPrim& i, const _BVHBoxedPrim& j) { return i.center.y < j.center.y; });
    } else {
        std::sort(prim.begin()+start,prim.begin()+end,
                  [](const _BVHBoxedPrim& i, const _BVHBoxedPrim& j) { return i.center.z < j.center.z; });
    }
    return (start+end)/2;
}

void intersect_bvh_build_node(BVHAccelerator* bvh, int nodeid, vector<_BVHBoxedPrim>& prim, int start, int end) {
    range3f bbox;
    auto node = BVHNode();
    for(auto i : range(start, end)) bbox = runion(bbox,prim[i].bbox);
    if(end-start <= BVHAccelerator::min_prims) {
        node.bbox = bbox;
        node.leaf = true;
        node.start = start;
        node.end = end;
    } else {
        int middle = intersect_bvh_build_split(bvh,prim,start,end,bbox);
        node.bbox = bbox;
        node.leaf = false;
        bvh->nodes.push_back(BVHNode());
        node.n0 = bvh->nodes.size();
        bvh->nodes.push_back(BVHNode());
        node.n1 = bvh->nodes.size();
        bvh->nodes.push_back(BVHNode());
        intersect_bvh_build_node(bvh,node.n0,prim,start,middle);
        intersect_bvh_build_node(bvh,node.n1,prim,middle,end);
    }
    bvh->nodes[nodeid] = node;
}

void intersect_bvh_accelerate(BVHAccelerator* bvh)  {
    vector<_BVHBoxedPrim> prims(bvh->_intersect_elem_num);
    for(auto i : range(prims.size())) {
        prims[i].i = i;
        prims[i].bbox = bvh->_intersect_elem_bounds(i);
        prims[i].bbox = rscale(prims[i].bbox,1+BVHAccelerator::epsilon);
        prims[i].center = center(prims[i].bbox);
    }
    bvh->nodes.push_back(BVHNode());
    intersect_bvh_build_node(bvh,0,prims,0,prims.size());
    bvh->sorted_prims.resize(prims.size());
    for(auto i : range(prims.size())) bvh->sorted_prims[i] = prims[i].i;
}

range3f intersect_bvh_bounds(BVHAccelerator* bvh) {
    return bvh->nodes[0].bbox;
}

