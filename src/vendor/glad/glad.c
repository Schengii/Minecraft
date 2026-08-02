#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>

PFNGLCLEARPROC glClear = NULL;
PFNGLCLEARCOLORPROC glClearColor = NULL;
PFNGLVIEWPORTPROC glViewport = NULL;
PFNGLENABLEPROC glEnable = NULL;
PFNGLDISABLEPROC glDisable = NULL;
PFNGLDEPTHFUNCPROC glDepthFunc = NULL;
PFNGLCULLFACEPROC glCullFace = NULL;
PFNGLBLENDFUNCPROC glBlendFunc = NULL;

PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = NULL;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = NULL;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = NULL;

PFNGLGENBUFFERSPROC glGenBuffers = NULL;
PFNGLBINDBUFFERPROC glBindBuffer = NULL;
PFNGLBUFFERDATAPROC glBufferData = NULL;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = NULL;

PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = NULL;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = NULL;

PFNGLCREATESHADERPROC glCreateShader = NULL;
PFNGLSHADERSOURCEPROC glShaderSource = NULL;
PFNGLCOMPILESHADERPROC glCompileShader = NULL;
PFNGLGETSHADERIVPROC glGetShaderiv = NULL;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = NULL;
PFNGLDELETESHADERPROC glDeleteShader = NULL;

PFNGLCREATEPROGRAMPROC glCreateProgram = NULL;
PFNGLATTACHSHADERPROC glAttachShader = NULL;
PFNGLLINKPROGRAMPROC glLinkProgram = NULL;
PFNGLGETPROGRAMIVPROC glGetProgramiv = NULL;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = NULL;
PFNGLUSEPROGRAMPROC glUseProgram = NULL;
PFNGLDELETEPROGRAMPROC glDeleteProgram = NULL;

PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = NULL;
PFNGLUNIFORM1IPROC glUniform1i = NULL;
PFNGLUNIFORM1FPROC glUniform1f = NULL;
PFNGLUNIFORM3FPROC glUniform3f = NULL;
PFNGLUNIFORM4FPROC glUniform4f = NULL;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = NULL;

PFNGLGENTEXTURESPROC glGenTextures = NULL;
PFNGLBINDTEXTUREPROC glBindTexture = NULL;
PFNGLTEXIMAGE2DPROC glTexImage2D = NULL;
PFNGLTEXPARAMETERIPROC glTexParameteri = NULL;
PFNGLTEXPARAMETERFVPROC glTexParameterfv = NULL;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap = NULL;
PFNGLACTIVETEXTUREPROC glActiveTexture = NULL;
PFNGLDELETETEXTURESPROC glDeleteTextures = NULL;

PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers = NULL;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer = NULL;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D = NULL;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers = NULL;

PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers = NULL;
PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer = NULL;
PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage = NULL;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer = NULL;
PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers = NULL;

PFNGLDRAWBUFFERPROC glDrawBuffer = NULL;
PFNGLREADBUFFERPROC glReadBuffer = NULL;

PFNGLDRAWARRAYSPROC glDrawArrays = NULL;
PFNGLDRAWELEMENTSPROC glDrawElements = NULL;

static void* load(GLADloadproc loadproc, const char *name) {
    void *result = loadproc(name);
    if(result == NULL) {
        // Fallback for Windows
#if defined(_WIN32)
        static HMODULE libGL = NULL;
        if (!libGL) {
            libGL = LoadLibraryA("opengl32.dll");
        }
        if (libGL) {
            result = (void*)GetProcAddress(libGL, name);
        }
#endif
    }
    return result;
}

int gladLoadGLLoader(GLADloadproc loadproc) {
    glClear = (PFNGLCLEARPROC)load(loadproc, "glClear");
    glClearColor = (PFNGLCLEARCOLORPROC)load(loadproc, "glClearColor");
    glViewport = (PFNGLVIEWPORTPROC)load(loadproc, "glViewport");
    glEnable = (PFNGLENABLEPROC)load(loadproc, "glEnable");
    glDisable = (PFNGLDISABLEPROC)load(loadproc, "glDisable");
    glDepthFunc = (PFNGLDEPTHFUNCPROC)load(loadproc, "glDepthFunc");
    glCullFace = (PFNGLCULLFACEPROC)load(loadproc, "glCullFace");
    glBlendFunc = (PFNGLBLENDFUNCPROC)load(loadproc, "glBlendFunc");

    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)load(loadproc, "glGenVertexArrays");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)load(loadproc, "glBindVertexArray");
    glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)load(loadproc, "glDeleteVertexArrays");

    glGenBuffers = (PFNGLGENBUFFERSPROC)load(loadproc, "glGenBuffers");
    glBindBuffer = (PFNGLBINDBUFFERPROC)load(loadproc, "glBindBuffer");
    glBufferData = (PFNGLBUFFERDATAPROC)load(loadproc, "glBufferData");
    glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)load(loadproc, "glDeleteBuffers");

    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)load(loadproc, "glEnableVertexAttribArray");
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)load(loadproc, "glVertexAttribPointer");

    glCreateShader = (PFNGLCREATESHADERPROC)load(loadproc, "glCreateShader");
    glShaderSource = (PFNGLSHADERSOURCEPROC)load(loadproc, "glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC)load(loadproc, "glCompileShader");
    glGetShaderiv = (PFNGLGETSHADERIVPROC)load(loadproc, "glGetShaderiv");
    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)load(loadproc, "glGetShaderInfoLog");
    glDeleteShader = (PFNGLDELETESHADERPROC)load(loadproc, "glDeleteShader");

    glCreateProgram = (PFNGLCREATEPROGRAMPROC)load(loadproc, "glCreateProgram");
    glAttachShader = (PFNGLATTACHSHADERPROC)load(loadproc, "glAttachShader");
    glLinkProgram = (PFNGLLINKPROGRAMPROC)load(loadproc, "glLinkProgram");
    glGetProgramiv = (PFNGLGETPROGRAMIVPROC)load(loadproc, "glGetProgramiv");
    glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)load(loadproc, "glGetProgramInfoLog");
    glUseProgram = (PFNGLUSEPROGRAMPROC)load(loadproc, "glUseProgram");
    glDeleteProgram = (PFNGLDELETEPROGRAMPROC)load(loadproc, "glDeleteProgram");

    glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)load(loadproc, "glGetUniformLocation");
    glUniform1i = (PFNGLUNIFORM1IPROC)load(loadproc, "glUniform1i");
    glUniform1f = (PFNGLUNIFORM1FPROC)load(loadproc, "glUniform1f");
    glUniform3f = (PFNGLUNIFORM3FPROC)load(loadproc, "glUniform3f");
    glUniform4f = (PFNGLUNIFORM4FPROC)load(loadproc, "glUniform4f");
    glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)load(loadproc, "glUniformMatrix4fv");

    glGenTextures = (PFNGLGENTEXTURESPROC)load(loadproc, "glGenTextures");
    glBindTexture = (PFNGLBINDTEXTUREPROC)load(loadproc, "glBindTexture");
    glTexImage2D = (PFNGLTEXIMAGE2DPROC)load(loadproc, "glTexImage2D");
    glTexParameteri = (PFNGLTEXPARAMETERIPROC)load(loadproc, "glTexParameteri");
    glTexParameterfv = (PFNGLTEXPARAMETERFVPROC)load(loadproc, "glTexParameterfv");
    glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)load(loadproc, "glGenerateMipmap");
    glActiveTexture = (PFNGLACTIVETEXTUREPROC)load(loadproc, "glActiveTexture");
    glDeleteTextures = (PFNGLDELETETEXTURESPROC)load(loadproc, "glDeleteTextures");

    glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)load(loadproc, "glGenFramebuffers");
    glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)load(loadproc, "glBindFramebuffer");
    glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)load(loadproc, "glFramebufferTexture2D");
    glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)load(loadproc, "glDeleteFramebuffers");

    glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)load(loadproc, "glGenRenderbuffers");
    glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)load(loadproc, "glBindRenderbuffer");
    glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)load(loadproc, "glRenderbufferStorage");
    glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)load(loadproc, "glFramebufferRenderbuffer");
    glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)load(loadproc, "glDeleteRenderbuffers");

    glDrawBuffer = (PFNGLDRAWBUFFERPROC)load(loadproc, "glDrawBuffer");
    glReadBuffer = (PFNGLREADBUFFERPROC)load(loadproc, "glReadBuffer");

    glDrawArrays = (PFNGLDRAWARRAYSPROC)load(loadproc, "glDrawArrays");
    glDrawElements = (PFNGLDRAWELEMENTSPROC)load(loadproc, "glDrawElements");

    return 1;
}

int gladLoadGL(void) {
    return 1;
}
