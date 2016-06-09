#include "RenderObject.h"


RenderObject::RenderObject(void)	{
	mesh = NULL;
	shader = NULL;
	texture = NULL;
	parent = NULL;
}
//Extended RenderObject to set vairables of objects with specific mesh types
RenderObject::RenderObject(Mesh*m, Shader*s, GLuint t) {
	mesh = m;
	shader = s;
	texture = t;
	parent = NULL;

	if (m->getType() == CIRCLE_MESH || STATIC_CIRCLE_MESH)
	{
		circleCenter = Vector3(m->centerPoint.x, m->centerPoint.y, m->centerPoint.z);
		circleRadius = m->radius;
	}
	if (m->getType() == RECTANGLE_MESH)
	{
		rectCenter = m->centerPoint;
		h = m->h;
		w = m->w;
	}
	if (m->getType() == LINE_MESH)
	{
		lineCenter = m->centerPoint;
		length = m->length;
	}
}


RenderObject::~RenderObject(void)
{
}

//Update also updates the position by calling integrate, which updates an objects position if the objects velocity is not 0,0,0
//It also handles the situation when a ball is potted, by setting the view matrix of z to be positive and therefore off screen
void RenderObject::Update(float msec) {
	if (parent) {
		worldTransform = parent->modelMatrix * modelMatrix;
	}
	else {
		worldTransform = modelMatrix;
	}
	for (vector<RenderObject*>::const_iterator i = children.begin(); i != children.end(); ++i) {
		(*i)->Update(msec);
	}
	if (potted == true)
	{
		circleCenter.z = 1;
		SetModelMatrix(Matrix4::Translation(circleCenter) * Matrix4::Scale(Vector3(1, 1, 1)));
	}
	if (v != Vector3(0, 0, 0))
	{
		Integrate(float (1.f / 200.f));//Passing a fixed timestep to the physics
	}

}

void RenderObject::Draw() const {
	if (mesh) {
		mesh->Draw();
	}
}
//Integrate will move an object for a given initial velocity. Acceleration, or deceleration in this case can be applied,
//however I apply a friction dampening factor instead as it worked nicely. I follow semi-implict euler by using the next vn+1
//to calculate the displacement. Displacement could be seen as displacement from 0,0,0 = center of the table.
bool RenderObject::Integrate(float msec) //msec is fixed
{
	v = v - acc * msec; //acceleration / decelartion

	v = v *  0.99; // Damper

	circleCenter = circleCenter + v * msec; //Update position for a given displacement

	SetModelMatrix(Matrix4::Translation(circleCenter) * Matrix4::Scale(Vector3(1, 1, 1)));

	if ((v.x * v.x) + (v.y * v.y) < 0.001) //If the object is moving too slow, just set v to 0,0,0
	{
		v = Vector3(0, 0, 0);
		return false;
	}
	return true;
} //outside this method velocities can be set by other objects hitting the object and therefore setting the velocity