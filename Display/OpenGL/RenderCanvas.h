// Render canvas implementation templated by texture backend
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#ifndef _OE_RENDER_CANVAS_H_
#define _OE_RENDER_CANVAS_H_

#include <Display/IRenderCanvas.h>
#include <Display/OpenGL/TextureCopy.h>
#include <Display/IViewingVolume.h>
#include <Resources/ITexture2D.h>
#include <Renderers/IRenderer.h>

namespace OpenEngine {
namespace Display {
namespace OpenGL {

template <class Backend>
class RenderCanvas : public Display::IRenderCanvas {
private:
    bool init;
    Backend backend;
public:
    RenderCanvas()    
        : Display::IRenderCanvas()
        , init(false) {}

    virtual ~RenderCanvas() {}

    void Handle(Display::InitializeEventArg arg) {
#if OE_SAFE
        if (renderer == NULL) throw new Exception("NULL renderer in RenderCanvas.");
#endif
        if (init) return;
        backend.Init(arg.canvas.GetWidth(), arg.canvas.GetHeight());
        vv->Update(arg.canvas.GetWidth(), arg.canvas.GetHeight());
        ((IListener<Renderers::InitializeEventArg>*)renderer)->Handle(Renderers::InitializeEventArg(*this));
        init = true;
    }
    
    void Handle(Display::ProcessEventArg arg) {
#if OE_SAFE
        if (renderer == NULL) throw new Exception("NULL renderer in RenderCanvas.");
#endif
        backend.Pre();
        ((IListener<Renderers::ProcessEventArg>*)renderer)
            ->Handle(Renderers::ProcessEventArg(*this, arg.start, arg.approx));
        backend.Post();
    }
    
    void Handle(Display::ResizeEventArg arg) {
        backend.SetDimensions(arg.canvas.GetWidth(), arg.canvas.GetHeight());
        vv->Update(arg.canvas.GetWidth(), arg.canvas.GetHeight());        
    }

    void Handle(Display::DeinitializeEventArg arg) {
        if (!init) return;
        ((IListener<Renderers::DeinitializeEventArg>*)renderer)->Handle(Renderers::DeinitializeEventArg(*this));
        backend.Deinit();
        init = false;
    }

    unsigned int GetWidth() const {
        return backend.GetWidth();
    }

    unsigned int GetHeight() const {
        return backend.GetHeight();
    }

    void SetWidth(const unsigned int width) {
        backend.SetDimensions(width, backend.GetHeight());
    }

    void SetHeight(const unsigned int height) {
        backend.SetDimensions(backend.GetWidth(), height);
    }

    ITexture2DPtr GetTexture() {
        return backend.tex;
    }
};

} // NS OpenGL
} // NS Display
} // NS OpenEngine

#endif //_OE_RENDER_CANVAS_H_
