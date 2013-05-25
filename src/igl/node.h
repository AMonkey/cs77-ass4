#ifndef _NODE_H_
#define _NODE_H_

#include "common/common.h"
#include "vmath/vmath.h"

#define _FAST_RTTI
// #define _FAST_RTTI_VIRTUAL

///@file igl/node.h Scene Nodes. @ingroup igl
///@defgroup node Scene Nodes
///@ingroup igl
///@{

/// Abstract Scene Node
struct Node {
    static const int _typeuid = 0;
    int _tid = 0;
    
    virtual ~Node() { }
    virtual unsigned int _gettypeuid() { return 0; }
};

#ifdef _FAST_RTTI
#ifdef _FAST_RTTI_VIRTUAL
#define REGISTER_FAST_RTTI(Super,Class,ID) \
    static constexpr unsigned int _typeuid = Super::_typeuid * 256 + ID; \
    virtual unsigned int _gettypeuid() override { return _typeuid; }
#else
#define REGISTER_FAST_RTTI(Super,Class,ID) \
    static constexpr unsigned int _typeuid = Super::_typeuid * 256 + ID; \
    Class() { _tid = _typeuid; }
#endif
#else
#define REGISTER_FAST_RTTI(Super,Class,ID)
#endif

/// cast to a subtype
template<typename T, typename U>
inline T* cast(U* ptr) {
    return static_cast<T*>(ptr);
}

/// check if a pointer is a pairtcular type (uses rtti)
template <typename T, typename U>
inline bool is(U* ptr) {
#ifdef _FAST_RTTI
#ifdef _FAST_RTTI_VIRTUAL
    auto pid = ptr->_gettypeuid();
#else
    auto pid = ptr->_tid;
#endif
    auto cid = T::_typeuid;
    return cid == pid or cid == pid / 256 or cid == pid / (256*256) or cid == pid / (256*256*256);
#else
    return bool(dynamic_cast<T*>(ptr));
#endif
}

///@}

#endif
