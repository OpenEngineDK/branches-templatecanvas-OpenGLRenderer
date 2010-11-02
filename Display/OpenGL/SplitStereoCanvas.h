// OpenGL split screen stereo canvas
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#ifndef _OPENGL_SPLIT_STEREO_CANVAS_H_
#define _OPENGL_SPLIT_STEREO_CANVAS_H_

#include <Display/ICanvas.h>
#include <Display/OpenGL/SplitScreenCanvas.h>
#include <Display/OpenGL/RenderCanvas.h>
#include <Display/IRenderCanvas.h>
#include <Display/StereoCamera.h>
#include <Display/ViewingVolume.h>

namespace OpenEngine {
namespace Display {
namespace OpenGL {

template <class Backend>
class SplitStereoCanvas : public IRenderCanvas {
private:
    IViewingVolume* dummyCam;
    StereoCamera* stereoCam;
    RenderCanvas<Backend> left, right;
    SplitScreenCanvas<Backend> split;
public:
    SplitStereoCanvas()
        : IRenderCanvas()
        , dummyCam(new ViewingVolume())
        , stereoCam(new StereoCamera(*dummyCam))
        , split(SplitScreenCanvas<Backend>(left, right))
    {}

    virtual ~SplitStereoCanvas() {
        delete stereoCam;
        delete dummyCam;
    }

    void Handle(Display::InitializeEventArg arg) {
        ((IListener<Display::InitializeEventArg>&)split).Handle(arg);
    }

    void Handle(Display::ProcessEventArg arg) {
        stereoCam->SignalRendering(arg.approx);
        ((IListener<Display::ProcessEventArg>&)split).Handle(arg);
    }
    
    void Handle(Display::ResizeEventArg arg) {
        ((IListener<Display::ResizeEventArg>&)split).Handle(arg);
    }

    void Handle(Display::DeinitializeEventArg arg) {
        ((IListener<Display::DeinitializeEventArg>&)split)
            .Handle(Display::DeinitializeEventArg(arg));
    }

    unsigned int GetWidth() const {
        return split.GetWidth();
    }
    
    unsigned int GetHeight() const {
        return split.GetHeight();
    }

    void SetWidth(const unsigned int width) {
        split.SetWidth(width);
    }

    void SetHeight(const unsigned int height) {
        split.SetHeight(height);
    }
    
    ITexture2DPtr GetTexture() {
        return split.GetTexture();
    }

    void SetRenderer(IRenderer* renderer) {
        this->renderer = renderer;
        left.SetRenderer(renderer);
        right.SetRenderer(renderer);
    }
    
    void SetViewingVolume(IViewingVolume* vv) {
        this->vv = vv;
        delete stereoCam;
        //stereoCam->SetViewingVolume(*vv);
        stereoCam = new StereoCamera(*vv);
        left.SetViewingVolume(stereoCam->GetLeft());
        right.SetViewingVolume(stereoCam->GetRight());
    }
    
    void SetScene(ISceneNode* scene) {
        this->scene = scene;
        left.SetScene(scene);
        right.SetScene(scene);
    }
};

} // NS OpenGL
} // NS Display
} // NS OpenEngine

#endif // #define _OPENGL_SPLIT_STEREO_CANVAS_H_
