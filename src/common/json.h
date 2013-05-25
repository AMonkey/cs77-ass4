#ifndef _JSON_H_
#define _JSON_H_

#include "std.h"
#include "debug.h"
#include <string.h>

///@file common/json.h Json format support @ingroup common
///@defgroup json Json format support
///@ingroup common
///@{

/// Parses a Json file in memory and allow access to its members
struct ParsedJson {
    string _json;
    struct _Value { int start, end; vector<int> children; };
    vector<_Value> _values;
    
    /// Parses a JSON string
    ParsedJson(const string& json) { _parse(json); }
    /// Parses a JSON file
    ParsedJson(FILE* file) { _parse(file); }
    
    ///@name external interface
    ///@{
    bool is_null(int v) { return _json[_values[v].start] == 'n'; }
    bool is_true(int v) { return _json[_values[v].start] == 't'; }
    bool is_false(int v) { return _json[_values[v].start] == 'f'; }
    bool is_bool(int v) { return is_true(v) or is_false(v); }
    bool is_number(int v) { return _json[_values[v].start] == '+' or _json[_values[v].start] == '-' or isdigit(_json[_values[v].start]); }
    bool is_string(int v) { return _json[_values[v].start] == '"'; }
    bool is_object(int v) { return _json[_values[v].start] == '{'; }
    bool is_array(int v) { return _json[_values[v].start] == '['; }
    
    void _debug(int v) {
        printf("json (debug): ");
        for( int i = _values[v].start; i < _values[v].end; i++ ) {
            printf("%c",_json[i]);
        }
        printf("\n");
    }
    
    void get_value(int v, bool& value) { ERROR_IF_NOT(is_bool(v), "bool expected"); value = is_true(v); }
    void get_value(int v, int& value) { ERROR_IF_NOT(is_number(v), "number (int) expected"); ERROR_IF_NOT(sscanf(_json.c_str()+_values[v].start, "%d", &value) == 1, "int expected"); }
    void get_value(int v, float& value) {
        /*if( !is_number(v) ) {
            printf( "json: float expected\n" );
            _debug(v);
            error("");
        }*/
        ERROR_IF_NOT(is_number(v), "number (float) expected");
        ERROR_IF_NOT(sscanf(_json.c_str()+_values[v].start, "%f", &value) == 1, "float expected");
    }
    void get_value(int v, double& value) { ERROR_IF_NOT(is_number(v), "number (double) expected"); ERROR_IF_NOT(sscanf(_json.c_str()+_values[v].start, "%lf", &value) == 1, "double expected"); }
    void get_value(int v, string& value) { ERROR_IF_NOT(is_string(v), "string expected"); value = _json.substr(_values[v].start+1,_values[v].end-_values[v].start-1); }

    int get_size(int v) { return _values[v].children.size(); }
    int get_child(int v, int i) { ERROR_IF_NOT(i >= 0 and i < get_size(v), "index out of range"); return _values[v].children[i]; }
    
    int get_array_size(int v) { ERROR_IF_NOT(is_array(v), "array expected"); return get_size(v); }
    int get_array_member(int v, int i) { ERROR_IF_NOT(is_array(v), "array expected"); return get_child(v, i); }
    
    bool has_object_member(int v, const char* name) { return _find_object_member(v, name) >= 0; }
    int get_object_member(int v, const char* name) {
        int ret = _find_object_member(v, name);
        ERROR_IF_NOT(ret >= 0, "object member not found");
        return ret;
    }
    ///@}
    
    ///@name implementation
    ///@{
    int _find_object_member(int v, const char* name) {
        ERROR_IF_NOT(is_object(v), "object expected");
        for(int i = 0; i < get_size(v); i += 2) {
            auto c = get_child(v, i);
            auto s = _values[c].start+1;
            auto l = _values[c].end-_values[c].start-1;
            if(strlen(name) != l) continue;
            if(not strncmp(name,_json.c_str()+s,l)) return get_child(v,i+1);
        }
        return -1;
    }
    
    void _parse(FILE* file);
    void _parse(const string& _json);
    
    int _rec_parse(int v, int cur);
    void _parse_error(int pos, const char* msg) { ERROR(msg); }
    int _add_value() { _values.push_back(_Value()); return _values.size()-1; }
    void _add_child(int v, int i) { _values[v].children.push_back(i); }
    int _skipws(int cur);
    void _check(int cur, const char* msg, int n);
    ///@}
};

///@}

#endif
