#include "igl/serialize.h"
#include <map>
#include <tclap/CmdLine.h>

///@file convert/convert_obj.cpp ConvertObj: Converter from OBJ @ingroup apps
///@defgroup convert_obj ConvertObj: Converter from OBJ
///@ingroup convert
///@{

string filename_obj;
string filename_igl;

bool flipface = false;
bool normalizesize = false;
bool flipyz = false;
bool smooth = false;

#define LINE_SIZE 1024

bool is_blank(const char* line) {
    while(*line) {
        if((*line) != ' ' and (*line) != '\n' and
           (*line) != '\r' and (*line) != '\t') return false;
        line++;
    }
    return true;
}

const char* _skipws(const char* s) {
    while(*s) { if(*s != ' ' and *s != '\t') break; s++; }
    return s;
}

bool _startswith(const char* s, const char* s1) {
    return not strncmp(s, s1, strlen(s1));
}

bool _startswith_ws(const char* s, const char* s1) {
    return _startswith(_skipws(s), s1);
}

void _tolower(char* str) {
    while(*str) { *str = tolower(*str); str++; }
}

void add_material(std::map<string,Material*>& materials, std::map<string,Texture*>& textures,
                  string& cur_name, int& cur_illum,
                  vec3f& cur_ka, vec3f& cur_kd, vec3f& cur_ks, vec3f& cur_kt,
                  float& cur_o, float& cur_n,
                  string& cur_map_ka, string& cur_map_kd, string& cur_map_ks, string& cur_map_bump) {
    if(cur_illum  == 0 or cur_name == "") return;
    
    auto material = new Phong();
    material->diffuse = cur_kd;
    material->specular = cur_ks;
    material->exponent = cur_n;
    if(cur_map_kd != "") {
        if(textures.find(cur_map_kd) != textures.end()) {
            material->diffuse_texture = textures[cur_map_kd];
        } else {
            auto t = new Texture();
            t->filename = cur_map_kd;
            material->diffuse_texture = t;
            textures[cur_map_kd] = t;
        }
    }
    if(cur_map_ks != "") {
        if(textures.find(cur_map_ks) != textures.end()) {
            material->specular_texture = textures[cur_map_ks];
        } else {
            auto t = new Texture();
            t->filename = cur_map_ks;
            material->specular_texture = t;
            textures[cur_map_ks] = t;
        }
    }
    materials[cur_name] = material;
    
    cur_illum = 0;
    cur_name = "";
    cur_illum = 0;
    cur_ka = cur_kd = cur_ks = cur_ks = zero3f;
    cur_o = cur_n = 0;
    cur_map_ka = cur_map_ks = cur_map_kd = cur_map_bump = "";
}

string tolowerstr(const string& s) {
    string ret = "";
    for(auto c : s) ret += tolower(c);
    return ret;
}

void _topng(string& filename) {
    auto head = filename.substr(0,filename.length()-3);
    auto tail = filename.substr(filename.length()-3,filename.length());
    if(tail == "jpg" or tail == "JPG") filename = tolowerstr(head)+"png";
    else filename = tolowerstr(head+tail);
}

std::map<string,Material*> read_mtl(const string& filename) {
    std::map<string,Material*> materials;
    std::map<string,Texture*> textures;
    
    FILE* f = fopen(filename.c_str(), "rt");
    if(not f) { WARNING("cannot open file: %s", filename.c_str()); return materials; }
    
    string cur_name = "";
    int cur_illum;
    vec3f cur_ka, cur_kd, cur_ks, cur_kt;
    float cur_o = 1, cur_n = 1;
    string cur_map_ka, cur_map_kd, cur_map_ks, cur_map_bump;
    
    char line[LINE_SIZE], token[LINE_SIZE];
    while(fgets(line, LINE_SIZE, f)) {        
        if(is_blank(line)) continue;
        if(_startswith(line, "#")) continue;
        
        if(_startswith(line, "newmtl")) {
            add_material(materials, textures, cur_name, cur_illum,
                         cur_ka, cur_kd, cur_ks, cur_kt,
                         cur_o, cur_n,
                         cur_map_ka, cur_map_kd, cur_map_ks, cur_map_bump);
            sscanf(line, "newmtl %s", token);
            _tolower(token);
            cur_name = token;
        } else if(_startswith_ws(line, "illum")) {
            sscanf(_skipws(line), "illum %d", &cur_illum);
        } else if(_startswith_ws(line, "Ka")) {
            sscanf(_skipws(line), "Ka %f %f %f", &cur_ka.x, &cur_ka.y, &cur_ka.z);
        } else if(_startswith_ws(line, "Kd")) {
            sscanf(_skipws(line), "Kd %f %f %f", &cur_kd.x, &cur_kd.y, &cur_kd.z);
        } else if(_startswith_ws(line, "Ks")) {
            sscanf(_skipws(line), "Ks %f %f %f", &cur_ks.x, &cur_ks.y, &cur_ks.z);
        } else if(_startswith_ws(line, "Tf")) {
            sscanf(_skipws(line), "Tf %f %f %f", &cur_kt.x, &cur_kt.y, &cur_kt.z);
        } else if(_startswith_ws(line, "d")) {
            sscanf(_skipws(line), "d %f", &cur_o);
        } else if(_startswith_ws(line, "Ns")) {
            sscanf(_skipws(line), "Ns %f", &cur_n);
        } else if(_startswith_ws(line, "map_Ka")) {
            sscanf(_skipws(line), "map_Ka %s", token);
            cur_map_ka = token;
            _topng(cur_map_ka);
        } else if(_startswith_ws(line, "map_Kd")) {
            sscanf(_skipws(line), "map_Kd %s", token);
            cur_map_kd = token;
            _topng(cur_map_kd);
        } else if(_startswith_ws(line, "map_Ks")) {
            sscanf(_skipws(line), "map_Ks %s", token);
            cur_map_ks = token;
            _topng(cur_map_ks);
        } else if(_startswith_ws(line, "map_bump")) {
            sscanf(_skipws(line), "map_bump %s", token);
            cur_map_bump = token;
            _topng(cur_map_bump);
        } else WARNING("unknown material tag %s",line);
    }
    add_material(materials, textures, cur_name, cur_illum,
                 cur_ka, cur_kd, cur_ks, cur_kt,
                 cur_o, cur_n,
                 cur_map_ka, cur_map_kd, cur_map_ks, cur_map_bump);
    
    fclose(f);
    return materials;
}

struct Compare_vec3i {
    bool operator()(const vec3i& a, const vec3i& b) const {
        if(a.x<b.x) return true;
        if(a.x>b.x) return false;
        if(a.y<b.y) return true;
        if(a.y>b.y) return false;
        if(a.z<b.z) return true;
        if(a.z>b.z) return false;
        return false;
    }
};

void add_surface(vector<vec3f>& cur_pos, vector<vec3f>& cur_norm, vector<vec2f>& cur_uv,
                 vector<vec3i>& cur_triangle, vector<vec4i>& cur_quad,
                 Material* cur_material,
                 std::map<vec3i,int,Compare_vec3i>& cur_index, PrimitiveGroup* cur_group) {
    if(cur_triangle.empty() and cur_quad.empty()) return;
    auto surface = new Surface();
    if(cur_triangle.size() == 0 and cur_quad.size() > 0) {
        auto mesh = new Mesh();
        mesh->pos = cur_pos;
        mesh->norm = cur_norm;
        mesh->texcoord = cur_uv;
        mesh->quad = cur_quad;
        surface->shape = mesh;
    } else if(cur_triangle.size() > 0 and cur_quad.size() == 0) {
        auto mesh = new TriangleMesh();
        mesh->pos = cur_pos;
        mesh->norm = cur_norm;
        mesh->texcoord = cur_uv;
        mesh->triangle = cur_triangle;
        surface->shape = mesh;
    } else if(cur_triangle.size() > 0 and cur_quad.size() > 0) {
        auto mesh = new Mesh();
        mesh->pos = cur_pos;
        mesh->norm = cur_norm;
        mesh->texcoord = cur_uv;
        mesh->triangle = cur_triangle;
        mesh->quad = cur_quad;
        surface->shape = mesh;
    }
    surface->material = cur_material;
    cur_group->prims.push_back(surface);
    cur_pos.clear();
    cur_norm.clear();
    cur_uv.clear();
    cur_triangle.clear();
    cur_quad.clear();
    cur_index.clear();
}

void _fix(PrimitiveGroup* group, bool flipface, bool normalizesize, bool flipyz, bool smooth) {
    if(smooth) {
        for(auto p : group->prims) {
            auto s = dynamic_cast<Surface*>(p);
            if(is<TriangleMesh>(s->shape)) { if((cast<TriangleMesh>(s->shape))->norm.empty()) shape_smooth_frames(cast<TriangleMesh>(s->shape)); }
            if(is<Mesh>(s->shape)) { if((cast<Mesh>(s->shape))->norm.empty()) shape_smooth_frames(cast<Mesh>(s->shape)); }
        }
    }
    range3f bbox;
    for(auto p : group->prims) {
        auto s = dynamic_cast<Surface*>(p);
        vector<vec3f>* pos_ptr;
        if(dynamic_cast<TriangleMesh*>(s->shape)) { pos_ptr = &(dynamic_cast<TriangleMesh*>(s->shape))->pos; }
        if(dynamic_cast<Mesh*>(s->shape)) { pos_ptr = &(dynamic_cast<Mesh*>(s->shape))->pos; }
        for(auto pos : *pos_ptr) bbox = runion(bbox,pos);
    }
    for(auto p : group->prims) {
        auto s = dynamic_cast<Surface*>(p);
        vector<vec3f>* pos_ptr = 0;
        vector<vec3f>* norm_ptr = 0;
        vector<vec3i>* triangle_ptr = 0;
        vector<vec4i>* quad_ptr = 0;
        if(dynamic_cast<TriangleMesh*>(s->shape)) { pos_ptr = &(dynamic_cast<TriangleMesh*>(s->shape))->pos; norm_ptr = &(dynamic_cast<TriangleMesh*>(s->shape))->norm; triangle_ptr = &(dynamic_cast<TriangleMesh*>(s->shape))->triangle; }
        if(dynamic_cast<Mesh*>(s->shape)) { pos_ptr = &(dynamic_cast<Mesh*>(s->shape))->pos; norm_ptr = &(dynamic_cast<Mesh*>(s->shape))->norm; quad_ptr = &(dynamic_cast<Mesh*>(s->shape))->quad; triangle_ptr = &(dynamic_cast<Mesh*>(s->shape))->triangle; }
        if(normalizesize) {
            vec3f c = center(bbox); float s = 1/max_component(size(bbox));
            for(vec3f& v : *pos_ptr) v = (v-c)*s;
        }
        if(flipyz) {
            for(vec3f& p : *pos_ptr) swap(p.y,p.z);
            for(vec3f& n : *norm_ptr) swap(n.y,n.z);
        }
        if(flipface) {
            if(triangle_ptr) for(vec3i& f : *triangle_ptr) swap(f.y,f.z);
            if(quad_ptr) for(vec4i& f : *quad_ptr) swap(f.y,f.w);
        }
    }
}

PrimitiveGroup* read_obj(const string& filename, bool flipface, bool normalizesize, bool flipyz, bool smooth) {
    FILE* f = fopen(filename.c_str(), "rt");
    if(not f) { WARNING("cannot open file: %s", filename.c_str()); return 0; }
    
    std::map<string,Material*> materials;
    
    vector<vec3f>   pos;
    vector<vec3f>   norm;
    vector<vec2f>   uv;
    
    vector<vec3f> cur_pos;
    vector<vec3f> cur_norm;
    vector<vec2f> cur_uv;
    vector<vec3i> cur_triangle;
    vector<vec4i> cur_quad;
    auto                    cur_group =  new PrimitiveGroup();
    Material*    cur_material;
    std::map<vec3i,int,Compare_vec3i>  cur_index;
    
    char line[LINE_SIZE], token[LINE_SIZE];
    while(fgets(line, LINE_SIZE, f)) {        
        if(is_blank(line)) continue;
        if(_startswith(line, "#")) continue;
        
        if(_startswith(line, "mtllib")) {
            add_surface(cur_pos,cur_norm,cur_uv,cur_triangle,cur_quad,cur_material,cur_index,cur_group);
            sscanf(line, "mtllib %s", token);
            materials = read_mtl(token);
        } else if(_startswith(line, "usemtl")) {
            add_surface(cur_pos,cur_norm,cur_uv,cur_triangle,cur_quad,cur_material,cur_index,cur_group);
            sscanf(line, "usemtl %s", token);
            _tolower(token);
            cur_material = materials[token];
        } else if(_startswith(line, "vt")) {
            vec3f uvw;
            sscanf(line, "vt %f %f %f", &uvw.x, &uvw.y, &uvw.z);
            uv.push_back(vec2f(uvw.x,uvw.y));
        } else if(_startswith(line, "vn")) {
            vec3f n;
            sscanf(line, "vn %f %f %f", &n.x, &n.y, &n.z);
            norm.push_back(n);
        } else if(_startswith(line, "v")) {
            vec4f p = vec4f(0,0,0,1);
            sscanf(line, "v %f %f %f %f", &p.x, &p.y, &p.z, &p.w);
            pos.push_back(vec3f(p.x,p.y,p.z) / p.w);
        } else if(_startswith(line, "f")) {
            vec3i f[4]; int n = 0; bool use_pos, use_norm, use_uv;
            bool ok = false;
            if(not ok) {
                n = sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
                           &f[0].x,&f[0].y,&f[0].z,&f[1].x,&f[1].y,&f[1].z,
                           &f[2].x,&f[2].y,&f[2].z,&f[3].x,&f[3].y,&f[3].z);
                if(n >= 9) { ok = true; n /= 3; use_pos = true; use_norm = true; use_uv = true; }
            }
            if(not ok) {
                n = sscanf(line, "f %d/%d %d/%d %d/%d %d/%d",
                            &f[0].x,&f[0].y,&f[1].x,&f[1].y,
                            &f[2].x,&f[2].y,&f[3].x,&f[3].y);
                if(n >= 6) { ok = true; n /= 2; use_pos = true; use_norm = false; use_uv = true; }
            }
            if(not ok) {
                n = sscanf(line, "f %d//%d %d//%d %d//%d %d//%d",
                           &f[0].x,&f[0].z,&f[1].x,&f[1].z,
                           &f[2].x,&f[2].z,&f[3].x,&f[3].z);
                if(n >= 6) { ok = true; n /= 2; use_pos = true; use_norm = true; use_uv = false; }
            }
            if(not ok) {
                n = sscanf(line, "f %d %d %d %d",
                           &f[0].x,&f[1].x,&f[2].x,&f[3].x);
                if(n >= 3) { ok = true; n /= 1; use_pos = true; use_norm = false; use_uv = false; }
            }
            auto sizes = vec3i(pos.size(),uv.size(),norm.size());
            for(int i = 0; i < n; i ++) {
                for(int j = 0; j < 3; j ++) {
                    if(f[i][j] < 0) f[i][j] += sizes[j];
                    else if(f[i][j] > 0) f[i][j] --;
                }
                if(cur_index.find(f[i]) == cur_index.end()) {
                    if(use_pos)cur_pos.push_back(pos[f[i].x]);
                    if(use_uv)cur_uv.push_back(uv[f[i].y]);
                    if(use_norm)cur_norm.push_back(norm[f[i].z]);
                    cur_index[f[i]] = cur_pos.size()-1;
                }
            }
            if(n == 3) cur_triangle.push_back(vec3i(cur_index[f[0]],cur_index[f[1]],cur_index[f[2]]));
            if(n == 4) cur_quad.push_back(vec4i(cur_index[f[0]],cur_index[f[1]],cur_index[f[2]],cur_index[f[3]]));
        } else if(_startswith(line, "g")) {
            add_surface(cur_pos,cur_norm,cur_uv,cur_triangle,cur_quad,cur_material,cur_index,cur_group);
        } else if(_startswith(line, "o")) {
            add_surface(cur_pos,cur_norm,cur_uv,cur_triangle,cur_quad,cur_material,cur_index,cur_group);
        } else WARNING("unknown obj command in line: %s", line);
    }
    add_surface(cur_pos,cur_norm,cur_uv,cur_triangle,cur_quad,cur_material, cur_index, cur_group);
    
    fclose(f);
    
    _fix(cur_group,flipface,normalizesize,flipyz,smooth);
    
    return cur_group;
}

void parse_args(int argc, char** argv) {
	try {
        TCLAP::CmdLine cmd("ply2igl", ' ', "0.0");
        
        TCLAP::SwitchArg flipyzArg("y","flipyz","Flip Y and Z coordinates",cmd);
        TCLAP::SwitchArg normalizeSizeArg("n","normalize","Normalize in the unit cube",cmd);
        TCLAP::SwitchArg flipfaceArg("f","flipface","Flip face winding",cmd);
        TCLAP::SwitchArg smoothArg("s","smooth","Smooth meshes",cmd);
        
        TCLAP::UnlabeledValueArg<string> filenameObjArg("obj","Obj filename",true,"","obj mesh",cmd);
        TCLAP::UnlabeledValueArg<string> filenameIglArg("igl","Igl filename",true,"","igl mesh",cmd);
        
        cmd.parse( argc, argv );
        
        flipyz = flipyzArg.getValue();
        normalizesize = normalizeSizeArg.getValue();
        flipface = flipfaceArg.getValue();
        smooth = smoothArg.getValue();
        
        filename_obj = filenameObjArg.getValue();
        filename_igl = filenameIglArg.getValue();
	} catch (TCLAP::ArgException &e) {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }
}

int main(int argc, char** argv) {
    parse_args(argc,argv);
    auto group = read_obj(filename_obj, flipface, normalizesize, flipyz, smooth);
    Serializer::write_json(group, filename_igl, false);
}

///@}

