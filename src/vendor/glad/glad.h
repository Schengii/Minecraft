#ifndef __glad_h_
#define __glad_h_

#ifdef __gl_h_
#error OpenGL header already included, remove this include, glad already provides it
#endif

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#ifndef NOMINMAX
#define NOMINMAX 1
#endif
#include <windows.h>
#endif

#include <KHR/khrplatform.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GLAD_API_PTR
#define GLAD_API_PTR
#endif
#ifndef GLAD_API_PTRPREFIX
#define GLAD_API_PTRPREFIX
#endif

#ifndef GLAD_GLAPI
#define GLAD_GLAPI extern
#endif

typedef void* (*GLADloadproc)(const char *name);

GLAD_GLAPI int gladLoadGLLoader(GLADloadproc);
GLAD_GLAPI int gladLoadGL(void);

typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef void GLvoid;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef char GLchar;
typedef char GLcharARB;
typedef khronos_intptr_t GLintptr;
typedef khronos_sizei_t GLsizeiptr;

#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_FALSE 0
#define GL_TRUE 1
#define GL_TRIANGLES 0x0004
#define GL_CULL_FACE 0x0B44
#define GL_DEPTH_TEST 0x0B71
#define GL_BLEND 0x0BE2
#define GL_SRC_ALPHA 0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#define GL_BACK 0x0405
#define GL_FRONT 0x0404
#define GL_CCW 0x0901
#define GL_CW 0x0900

#define GL_BYTE 0x1400
#define GL_UNSIGNED_BYTE 0x1401
#define GL_SHORT 0x1402
#define GL_UNSIGNED_SHORT 0x1403
#define GL_INT 0x1404
#define GL_UNSIGNED_INT 0x1405
#define GL_FLOAT 0x1406

#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_STATIC_DRAW 0x88E4
#define GL_DYNAMIC_DRAW 0x88E8

#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_INFO_LOG_LENGTH 0x8B84

#define GL_TEXTURE_2D 0x0DE1
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_NEAREST 0x2600
#define GL_LINEAR 0x2601
#define GL_NEAREST_MIPMAP_NEAREST 0x2700
#define GL_LINEAR_MIPMAP_LINEAR 0x2703
#define GL_REPEAT 0x2901
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_CLAMP_TO_BORDER 0x812D
#define GL_TEXTURE_BORDER_COLOR 0x1004
#define GL_RGB 0x1907
#define GL_RGBA 0x1908
#define GL_RGB16F 0x881B
#define GL_DEPTH_COMPONENT 0x1902
#define GL_NONE 0
#define GL_TEXTURE0 0x84C0

#define GL_FRAMEBUFFER 0x8D40
#define GL_RENDERBUFFER 0x8D41
#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_DEPTH_ATTACHMENT 0x8D00
#define GL_DEPTH_STENCIL_ATTACHMENT 0x821A
#define GL_DEPTH24_STENCIL8 0x88F0

typedef void (GLAD_API_PTR *PFNGLCLEARPROC)(GLbitfield mask);
typedef void (GLAD_API_PTR *PFNGLCLEARCOLORPROC)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void (GLAD_API_PTR *PFNGLVIEWPORTPROC)(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (GLAD_API_PTR *PFNGLENABLEPROC)(GLenum cap);
typedef void (GLAD_API_PTR *PFNGLDISABLEPROC)(GLenum cap);
typedef void (GLAD_API_PTR *PFNGLDEPTHFUNCPROC)(GLenum func);
typedef void (GLAD_API_PTR *PFNGLCULLFACEPROC)(GLenum mode);
typedef void (GLAD_API_PTR *PFNGLBLENDFUNCPROC)(GLenum sfactor, GLenum dfactor);

typedef void (GLAD_API_PTR *PFNGLGENVERTEXARRAYSPROC)(GLsizei n, GLuint *arrays);
typedef void (GLAD_API_PTR *PFNGLBINDVERTEXARRAYPROC)(GLuint array);
typedef void (GLAD_API_PTR *PFNGLDELETEVERTEXARRAYSPROC)(GLsizei n, const GLuint *arrays);

typedef void (GLAD_API_PTR *PFNGLGENBUFFERSPROC)(GLsizei n, GLuint *buffers);
typedef void (GLAD_API_PTR *PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
typedef void (GLAD_API_PTR *PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void (GLAD_API_PTR *PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint *buffers);

typedef void (GLAD_API_PTR *PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef void (GLAD_API_PTR *PFNGLVERTEXATTRIBPOINTERPROC)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);

typedef GLuint (GLAD_API_PTR *PFNGLCREATESHADERPROC)(GLenum type);
typedef void (GLAD_API_PTR *PFNGLSHADERSOURCEPROC)(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
typedef void (GLAD_API_PTR *PFNGLCOMPILESHADERPROC)(GLuint shader);
typedef void (GLAD_API_PTR *PFNGLGETSHADERIVPROC)(GLuint shader, GLenum pname, GLint *params);
typedef void (GLAD_API_PTR *PFNGLGETSHADERINFOLOGPROC)(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (GLAD_API_PTR *PFNGLDELETESHADERPROC)(GLuint shader);

typedef GLuint (GLAD_API_PTR *PFNGLCREATEPROGRAMPROC)(void);
typedef void (GLAD_API_PTR *PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
typedef void (GLAD_API_PTR *PFNGLLINKPROGRAMPROC)(GLuint program);
typedef void (GLAD_API_PTR *PFNGLGETPROGRAMIVPROC)(GLuint program, GLenum pname, GLint *params);
typedef void (GLAD_API_PTR *PFNGLGETPROGRAMINFOLOGPROC)(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (GLAD_API_PTR *PFNGLUSEPROGRAMPROC)(GLuint program);
typedef void (GLAD_API_PTR *PFNGLDELETEPROGRAMPROC)(GLuint program);

typedef GLint (GLAD_API_PTR *PFNGLGETUNIFORMLOCATIONPROC)(GLuint program, const GLchar *name);
typedef void (GLAD_API_PTR *PFNGLUNIFORM1IPROC)(GLint location, GLint v0);
typedef void (GLAD_API_PTR *PFNGLUNIFORM1FPROC)(GLint location, GLfloat v0);
typedef void (GLAD_API_PTR *PFNGLUNIFORM3FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (GLAD_API_PTR *PFNGLUNIFORM4FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void (GLAD_API_PTR *PFNGLUNIFORMMATRIX4FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

typedef void (GLAD_API_PTR *PFNGLGENTEXTURESPROC)(GLsizei n, GLuint *textures);
typedef void (GLAD_API_PTR *PFNGLBINDTEXTUREPROC)(GLenum target, GLuint texture);
typedef void (GLAD_API_PTR *PFNGLTEXIMAGE2DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
typedef void (GLAD_API_PTR *PFNGLTEXPARAMETERIPROC)(GLenum target, GLenum pname, GLint param);
typedef void (GLAD_API_PTR *PFNGLTEXPARAMETERFVPROC)(GLenum target, GLenum pname, const GLfloat *params);
typedef void (GLAD_API_PTR *PFNGLGENERATEMIPMAPPROC)(GLenum target);
typedef void (GLAD_API_PTR *PFNGLACTIVETEXTUREPROC)(GLenum texture);
typedef void (GLAD_API_PTR *PFNGLDELETETEXTURESPROC)(GLsizei n, const GLuint *textures);

typedef void (GLAD_API_PTR *PFNGLGENFRAMEBUFFERSPROC)(GLsizei n, GLuint *framebuffers);
typedef void (GLAD_API_PTR *PFNGLBINDFRAMEBUFFERPROC)(GLenum target, GLuint framebuffer);
typedef void (GLAD_API_PTR *PFNGLFRAMEBUFFERTEXTURE2DPROC)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void (GLAD_API_PTR *PFNGLDELETEFRAMEBUFFERSPROC)(GLsizei n, const GLuint *framebuffers);

typedef void (GLAD_API_PTR *PFNGLGENRENDERBUFFERSPROC)(GLsizei n, GLuint *renderbuffers);
typedef void (GLAD_API_PTR *PFNGLBINDRENDERBUFFERPROC)(GLenum target, GLuint renderbuffer);
typedef void (GLAD_API_PTR *PFNGLRENDERBUFFERSTORAGEPROC)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (GLAD_API_PTR *PFNGLFRAMEBUFFERRENDERBUFFERPROC)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
typedef void (GLAD_API_PTR *PFNGLDELETERENDERBUFFERSPROC)(GLsizei n, const GLuint *renderbuffers);

typedef void (GLAD_API_PTR *PFNGLDRAWBUFFERPROC)(GLenum buf);
typedef void (GLAD_API_PTR *PFNGLREADBUFFERPROC)(GLenum mode);

typedef void (GLAD_API_PTR *PFNGLDRAWARRAYSPROC)(GLenum mode, GLint first, GLsizei count);
typedef void (GLAD_API_PTR *PFNGLDRAWELEMENTSPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices);

GLAD_GLAPI PFNGLCLEARPROC glClear;
GLAD_GLAPI PFNGLCLEARCOLORPROC glClearColor;
GLAD_GLAPI PFNGLVIEWPORTPROC glViewport;
GLAD_GLAPI PFNGLENABLEPROC glEnable;
GLAD_GLAPI PFNGLDISABLEPROC glDisable;
GLAD_GLAPI PFNGLDEPTHFUNCPROC glDepthFunc;
GLAD_GLAPI PFNGLCULLFACEPROC glCullFace;
GLAD_GLAPI PFNGLBLENDFUNCPROC glBlendFunc;

GLAD_GLAPI PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
GLAD_GLAPI PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
GLAD_GLAPI PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;

GLAD_GLAPI PFNGLGENBUFFERSPROC glGenBuffers;
GLAD_GLAPI PFNGLBINDBUFFERPROC glBindBuffer;
GLAD_GLAPI PFNGLBUFFERDATAPROC glBufferData;
GLAD_GLAPI PFNGLDELETEBUFFERSPROC glDeleteBuffers;

GLAD_GLAPI PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
GLAD_GLAPI PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;

GLAD_GLAPI PFNGLCREATESHADERPROC glCreateShader;
GLAD_GLAPI PFNGLSHADERSOURCEPROC glShaderSource;
GLAD_GLAPI PFNGLCOMPILESHADERPROC glCompileShader;
GLAD_GLAPI PFNGLGETSHADERIVPROC glGetShaderiv;
GLAD_GLAPI PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
GLAD_GLAPI PFNGLDELETESHADERPROC glDeleteShader;

GLAD_GLAPI PFNGLCREATEPROGRAMPROC glCreateProgram;
GLAD_GLAPI PFNGLATTACHSHADERPROC glAttachShader;
GLAD_GLAPI PFNGLLINKPROGRAMPROC glLinkProgram;
GLAD_GLAPI PFNGLGETPROGRAMIVPROC glGetProgramiv;
GLAD_GLAPI PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
GLAD_GLAPI PFNGLUSEPROGRAMPROC glUseProgram;
GLAD_GLAPI PFNGLDELETEPROGRAMPROC glDeleteProgram;

GLAD_GLAPI PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
GLAD_GLAPI PFNGLUNIFORM1IPROC glUniform1i;
GLAD_GLAPI PFNGLUNIFORM1FPROC glUniform1f;
GLAD_GLAPI PFNGLUNIFORM3FPROC glUniform3f;
GLAD_GLAPI PFNGLUNIFORM4FPROC glUniform4f;
GLAD_GLAPI PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;

GLAD_GLAPI PFNGLGENTEXTURESPROC glGenTextures;
GLAD_GLAPI PFNGLBINDTEXTUREPROC glBindTexture;
GLAD_GLAPI PFNGLTEXIMAGE2DPROC glTexImage2D;
GLAD_GLAPI PFNGLTEXPARAMETERIPROC glTexParameteri;
GLAD_GLAPI PFNGLTEXPARAMETERFVPROC glTexParameterfv;
GLAD_GLAPI PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
GLAD_GLAPI PFNGLACTIVETEXTUREPROC glActiveTexture;
GLAD_GLAPI PFNGLDELETETEXTURESPROC glDeleteTextures;

GLAD_GLAPI PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
GLAD_GLAPI PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
GLAD_GLAPI PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
GLAD_GLAPI PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;

GLAD_GLAPI PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers;
GLAD_GLAPI PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer;
GLAD_GLAPI PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;
GLAD_GLAPI PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;
GLAD_GLAPI PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers;

GLAD_GLAPI PFNGLDRAWBUFFERPROC glDrawBuffer;
GLAD_GLAPI PFNGLREADBUFFERPROC glReadBuffer;

GLAD_GLAPI PFNGLDRAWARRAYSPROC glDrawArrays;
GLAD_GLAPI PFNGLDRAWELEMENTSPROC glDrawElements;

#ifdef __cplusplus
}
#endif

#endif
