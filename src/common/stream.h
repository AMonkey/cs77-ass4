#ifndef _IO_ARCHIVER_H_
#define _IO_ARCHIVER_H_

#include "json.h"

///@file common/stream.h Structured stream IO @ingroup common
///@defgroup steam Structured stream IO
///@ingroup common
///@{

/// Structured stream abstract interface
struct StructuredStream {
    virtual ~StructuredStream() { }
    
    virtual bool is_reading() = 0;
    
    virtual bool null() = 0;
    
    virtual void value(bool& value) = 0;
    virtual void value(int& value) = 0;
    virtual void value(float& value) = 0;
    virtual void value(double& value) = 0;
    virtual void value(string& value) = 0;
    virtual void value(const char* value) = 0;
        
    virtual void array(int* values, int n) = 0;
    virtual void array(float* values, int n) = 0;
    virtual void array(double* values, int n) = 0;
    
    virtual int array_size() = 0;
    virtual void array_begin() = 0;
    virtual void array_end() = 0;
    virtual void array_elem_begin() = 0;
    virtual void array_elem_end() = 0;
    
    virtual bool struct_has_member(const char* name) = 0;
    virtual void struct_begin() = 0;
    virtual void struct_end() = 0;
    virtual bool struct_member_begin(const char* name) = 0;
    virtual void struct_member_end() = 0;
};

/// Stream to write to JSON
struct JsonOutputStream : StructuredStream {
    struct _Level { string indentation = ""; bool obj = false; int value = 0; };
    vector<_Level>  _stack;
    FILE*           _f;
    
    JsonOutputStream(FILE* f) : _f(f) { }
    
    virtual bool is_reading() { return false; }
    
    virtual bool null() { _nextvalue(); fprintf(_f, "null"); return false; }
    
    virtual void value(bool& value) { _nextvalue(); fprintf(_f, "%s", (value)?"true":"false"); }
    virtual void value(int& value) { _nextvalue(); fprintf(_f, "%d", value); }
    virtual void value(float& value) { _nextvalue(); fprintf(_f, "%f", value); }
    virtual void value(double& value) { _nextvalue(); fprintf(_f, "%f", value); }
    virtual void value(string& value) { _nextvalue(); fprintf(_f, "\"%s\"", value.c_str()); }
    virtual void value(const char* value) { _nextvalue(); fprintf(_f, "\"%s\"", value); }
    
    virtual void array(int* values, int n) { _nextvalue(); _array(values,n,"%i"); }
    virtual void array(float* values, int n) { _nextvalue(); _array(values,n,"%f"); }
    virtual void array(double* values, int n) { _nextvalue(); _array(values,n,"%d"); }
    
    virtual int array_size() { NOT_IMPLEMENTED_ERROR(); return 0; }
    virtual void array_begin() { _nextvalue(); _begin_compound(false); }
    virtual void array_end() { _end_compound(false); }
    virtual void array_elem_begin() { }
    virtual void array_elem_end() { }
    
    virtual void struct_begin()  { _nextvalue(); _begin_compound(true); }
    virtual void struct_end() { _end_compound(true); }
    virtual bool struct_has_member(const char* name) { NOT_IMPLEMENTED_ERROR(); return false; }
    virtual bool struct_member_begin(const char* name) { value(name); return true; }
    virtual void struct_member_end() { }
    
    void _nextvalue() {
        if(_stack.empty()) return;
        if(_stack.back().obj and _stack.back().value % 2 == 1) fprintf(_f, ": ");
        else {
            if(_stack.back().value > 0) fprintf(_f, ", ");
            _indent();
        }
        
        _stack.back().value++;
        // fprintf(_f, _stack.back().indentation.c_str());
    }
    template<typename T>
    void _array(T* values, int n, const char* fmt) {
        fprintf(_f,"[ ");
        for(int i = 0; i < n; i ++) {
            fprintf(_f, fmt, values[i]);
            if(i < n-1) fprintf(_f, ", ");
        }
        fprintf(_f, " ]");
    }
    void _begin_compound(bool obj) {
        fprintf(_f, (obj)?"{ ":"[ ");
        
        _stack.push_back(_Level());
        _stack.back().obj = obj;
        if(_stack.size()>1) _stack.back().indentation = _stack[_stack.size()-2].indentation + "  ";
        else _stack.back().indentation = "  ";
    }
    void _end_compound(bool obj) {
        bool empty = _stack.back().value == 0;
        _stack.pop_back();
        if(not empty) _indent();
        fprintf(_f, (obj)?"}":"]");
    }
    void _indent() {
        fprintf(_f, "\n");
        if(_stack.empty()) return;
        fprintf(_f, "%s", _stack.back().indentation.c_str());
    }
};

/// Stream to read JSON
struct JsonInputStream : StructuredStream {
    vector<int>                     _stack;
    vector<vector<string>>          _used_vars;
    vector<int>                     _array_idx;
    ParsedJson*                     _json = nullptr;
    
    JsonInputStream(FILE* f) { _json = new ParsedJson(f); _stack.push_back(0); }
    virtual ~JsonInputStream() { if(_json) delete _json; }
    
    virtual bool is_reading() { return true; }

    virtual bool null() { return _json->is_null(_stack.back()); }
    
    virtual void value(bool& value) { _json->get_value(_stack.back(),value); }
    virtual void value(int& value) { _json->get_value(_stack.back(),value); }
    virtual void value(float& value) { _json->get_value(_stack.back(),value); }
    virtual void value(double& value) { _json->get_value(_stack.back(),value); }
    virtual void value(string& value) { _json->get_value(_stack.back(),value); }
    virtual void value(const char* value) { ERROR("should not have gotten here"); }
    
    virtual void array(int* values, int n) { _array(values, n); }
    virtual void array(float* values, int n) { _array(values, n); }
    virtual void array(double* values, int n) { _array(values, n); }
    
    virtual int array_size() { return _json->get_array_size(_stack.back()); }
    
    virtual void array_begin() { _array_idx.push_back(0); }
    virtual void array_end() { _array_idx.pop_back(); }
    
    virtual void array_elem_begin() { _stack.push_back(_json->get_child(_stack.back(),_array_idx.back())); }
    virtual void array_elem_end() { _stack.pop_back(); _array_idx.back() += 1; }
    
    virtual void struct_begin() { _used_vars.push_back(vector<string>()); }
    virtual void struct_end() {
        for(int i = 0; i < _json->get_size(_stack.back()); i += 2) {
            string name;
            _json->get_value(_json->get_child(_stack.back(),i),name);
            if(name == "_type" or name == "_id" or name == "_comment") continue;
            bool found = false;
            for(auto vv : _used_vars.back()) { found = vv == name; if(found) break; }
            WARNING_IF_NOT(found, "unknown member %s", name.c_str());
        }
        _used_vars.pop_back();
    }
    
    virtual bool struct_has_member(const char* name) { return _json->has_object_member(_stack.back(),name); }
    virtual bool struct_member_begin(const char* name) {
        if(struct_has_member(name)) {
            _used_vars.back().push_back(name);
            _stack.push_back(_json->get_object_member(_stack.back(),name));
            return true;
        } else return false;
    }
    virtual void struct_member_end() { _stack.pop_back(); }
    
    template<typename T>
    void _array(T* values, int n) {
        for(int i = 0; i < n; i ++) {
            _json->get_value(_json->get_array_member(_stack.back(), i), values[i]);
        }
    }
};

///@}

#endif
