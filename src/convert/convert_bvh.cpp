#include "io.h"
#include <tclap/CmdLine.h>

///@file convert/convert_bvh.cpp ConvertBVH: Converter from BVH @ingroup apps
///@defgroup convert_bvh ConvertBVH: Converter from BVH
///@ingroup convert
///@{

#define LINE_SIZE 1024

string filename_bvh;
string filename_igl;

bool normalizesize = false;
bool flipyz = false;
int subsampleratio = 1;

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

struct _ParseBone {
    string                  name;
    vec3f                   offset;
    vector<vec3f>           rotations;
    vector<vec3f>           offsets;
    vector<_ParseBone*>     bones;
    
    ~_ParseBone() { for(auto b : bones) delete b; }
};

void parse_bone(FILE* f, _ParseBone* bone) {
    char line[LINE_SIZE];
    
    fgets(line, LINE_SIZE, f);
    if(not _startswith_ws(line,"{")) { error("bad file format"); return; }
    
    vec3f offset; vec3i pos_order(-1,-1,-1); vec3i rot_order(-1,-1,-1);
    
    while (fgets(line, LINE_SIZE, f)) {
        if(is_blank(line)) continue;
        if(_startswith_ws(line,"}")) break;
        else if(_startswith_ws(line,"OFFSET")) {
            sscanf(_skipws(line), "OFFSET %f %f %f", &offset.x, &offset.y, &offset.z);
            bone->offset = offset;
        } else if(_startswith_ws(line,"CHANNELS")) {
            int n; char str[6][LINE_SIZE];
            sscanf(_skipws(line), "CHANNELS %d %s %s %s %s %s %s", &n, str[0], str[1], str[2], str[3], str[4], str[5]);
        } else if(_startswith_ws(line,"JOINT") or _startswith_ws(line,"End Site")) {
            char str[LINE_SIZE];
            if(_startswith_ws(line,"JOINT")) sscanf(_skipws(line), "JOINT %s", str);
            else { sscanf(_skipws(line), "End Site"); str[0] = 0; }
            auto child = new _ParseBone();
            child->name = str;
            parse_bone(f, child);
            bone->bones.push_back(child);
        }
    }
}

void parse_anim(FILE* f, _ParseBone* bone, bool root) {
    if(root) { vec3f o, r; fscanf(f, "%f %f %f %f %f %f", &o.x, &o.y, &o.z, &r.z, &r.y, &r.x); bone->offsets.push_back(o); bone->rotations.push_back(r); }
    else { vec3f r; fscanf(f, "%f %f %f", &r.z, &r.x, &r.y); bone->rotations.push_back(r); }
    for(auto c : bone->bones) parse_anim(f, c, false);
}

void _flipyz(Bone* bone) {
    swap(bone->f.o.y,bone->f.o.z);
    //    swap(bone->f.x.y,bone->f.x.z);
    //    swap(bone->f.y.y,bone->f.y.z);
    //    swap(bone->f.z.y,bone->f.z.z);
    for(auto c : bone->bones) _flipyz(c);
}

_ParseBone* parse_hierarchy(const string& filename) {
    FILE* f = fopen(filename.c_str(), "rt");
    if(not f) { warning_va("cannot open file %s", filename.c_str()); return 0; }
    
    _ParseBone* root = 0;
    int nframes = 0;
    float frametime = 0;
    
    char line[LINE_SIZE];
    
    while (fgets(line, LINE_SIZE, f)) if(not is_blank(line)) break;
    if(not _startswith_ws(line,"HIERARCHY")) { error("bad file format"); return 0; }
    
    while (fgets(line, LINE_SIZE, f)) if(not is_blank(line)) break;
    if(not _startswith_ws(line,"ROOT")) { error("bad file format"); return 0; }
    else {
        root = new _ParseBone();
        char str[LINE_SIZE];
        sscanf(_skipws(line), "ROOT %s", str);
        root->name = str;
        parse_bone(f, root);
    }
    
    while (fgets(line, LINE_SIZE, f)) if(not is_blank(line)) break;
    if(not _startswith_ws(line,"MOTION")) { error("bad file format"); return 0; }
    
    while (fgets(line, LINE_SIZE, f)) if(not is_blank(line)) break;
    if(not _startswith_ws(line,"Frames")) { error("bad file format"); return 0; }
    else {
        sscanf(_skipws(line), "Frames: %d", &nframes);
    }
    
    while (fgets(line, LINE_SIZE, f)) if(not is_blank(line)) break;
    if(not _startswith_ws(line,"Frame Time")) { error("bad file format"); return 0; }
    else {
        sscanf(_skipws(line), "Frame Time %f", &frametime);
    }
    
    while (fgets(line, LINE_SIZE, f)) if(not is_blank(line)) break;
    for(auto i = 0; i < nframes; i ++) {
        parse_anim(f, root, true);
        fgets(line, LINE_SIZE, f);
    }
        
    fclose(f);
    
    return root;
}

Bone* _bone_convert(_ParseBone* pbone) {
    auto bone = new Bone();
    bone->f_ref.o = pbone->offset;
    for(int i = 0; i < pbone->rotations.size() / subsampleratio; i ++ ) {
        auto xf = rotation(y3f,radians(pbone->rotations[i].y))*rotation(x3f,radians(pbone->rotations[i].x))*rotation(z3f,radians(pbone->rotations[i].z));
        // auto xf = rotation(z3f,radians(pbone->rotations[i].z))*rotation(x3f,radians(pbone->rotations[i].x))*rotation(y3f,radians(pbone->rotations[i].y));
        frame3f f;
        if(pbone->offsets.size() > 0) f.o = pbone->offsets[i];
        else f.o = pbone->offset;
        f.x = transform_direction(xf, f.x);
        f.y = transform_direction(xf, f.y);
        f.z = transform_direction(xf, f.z);
        bone->f_anim.push_back(f);
    }
    for(auto c : pbone->bones) bone->bones.push_back(_bone_convert(c));
    return bone;
}

Bone* read_bvh(const string& filename, bool flipyz, int subsampleratio) {
    auto pbone = parse_hierarchy(filename);
    auto bone = _bone_convert(pbone);
    if(flipyz) _flipyz(bone);
    delete pbone;
    return bone;
}

void parse_args(int argc, char** argv) {
	try {
        TCLAP::CmdLine cmd("bvh2igl", ' ', "0.0");
        
        TCLAP::SwitchArg flipyzArg("y","flipyz","Flip Y and Z coordinates",cmd);
        TCLAP::SwitchArg normalizeSizeArg("n","normalize","Normalize in the unit cube",cmd);
        // TCLAP::SwitchArg flipfaceArg("f","flipface","Flip face winding",cmd);
        // TCLAP::SwitchArg smoothArg("s","smooth","Smooth meshes",cmd);
        TCLAP::ValueArg<int> subsampleRatioArg("r","subsampleratio","Subsample ratio",false,1,"ratio",cmd);
        
        TCLAP::UnlabeledValueArg<string> filenameBvhArg("bvh","Bvh filename",true,"","bvh filename",cmd);
        TCLAP::UnlabeledValueArg<string> filenameIglArg("igl","Igl filename",true,"","igl filename",cmd);
        
        cmd.parse( argc, argv );
        
        flipyz = flipyzArg.getValue();
        normalizesize = normalizeSizeArg.getValue();
        // flipface = flipfaceArg.getValue();
        // smooth = smoothArg.getValue();
        subsampleratio = subsampleRatioArg.getValue();
        
        filename_bvh = filenameBvhArg.getValue();
        filename_igl = filenameIglArg.getValue();
	} catch (TCLAP::ArgException &e) {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }
}

int main(int argc, char** argv) {
    parse_args(argc,argv);
    auto bone = read_bvh(filename_bvh, flipyz, subsampleratio);
    write_object(filename_igl, bone);
}

///@}

