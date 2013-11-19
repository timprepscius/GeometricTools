// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

#ifndef OPENGLHELPER_H
#define OPENGLHELPER_H

#ifdef WIN32
#include "Wm5GlExtensions.h"
#endif

#ifdef __APPLE__
#include <AGL/agl.h>
#endif

#ifdef __LINUX__
#include "Wm5GlxExtensions.h"
#endif

class OpenGL
{
public:
    // Support for error reporting.
    static bool IsSuccessful ();

    // Buffer creation and destruction.
    static GLuint CreateVertexBuffer (int numVertices, int stride,
        GLenum usage, const GLvoid* initialData);
    static GLuint CreateIndexBuffer (int numIndices, int stride, GLenum usage,
        const GLvoid* initialData);
    static void DestroyBuffer (GLuint& buffer);

    // Framebuffer creation and destruction.  The texture is attached to the
    // framebuffer.  The framebuffer does not have a depth-stencil attachment.
    static GLuint CreateFrameBuffer (GLuint texture);
    static void DestroyFrameBuffer (GLuint& framebuffer);

    // Shader and program creation and destruction.  A positive handle is
    // returned when the creation is successful; otherwise, zero is returned.
    static GLuint CreateVertexShader (const char* text);
    static GLuint CreateFragmentShader (const char* text);
    static void DestroyShader (GLuint& shader);
    static GLuint CreateProgram (GLuint vertexShader, GLuint fragmentShader);
    static void DestroyProgram (GLuint& program);

    // Texture creation and destruction.
    static GLuint CreateTexture1D (int width, GLenum format,
        GLint internalFormat, GLenum type, const GLvoid* initialData);
    static GLuint CreateTexture2D (int width, int height, GLenum format,
        GLint internalFormat, GLenum type, const GLvoid* initialData);
    static GLuint CreateTexture3D (int width, int height, int depth,
        GLenum format, GLint internalFormat, GLenum type,
        const GLvoid* initialData);
    static GLuint CreateTextureRectangle (int width, int height,
        GLenum format, GLint internalFormat, GLenum type,
        const GLvoid* initialData);
    static void DestroyTexture (GLuint& texture);

private:
    // Code common to vertex and index buffer creation and destruction.
    static GLuint CreateBuffer (GLenum type, int numElements, int stride,
        GLenum usage, const GLvoid* initialData);

    // Code common to vertex and fragment shader creation and destruction.
    static GLuint CreateShader (GLenum type, const char* text);

    // Support for errors.
    static const GLchar* GetErrorString (GLenum code);
    static const GLchar* msErrorString[7];
};

#endif
