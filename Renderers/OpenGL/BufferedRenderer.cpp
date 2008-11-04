#include <Renderers/OpenGL/BufferedRenderer.h>

#include <string>

#include <Logging/Logger.h>

namespace OpenEngine {
namespace Renderers {
namespace OpenGL {

BufferedRenderer::BufferedRenderer() {
    fbo = depthbuffer = img = 0;
    width = 800;
    height = 600;
}

BufferedRenderer::~BufferedRenderer() {}

void BufferedRenderer::Handle(InitializeEventArg arg) {
    CHECK_FOR_GL_ERROR();

    Renderer::Handle(arg);
    CHECK_FOR_GL_ERROR();

    //@todo: make sure that there is a gl context

    // maybe there should be setup a texture env:
    //glTexParameteriv(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR) ;

    const std::string fboExt = "GL_EXT_framebuffer_object";
    if (glewGetExtension(fboExt.c_str()) != GL_TRUE )
        throw Exception(fboExt + " not supported");

    glGenFramebuffersEXT(1, &fbo);
    CHECK_FOR_GL_ERROR();

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
    CHECK_FOR_GL_ERROR();

    // Adding a depth buffer
    glGenRenderbuffersEXT(1, &depthbuffer);
    CHECK_FOR_GL_ERROR();
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthbuffer);
    CHECK_FOR_GL_ERROR();
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,
                             GL_DEPTH_COMPONENT, 
                             width, height);
    CHECK_FOR_GL_ERROR();
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,
                                 GL_DEPTH_ATTACHMENT_EXT,
                                 GL_RENDERBUFFER_EXT, depthbuffer);
    CHECK_FOR_GL_ERROR();

    // Adding a Texture
    glGenTextures(1, &img);
    CHECK_FOR_GL_ERROR();
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, img);
    CHECK_FOR_GL_ERROR();
    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA,
                 width, height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, NULL);
    CHECK_FOR_GL_ERROR();
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, 
                              GL_COLOR_ATTACHMENT0_EXT, 
                              GL_TEXTURE_RECTANGLE_ARB, img, 0);
    CHECK_FOR_GL_ERROR();

    // check FBO state for errors
#if OE_DEBUG_GL
    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
        throw Exception("Incomplete frame buffer object: " + EnumToString(status) );
#endif

    // done initializing, go back to the main gl context 
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); //unbind
    CHECK_FOR_GL_ERROR();

    glEnable(GL_TEXTURE_RECTANGLE_ARB);
}

std::string BufferedRenderer::EnumToString(GLenum status) {
    std::string error = "";
    switch(status) {
    case GL_FRAMEBUFFER_COMPLETE_EXT:
        break;
    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
        error = "GL_FRAMEBUFFER_UNSUPPORTED_EXT\n";
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
        error = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT\n";
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
        error = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT\n";
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: 
        error = "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT\n";
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
        error = "GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT\n";
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
        error = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT\n";
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
        error = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT\n";
        break;
    default: break;
    }
    return error;
}


void BufferedRenderer::Handle(ProcessEventArg arg) {
    CHECK_FOR_GL_ERROR();

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo); //bind
    CHECK_FOR_GL_ERROR();

    glPushAttrib(GL_VIEWPORT_BIT);
    CHECK_FOR_GL_ERROR();

    glViewport(0,0,width, height);
    CHECK_FOR_GL_ERROR();

    //glClearColor(1,0,1,1);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    CHECK_FOR_GL_ERROR();

    Renderer::Handle(arg);

    //test: glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);


    CHECK_FOR_GL_ERROR();
    glPopAttrib();

    CHECK_FOR_GL_ERROR();
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); //unbind
    CHECK_FOR_GL_ERROR();

    // for debugging
    RenderTextureInOrtho();
    CHECK_FOR_GL_ERROR();
}

void BufferedRenderer::RenderTextureInOrtho() {

    GLboolean t = glIsEnabled(GL_TEXTURE_2D);
    /*
    GLboolean l = glIsEnabled(GL_LIGHTING);
    GLboolean d = glIsEnabled(GL_DEPTH_TEST);
    */
    // render a screen sized quad
    //glDisable(GL_DEPTH_TEST);
    CHECK_FOR_GL_ERROR();
    //glDisable(GL_LIGHTING);
    CHECK_FOR_GL_ERROR();
    glEnable(GL_TEXTURE_2D);
    CHECK_FOR_GL_ERROR();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    CHECK_FOR_GL_ERROR();

    // quad rendering of texture
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, img);
    CHECK_FOR_GL_ERROR();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    CHECK_FOR_GL_ERROR();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    CHECK_FOR_GL_ERROR();

    glViewport(0, 0, width, height);
    CHECK_FOR_GL_ERROR();

    glBegin(GL_QUADS);

    glTexCoord2f(0.0, 0.0);
    glVertex3f(-1.0, -1.0, 0.5);

    glTexCoord2f(1.0, 0.0);
    glVertex3f(1.0, -1.0, 0.5);

    glTexCoord2f(1.0, 1.0);
    glVertex3f(1.0, 1.0, 0.5);

    glTexCoord2f(0.0, 1.0);
    glVertex3f(-1.0, 1.0, 0.5);

    glEnd();
    CHECK_FOR_GL_ERROR();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
    CHECK_FOR_GL_ERROR();

    if (!t) glDisable(GL_TEXTURE_2D);
    /*
    if (l) glEnable(GL_LIGHTING);
    if (d) glEnable(GL_DEPTH_TEST);
    CHECK_FOR_GL_ERROR();*/
}

void BufferedRenderer::Handle(DeinitializeEventArg arg) {
    glDeleteFramebuffersEXT(1, &fbo);
    CHECK_FOR_GL_ERROR();
    glDeleteRenderbuffersEXT(1, &depthbuffer);
    CHECK_FOR_GL_ERROR();
    Renderer::Handle(arg);
    CHECK_FOR_GL_ERROR();
}

} // NS OpenGL
} // NS OpenEngine
} // NS Renderers
