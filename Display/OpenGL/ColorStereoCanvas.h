// OpenGL color stereo canvas
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#ifndef _OPENGL_COLOR_STEREO_CANVAS_H_
#define _OPENGL_COLOR_STEREO_CANVAS_H_

#include <Display/ICanvas.h>
#include <Display/OpenGL/RenderCanvas.h>

namespace OpenEngine {
namespace Display {
namespace OpenGL {

template <class Backend>
class ColorStereoCanvas : public IRenderCanvas {
private:
    Backend backend;
    IViewingVolume* dummyCam;
    StereoCamera* stereoCam;
    RenderCanvas<Backend> left, right;
    bool init;
public:
    ColorStereoCanvas() 
        : IRenderCanvas()
        , dummyCam(new ViewingVolume())
        , stereoCam(new StereoCamera(*dummyCam))
        , init(false)
    {
        ITextureResourcePtr ltex = left.GetTexture();
        ITextureResourcePtr rtex = right.GetTexture();
        ltex->SetColorFormat(Resources::LUMINANCE);
        //rtex->SetColorFormat(Resources::LUMINANCE);
    }

    virtual ~ColorStereoCanvas() {
        delete stereoCam;
        delete dummyCam;
    }

    void Handle(Display::InitializeEventArg arg) {
        if (init) return;
        backend.Init(arg.canvas.GetWidth(), arg.canvas.GetHeight());
        ((IListener<Display::InitializeEventArg>&)left).Handle(arg);
        ((IListener<Display::InitializeEventArg>&)right).Handle(arg);
        init = true;
    }

    void Handle(Display::ProcessEventArg arg) {
        stereoCam->SignalRendering(arg.approx);
        ((IListener<Display::ProcessEventArg>&)left).Handle(arg);
        ((IListener<Display::ProcessEventArg>&)right).Handle(arg);

        backend.Pre();
        
        unsigned int width = GetWidth();
        unsigned int height = GetHeight();

        Vector<4,int> d(0, 0, width, height);
        glViewport((GLsizei)d[0], (GLsizei)d[1], (GLsizei)d[2], (GLsizei)d[3]);
        OrthogonalViewingVolume volume(-1, 1, 0, width, 0, height);

        // Select The Projection Matrix
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        CHECK_FOR_GL_ERROR();
    
        // Reset The Projection Matrix
        glLoadIdentity();
        CHECK_FOR_GL_ERROR();
    
        // Setup OpenGL with the volumes projection matrix
        Matrix<4,4,float> projMatrix = volume.GetProjectionMatrix();
        float arr[16] = {0};
        projMatrix.ToArray(arr);
        glMultMatrixf(arr);
        CHECK_FOR_GL_ERROR();
    
        // Select the modelview matrix
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        CHECK_FOR_GL_ERROR();
    
        // Reset the modelview matrix
        glLoadIdentity();
        CHECK_FOR_GL_ERROR();
        
        // Get the view matrix and apply it
        Matrix<4,4,float> matrix = volume.GetViewMatrix();
        float f[16] = {0};
        matrix.ToArray(f);
        glMultMatrixf(f);
        CHECK_FOR_GL_ERROR();
        
        bool depth = glIsEnabled(GL_DEPTH_TEST);
        GLboolean lighting = glIsEnabled(GL_LIGHTING);
        GLboolean blending = glIsEnabled(GL_BLEND);
        GLboolean texture = glIsEnabled(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glDisable(GL_BLEND);
        glEnable(GL_TEXTURE_2D);
        GLint texenv;
        glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &texenv);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const float z = 0.0;
        glColorMask (GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE);

        // glEnable(GL_BLEND);
        // glBlendColor(1.0,0.0,0.0,1.0);
        // glBlendFunc(GL_ZERO, GL_CONSTANT_COLOR);

        glBindTexture(GL_TEXTURE_2D, left.GetTexture()->GetID());
        CHECK_FOR_GL_ERROR();
        glBegin(GL_QUADS);
        // glColor4f(1.0,.0,0.0,.5);
        glColor4f(1.0,1.0,1.0,1.0);
        glTexCoord2f(0.0, 0.0);
        glVertex3f(0, height, z);
        glTexCoord2f(0.0, 1.0);
        glVertex3f(0.0, 0.0, z);
        glTexCoord2f(1.0, 1.0);
        glVertex3f(width, 0.0, z);
        glTexCoord2f(1.0, 0.0);
        glVertex3f(width, height, z);
        glEnd();
 

        // glEnable(GL_BLEND);
        // glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
        // glBlendEquation(GL_FUNC_ADD);
        glColorMask (GL_FALSE, GL_TRUE, GL_TRUE, GL_FALSE);
        glBindTexture(GL_TEXTURE_2D, right.GetTexture()->GetID());
        CHECK_FOR_GL_ERROR();
        glBegin(GL_QUADS);
        //glColor4f(0.0,1.0,1.0,1.0);
        glTexCoord2f(0.0, 0.0);
        glVertex3f(0, height, z);
        glTexCoord2f(0.0, 1.0);
        glVertex3f(0.0, 0.0, z);
        glTexCoord2f(1.0, 1.0);
        glVertex3f(width, 0.0, z);
        glTexCoord2f(1.0, 0.0);
        glVertex3f(width, height, z);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, 0);
        glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        CHECK_FOR_GL_ERROR();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        CHECK_FOR_GL_ERROR();
        
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, texenv);
        if (depth)    glEnable(GL_DEPTH_TEST);
        if (lighting) glEnable(GL_LIGHTING);
        if (blending) glEnable(GL_BLEND);
        if (!texture) glDisable(GL_TEXTURE_2D);

        backend.Post();
    }
    
    void Handle(Display::ResizeEventArg arg) {
        backend.SetDimensions(arg.canvas.GetWidth(), arg.canvas.GetHeight());
        ((IListener<Display::ResizeEventArg>&)left).Handle(ResizeEventArg(*this));
        ((IListener<Display::ResizeEventArg>&)right).Handle(ResizeEventArg(*this));
    }
    
    void Handle(Display::DeinitializeEventArg arg) {
        if (!init) return;
        ((IListener<Display::DeinitializeEventArg>&)left)
            .Handle(Display::DeinitializeEventArg(arg));
        ((IListener<Display::DeinitializeEventArg>&)right)
            .Handle(Display::DeinitializeEventArg(arg));
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
        left.SetWidth(width);
        right.SetWidth(width);
    }

    void SetHeight(const unsigned int height) {
        backend.SetDimensions(backend.GetWidth(), height);
        left.SetHeight(height);
        right.SetHeight(height);
    }

    ITexture2DPtr GetTexture() {
        return backend.tex;
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
