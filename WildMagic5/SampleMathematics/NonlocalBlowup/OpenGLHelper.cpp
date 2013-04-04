// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

#include "OpenGLHelper.h"
#include "Wm5Memory.h"

//----------------------------------------------------------------------------
bool OpenGL::IsSuccessful ()
{
    GLenum result = glGetError();
    if (result != GL_NO_ERROR)
    {
        const GLchar* errorString = GetErrorString(result);
        assertion(false, "OpenGL error: %s.\n", errorString);
        while (result != GL_NO_ERROR)
        {
            errorString = GetErrorString(result);
            assertion(false, "OpenGL error: %s\n", errorString);
            result = glGetError();
        }
        return false;
    }
    return true;
}
//----------------------------------------------------------------------------
GLuint OpenGL::CreateVertexBuffer (int numVertices, int stride, GLenum usage,
    const GLvoid* initialData)
{
    return CreateBuffer(GL_ARRAY_BUFFER, numVertices, stride, usage,
        initialData);
}
//----------------------------------------------------------------------------
GLuint OpenGL::CreateIndexBuffer (int numIndices, int stride, GLenum usage,
    const GLvoid* initialData)
{
    return CreateBuffer(GL_ELEMENT_ARRAY_BUFFER, numIndices, stride, usage,
        initialData);
}
//----------------------------------------------------------------------------
void OpenGL::DestroyBuffer (GLuint& buffer)
{
    glDeleteBuffers(1, &buffer);
    buffer = 0;
}
//----------------------------------------------------------------------------
GLuint OpenGL::CreateFrameBuffer (GLuint texture)
{
    GLuint frameBuffer = 0;
    glGenFramebuffersEXT(1, &frameBuffer);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBuffer);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
        GL_TEXTURE_2D, texture, 0);

    switch (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT))
    {
    case GL_FRAMEBUFFER_COMPLETE_EXT:
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        return frameBuffer;
#ifndef __APPLE__
    // TODO.  Update Wm5GlExtensions.h to include the latest stuff.
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENTS_EXT:
#else
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
#endif
        assertion(false, "Incomplete attachments.\n");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
        assertion(false, "Incomplete missing attachments.\n");
        break;
#ifdef WIN32
    // TODO.  Update Wm5GlExtensions.h to include the latest stuff.
    case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
        assertion(false, "Incomplete duplicate attachment.\n");
        break;
#endif
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
        assertion(false, "Incomplete dimensions.\n");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
        assertion(false, "Incomplete formats.\n");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
        assertion(false, "Incomplete draw buffer.\n");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
        assertion(false, "Incomplete read buffer.\n");
        break;
    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
        assertion(false, "Framebuffer unsupported.\n");
        break;
    default:
        break;
    }

    glDeleteFramebuffersEXT(1, &frameBuffer);
    return 0;
}
//----------------------------------------------------------------------------
void OpenGL::DestroyFrameBuffer (GLuint& framebuffer)
{
    glDeleteFramebuffersEXT(1, &framebuffer);
    framebuffer = 0;
}
//----------------------------------------------------------------------------
GLuint OpenGL::CreateVertexShader (const char* text)
{
    return CreateShader(GL_VERTEX_SHADER, text);
}
//----------------------------------------------------------------------------
GLuint OpenGL::CreateFragmentShader (const char* text)
{
    return CreateShader(GL_FRAGMENT_SHADER, text);
}
//----------------------------------------------------------------------------
void OpenGL::DestroyShader (GLuint& shader)
{
    glDeleteShader(shader);
    shader = 0;
}
//----------------------------------------------------------------------------
GLuint OpenGL::CreateProgram (GLuint vertexShader, GLuint fragmentShader)
{
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    // Link the program.
    glLinkProgram(program);
    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
    if (!isLinked)
    {
        std::string linkStatus = "Program link error: ";
        GLint infoLogLength = 0;
        glGetShaderiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0)
        {
            GLchar* infoLog = new1<GLchar>(infoLogLength);
            glGetShaderInfoLog(program, infoLogLength, 0, infoLog);
            linkStatus += std::string(infoLog);
            delete1(infoLog);
        }
        else
        {
            linkStatus += "<no log>";
        }
        glDetachShader(program, vertexShader);
        glDetachShader(program, fragmentShader);
        glDeleteProgram(program);

        assertion(false, "CreateProgram failed: %s\n", linkStatus.c_str());
        return 0;
    }

    return program;
}
//----------------------------------------------------------------------------
void OpenGL::DestroyProgram (GLuint& program)
{
    const GLsizei maxShaders = 16;
    GLuint shaders[maxShaders];
    GLsizei numShaders = 0;
    glGetAttachedShaders(program, maxShaders, &numShaders, shaders);
    for (GLsizei i = 0; i < numShaders; ++i)
    {
        glDetachShader(program, shaders[i]);
    }
    glDeleteProgram(program);
    program = 0;
}
//----------------------------------------------------------------------------
GLuint OpenGL::CreateTexture1D (int width, GLenum format,
    GLint internalFormat, GLenum type, const GLvoid* initialData)
{
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_1D, texture);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage1D(GL_TEXTURE_1D, 0, internalFormat, width, 0, format, type,
        initialData);
    glBindTexture(GL_TEXTURE_1D, 0);
    if (!IsSuccessful())
    {
        glDeleteTextures(1, &texture);
        assertion(false, "CreateTexture1D failed.\n");
        return 0;
    }
    return texture;
}
//----------------------------------------------------------------------------
GLuint OpenGL::CreateTexture2D (int width, int height, GLenum format,
    GLint internalFormat, GLenum type, const GLvoid* initialData)
{
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0,
        format, type, initialData);
    glBindTexture(GL_TEXTURE_2D, 0);
    if (!IsSuccessful())
    {
        glDeleteTextures(1, &texture);
        assertion(false, "CreateTexture2D failed.\n");
        return 0;
    }
    return texture;
}
//----------------------------------------------------------------------------
GLuint OpenGL::CreateTexture3D (int width, int height, int depth,
    GLenum format, GLint internalFormat, GLenum type,
    const GLvoid* initialData)
{
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_3D, texture);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage3D(GL_TEXTURE_3D, 0, internalFormat, width, height, depth, 0,
        format, type, initialData);
    glBindTexture(GL_TEXTURE_3D, 0);
    if (!IsSuccessful())
    {
        glDeleteTextures(1, &texture);
        assertion(false, "CreateTexture3D failed.\n");
        return 0;
    }
    return texture;
}
//----------------------------------------------------------------------------
GLuint OpenGL::CreateTextureRectangle (int width, int height, GLenum format,
    GLint internalFormat, GLenum type, const GLvoid* initialData)
{
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texture);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER,
        GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER,
        GL_NEAREST);
    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, internalFormat, width, height,
        0, format, type, initialData);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
    if (!IsSuccessful())
    {
        glDeleteTextures(1, &texture);
        assertion(false, "CreateTextureRectangle failed.\n");
        return 0;
    }
    return texture;
}
//----------------------------------------------------------------------------
void OpenGL::DestroyTexture (GLuint& texture)
{
    glDeleteTextures(1, &texture);
    texture = 0;
}
//----------------------------------------------------------------------------
GLuint OpenGL::CreateBuffer (GLenum type, int numElements, int stride,
    GLenum usage, const GLvoid* initialData)
{
    GLuint buffer = 0;
    glGenBuffers(1, &buffer);
    glBindBuffer(type, buffer);
    glBufferData(type, numElements*stride, initialData, usage);
    glBindBuffer(type, 0);
    if (!IsSuccessful())
    {
        glDeleteBuffers(1, &buffer);
        assertion(false, "CreateBuffer failed.\n");
        return 0;
    }
    return buffer;
}
//----------------------------------------------------------------------------
GLuint OpenGL::CreateShader (GLenum type, const char* text)
{
    // Create the shader.
    GLuint shader = glCreateShader(type);
    GLint textLength = (GLint)strlen(text);
    glShaderSource(shader, 1, &text, &textLength);

    // Compile the shader.
    glCompileShader(shader);
    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        std::string compileStatus = "Shader compiler error: ";
        GLint infoLogLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0)
        {
            GLchar* infoLog = new1<GLchar>(infoLogLength);
            glGetShaderInfoLog(shader, infoLogLength, 0, infoLog);
            compileStatus += std::string(infoLog);
            delete1(infoLog);
        }
        else
        {
            compileStatus += "<no log>";
        }
        glDeleteShader(shader);

        assertion(false, "CreateShader failed: %s\n", compileStatus.c_str());
        return 0;
    }

    return shader;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Support for error checking.
//----------------------------------------------------------------------------
const GLchar* OpenGL::msErrorString[7] =
{
    "GL_INVALID_ENUM",
    "GL_INVALID_VALUE",
    "GL_INVALID_OPERATION",
    "GL_STACK_OVERFLOW",
    "GL_STACK_UNDERFLOW",
    "GL_OUT_OF_MEMORY",
    "GL_UNKNOWN_ERROR_CODE"
};
//----------------------------------------------------------------------------
const GLchar* OpenGL::GetErrorString (GLenum code)
{
    int index = (int)code - (int)GL_INVALID_ENUM;
    if (0 <= index && index < 6)
    {
        return msErrorString[index];
    }
    return msErrorString[6];
}
//----------------------------------------------------------------------------
