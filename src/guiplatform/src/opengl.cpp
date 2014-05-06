#include "stdafx.h"
#include "opengl.h"

#pragma comment(lib, "opengl32.lib")


PFNGLCREATESHADERPROC glCreateShader = 0;
PFNGLDELETESHADERPROC glDeleteShader = 0;
PFNGLSHADERSOURCEPROC glShaderSource = 0;
PFNGLCOMPILESHADERPROC glCompileShader = 0;
PFNGLCREATEPROGRAMPROC glCreateProgram = 0;
PFNGLDELETEPROGRAMPROC glDeleteProgram = 0;
PFNGLLINKPROGRAMPROC	glLinkProgram = 0;
PFNGLUSEPROGRAMPROC		glUseProgram = 0;
PFNGLATTACHSHADERPROC glAttachShader = 0;
PFNGLDETACHSHADERPROC glDetachShader = 0;
PFNGLGETSHADERIVPROC glGetShaderiv = 0;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = 0;
PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib = 0;
PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform = 0;
PFNGLGETPROGRAMIVPROC glGetProgramiv = 0;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = 0;
PFNWGLGETEXTENSIONSSTRINGEXTPROC wglGetExtensionsStringEXT = 0;
PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = 0;

PFNGLGENQUERIESPROC glGenQueries = 0;
PFNGLDELETEQUERIESPROC glDeleteQueries = 0;
PFNGLISQUERYPROC glIsQuery = 0;
PFNGLBEGINQUERYPROC glBeginQuery = 0;
PFNGLENDQUERYPROC glEndQuery = 0;
PFNGLGETQUERYIVPROC glGetQueryiv = 0;
PFNGLGETQUERYOBJECTIVPROC glGetQueryObjectiv = 0;
PFNGLGETQUERYOBJECTUIVPROC glGetQueryObjectuiv = 0;
PFNGLBINDBUFFERPROC glBindBuffer = 0;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = 0;
PFNGLGENBUFFERSPROC glGenBuffers = 0;
PFNGLISBUFFERPROC glIsBuffer = 0;
PFNGLBUFFERDATAPROC glBufferData = 0;
PFNGLBUFFERSUBDATAPROC glBufferSubData = 0;
PFNGLGETBUFFERSUBDATAPROC glGetBufferSubData = 0;
PFNGLMAPBUFFERPROC glMapBuffer = 0;
PFNGLUNMAPBUFFERPROC glUnmapBuffer = 0;
PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv = 0;
PFNGLGETBUFFERPOINTERVPROC glGetBufferPointerv = 0;


PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate = 0;
PFNGLDRAWBUFFERSPROC glDrawBuffers = 0;
PFNGLSTENCILOPSEPARATEPROC glStencilOpSeparate = 0;
PFNGLSTENCILFUNCSEPARATEPROC glStencilFuncSeparate = 0;
PFNGLSTENCILMASKSEPARATEPROC glStencilMaskSeparate = 0;
PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation = 0;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = 0;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = 0;
PFNGLGETATTACHEDSHADERSPROC glGetAttachedShaders = 0;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation = 0;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = 0;
PFNGLGETSHADERSOURCEPROC glGetShaderSource = 0;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = 0;
PFNGLGETUNIFORMFVPROC glGetUniformfv = 0;
PFNGLGETUNIFORMIVPROC glGetUniformiv = 0;
PFNGLGETVERTEXATTRIBDVPROC glGetVertexAttribdv = 0;
PFNGLGETVERTEXATTRIBFVPROC glGetVertexAttribfv = 0;
PFNGLGETVERTEXATTRIBIVPROC glGetVertexAttribiv = 0;
PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv = 0;
PFNGLUNIFORM1FPROC glUniform1f = 0;
PFNGLUNIFORM2FPROC glUniform2f = 0;
PFNGLUNIFORM3FPROC glUniform3f = 0;
PFNGLUNIFORM4FPROC glUniform4f = 0;
PFNGLUNIFORM1IPROC glUniform1i = 0;
PFNGLUNIFORM2IPROC glUniform2i = 0;
PFNGLUNIFORM3IPROC glUniform3i = 0;
PFNGLUNIFORM4IPROC glUniform4i = 0;
PFNGLUNIFORM1FVPROC glUniform1fv = 0;
PFNGLUNIFORM2FVPROC glUniform2fv = 0;
PFNGLUNIFORM3FVPROC glUniform3fv = 0;
PFNGLUNIFORM4FVPROC glUniform4fv = 0;
PFNGLUNIFORM1IVPROC glUniform1iv = 0;
PFNGLUNIFORM2IVPROC glUniform2iv = 0;
PFNGLUNIFORM3IVPROC glUniform3iv = 0;
PFNGLUNIFORM4IVPROC glUniform4iv = 0;
PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fv = 0;
PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv = 0;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = 0;
PFNGLVALIDATEPROGRAMPROC glValidateProgram = 0;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = 0;

PFNGLBINDVERTEXARRAYPROC glBindVertexArray = 0;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = 0;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = 0;

PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements = 0;

PFNGLDRAWELEMENTSBASEVERTEXPROC glDrawElementsBaseVertex = 0;
PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC glDrawRangeElementsBaseVertex = 0;
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glDrawElementsInstancedBaseVertex = 0;
PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC glMultiDrawElementsBaseVertex = 0;


PFNGLDRAWARRAYSINSTANCEDPROC	glDrawArraysInstanced = 0;
PFNGLDRAWELEMENTSINSTANCEDPROC	glDrawElementsInstanced = 0;
PFNGLTEXBUFFERPROC				glTexBuffer = 0;
PFNGLPRIMITIVERESTARTINDEXPROC	glPrimitiveRestartIndex = 0;

PFNGLACTIVETEXTUREPROC			glActiveTexture = 0;
PFNGLSAMPLECOVERAGEPROC			glSampleCoverage = 0;
PFNGLCOMPRESSEDTEXIMAGE3DPROC	glCompressedTexImage3D = 0;
PFNGLCOMPRESSEDTEXIMAGE2DPROC	glCompressedTexImage2D = 0;
PFNGLCOMPRESSEDTEXIMAGE1DPROC	glCompressedTexImage1D = 0;
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC	glCompressedTexSubImage3D = 0;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC	glCompressedTexSubImage2D = 0;
PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC	glCompressedTexSubImage1D = 0;
PFNGLGETCOMPRESSEDTEXIMAGEPROC	glGetCompressedTexImage = 0;

PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = 0;

template<typename T>
bool getproc(T& proc_ptr, const char* proc_name) {
	proc_ptr = (T)wglGetProcAddress(proc_name);
	return proc_ptr != 0;
}

#define GLPROC( T ) getproc(T, # T)


const char* to_str(GLenum val){
#define CM(T) case T: return # T
	switch(val){
		CM(GL_FLOAT);
		CM(GL_FLOAT_VEC2);
		CM(GL_FLOAT_VEC3);
		CM(GL_FLOAT_VEC4);
		CM(GL_INT);
		CM(GL_INT_VEC2);
		CM(GL_INT_VEC3);
		CM(GL_INT_VEC4);
		CM(GL_BOOL);
		CM(GL_BOOL_VEC2);
		CM(GL_BOOL_VEC3);
		CM(GL_BOOL_VEC4);
		CM(GL_FLOAT_MAT2);
		CM(GL_FLOAT_MAT3);
		CM(GL_FLOAT_MAT4);
		CM(GL_FLOAT_MAT2x3);
		CM(GL_FLOAT_MAT2x4);
		CM(GL_FLOAT_MAT3x2);
		CM(GL_FLOAT_MAT3x4);
		CM(GL_FLOAT_MAT4x2);
		CM(GL_FLOAT_MAT4x3);
		CM(GL_SAMPLER_1D);
		CM(GL_SAMPLER_2D);
		CM(GL_SAMPLER_3D);
		CM(GL_SAMPLER_CUBE);
		CM(GL_SAMPLER_1D_SHADOW);
		CM(GL_SAMPLER_2D_SHADOW);
	};
	return 0;
}

void init_gl()
{
	//wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB");
	bool res = GLPROC(wglCreateContextAttribsARB);
	res &= GLPROC(glCreateShader);
	res &= GLPROC(glDeleteShader);
	res &= GLPROC(glShaderSource);
	res &= GLPROC(glCompileShader);
	res &= GLPROC(glCreateProgram);
	res &= GLPROC(glDeleteProgram);
	res &= GLPROC(glAttachShader);
	res &= GLPROC(glDetachShader);
	res &= GLPROC(glGetShaderiv);
	res &= GLPROC(glGetShaderInfoLog);
	res &= GLPROC(glLinkProgram);
	res &= GLPROC(glUseProgram);
	res &= GLPROC(glGetProgramiv);
	res &= GLPROC(glGetActiveAttrib);
	res &= GLPROC(glGetActiveUniform);
	res &= GLPROC(wglGetExtensionsStringEXT);
	res &= GLPROC(wglGetExtensionsStringARB);

	res &= GLPROC(glGenQueries);
	res &= GLPROC(glDeleteQueries);
	res &= GLPROC(glIsQuery);
	res &= GLPROC(glBeginQuery);
	res &= GLPROC(glEndQuery);
	res &= GLPROC(glGetQueryiv);
	res &= GLPROC(glGetQueryObjectiv);
	res &= GLPROC(glGetQueryObjectuiv);
	res &= GLPROC(glBindBuffer);
	res &= GLPROC(glDeleteBuffers);
	res &= GLPROC(glGenBuffers);
	res &= GLPROC(glIsBuffer);
	res &= GLPROC(glBufferData);
	res &= GLPROC(glBufferSubData);
	res &= GLPROC(glGetBufferSubData);
	res &= GLPROC(glMapBuffer);
	res &= GLPROC(glUnmapBuffer);
	res &= GLPROC(glGetBufferParameteriv);
	res &= GLPROC(glGetBufferPointerv);

	res &= GLPROC(glBlendEquationSeparate);
	res &= GLPROC(glDrawBuffers);
	res &= GLPROC(glStencilOpSeparate );
	res &= GLPROC(glStencilFuncSeparate );
	res &= GLPROC(glStencilMaskSeparate);
	res &= GLPROC(glBindAttribLocation );
	res &= GLPROC(glDisableVertexAttribArray);
	res &= GLPROC(glEnableVertexAttribArray );
	res &= GLPROC(glGetAttachedShaders );
	res &= GLPROC(glGetAttribLocation);
	res &= GLPROC(glGetProgramInfoLog);
	res &= GLPROC(glGetShaderSource );
	res &= GLPROC(glGetUniformLocation );
	res &= GLPROC(glGetUniformfv );
	res &= GLPROC(glGetUniformiv );
	res &= GLPROC(glGetVertexAttribdv);
	res &= GLPROC(glGetVertexAttribfv);
	res &= GLPROC(glGetVertexAttribiv);
	res &= GLPROC(glGetVertexAttribPointerv);
	res &= GLPROC(glUniform1f);
	res &= GLPROC(glUniform2f);
	res &= GLPROC(glUniform3f);
	res &= GLPROC(glUniform4f);
	res &= GLPROC(glUniform1i);
	res &= GLPROC(glUniform2i);
	res &= GLPROC(glUniform3i);
	res &= GLPROC(glUniform4i);
	res &= GLPROC(glUniform1fv);
	res &= GLPROC(glUniform2fv);
	res &= GLPROC(glUniform3fv);
	res &= GLPROC(glUniform4fv);
	res &= GLPROC(glUniform1iv);
	res &= GLPROC(glUniform2iv);
	res &= GLPROC(glUniform3iv);
	res &= GLPROC(glUniform4iv);
	res &= GLPROC(glUniformMatrix2fv);
	res &= GLPROC(glUniformMatrix3fv);
	res &= GLPROC(glUniformMatrix4fv);
	res &= GLPROC(glValidateProgram);
	res &= GLPROC(glVertexAttribPointer);
		
	res &= GLPROC(glBindVertexArray);
	res &= GLPROC(glDeleteVertexArrays);
	res &= GLPROC(glGenVertexArrays);

	res &= GLPROC(glDrawRangeElements);

	res &= GLPROC(glDrawElementsBaseVertex);
	res &= GLPROC(glDrawRangeElementsBaseVertex);
	res &= GLPROC(glDrawElementsInstancedBaseVertex);
	res &= GLPROC(glMultiDrawElementsBaseVertex);

	res &= GLPROC(glDrawArraysInstanced);
	res &= GLPROC(glDrawElementsInstanced);
	res &= GLPROC(glTexBuffer);
	res &= GLPROC(glPrimitiveRestartIndex);
	
	res &= GLPROC(glActiveTexture);
	res &= GLPROC(glSampleCoverage);
	res &= GLPROC(glCompressedTexImage3D);
	res &= GLPROC(glCompressedTexImage2D);
	res &= GLPROC(glCompressedTexImage1D);
	res &= GLPROC(glCompressedTexSubImage3D);
	res &= GLPROC(glCompressedTexSubImage2D);
	res &= GLPROC(glCompressedTexSubImage1D);
	res &= GLPROC(glGetCompressedTexImage);
	res &= GLPROC(wglChoosePixelFormatARB);

	//assert(res && "Error while getting OpenGL functions!");

	//__asm nop;
}