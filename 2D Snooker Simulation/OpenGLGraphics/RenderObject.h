#pragma once

#include "../nclgl/Matrix4.h"
#include "Mesh.h"
#include "Shader.h"
#include "../nclgl/OGLRenderer.h"
enum Object {
	circle,
	table
};
class RenderObject	{
public:
	RenderObject(void);
	RenderObject(Mesh*m, Shader*s, GLuint t = 0);
	~RenderObject(void);

	Mesh*	GetMesh()	const			{ return mesh; }
	void	SetMesh(Mesh*m)				{ mesh = m; }

	Shader* GetShader()		const		{ return shader; }
	void	SetShader(Shader*s)			{ shader = s; }

	GLuint	GetTexture()		const	{ return texture; }
	void	SetTexture(GLuint tex)		{ texture = tex; }

	void	SetModelMatrix(Matrix4 mat) { modelMatrix = mat; }
	Matrix4 GetModelMatrix()	const	{ return modelMatrix; }

	virtual void Update(float msec);

	virtual void Draw() const;

	void	AddChild(RenderObject &child) {
		children.push_back(&child);
		child.parent = this;
	}

	Matrix4 GetWorldTransform() const {
		return worldTransform;
	}

	const vector<RenderObject*>& GetChildren() const  {
		return children;
	}

	bool Integrate(float msec); //Update object position

	//Used if the Mesh is a circle
	Vector3 v, acc, circleCenter;
	float circleRadius;
	float mass = 1;

	bool potted = false;

	//Used if the Mesh is a rectangle, used for cushions
	Vector3 rectCenter;
	float h, w;

	//Used if the Mesh is the pointer
	Vector3 lineCenter;
	float length;
	float rotation = 0;
protected:
	Mesh*	mesh;
	Shader*	shader;

	GLuint	texture;

	Matrix4 modelMatrix;
	Matrix4 worldTransform;

	RenderObject*			parent;
	vector<RenderObject*>	children;
};

