#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{

	glEnable(GL_DEPTH_TEST);

	tableTex = LoadTexture("table1.png"); //Table picture source: http://www.talkgraphics.com/attachment.php?attachmentid=44535&d=1200631844
	smileyTex = LoadTexture("smiley.png");
}

Renderer::~Renderer(void)	{
	glDeleteTextures(1, &tableTex);
	glDeleteTextures(1, &smileyTex);
}

void	Renderer::RenderScene() {
	for (vector<RenderObject*>::iterator i = renderObjects.begin(); i != renderObjects.end(); ++i) {
		Render(*(*i));
	}
}
//Load it textures
void	Renderer::Render(const RenderObject &o) {
	modelMatrix = o.GetWorldTransform();

	if (o.GetShader() && o.GetMesh()) {
		GLuint program = o.GetShader()->GetShaderProgram();

		glUseProgram(program);

		UpdateShaderMatrices(program);

		glUniform1i(glGetUniformLocation(program, "smileyTex"), 0);
		glUniform1i(glGetUniformLocation(program, "tableTex"), 1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, smileyTex);


		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tableTex);

		o.Draw();
	}

	for (vector<RenderObject*>::const_iterator i = o.GetChildren().begin(); i != o.GetChildren().end(); ++i) {
		Render(*(*i));
	}
}
//The Updating function returns a boolean which is determined by the velocity of balls. I know, due to my implementation that
//If balls are moving they will have a velocity that is not equal to 0,0,0. If the velocity is 0,0,0 I know for a fact that they
//are not moving. If ALL the balls are 0,0,0 I know that no other ball can gain a veloicty from another ball and I can return this
//In the main function I can then call this boolean and allow the user to take another shot or perform an action, if and only if
//The renderer is currently not updating the positions of anything.
bool	Renderer::Updating() {
	for (vector<RenderObject*>::iterator i = renderObjects.begin(); i != renderObjects.end(); ++i) {
		if ((*i)->v != Vector3(0, 0, 0))
		{
			return true;
		}
	}
	return false;
}
//I extended the function to, call collision detection on all renderObjects (could maybe be made efficient by hard coding some limits
//but this way it is more extendable). I call CircleCollision - determines new velocities of balls when they colide with each other.
//AABBResponse - determines new velocity of ball when it hits the sides/cushions of the table.
//InRange - determines if a ball falls into a pocket by using an inrange rather than collision.

//Due to the way renderobjects are put into the vector RenderObjects I stated which if statements are useless however if the renderObjects
//were added in a different order, they would be needed
void	Renderer::UpdateScene(float msec) {
	for (vector<RenderObject*>::iterator i = renderObjects.begin(); i != renderObjects.end(); ++i) {
		(*i)->Update(msec);
	}
	if (gamestate != 0) //Dont run collision detection when gamestate is 0 (0 is when we place the whiteball)
	{
		for (int i = 0; i < renderObjects.size(); i++)//Collision detection loop
		{
			for (int j = i + 1; j < renderObjects.size(); j++)
			{
				if ((renderObjects.at(i)->GetMesh()->getType() == CIRCLE_MESH && !renderObjects.at(i)->potted) && (renderObjects.at(j)->GetMesh()->getType() == CIRCLE_MESH && !renderObjects.at(j)->potted))
				{
					if (CircleCollision(renderObjects.at(i), renderObjects.at(j)))
					{
						CircleResponse(renderObjects.at(i), renderObjects.at(j));
					}
				}
				else
				if ((renderObjects.at(i)->GetMesh()->getType() == CIRCLE_MESH) && (renderObjects.at(j)->GetMesh()->getType() == RECTANGLE_MESH))
				{
					if (AABBCollision(renderObjects.at(i), renderObjects.at(j)))
					{
						AABBResponse(renderObjects.at(i), renderObjects.at(j));
					}
				}
				else
				if ((renderObjects.at(i)->GetMesh()->getType() == CIRCLE_MESH) && (renderObjects.at(j)->GetMesh()->getType() == STATIC_CIRCLE_MESH))
				{
					if (InRange(renderObjects.at(i), renderObjects.at(j)))
					{
						renderObjects.at(i)->potted = true;
					}
				}
				//Due to the way the objects are stored this loop is not needed
				if ((renderObjects.at(i)->GetMesh()->getType() == RECTANGLE_MESH) && (renderObjects.at(j)->GetMesh()->getType() == CIRCLE_MESH))
				{
					if (AABBCollision(renderObjects.at(j), renderObjects.at(i)))
					{
						AABBResponse(renderObjects.at(j), renderObjects.at(i));
					}
				}
				//This loop ensures that the line follows the whiteball
				if (renderObjects.at(i)->GetMesh()->getType() == LINE_MESH)
				{
					if (renderObjects.at(i)->lineCenter != renderObjects.at(i - 1)->circleCenter)
					{
						renderObjects.at(i)->SetModelMatrix(Matrix4::Translation(renderObjects.at(i - 1)->circleCenter));
						renderObjects.at(i)->lineCenter = renderObjects.at(i - 1)->circleCenter;
						renderObjects.at(i)->rotation = 0;
					}
				}
			}
		}
	}
}
//InRange returns a boolean:
//TRUE = object o1 center is with the radius of object o2.
//FALSE = object o1 center is not within the radius of object o2.
bool Renderer::InRange(RenderObject* o1, RenderObject* o2)
{
	float x = o1->circleCenter.x;
	float y = o1->circleCenter.y;
	if ((((x - o2->circleCenter.x) * (x - o2->circleCenter.x)) + ((y - o2->circleCenter.y) * (y - o2->circleCenter.y))) < (o2->circleRadius * o2->circleRadius))
	{
		return true;
	}
	return false;
}
//Based on another modules algorithm for circle collision and slight optimization by introducing bounding boxes,
//This method returns true if two objects (circles) are inside of each other.
bool Renderer::CircleCollision(RenderObject* o1, RenderObject* o2)
{
	/* Circle Circle collision, based on the Games Programming Coursework I completed in first semester */
	Vector3 c1 = o1->circleCenter;
	Vector3 c2 = o2->circleCenter;

	float r1 = o1->circleRadius;
	float r2 = o2->circleRadius;
	//The first loop creates bounding boxes around the circles before performing trig to find if they are truly coliding
	if (c1.x + r1 + r2 > c2.x
		&& c1.x < c2.x + r1 + r2
		&& c1.y + r1 + r2 > c2.y
		&& c1.y < c2.y + r1 + r2)
	{

		float r3 = r1 + r2;
		float distance = (c1.x - c2.x) * (c1.x - c2.x) + (c1.y - c2.y) * (c1.y - c2.y);
		return distance < r3 * r3;
	}
	return false;
}
/* Circle AABB collision, based on the Games Programming Coursework I completed in first semester */
bool Renderer::AABBCollision(RenderObject* o1, RenderObject* o2)
{
	float differenceX = abs(o1->circleCenter.x - o2->rectCenter.x);
	float differenceY = abs(o1->circleCenter.y - o2->rectCenter.y);
	if (differenceX >(o2->w / 2 + o1->circleRadius))
	{
		return false;
	}
	if (differenceY >(o2->h / 2 + o1->circleRadius))
	{
		return false;
	}
	if (differenceX <= (o2->w / 2))
	{
		return true;
	}
	if (differenceY <= (o2->h / 2))
	{
		return true;
	}
	float corner = ((differenceX - o2->w / 2) * (differenceX - o2->w / 2)) + ((differenceY - o2->h / 2) * (differenceY - o2->h / 2));
	return (corner <= (o1->circleRadius * o1->circleRadius));
}
void Renderer::CircleResponse(RenderObject* o1, RenderObject* o2)
{
	/* Object pen fix, I followed the tutorial: http://gamedevelopment.tutsplus.com/tutorials/how-to-create-a-custom-2d-physics-engine-the-basics-and-impulse-resolution--gamedev-6331
	to get a general idea on how to fix to penetration however the code is not exactly the same as the tutorial, just uses the general idea. */
	Vector3 c1 = o1->circleCenter;
	Vector3 c2 = o2->circleCenter;

	float r1 = o1->circleRadius;
	float r2 = o2->circleRadius;
	//I find the point and depth, then push both objects both ways across the normal
	Vector3 point = c2 - c1;
	float length = point.Length();
	point.Normalise();//Normalise so I know the direction to push the balls away from each other
	float depth = ((r1 + r2) - length) + r1 / 4;// r/4 avoids balls sticking together. It was a number I had to play around with to find a number that avoided the balls sticking together
	point = point * depth;
	o1->circleCenter -= point / 2.f; //half one way, half the other way
	o2->circleCenter += point / 2.f;
	o1->SetModelMatrix(Matrix4::Translation(o1->circleCenter) * Matrix4::Scale(Vector3(1, 1, 1)));
	o2->SetModelMatrix(Matrix4::Translation(o2->circleCenter) * Matrix4::Scale(Vector3(1, 1, 1)));

	c1 = o1->circleCenter;
	c2 = o2->circleCenter;

	/* Circle to circle response based on lecture notes and the following tutorial : http://www.plasmaphysics.org.uk/programs/coll2d_cpp.htm
	on a general way how to implement collision response.  */
	float differencex = c2.x - c1.x;
	float differencey = c2.y - c1.y;

	float massover = o2->mass / o1->mass; //returns 1, still included it for feature changes. Also extermented with different ball masses and allows for different implementations...

	//Based on the tutorail, detemrines new velocites based on the normal of the colision and calculates thew new initial velocity based on the balls velocities on impact
	float differenceVelx = o2->v.x - o1->v.x;
	float differenceVely = o2->v.y - o1->v.y;

	float a = differencey / differencex;

	float dvx = -2 * (differenceVelx + a * differenceVely) / ((1 + a * a)*(1 + massover));

	float o1velX = o1->v.x - massover * dvx;
	float o1velY = o1->v.y - a * massover * dvx;
	float o2velX = o2->v.x + dvx;
	float o2velY = o2->v.y + a * dvx;


	o1->v = Vector3(o1velX, o1velY, o1->v.z);
	o2->v = Vector3(o2velX, o2velY, o2->v.z);
}
//Inverses the velocity and also dampens the velocity, could use mass to work out the dampen but I hard code 0.5
//Also uses similar penetration method that works.
void Renderer::AABBResponse(RenderObject* o1, RenderObject* o2)
{
	/* The penetration fix is based on the circle-circle one, I just experimeneted until I found good values.
		The If loops decide which cushion to perform the penetration fix and velocity changes on */
	if (o2->rectCenter.y > 0.22f)
	{
		float depth = o2->rectCenter.y - (o2->h * 2) - o1->circleCenter.y + o1->circleRadius;
		float total = o1->circleRadius - depth;
		o1->circleCenter.y -= total;
		o1->SetModelMatrix(Matrix4::Translation(o1->circleCenter) * Matrix4::Scale(Vector3(1, 1, 1)));
		o1->v.y = o1->v.y * (-1);
		o1->v = o1->v * 0.5;
		return;
	}
	if (o2->rectCenter.y < -0.22f)
	{
		float depth = o2->rectCenter.y + o2->h - (o1->circleCenter.y - o1->circleRadius);
		o1->circleCenter.y += depth;
		o1->SetModelMatrix(Matrix4::Translation(o1->circleCenter) * Matrix4::Scale(Vector3(1, 1, 1)));
		o1->v.y = o1->v.y * (-1);
		o1->v = o1->v * 0.5;
		return;
	}
	if (o2->rectCenter.x > 0.49f)
	{
		float depth = o2->rectCenter.x - (o2->w * 2) - o1->circleCenter.x + o1->circleRadius;
		float total = o1->circleRadius - depth;
		o1->circleCenter.x -= total;
		o1->SetModelMatrix(Matrix4::Translation(o1->circleCenter) * Matrix4::Scale(Vector3(1, 1, 1)));
		o1->v.x = o1->v.x * (-1);
		o1->v = o1->v * 0.5;
		return;
	}
	if (o2->rectCenter.x < -0.49f)
	{
		float depth = o2->rectCenter.x + (o2->w * 2) - o1->circleCenter.x + o1->circleRadius;
		float total = o1->circleRadius - depth;
		o1->circleCenter.x -= total;
		o1->SetModelMatrix(Matrix4::Translation(o1->circleCenter) * Matrix4::Scale(Vector3(1, 1, 1)));
		o1->v.x = o1->v.x * (-1);
		o1->v = o1->v * 0.5;
		return;
	}


}
//From Hardware rateriser cw
GLuint Renderer::LoadTexture(const char * c)
{
	GLuint tex;
	tex = SOIL_load_OGL_texture(c, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);

	//GL Binds for texture and also setting the parameters
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	return tex;
}