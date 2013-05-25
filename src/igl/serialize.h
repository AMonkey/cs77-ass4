#ifndef _SERIALIZE_H_
#define _SERIALIZE_H_

#include "scene.h"
#include "common/stream.h"
#include <map>
#include <typeinfo>
#include <typeindex>

///@file igl/serialize.h Serialization. @ingroup igl
///@defgroup serialize Serialization
///@ingroup igl
///@{

using std::type_info;
using std::type_index;

struct Serializer;

///@name serialize interface
///@{
/// object typename for serialization
const char* serialize_typename(Node* node);
/// serialize object member via the serializer 
void serialize_members(Node* node, Serializer& ser);
///@}

/// Serializer object
struct Serializer {    
    StructuredStream*                   _ser = nullptr;
    bool                                _write_externals = true;
    
    Serializer(StructuredStream* ser, bool write_externals) :
        _ser(ser), _write_externals(write_externals) { register_object_types(); }
    
    ///@name usage interface
    ///@{
    template<typename T>
    static void print_json(T& value) { write_json(value, stdout, false); }
    
    template<typename T>
    static void write_json(T& value, const string& filename, bool write_externals) {
        auto f = fopen(filename.c_str(), "wt");
        ERROR_IF_NOT(f, "cannot open file %s", filename.c_str());
        write_json(value,f,write_externals);
        fclose(f);
    }
    
    template<typename T>
    static void write_json(T& value, FILE* f, bool write_externals) {
        auto ser = new JsonOutputStream(f);
        write(value,ser,write_externals);
        delete ser;
    }
    
    template<typename T>
    static void read(T& value, StructuredStream* ser) {
        auto s = Serializer(ser,false);
        s.serialize(value);
    }
    
    template<typename T>
    static void write(T& value, StructuredStream* ser, bool serialize_externals) {
        auto s = Serializer(ser,serialize_externals);
        s.serialize(value);
    }
    
    template<typename T>
    static void read_json(T& value, const string& filename) {
        auto f = fopen(filename.c_str(), "rt");
        ERROR_IF_NOT(f, "cannot open file %s", filename.c_str());
        read_json(value,f);
        fclose(f);
    }
    
    template<typename T>
    static void read_json(T& value, FILE* f) {
        auto ser = new JsonInputStream(f);
        read(value,ser);
        delete ser;
    }
    ///@}
    
    ///@name value and member serialization interface
    ///@{
    bool is_reading() { return _ser->is_reading(); }
    bool is_writing_externals() { return _write_externals; }
    
    template<typename T>
    void serialize_member(const char* name, T& value) {
        if(not _ser->struct_member_begin(name)) return;
        serialize(value);
        _ser->struct_member_end();
    }

    void serialize(const char* value) { _ser->value(value); }

    void serialize(int& value) { _serialize_value(value); }
    void serialize(float& value) { _serialize_value(value); }
    void serialize(bool& value) { _serialize_value(value); }
    void serialize(double& value) { _serialize_value(value); }
    void serialize(string& value) { _serialize_value(value); }
    void serialize(vec2f& value) { _serialize_rawdata(value); }
    void serialize(vec3f& value) { _serialize_rawdata(value); }
    void serialize(vec4f& value) { _serialize_rawdata(value); }
    void serialize(vec2i& value) { _serialize_rawdata(value); }
    void serialize(vec3i& value) { _serialize_rawdata(value); }
    void serialize(vec4i& value) { _serialize_rawdata(value); }
    void serialize(mat2f& value) { _serialize_rawdata(value); }
    void serialize(mat3f& value) { _serialize_rawdata(value); }
    void serialize(mat4f& value) { _serialize_rawdata(value); }
    void serialize(mat2i& value) { _serialize_rawdata(value); }
    void serialize(mat3i& value) { _serialize_rawdata(value); }
    void serialize(mat4i& value) { _serialize_rawdata(value); }
    void serialize(range1i& value) { _serialize_rawdata(value); }
    void serialize(range1f& value) { _serialize_rawdata(value); }
    void serialize(range2f& value) { _serialize_rawdata(value); }
    void serialize(range3f& value) { _serialize_rawdata(value); }
    void serialize(vector<int>& value) { _serialize_vector_value(value); }
    void serialize(vector<float>& value) { _serialize_vector_value(value); }
    void serialize(vector<double>& value) { _serialize_vector_value(value); }
    void serialize(vector<vec2f>& value) { _serialize_vector_rawdata(value); }
    void serialize(vector<vec3f>& value) { _serialize_vector_rawdata(value); }
    void serialize(vector<vec4f>& value) { _serialize_vector_rawdata(value); }
    void serialize(vector<vec2i>& value) { _serialize_vector_rawdata(value); }
    void serialize(vector<vec3i>& value) { _serialize_vector_rawdata(value); }
    void serialize(vector<vec4i>& value) { _serialize_vector_rawdata(value); }
    void serialize(vector<mat2f>& value) { _serialize_vector_rawdata(value); }
    void serialize(vector<mat3f>& value) { _serialize_vector_rawdata(value); }
    void serialize(vector<mat4f>& value) { _serialize_vector_rawdata(value); }
    void serialize(vector<mat2i>& value) { _serialize_vector_rawdata(value); }
    void serialize(vector<mat3i>& value) { _serialize_vector_rawdata(value); }
    void serialize(vector<mat4i>& value) { _serialize_vector_rawdata(value); }
    
    void serialize(frame3f& value) {
        _ser->struct_begin();
        serialize_member("o",value.o);
        serialize_member("x",value.x);
        serialize_member("y",value.y);
        serialize_member("z",value.z);
        _ser->struct_end();
        if(_ser->is_reading()) value = orthonormalize(value);
    }
    void serialize(vector<frame3f>& value) { _serialize_vector_struct(value); }
    
    // TODO: this is really bad
    template<typename T>
    void serialize(T*& value) { _serialize_object(value); }
    
    template<typename T>
    void serialize(vector<T*>& value) { _serialize_vector_object(value); }
    ///@}
    
    ///@name registry handling
    ///@{
    template<typename T>
    static void register_object_type() {
        auto func_new = []() { return dynamic_cast<Node*>(new T()); };
        auto obj = func_new();
        auto name = serialize_typename(obj);
        _registry.add(name,func_new);
    }
    
    static void register_object_types();
    ///@}
    
    ///@name implementation details
    ///@{
    template<typename T>
    void _serialize_value(T& value) { _ser->value(value); }

    template<typename T>
    void _serialize_rawdata(T& value) { _ser->array(value.raw_data(),value.raw_size()); }
    
    template<typename T>
    void _serialize_vector_value(T& value) {
        if(_ser->is_reading()) { value.resize(_ser->array_size()); }
        _ser->array(value.data(),value.size());
    }
    
    template<typename T>
    void _serialize_vector_rawdata(vector<T>& value) {
        if(_ser->is_reading()) { value.resize(_ser->array_size()/T::raw_size()); }
        _ser->array(value.data()->raw_data(),value.size()*T::raw_size());
    }
    
    template<typename T>
    void _serialize_vector_struct(vector<T>& value) {
        if(_ser->is_reading()) value.resize(_ser->array_size());
        _ser->array_begin();
        for(auto& v : value) { _ser->array_elem_begin(); serialize(v); _ser->array_elem_end(); }
        _ser->array_end();
    }
    
    template<typename T>
    void _serialize_object(T*& value) {
        static_assert(std::is_base_of<Node, T>::value, "supports only classes derived from object");
        if(_ser->is_reading()) {
            if(_ser->null()) { value = nullptr; return; }
            _ser->struct_begin();
            if(_ser->struct_has_member("_include")) {
                string filename;
                serialize_member("_include",filename);
                read_json(value,filename);
            } else if(_ser->struct_has_member("_ref")) {
                int ref;
                serialize_member("_ref",ref);
                auto ref_ptr = _object_map.get_obj(ref);
                ERROR_IF_NOT(ref_ptr, "unknown object name %d", ref);
                // TODO: object cast is a hack
                value = dynamic_cast<T*>(ref_ptr);
                ERROR_IF_NOT(value, "uncompatible types");
            } else {
                string type;
                serialize_member("_type",type);
                _registry.make_new(value,type);
                if(_ser->struct_has_member("_id")) {
                    int id;
                    serialize_member("_id",id);
                    _object_map.add(value,id);
                }
                serialize_members(value,*this);
            }
            _ser->struct_end();
        } else {
            if(not value) { _ser->null(); return; }
            _ser->struct_begin();
            if(_object_map.get_tag(value)) {
                auto tag = _object_map.get_tag(value);
                serialize_member("_ref",tag);
            } else {
                auto tn = serialize_typename(value);
                serialize_member("_type",tn);
                // TODO: serialize only used ids
                _object_map.add(value);
                auto tag = _object_map.get_tag(value);
                serialize_member("_id",tag);
                serialize_members(value,*this);
            }
            _ser->struct_end();
        }
    }
    
    template<typename T>
    void _serialize_vector_object(vector<T*>& value) {
        if(_ser->is_reading()) value.resize(_ser->array_size());
        _ser->array_begin();
        for(auto& v : value) { _ser->array_elem_begin(); serialize(v); _ser->array_elem_end(); }
        _ser->array_end();
    }
    ///@}
    
    ///@name implementation details - types
    ///@{
    struct _ObjectMap {
        std::map<Node*,int> obj2tag;
        std::map<int,Node*> tag2obj;
        int cur_tag = 1;
        
        void add(Node* obj) { int tag = cur_tag; obj2tag[obj] = tag; tag2obj[tag] = obj; cur_tag++; }
        void add(Node* obj, int tag) { obj2tag[obj] = tag; tag2obj[tag] = obj; }
        Node* get_obj(int tag) { if(tag2obj.find(tag) == tag2obj.end()) return 0; else return tag2obj[tag]; }
        int get_tag(Node* obj) { if(obj2tag.find(obj) == obj2tag.end()) return 0; else return obj2tag[obj]; }
    };
    _ObjectMap   _object_map;
    
    struct _Registry {
        std::map<string,function<Node* (void)>> funcs_new;
        void add(const string& name, const function<Node* (void)>& func_new) {
            ERROR_IF_NOT(funcs_new.find(name) == funcs_new.end(), "type %s already registered", name.c_str());
            funcs_new[name] = func_new;
        }
        template<typename T>
        T* make_new(const string& name) {
            ERROR_IF_NOT(funcs_new.find(name) != funcs_new.end(), "unregistered type %s", name.c_str());
            auto ptr = funcs_new[name]();
            // TODO: object cast is a hack
            auto ret = dynamic_cast<T*>(ptr);
            ERROR_IF_NOT(ret, "incompatible type: %s", name.c_str());
            return ret;
        }
        template<typename T>
        void make_new(T*& value, const string& name) { value = make_new<T>(name); }
    };
    static _Registry _registry;
    ///@}
};

///@}

#endif
