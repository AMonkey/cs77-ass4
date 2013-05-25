#ifndef _TESSELATE_H_
#define _TESSELATE_H_

#include "scene.h"

///@file igl/tesselate.h Tesselation. @ingroup igl
///@defgroup tesselate Tesselation
///@ingroup igl
///@{

/// Hashtable for edges as pair of indices (allows to quickly lookup edge index by vertex indices)
struct EdgeHashTable {
    vector<vec2i>                       edges; ///< edge list with two vertex indices per list
    map<pair<int,int>,int>              _edgemap; ///< edge hashtable
    
    /// Contructor that builds an edge lookup hash from triangles and quads
    EdgeHashTable(const vector<vec3i>& triangle, const vector<vec4i>& quad) {
        add_faces(triangle);
        add_faces(quad);
    }
    
    /// add more faces to the edge hash
    template<typename T>
    void add_faces(const vector<T>& face) {
        for(auto f : face) {
            for(int i = 0; i < f.size(); i ++) {
                int v0idx = f[i];
                int v1idx = f[(i+1)%f.size()];
                if(_edgemap.find(std::pair<int,int>(v0idx,v1idx)) != _edgemap.end()) continue;
                edges.push_back(vec2i(v0idx,v1idx));
                _edgemap[std::pair<int,int>(v0idx,v1idx)] = edges.size()-1;
                _edgemap[std::pair<int,int>(v1idx,v0idx)] = edges.size()-1;
            }
        }
    }
    
    /// lookup the edge index from two vertex indices (-1 if not found)
    int edge(int v0, int v1) {
        if(_edgemap.find(std::pair<int,int>(v0,v1)) != _edgemap.end())
            return _edgemap[std::pair<int,int>(v0,v1)];
        else return -1;
    }
};

///@name shape tesselate interface
///@{
Shape* tesselate_shape(Shape* shape, int level, bool smooth);
///@}

///@name tesselation interface
///@{
void primitive_tesselation_init(Primitive* prim, bool override = false, int override_level = 0, bool override_smooth = false);
void primitives_tesselation_init(PrimitiveGroup* prim, bool override = false, int override_level = 0, bool override_smooth = false);
void shape_tesselation_init(Shape* shape, bool override = false, int override_level = 0, bool override_smooth = false);
void scene_tesselation_init(Scene* scene, bool override = false, int override_level = 0, bool override_smooth = false);
///@}

///@}

#endif