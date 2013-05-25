#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "node.h"
#include "image.h"

///@file igl/texture.h Textures. @ingroup igl
///@defgroup texture Textures
///@ingroup igl
///@{

/// Color Texture from a file
struct Texture : Node {
    REGISTER_FAST_RTTI(Node,Texture,15)
    
    string  filename; ///< texture filename
    
	image3f image; ///< texture image
    bool flipy = true; ///< whether to flip the images on load
    
    unsigned int _shade_glid = 0; ///< opengl shading texture id
};


///@}

#endif
