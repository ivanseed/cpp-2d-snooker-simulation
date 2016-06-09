#pragma once
/*
Class:OGLRenderer
Author:Rich Davison	<richard.davison4@newcastle.ac.uk>
Description:Abstract base class for the graphics tutorials. Creates an OpenGL 
3.2 CORE PROFILE rendering context. Each lesson will create a renderer that 
inherits from this class - so all context creation is handled automatically,
but students still get to see HOW such a context is created.

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*/

#include <string>
#include <fstream>
#include <vector>

#define GLEW_STATIC 
#include "GL/glew.h"
#include "GL/wglew.h"

#include "SOIL.h"

#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Quaternion.h"
#include "Matrix4.h"
#include "Window.h"

using std::vector;

#ifdef _DEBUG
#define GL_BREAKPOINT glUniform4uiv(0,0,0);//Invalid, but triggers gdebugger / codeXL ;)
#else
#define GL_BREAKPOINT //
#endif

#ifdef _DEBUG
#define OPENGL_DEBUGGING
#endif

class OGLRenderer	{
public:
	friend class Window;
	OGLRenderer(Window &parent);
	virtual ~OGLRenderer(void);

	virtual void	RenderScene()		= 0;
	virtual void	UpdateScene(float msec);
	void			SwapBuffers();

	bool			HasInitialised() const;	

	void			SetProjectionMatrix(Matrix4 m) {
		projMatrix = m;
	}

	void			SetViewMatrix(Matrix4 m) {
		viewMatrix = m;
	}

	void	ClearBuffers();
	
protected:
	virtual void	Resize(int x, int y);

	void			UpdateShaderMatrices(GLuint program);

	Matrix4 projMatrix;		//Projection matrix
	Matrix4 modelMatrix;	//Model matrix. NOT MODELVIEW
	Matrix4 viewMatrix;		//View matrix
	Matrix4 textureMatrix;	//Texture matrix

	bool	init;			//Did the renderer initialise properly?

	HDC		deviceContext;	//...Device context?
	HGLRC	renderContext;	//Permanent Rendering Context

	static int		width;			//Render area width (not quite the same as window width)
	static int		height;			//Render area height (not quite the same as window height)


#ifdef _DEBUG
	static void CALLBACK DebugCallback(	GLenum source, GLenum type, GLuint id, GLenum severity,
										GLsizei length, const GLchar* message, void* userParam);
#endif

	static void OGLRenderer::BasicResizeFunc(int x, int y);
};