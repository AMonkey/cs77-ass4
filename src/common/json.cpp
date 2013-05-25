#include "json.h"

///@file common/json.cpp Json format support @ingroup common

void ParsedJson::_parse(const string& str) {
    _json = str;
    _values.clear();
    auto v = _add_value();
    int end = _rec_parse(0,v);
    if(end != _json.size()) _parse_error(end,"_json not closed");
}

void ParsedJson::_parse(FILE *file) {
    string _json = "";
    while(not feof(file)) {
        char buf[1024];
        fgets(buf, 1024, file);
        _json += buf;
    }
    _parse(_json);
}

int ParsedJson::_skipws(int cur) {
    while(_json[cur] == ' ' or _json[cur] == '\t' or _json[cur] == '\r' or _json[cur] == '\n') cur++;
    return cur;
}
void ParsedJson::_check(int cur, const char* msg, int n) {
    for(int i = 0; i < n; i ++) if(_json[cur+i] != msg[i]) _parse_error(cur, "constant expected");
}

int ParsedJson::_rec_parse(int v, int cur) {
    cur = _skipws(cur);
    if(cur >= _json.length()) return cur;
    _values[v].start = cur;
    if(_json[cur] == '{') {
        cur++;
        cur = _skipws(cur);
        while(_json[cur] != '}') {
            cur = _skipws(cur);
            if(_json[cur] != '\"') _parse_error(cur, "string expected");
            auto n = _add_value();
            _add_child(v, n);
            cur = _rec_parse(n,cur);
            cur = _skipws(cur);
            if(_json[cur] != ':') _parse_error(cur, ": expected");
            cur++;
            cur = _skipws(cur);
            auto m = _add_value();
            _add_child(v, m);
            cur = _rec_parse(m,cur);
            cur = _skipws(cur);
            if(_json[cur] != '}' and _json[cur] != ',') _parse_error(cur,"} or , expected");
            if(_json[cur] == ',') cur++;
            cur = _skipws(cur);            
        }
        _values[v].end = cur;
    } else if(_json[cur] == '[') {
        cur++;
        cur = _skipws(cur);
        while(_json[cur] != ']') {
            cur = _skipws(cur);
            auto m = _add_value();
            _add_child(v, m);
            cur = _rec_parse(m,cur);
            cur = _skipws(cur);
            if(_json[cur] != ']' and _json[cur] != ',') _parse_error(cur,"] or , expected");
            if(_json[cur] == ',') cur++;
            cur = _skipws(cur);
        }
        cur = _skipws(cur);
    } else if(_json[cur] == 't') {
        _check(cur,"true",4);
        cur += 3;
    } else if(_json[cur] == 'f') {
        _check(cur,"false",5);
        cur += 4;
    } else if(_json[cur] == 'n') {
        _check(cur,"null",4);
        cur += 3;
    } else if(_json[cur] == '"') {
        cur++;
        while(_json[cur] != '"') {
            if(_json[cur] == '\\') {
                cur++;
                if(_json[cur] == 'u') {
                    _parse_error(cur, "unsupported unicode strings");
                } else if(_json[cur] != '"' and _json[cur] != '\\' and _json[cur] != '/' and _json[cur] != 't' and
                   _json[cur] != 'n' and _json[cur] != 'r' and _json[cur] != 'b' and _json[cur] != 'f') {
                     _parse_error(cur, "unsupported string escapes");
                } else {
                    _parse_error(cur, "unknown string escape");
                }
            }
            cur++;
        }
    } else if(_json[cur] == '+' or _json[cur] == '-' or isdigit(_json[cur])) {
        if(_json[cur] == '+' or _json[cur] == '-') cur++;
        while(isdigit(_json[cur])) cur++;
        if(_json[cur] == '.') {
            cur++;
            while(isdigit(_json[cur])) cur++;
        }
        if(_json[cur] == 'e' or _json[cur] == 'E') {
            cur++;
            if(_json[cur] == '+' or _json[cur] == '-') cur++;
            while(isdigit(_json[cur])) cur++;
        }
        cur--;
    } else _parse_error(cur, "unknown token type");
    _values[v].end = cur;
    cur++;
    cur = _skipws(cur);
    return cur;
}
