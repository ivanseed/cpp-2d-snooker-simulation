/******************************************************************************
Class:Mesh
Implements:
Author:Rich Davison	<richard.davison4@newcastle.ac.uk>
Description:Wrapper around OpenGL primitives, geometry and related
OGL functions.

There's a couple of extra functions in here that you didn't get in the tutorial
series, to draw debug normals and tangents.


-_-_-_-_-_-_-_,------,
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""

*//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../nclgl/OGLRenderer.h"
#include <vector>
#include <string>
#include <fstream>

using std::ifstream;
using std::string;

//A handy enumerator, to determine which member of the bufferObject array
//holds which data
enum MeshBuffer {
	VERTEX_BUFFER = 0,
	COLOUR_BUFFER = 1,
	TEXTURE_BUFFER,
	NORMAL_BUFFER,
	TANGENT_BUFFER,
	INDEX_BUFFER,
	MAX_BUFFER
};
//Mesh types are used to determine what the object is and what to do with it i.e. if mesh type is CIRCLE_MESH
//I know that it is a ball. This could have been implemented in another way and if I were to change a feature of this
//submission it would be this.
enum MeshType {
	TRIANGLE_MESH,
	RECTANGLE_MESH,
	CIRCLE_MESH,
	FILE_MESH,
	STATIC_CIRCLE_MESH,
	LINE_MESH,
	STATIC_MESH
};

class Mesh	{

public:
	friend class MD5Mesh;
	Mesh(void);
	virtual ~Mesh(void);

	virtual void Draw();

	//Generates a single triangle, with RGB colours
	static Mesh*	GenerateTriangle();
	static Mesh*	GenerateRectangle();
	static Mesh*	GenerateLine(Vector3 a, float l); //Generates line with a set length
	static Mesh*	GenerateAABB(Vector3 center, float h, float w, Vector4 colour); //Generates a rectangle with a center point w and h. Also can set the colour of it
	static Mesh*	GeneratePowerBar(Vector3 center, float h, float w); //A power b ar is similar to the AABB but has interpoliated colours
	static Mesh*	GenerateCircle(Vector3 center, Vector4 colour, float r); //Creates a circle, I initially make them at 0,0,0 so I can move them later. Allows for radius specification as well as colour
	static Mesh*	GenerateStaticCircle(Vector3 center, Vector4 colour, float r); //Static circles are ones that do not move and should not be in the collision check for sides of the table/normal circles
	static Mesh*	LoadMeshFile(const string &filename);

	GLuint	type;	//Primitive type for this mesh (GL_TRIANGLES...etc)

	MeshType getType()
	{
		return enumType;
	}
	//Used for circles then passed onto renderobject as it is the easiest way to pass the values
	//without repeating passed value in the main method
	Vector3 centerPoint; //also used for rect
	float radius;

	//Rectangle corners
	float h, w;

	//Line length
	float length;
protected:
	MeshType enumType;
	//Buffers all VBO data into graphics memory. Required before drawing!
	void	BufferData();

	//VAO for this mesh
	GLuint	arrayObject;
	//VBOs for this mesh
	GLuint	bufferObject[MAX_BUFFER];
	//Number of vertices for this mesh
	GLuint	numVertices;

	//Number of indices for this mesh
	GLuint			numIndices;

	//Pointer to vertex position attribute data (badly named...?)
	Vector3*		vertices;
	//Pointer to vertex colour attribute data
	Vector4*		colours;
	//Pointer to vertex texture coordinate attribute data
	Vector2*		textureCoords;
	//Pointer to vertex normals attribute data
	Vector3*		normals;
	//Pointer to vertex tangents attribute data
	Vector3*		tangents;
	//Pointer to vertex indices attribute data
	unsigned int*	indices;

};

