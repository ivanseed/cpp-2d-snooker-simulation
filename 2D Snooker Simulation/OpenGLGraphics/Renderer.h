#pragma once
#include "../nclgl/OGLRenderer.h"

#include "RenderObject.h"

#include <vector>

using std::vector;

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	~Renderer(void);

	virtual void	RenderScene();

	virtual void	Render(const RenderObject &o);

	virtual void	UpdateScene(float msec);

	void	AddRenderObject(RenderObject &r) {
		renderObjects.push_back(&r);
	}
	//Aditional functions
	bool Updating();
	bool CircleCollision(RenderObject* o1, RenderObject* o2);
	bool AABBCollision(RenderObject* o1, RenderObject* o2);
	bool InRange(RenderObject* o1, RenderObject* o2);
	void CircleResponse(RenderObject* o1, RenderObject* o2);
	void AABBResponse(RenderObject* o1, RenderObject* o2);

	//Gamestate int to represent different states of the renderer and the game
	int gamestate = 0;
protected:
	//tex for table and one that I used for testing, func for loading textures too.
	GLuint tableTex;
	GLuint smileyTex;
	GLuint LoadTexture(const char * c);
	vector<RenderObject*> renderObjects;
};

