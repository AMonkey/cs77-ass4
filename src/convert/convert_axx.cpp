#include "io.h"
#include <tclap/CmdLine.h>

///@file convert/convert_axx.cpp ConvertAxx: Converter from AXX @ingroup apps
///@defgroup convert_axx ConvertAxx: Converter from AXX
///@ingroup convert
///@{

#define LINE_SIZE 1024

string filename_amc;
string filename_asf;
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

struct _PBone {
    string name;
    
    vec3f   dir;
    float   len;
    vec3f   rot;
    
    vec3i   dof;
    
    vector<vec3f> rot_anim;
    
    vector<_PBone*> bones;
    
    ~_PBone() { for(auto b : bones) delete b; }
};

struct _PRoot {
    vec3f   pos;
    vec3f   rot;
    
    vector<vec3f> pos_anim;
    vector<vec3f> rot_anim;

    vector<_PBone*> bones;
    
    ~_PRoot() { for(auto b : bones) delete b; }
};

_PRoot* read_asf(const string& filename, map<string,_PBone*>& bonemap) {
    auto f = fopen(filename.c_str(), "rt");
    if(not f) { warning_va("cannot open file %s", filename.c_str()); return 0; }
    
    _PRoot* root = 0;
    
    char line[LINE_SIZE];
    
    while(fgets(line, LINE_SIZE, f)) {
        // there is no else on purpose
        if(is_blank(line)) continue;
        else if(_startswith_ws(line, "#")) continue;
        else if(_startswith_ws(line, ":name")) continue;
        else if(_startswith_ws(line, ":version")) continue;
        else if(_startswith_ws(line, ":documentation")) {
            long fpos = 0;
            while (fgets(line, LINE_SIZE, f)) { if(_startswith_ws(line, ":")) break; fpos = ftell(f); }
            fseek(f, fpos, SEEK_SET);
        } else if(_startswith_ws(line, ":units")) {
            long fpos = 0;
            while (fgets(line, LINE_SIZE, f)) { if(_startswith_ws(line, ":")) break; fpos = ftell(f); }
            fseek(f, fpos, SEEK_SET);
        } else if(_startswith_ws(line, ":root")) {
            long fpos = 0;
            root = new _PRoot();
            while (fgets(line, LINE_SIZE, f)) {
                if(_startswith_ws(line, "order")) continue;
                if(_startswith_ws(line, "axis")) continue;
                if(_startswith_ws(line, "position")) {
                    sscanf(_skipws(line), "position %f %f %f",
                           &root->pos.x, &root->pos.y, &root->pos.z);
                }
                if(_startswith_ws(line, "orientation")) {
                    sscanf(_skipws(line), "orientation %f %f %f",
                           &root->rot.x, &root->rot.y, &root->rot.z);
                }
                if(_startswith_ws(line, ":")) break;
                fpos = ftell(f);
            }
            fseek(f, fpos, SEEK_SET);
        } else if(_startswith_ws(line, ":bonedata")) {
            long pos = 0; int ndof;
            _PBone* bone = 0;
            while (fgets(line, LINE_SIZE, f)) {
                if(_startswith_ws(line, "begin")) {
                    bone = new _PBone();
                } else if(_startswith_ws(line, "end")) {
                } else if(_startswith_ws(line, "id")) {
                } else if(_startswith_ws(line, "name")) {
                    char name[LINE_SIZE];
                    sscanf(_skipws(line), "name %s", name);
                    bone->name = name;
                    bonemap[string(name)] = bone;
                } else if(_startswith_ws(line, "direction")) {
                    sscanf(_skipws(line), "direction %f %f %f", &bone->dir.x, &bone->dir.y, &bone->dir.z);
                } else if(_startswith_ws(line, "length")) {
                    sscanf(_skipws(line), "length %f", &bone->len);
                } else if(_startswith_ws(line, "axis")) {
                    sscanf(_skipws(line), "axis %f %f %f", &bone->rot.x, &bone->rot.y, &bone->rot.z);
                } else if(_startswith_ws(line, "dof")) {
                    char dof[3][LINE_SIZE];
                    ndof = sscanf(_skipws(line), "dof %s %s %s", dof[0], dof[1], dof[2]);
                    bone->dof = -one3i;
                    for(int i = 0; i < ndof; i ++) {
                        if(string(dof[i]) == "rx") bone->dof[i] = 0;
                        if(string(dof[i]) == "ry") bone->dof[i] = 1;
                        if(string(dof[i]) == "rz") bone->dof[i] = 2;
                    }
                } else if(_startswith_ws(line, "limits")) {
                    float _aux0, _aux1;
                    sscanf(_skipws(line), "limits (%f,%f)", &_aux0, &_aux1);
                    for(int i = 0; i < ndof-1; i ++) {
                        fgets(line, LINE_SIZE, f);
                        sscanf(_skipws(line), "(%f,%f)", &_aux0, &_aux1);
                    }
                } else if(_startswith_ws(line, ":")) break;
                else warning_va("unknown tag %s", line);
                pos = ftell(f);
            }
            fseek(f, pos, SEEK_SET);
        } else if(_startswith_ws(line, ":hierarchy")) {
            while (fgets(line, LINE_SIZE, f)) {
                if(_startswith_ws(line, "#")) continue;
                if(_startswith_ws(line, "begin")) continue;
                if(_startswith_ws(line, "end")) break;
                char tokens[10][LINE_SIZE];
                int n = sscanf(_skipws(line), "%s %s %s %s %s %s %s %s %s %s",
                               tokens[0], tokens[1], tokens[2], tokens[3], tokens[4],
                               tokens[5], tokens[6], tokens[7], tokens[8], tokens[9]);
                if(tokens[0] == string("root")) {
                    for(int i = 1; i < n; i ++) {
                        root->bones.push_back(bonemap[tokens[i]]);
                    }
                } else {
                    for(int i = 1; i < n; i ++) {
                        bonemap[tokens[0]]->bones.push_back(bonemap[tokens[i]]);
                    }
                }
            }
        } else warning_va("unknown tag %s", line);
    }
    
    fclose(f);
    
    return root;
}

void read_amc(const string& filename, _PRoot* root, map<string,_PBone*>& bonemap) {
    auto f = fopen(filename.c_str(), "rt");
    if(not f) { warning_va("cannot open file %s", filename.c_str()); return; }

    char line[LINE_SIZE];
    
    while(fgets(line, LINE_SIZE, f)) {
        int _aux = 0; char name[LINE_SIZE];
        if(is_blank(line)) continue;
        else if(_startswith_ws(line, "#")) continue;
        else if(_startswith_ws(line, ":")) continue;
        else if(sscanf(_skipws(line), "%d", &_aux)) {
            root->pos_anim.push_back(zero3f);
            root->rot_anim.push_back(zero3f);
            for(auto p : bonemap) p.second->rot_anim.push_back(zero3f);
        } else if(sscanf(line, "%s", name)) {
            if(string(name) == "root") {
                vec3f& pos = root->pos_anim.back();
                vec3f& rot = root->rot_anim.back();
                sscanf(_skipws(line), "%f %f %f %f %f %f", &pos.x, &pos.y, &pos.z, &rot.x, &rot.y, &rot.z);
            } else {
                auto bone = bonemap[string(name)];
                auto dof = bone->dof;
                vec3f& rot = bone->rot_anim.back();
                if(bone->dof.y < 0) sscanf(_skipws(line), "%f", &rot[dof.x]);
                else if(bone->dof.z < 0) sscanf(_skipws(line), "%f %f", &rot[dof.x], &rot[dof.y]);
                else sscanf(_skipws(line), "%f %f %f", &rot[dof.x], &rot[dof.y], &rot[dof.z]);
            }
        } else warning("unknown value");
    }

    fclose(f);
}

frame3f make_frame(const vec3f& pos, const vec3f& rot) {
    auto ret = identity_frame3f;
    ret.o = pos;
    auto xf = rotation(z3f, radians(rot.z)) * rotation(y3f, radians(rot.y)) * rotation(x3f, radians(rot.x));
    ret.x = transform_direction(xf, x3f);
    ret.y = transform_direction(xf, y3f);
    ret.z = transform_direction(xf, z3f);
    return ret;
}

Bone* convert(_PBone* pbone, const vec3f& pos) {
	auto bone = new Bone();
	bone->f = make_frame(pos,pbone->rot);
	bone->f_ref = bone->f;
	for(auto pb : pbone->bones) bone->bones.push_back(convert(pb,pbone->dir*pbone->len));
	return bone;
}

Bone* convert(_PRoot* proot) {
    auto root = new Bone();
    root->f = make_frame(proot->pos,proot->rot);
	root->f_ref = root->f;
	for(auto pb : proot->bones) root->bones.push_back(convert(pb,zero3f));
	return root;
}

Bone* read_files(const string& filename_asf, const string& filename_amc) {
    map<string,_PBone*> bonemap;
    auto proot = read_asf(filename_asf, bonemap);
    // read_amc(filename_amc, bonemap, dofmap);
    return convert(proot);
}

void parse_args(int argc, char** argv) {
	try {
        TCLAP::CmdLine cmd("axx2igl", ' ', "0.0");
        
        TCLAP::SwitchArg flipyzArg("y","flipyz","Flip Y and Z coordinates",cmd);
        TCLAP::SwitchArg normalizeSizeArg("n","normalize","Normalize in the unit cube",cmd);
        // TCLAP::SwitchArg flipfaceArg("f","flipface","Flip face winding",cmd);
        // TCLAP::SwitchArg smoothArg("s","smooth","Smooth meshes",cmd);
        TCLAP::ValueArg<int> subsampleRatioArg("r","subsampleratio","Subsample ratio",false,1,"ratio",cmd);
        
        TCLAP::UnlabeledValueArg<string> filenameAsfArg("asf","Asf filename",true,"","asf filename",cmd);
        TCLAP::UnlabeledValueArg<string> filenameAmcArg("amc","Amc filename",true,"","amc filename",cmd);
        TCLAP::UnlabeledValueArg<string> filenameIglArg("igl","Igl filename",true,"","igl filename",cmd);
        
        cmd.parse( argc, argv );
        
        flipyz = flipyzArg.getValue();
        normalizesize = normalizeSizeArg.getValue();
        // flipface = flipfaceArg.getValue();
        // smooth = smoothArg.getValue();
        subsampleratio = subsampleRatioArg.getValue();
        
        filename_asf = filenameAsfArg.getValue();
        filename_amc = filenameAmcArg.getValue();
        filename_igl = filenameIglArg.getValue();
	} catch (TCLAP::ArgException &e) {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }
}

int main(int argc, char** argv) {
    parse_args(argc,argv);
    auto bone = read_files(filename_asf,filename_amc);
    write_object(filename_igl, bone);
}

///@}
