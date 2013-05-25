#include "geom.h"

///@file vmath/geom.cpp Geometric math. @ingroup vmath

// from pbrt
bool intersect_bbox(const ray3f& ray, const range3f& bbox, float& t0, float& t1) {
    t0 = ray.tmin; t1 = ray.tmax;
    for (int i = 0; i < 3; ++i) {
        auto invRayDir = 1.f / ray.d[i];
        auto tNear = (bbox.min[i] - ray.e[i]) * invRayDir;
        auto tFar  = (bbox.max[i] - ray.e[i]) * invRayDir;
        if (tNear > tFar) std::swap(tNear, tFar);
        t0 = tNear > t0 ? tNear : t0;
        t1 = tFar  < t1 ? tFar  : t1;
        if (t0 > t1) return false;
    }
    return true;
}
    
bool intersect_triangle(const ray3f& ray,
                        const vec3f& v0, const vec3f& v1, const vec3f& v2,
                        float& t, float& ba, float& bb) {
    auto a = v0 - v2;
    auto b = v1 - v2;
    auto e = ray.e - v2;
    auto i = ray.d;
    
    auto d = dot(cross(i,b),a);
    if(d == 0) return false;
    
    t =  dot(cross(e,a),b) / d;
    if(t < ray.tmin or t > ray.tmax) return false;
    
    ba = dot(cross(i,b),e) / d;
    bb = dot(cross(a,i),e) / d;
    if(ba < 0 or bb < 0 or ba+bb > 1) return false;
    
    return true;
}

bool intersect_sphere(const ray3f& ray, const vec3f& o, float r, float& t) {
    auto a = lengthSqr(ray.d);
    auto b = 2*dot(ray.d,ray.e-o);
    auto c = lengthSqr(ray.e-o) - r*r;
    auto d = b*b-4*a*c;
    if(d < 0) return false;
    auto tmin = (-b-sqrt(d)) / (2*a);
    auto tmax = (-b+sqrt(d)) / (2*a);
    if (tmin >= ray.tmin && tmin <= ray.tmax) {
        t = tmin;
        return true;
    }
    if (tmax >= ray.tmin && tmax <= ray.tmax) {
        t = tmax;
        return true;
    }
    return false;
}

// http://www.cl.cam.ac.uk/teaching/1999/AGraphHCI/SMAG/node2.html
// http://www.gamedev.net/topic/467789-raycylinder-intersection/
bool intersect_cylinder(const ray3f& ray, float r, float h, float& t) {
    auto a = ray.d.x*ray.d.x+ray.d.y*ray.d.y;
    auto b = 2*ray.e.x*ray.d.x+2*ray.e.y*ray.d.y;
    auto c = ray.e.x*ray.e.x+ray.e.y*ray.e.y-r*r;
    auto d = b*b-4*a*c;
    if(d < 0) return false;
    auto tmin = (-b-sqrt(d)) / (2*a);
    auto tmax = (-b+sqrt(d)) / (2*a);
    auto zmin = ray.e.z+tmin*ray.d.z;
    auto zmax = ray.e.z+tmax*ray.d.z;
    if (tmin >= ray.tmin && tmin <= ray.tmax && zmin >= 0 && zmin <= h) {
        t = tmin;
        return true;
    }
    if (tmax >= ray.tmin && tmax <= ray.tmax && zmax >= 0 && zmax <= h) {
        t = tmax;
        return true;
    }
    return false;
}

bool intersect_quad(const ray3f& ray, float w, float h, float& t, float& ba, float& bb) {
    // TODO: BUG: handle infinite intersections
    if(ray.d.z == 0) return false;
    t = - ray.e.z / ray.d.z;
    if(t < ray.tmin or t > ray.tmax) return false;
    auto p = ray.eval(t);
    if(w/2 < p.x or -w/2 > p.x or h/2 < p.y or -h/2 > p.y) return false;
    ba = p.x/w+0.5;
    bb = p.y/h+0.5;
    return true;
}

// http://geomalgorithms.com/a02-_lines.html
//    distance( Point P,  Segment P0:P1 )
//    {
//        v = P1 - P0
//        w = P - P0
//
//        if ( (c1 = w·v) <= 0 )  // before P0
//            return d(P, P0)
//            if ( (c2 = v·v) <= c1 ) // after P1
//                return d(P, P1)
//
//                b = c1 / c2
//                Pb = P0 + bv
//                return d(P, Pb)
//                }
bool intersect_point_approximate(const ray3f& ray, const vec3f& p, float r, float& t) {
    t = - dot(ray.e - p, ray.d);
    t = clamp(t, ray.tmin, ray.tmax);
    return length(p-ray.eval(t)) <= r;
}

// http://geomalgorithms.com/a05-_intersect-1.html
//    // intersect2D_2Segments(): find the 2D intersection of 2 finite segments
//    //    Input:  two finite segments S1 and S2
//    //    Output: *I0 = intersect point (when it exists)
//    //            *I1 =  endpoint of intersect segment [I0,I1] (when it exists)
//    //    Return: 0=disjoint (no intersect)
//    //            1=intersect  in unique point I0
//    //            2=overlap  in segment from I0 to I1
//    int
//    intersect2D_2Segments( Segment S1, Segment S2, Point* I0, Point* I1 )
//    {
//        Vector    u = S1.P1 - S1.P0;
//        Vector    v = S2.P1 - S2.P0;
//        Vector    w = S1.P0 - S2.P0;
//        float     D = perp(u,v);
//
//        // test if  they are parallel (includes either being a point)
//        if (fabs(D) < SMALL_NUM) {           // S1 and S2 are parallel
//            if (perp(u,w) != 0 || perp(v,w) != 0)  {
//                return 0;                    // they are NOT collinear
//            }
//            // they are collinear or degenerate
//            // check if they are degenerate  points
//            float du = dot(u,u);
//            float dv = dot(v,v);
//            if (du==0 && dv==0) {            // both segments are points
//                if (S1.P0 !=  S2.P0)         // they are distinct  points
//                    return 0;
//                *I0 = S1.P0;                 // they are the same point
//                return 1;
//            }
//            if (du==0) {                     // S1 is a single point
//                if  (inSegment(S1.P0, S2) == 0)  // but is not in S2
//                    return 0;
//                *I0 = S1.P0;
//                return 1;
//            }
//            if (dv==0) {                     // S2 a single point
//                if  (inSegment(S2.P0, S1) == 0)  // but is not in S1
//                    return 0;
//                *I0 = S2.P0;
//                return 1;
//            }
//            // they are collinear segments - get  overlap (or not)
//            float t0, t1;                    // endpoints of S1 in eqn for S2
//            Vector w2 = S1.P1 - S2.P0;
//            if (v.x != 0) {
//                t0 = w.x / v.x;
//                t1 = w2.x / v.x;
//            }
//            else {
//                t0 = w.y / v.y;
//                t1 = w2.y / v.y;
//            }
//            if (t0 > t1) {                   // must have t0 smaller than t1
//                float t=t0; t0=t1; t1=t;    // swap if not
//            }
//            if (t0 > 1 || t1 < 0) {
//                return 0;      // NO overlap
//            }
//            t0 = t0<0? 0 : t0;               // clip to min 0
//            t1 = t1>1? 1 : t1;               // clip to max 1
//            if (t0 == t1) {                  // intersect is a point
//                *I0 = S2.P0 +  t0 * v;
//                return 1;
//            }
//
//            // they overlap in a valid subsegment
//            *I0 = S2.P0 + t0 * v;
//            *I1 = S2.P0 + t1 * v;
//            return 2;
//        }
//
//        // the segments are skew and may intersect in a point
//        // get the intersect parameter for S1
//        float     sI = perp(v,w) / D;
//        if (sI < 0 || sI > 1)                // no intersect with S1
//            return 0;
//
//        // get the intersect parameter for S2
//        float     tI = perp(u,w) / D;
//        if (tI < 0 || tI > 1)                // no intersect with S2
//            return 0;
//
//        *I0 = S1.P0 + sI * u;                // compute S1 intersect point
//        return 1;
//    }
//    //===================================================================
// http://geomalgorithms.com/a07-_distance.html#dist3D_Segment_to_Segment
bool intersect_line_approximate(const ray3f& ray, const vec3f& v0, const vec3f& v1, float r0, float r1, float& t, float& s) {
    auto r = ray3f::segment(v0, v1);
    if(abs(dot(ray.d,r.d)) == 1) return false;
    auto a = dot(ray.d,ray.d);
    auto b = dot(ray.d,r.d);
    auto c = dot(r.d,r.d);
    auto d = dot(ray.d,ray.e-r.e);
    auto e = dot(r.d,ray.e-r.e);
    t = (b*e-c*d)/(a*c-b*b);
    s = (a*e-b*d)/(a*c-b*b);
    t = clamp(t, ray.tmin, ray.tmax);
    s = clamp(s, r.tmin, r.tmax);
    auto ss = s / length(v1-v0);
    auto rr = r0*(1-ss)+r1*ss;
    if(dist(ray.eval(t), r.eval(s)) > rr) return false;
    s = ss;
    return true;
}
