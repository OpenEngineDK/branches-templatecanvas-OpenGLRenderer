// OpenGL texture copy backend for canvases
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#ifndef _OPENGL_TEXTURE_COPY_BACKEND_H_
#define _OPENGL_TEXTURE_COPY_BACKEND_H_

#include <Resources/ITexture2D.h>

namespace OpenEngine {
namespace Display {
namespace OpenGL {

class TextureCopy {
private:
    class CustomTexture : public Resources::ITexture2D {
        friend class TextureCopy;
    private:
    public:
        unsigned int GetChannelSize() { return 8; };
        ITexture2D* Clone() { return NULL; }
        void Load() {}
        void Unload() {}
        void Reverse() {}
        void ReverseVertecally() {}
        void ReverseHorizontally() {}
    private:
    };
    CustomTexture* ctex;
public:
    TextureCopy();
    virtual ~TextureCopy();
    Resources::ITexture2DPtr tex;
    void Init(const unsigned int width, const unsigned int height);
    void Deinit();
    void Pre();
    void Post();
    void SetDimensions(const unsigned int width, const unsigned int height);
    unsigned int GetWidth() const;
    unsigned int GetHeight() const;
};

} // NS OpenGL
} // NS Display
} // NS OpenEngine

#endif // _OPENGL_TEXTURE_COPY_BACKEND_H_
