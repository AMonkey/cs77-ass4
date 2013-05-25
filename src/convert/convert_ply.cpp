#include "igl/serialize.h"
#include <tclap/CmdLine.h>

///@file convert/convert_ply.cpp ConvertPly: Converter from PLY @ingroup apps
///@defgroup convert_ply ConvertPly: Converter from PLY
///@ingroup convert
///@{

string filename_ply;
string filename_igl;

bool flipface = false;
bool normalizesize = false;
bool flipyz = false;

#define LINE_SIZE 2048
#define WORD_SIZE 128

bool _startswith(const char* s, const char* s1) {
    return not strncmp(s, s1, strlen(s1));
}

Shape* read_ply(const string& filename, bool flipface, bool normalize, bool flipyz) {
    FILE* f = fopen(filename.c_str(), "rt");
    if(not f) ERROR("cannot open file %s", filename.c_str());
    
    char line[LINE_SIZE];
    
    // magic
    fgets(line, LINE_SIZE, f);
    if(not _startswith(line, "ply")) ERROR("unknown file format");
    
    // format
    fgets(line, LINE_SIZE, f);
    if(not _startswith(line, "format")) ERROR("unknown file format");
    
    // skip till vertex
    fgets(line, LINE_SIZE, f);
    while(not _startswith(line, "element vertex")) fgets(line, LINE_SIZE, f);
    
    // vertex
    int vertex_num = 0;
    sscanf(line, "element vertex %d", &vertex_num);
    
    // vertex props
    int vertex_pos = -1, vertex_norm = -1, vertex_uv = -1, vertex_col8 = -1, vertex_colf = -1;
    int vertex_vals = 0;
    fgets(line, LINE_SIZE, f);
    while(_startswith(line, "property")) {
        if(_startswith(line, "property float x") or _startswith(line, "property float32 x")) vertex_pos = vertex_vals;
        if(_startswith(line, "property float nx") or _startswith(line, "property float32 nx")) vertex_norm = vertex_vals;
        if(_startswith(line, "property uchar diffuse_red") or _startswith(line, "property uint8 diffuse_red")) vertex_col8 = vertex_vals;
        if(_startswith(line, "property uchar red") or _startswith(line, "property uint8 red")) vertex_col8 = vertex_vals;
        if(_startswith(line, "property float diffuse_red") or _startswith(line, "property float32 diffuse_red")) vertex_colf = vertex_vals;
        if(_startswith(line, "property float red") or _startswith(line, "property float32 red")) vertex_colf = vertex_vals;
        vertex_vals ++;
        fgets(line, LINE_SIZE, f);
    }
    
    // skip till faces
    while(not _startswith(line, "element face")) fgets(line, LINE_SIZE, f);
    
    // face
    int face_num = 0;
    sscanf(line, "element face %d", &face_num);
    
    // face props
    int face_start = -1;
    int face_vals = 0;
    fgets(line, LINE_SIZE, f);
    while(_startswith(line, "property")) {
        if(_startswith(line, "property list uchar int vertex_ind")) face_start = face_vals;
        if(_startswith(line, "property list uchar uint vertex_ind")) face_start = face_vals;
        if(_startswith(line, "property list uint8 int32 vertex_ind")) face_start = face_vals;
        if(_startswith(line, "property list char int vertex_ind")) face_start = face_vals;
        if(_startswith(line, "property list int8 int32 vertex_ind")) face_start = face_vals;
        face_vals ++;
        fgets(line, LINE_SIZE, f);
    }
    ERROR_IF_NOT(face_start>=0, "bad face start");
    
    // skip till end of header
    while(not _startswith(line, "end_header")) fgets(line, LINE_SIZE, f);
    
    // read vertex data
    vector<vec3f> pos; vector<vec3f> norm; vector<vec2f> uv;
    for(int i = 0; i < vertex_num; i ++) {
        vector<float> vdata;
        for(int j = 0; j < vertex_vals; j ++) {
            float v;
            fscanf(f, "%f", &v);
            vdata.push_back(v);
        }
        pos.push_back( vec3f(vdata[vertex_pos+0],vdata[vertex_pos+1],vdata[vertex_pos+2]) );
        if(vertex_norm >= 0) norm.push_back( vec3f(vdata[vertex_norm+0],vdata[vertex_norm+1],vdata[vertex_norm+2]) );
        if(vertex_uv >= 0) uv.push_back( vec2f(vdata[vertex_uv+0],vdata[vertex_uv+1] ));
    }
    
    if(normalize) {
        range3f bbox;
        for(auto v : pos) bbox = runion(bbox,v);
        vec3f c = center(bbox); float s = 1/max_component(size(bbox));
        for(vec3f& v : pos) v = (v-c)*s;
    }
    
    if(flipyz) {
        for(vec3f& v : pos) swap(v.y,v.z);
        for(vec3f& v : norm) swap(v.y,v.z);
    }
    
    // read vertex data
    vector<vec3i> triangles; vector<vec4i> quads;
    for(int i = 0; i < face_num; i ++) {
        int n = 0; fscanf(f, "%d", &n);
        if(n == 3) { vec3i face; for(int j=0;j<3;j++) fscanf(f, "%d", &(face[j])); triangles.push_back(face); }
        if(n == 4) { vec4i face; for(int j=0;j<4;j++) fscanf(f, "%d", &(face[j])); quads.push_back(face); }
        if(n != 3 and n != 4) ERROR("unsupported face type");
    }
    
    if(flipface) {
        for(vec3i& t : triangles) swap(t.y,t.z);
        for(vec4i& t : quads) swap(t.y,t.w);
    }
    
    fclose(f);
    
    if(triangles.size() == 0 and quads.size() > 0) {
        auto mesh = new Mesh();
        mesh->pos = pos;
        mesh->norm = norm;
        mesh->texcoord = uv;
        mesh->quad = quads;
        return mesh;
    } else if(triangles.size() > 0 and quads.size() == 0) {
        auto mesh = new TriangleMesh();
        mesh->pos = pos;
        mesh->norm = norm;
        mesh->texcoord = uv;
        mesh->triangle = triangles;
        return mesh;
    } else if(triangles.size() > 0 and quads.size() > 0) {
        auto mesh = new Mesh();
        mesh->pos = pos;
        mesh->norm = norm;
        mesh->texcoord = uv;
        mesh->triangle = triangles;
        mesh->quad = quads;
        return mesh;
    } else { WARNING("empty mesh"); return 0; }
}

void parse_args(int argc, char** argv) {
	try {
        TCLAP::CmdLine cmd("ply2igl", ' ', "0.0");
        
        TCLAP::ValueArg<int> resolutionArg("r","resolution","Image resolution",false,512,"resolution",cmd);
        TCLAP::ValueArg<int> samplesArg("s","samples","Image samples",false,4,"samples",cmd);
        
        TCLAP::SwitchArg flipyzArg("y","flipyz","Flip Y and Z coordinates",cmd);
        TCLAP::SwitchArg normalizeSizeArg("n","normalize","Normalize in the unit cube",cmd);
        TCLAP::SwitchArg flipfaceArg("f","flipface","Flip face winding",cmd);
        
        TCLAP::UnlabeledValueArg<string> filenamePlyArg("ply","Ply filename",true,"","ply mesh",cmd);
        TCLAP::UnlabeledValueArg<string> filenameIglArg("igl","Igl filename",true,"","igl mesh",cmd);
        
        cmd.parse( argc, argv );
        
        flipyz = flipyzArg.getValue();
        normalizesize = normalizeSizeArg.getValue();
        flipface = flipfaceArg.getValue();
        
        filename_ply = filenamePlyArg.getValue();
        filename_igl = filenameIglArg.getValue();
	} catch (TCLAP::ArgException &e) {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }
}

int main(int argc, char** argv) {
    parse_args(argc,argv);
    auto mesh = read_ply(filename_ply, flipface, normalizesize, flipyz);
    Serializer::write_json(mesh, filename_igl, false);
}

///@}
