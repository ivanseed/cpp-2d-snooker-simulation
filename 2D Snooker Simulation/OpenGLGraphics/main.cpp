#include "Renderer.h"
#include "RenderObject.h"

#pragma comment(lib, "nclgl.lib")

/*
	HOW TO PLAY:
		When the game loads you first must move the white ball using arrow keys and then you must confirm the position of the ball with
		the spacebar. It must be placed in the D section

		After placing the ball you can adjust the power of the show with left and right arrow keys and adjust the direction of the shot
		with the up and down keys. Confirm the shot with spacebar.

		Pressing F at anytime will output the FPS to console.

		If you pot the whiteball it will revert to the gamestate where you have to again place the whiteball back in the D section

		Press X to close the game
*/

//Takes an Array (this case the array of balls) and returns true if and only if ALL balls are currently not coliding, else 
//returns false. Used for placing the whiteball. Based on the circle circle collision in renderer.cpp
bool ArrayCollisionCheck(RenderObject arr[])
{
	for (int i = 0; i < 21; i++)
	{
		Vector3 c1 = arr[i].circleCenter;
		Vector3 c2 = arr[21].circleCenter;
		float r1 = arr[i].circleRadius;
		float r2 = arr[21].circleRadius;
		if (c1.x + r1 + r2 > c2.x
			&& c1.x < c2.x + r1 + r2
			&& c1.y + r1 + r2 > c2.y
			&& c1.y < c2.y + r1 + r2)
		{
			float r3 = r1 + r2;
			float distance = (c1.x - c2.x) * (c1.x - c2.x) + (c1.y - c2.y) * (c1.y - c2.y);
			if (distance < r3 * r3)
			{
				return false;
			}
		}
	}
	return true;
}
void main(void) {

	Window w = Window(800, 600);
	Renderer r(w);

	/* Some helpful positions */
	float diameter = 0.0221f; // Min diameter actual is 0.022f

	//For Alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//For Culling the back, useful for the alpha fade
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	//Default positions of the balls, which I worked out manually based on my texture
	Vector3 bluePosition(0.0f, 0.0f, -1.0f);
	Vector3 pinkPosition(-0.255f, 0.0f, -1.0f);
	Vector3 blackPosition(-0.404f, 0.0f, -1.0f);
	Vector3 brownPosition(0.292f, 0.0f, -1.0f);
	Vector3 yellowPosition(0.292f, 0.075f, -1.0f);
	Vector3 greenPosition(0.292f, -0.075f, -1.0f);
	Vector3 whitePosition(0.342f, 0.0f, -1.0f);

	Vector3 rb1Position(-0.2771f, 0.0f, -1.0f); //Redball 1 position


	Mesh* m = Mesh::GenerateRectangle();//Table

	//Meshes for each ball, colours
	Mesh* redBall = Mesh::GenerateCircle(Vector3(0, 0, 0), Vector4(0.85f, 0.1f, 0.1f, 1.0f), 0.011f); //(pos),(colour),(radius)
	Mesh* blueBall = Mesh::GenerateCircle(Vector3(0, 0, 0), Vector4(0.15f, 0.2f, 0.9f, 1.0f), 0.011f); //
	Mesh* pinkBall = Mesh::GenerateCircle(Vector3(0, 0, 0), Vector4(1.0f, 0.5f, 1.0f, 1.0f), 0.011f); //
	Mesh* blackBall = Mesh::GenerateCircle(Vector3(0, 0, 0), Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.011f); //
	Mesh* brownBall = Mesh::GenerateCircle(Vector3(0, 0, 0), Vector4(0.5f, 0.35f, 0.0f, 1.0f), 0.011f); //
	Mesh* yellowBall = Mesh::GenerateCircle(Vector3(0, 0, 0), Vector4(0.85f, 0.85f, 0.1f, 1.0f), 0.011f); //
	Mesh* greenBall = Mesh::GenerateCircle(Vector3(0, 0, 0), Vector4(0.1f, 0.3f, 0.05f, 1.0f), 0.011f); //
	Mesh* whiteBall = Mesh::GenerateCircle(Vector3(0, 0, 0), Vector4(1.0f, 1.0f, 1.0f, 1.0f), 0.011f); //

	//Shaders used for, table, balls, cushions
	Shader* s = new Shader("basicvert.glsl", "BallFrag.glsl");
	Shader* c = new Shader("basicvert.glsl", "cushFrag.glsl");
	Shader* tableShader = new Shader("basicvert.glsl", "tablefrag.glsl");
	Shader* basic = new Shader("basicvert.glsl", "basicFrag.glsl");

	if (s->UsingDefaultShader()) {
		cout << "Warning: Using default shader! Your shader probably hasn't worked..." << endl;
		cout << "Press any key to continue." << endl;
		std::cin.get();
	}

	//table
	RenderObject o(m, tableShader);

	RenderObject ballArray[22];//Ball Array

	//This for loop calculates all the redball positions using an algorithm that moves the X and Y accordingly based on the current ball.
	float changeX = 0.0f;
	float changeY = 0.0f;
	for (int i = 0; i < 15; i++)
	{
		if (i < 15)
		{
			if (5 > i && i > 0)
			{
				changeX -= 0.0221f;
				changeY += 0.01105f;
			}
			if (i == 5)
			{
				changeX = -0.0221f;
				changeY = -0.01105f;
			}
			if (9 > i && i > 5)
			{
				changeX -= 0.0221f;
				changeY += 0.01105f;
			}
			if (i == 9)
			{
				changeX = -0.0442f;
				changeY = -0.0221f;
			}
			if (12 > i && i > 9)
			{
				changeX -= 0.0221f;
				changeY += 0.01105f;
			}
			if (i == 12)
			{
				changeX = -0.0663f;
				changeY = -0.03315f;
			}
			if (i == 13)
			{
				changeX = -0.0884f;
				changeY = -0.0221f;
			}
			if (i == 14)
			{
				changeX = -0.0884f;
				changeY = -0.0442f;
			}
			//Uses the same mesh and shader for each ball, the center of the redball is based on the above if loops which calculates the positions from right to left ascending like:
			//https://gyazo.com/8dddd482a049d1649a7e0e54b8967e2c
			ballArray[i] = RenderObject(redBall, s);
			ballArray[i].SetModelMatrix(Matrix4::Translation(Vector3(rb1Position.x + changeX, rb1Position.y + changeY, rb1Position.z)) * Matrix4::Scale(Vector3(1, 1, 1)));
			ballArray[i].circleCenter = Vector3(rb1Position.x + changeX, rb1Position.y + changeY, rb1Position.z);
			r.AddRenderObject(ballArray[i]);
			cout << ballArray[i].GetModelMatrix() << endl;
			cout << ballArray[i].circleCenter << endl;
		}
	}//I could of done rb12 , 13 and 14 outside the loop as it is more efficient but I kept it in because it looked nice.
	//Black Ball
	ballArray[15] = RenderObject(blackBall, s);
	ballArray[15].SetModelMatrix(Matrix4::Translation(blackPosition) * Matrix4::Scale(Vector3(1, 1, 1))); //Set the model matrix to be moved by black position
	ballArray[15].circleCenter = blackPosition; //Update circle center which is used for several functions, keeps track of the center of the model matrix(ball) and edited to update the model matrix
	r.AddRenderObject(ballArray[15]);

	//Pink Ball
	ballArray[16] = RenderObject(pinkBall, s);
	ballArray[16].SetModelMatrix(Matrix4::Translation(pinkPosition) * Matrix4::Scale(Vector3(1, 1, 1)));
	ballArray[16].circleCenter = pinkPosition;
	r.AddRenderObject(ballArray[16]);

	//Blue Ball
	ballArray[17] = RenderObject(blueBall, s);
	ballArray[17].SetModelMatrix(Matrix4::Translation(bluePosition) * Matrix4::Scale(Vector3(1, 1, 1)));
	ballArray[17].circleCenter = bluePosition;
	r.AddRenderObject(ballArray[17]);

	//Brown Ball
	ballArray[18] = RenderObject(brownBall, s);
	ballArray[18].SetModelMatrix(Matrix4::Translation(brownPosition) * Matrix4::Scale(Vector3(1, 1, 1)));
	ballArray[18].circleCenter = brownPosition;
	r.AddRenderObject(ballArray[18]);

	//Yellow Ball
	ballArray[19] = RenderObject(yellowBall, s);
	ballArray[19].SetModelMatrix(Matrix4::Translation(yellowPosition) * Matrix4::Scale(Vector3(1, 1, 1)));
	ballArray[19].circleCenter = yellowPosition;
	r.AddRenderObject(ballArray[19]);


	//Green Ball
	ballArray[20] = RenderObject(greenBall, s);
	ballArray[20].SetModelMatrix(Matrix4::Translation(greenPosition) * Matrix4::Scale(Vector3(1, 1, 1)));
	ballArray[20].circleCenter = greenPosition;
	r.AddRenderObject(ballArray[20]);

	//White Ball
	ballArray[21] = RenderObject(whiteBall, s);
	ballArray[21].SetModelMatrix(Matrix4::Translation(whitePosition) * Matrix4::Scale(Vector3(1, 1, 1)));
	ballArray[21].circleCenter = whitePosition;
	r.AddRenderObject(ballArray[21]);

	//pointer used by the whiteball, line with center of the white ball and can rotated with arrow keys
	Mesh* line = Mesh::GenerateLine(Vector3(0, 0, 0), 0.05);
	RenderObject pointer(line, basic);
	pointer.SetModelMatrix(Matrix4::Translation(Vector3(0, 0, -1)));
	r.AddRenderObject(pointer);

	Vector4 cushionColour = Vector4(0.0f, 0.5f, 0.0f, 1.0f);//colour of table cushions
	//Since side cushions wont be moving around, I dont worry about the model matrix position
	//Side Cushions
	Mesh* leftmost = Mesh::GenerateAABB(Vector3(-0.493, 0, 0), 0.4f, 0.01f, cushionColour);
	Mesh* rightmost = Mesh::GenerateAABB(Vector3(0.493, 0, 0), 0.4f, 0.01f, cushionColour);
	Mesh* rightbot = Mesh::GenerateAABB(Vector3(0.246, -0.23, 0), 0.01f, 0.43f, cushionColour);
	Mesh* righttop = Mesh::GenerateAABB(Vector3(0.246, 0.23, 0), 0.01f, 0.43f, cushionColour);
	Mesh* leftbot = Mesh::GenerateAABB(Vector3(-0.246, -0.23, 0), 0.01f, 0.43f, cushionColour);
	Mesh* lefttop = Mesh::GenerateAABB(Vector3(-0.246, 0.23, 0), 0.01f, 0.43f, cushionColour);


	RenderObject leftCushion(leftmost, c);
	leftCushion.SetModelMatrix(Matrix4::Translation(Vector3(0, 0, -1)) * Matrix4::Scale(Vector3(1, 1, 1)));
	r.AddRenderObject(leftCushion);

	RenderObject rightCushion(rightmost, c);
	rightCushion.SetModelMatrix(Matrix4::Translation(Vector3(0, 0, -1)) * Matrix4::Scale(Vector3(1, 1, 1)));
	r.AddRenderObject(rightCushion);

	RenderObject botrightCushion(rightbot, c);
	botrightCushion.SetModelMatrix(Matrix4::Translation(Vector3(0, 0, -1)) * Matrix4::Scale(Vector3(1, 1, 1)));
	r.AddRenderObject(botrightCushion);

	RenderObject botleftCushion(leftbot, c);
	botleftCushion.SetModelMatrix(Matrix4::Translation(Vector3(0, 0, -1)) * Matrix4::Scale(Vector3(1, 1, 1)));
	r.AddRenderObject(botleftCushion);

	RenderObject topleftCushion(lefttop, c);
	topleftCushion.SetModelMatrix(Matrix4::Translation(Vector3(0, 0, -1)) * Matrix4::Scale(Vector3(1, 1, 1)));
	r.AddRenderObject(topleftCushion);

	RenderObject toprightCushion(righttop, c);
	toprightCushion.SetModelMatrix(Matrix4::Translation(Vector3(0, 0, -1)) * Matrix4::Scale(Vector3(1, 1, 1)));
	r.AddRenderObject(toprightCushion);


	//Pockets (same as ball but static)
	Vector3 topMidPos(0, 0.235, 0);
	Mesh* topMiddle = Mesh::GenerateStaticCircle(Vector3(0, 0, 0), Vector4(1.0f, 0.0f, 0.0f, 1.0f), 0.0275f);
	RenderObject topHole(topMiddle, basic);
	topHole.SetModelMatrix(Matrix4::Translation(topMidPos) * Matrix4::Scale(Vector3(1, 1, 1)));
	topHole.circleCenter = topMidPos;
	r.AddRenderObject(topHole);

	Vector3 botMidPos(0, -0.235, 0);
	Mesh* botMiddle = Mesh::GenerateStaticCircle(Vector3(0, 0, 0), Vector4(1.0f, 0.0f, 0.0f, 1.0f), 0.0275f);
	RenderObject botHole(botMiddle, basic);
	botHole.SetModelMatrix(Matrix4::Translation(botMidPos) * Matrix4::Scale(Vector3(1, 1, 1)));
	botHole.circleCenter = botMidPos;
	r.AddRenderObject(botHole);

	Vector3 botRightPos(0.492, -0.231, 0);
	Mesh* botRight = Mesh::GenerateStaticCircle(Vector3(0, 0, 0), Vector4(1.0f, 0.0f, 0.0f, 1.0f), 0.0255f);
	RenderObject botRightHole(botRight, basic);
	botRightHole.SetModelMatrix(Matrix4::Translation(botRightPos) * Matrix4::Scale(Vector3(1, 1, 1)));
	botRightHole.circleCenter = botRightPos;
	r.AddRenderObject(botRightHole);

	Vector3 topRightPos(0.492, 0.231, 0);
	Mesh* topRight = Mesh::GenerateStaticCircle(Vector3(0, 0, 0), Vector4(1.0f, 0.0f, 0.0f, 1.0f), 0.0255f);
	RenderObject topRightHole(topRight, basic);
	topRightHole.SetModelMatrix(Matrix4::Translation(topRightPos) * Matrix4::Scale(Vector3(1, 1, 1)));
	topRightHole.circleCenter = topRightPos;
	r.AddRenderObject(topRightHole);

	Vector3 topLeftPos(-0.492, 0.231, 0);
	Mesh* topLeft = Mesh::GenerateStaticCircle(Vector3(0, 0, 0), Vector4(1.0f, 0.0f, 0.0f, 1.0f), 0.0255f);
	RenderObject topLeftHole(topLeft, basic);
	topLeftHole.SetModelMatrix(Matrix4::Translation(topLeftPos) * Matrix4::Scale(Vector3(1, 1, 1)));
	topLeftHole.circleCenter = topLeftPos;
	r.AddRenderObject(topLeftHole);

	Vector3 botLeftPos(-0.492, -0.231, 0);
	Mesh* botLeft = Mesh::GenerateStaticCircle(Vector3(0, 0, 0), Vector4(1.0f, 0.0f, 0.0f, 1.0f), 0.0255f);
	RenderObject botLeftHole(botLeft, basic);
	botLeftHole.SetModelMatrix(Matrix4::Translation(botLeftPos) * Matrix4::Scale(Vector3(1, 1, 1)));
	botLeftHole.circleCenter = botLeftPos;
	r.AddRenderObject(botLeftHole);

	//The widget is the slider that goes onto the powerball
	Vector3 widgetCenter(-0.4f, -0.34f, -1.f);
	Mesh* widgetMesh = Mesh::GenerateAABB(Vector3(0, 0, 0), 0.09, 0.05, Vector4(0.3f, 0.3f, 0.3f, 1.0f));
	RenderObject widgetO(widgetMesh, basic);
	widgetO.SetModelMatrix(Matrix4::Translation(widgetCenter) * Matrix4::Scale(Vector3(1, 1, 1)));
	widgetO.rectCenter = widgetCenter;
	r.AddRenderObject(widgetO);

	//The powerbar is the interpolated AABB at the bottom of the screen
	Vector3 powerCenter(0.0f, -0.34f, -1.f);
	Mesh* powerMesh = Mesh::GeneratePowerBar(Vector3(0, 0, 0), 0.08, 0.8);
	RenderObject powerBar(powerMesh, basic);
	powerBar.SetModelMatrix(Matrix4::Translation(powerCenter) * Matrix4::Scale(Vector3(1, 1, 1)));
	powerBar.rectCenter = powerCenter;
	r.AddRenderObject(powerBar);

	//add the table, final renderer calls
	o.SetModelMatrix(Matrix4::Translation(Vector3(0, 0, -1.5)) * Matrix4::Scale(Vector3(1, 1, 1)));
	r.AddRenderObject(o);
	r.SetProjectionMatrix(Matrix4::Perspective(1, 100, 1.33f, 45.0f));
	r.SetViewMatrix(Matrix4::BuildViewMatrix(Vector3(0, 0, 0), Vector3(0, 0, -2)));

	Vector3 semi(0.292, 0, 0);//used to calcualte if the whiteball is placed in the semi circle d, which isnt an object as its a texture so have to have these values hardcoded
	float semiRadius = 0.078f;

	/* The following fps counter is based off the answer on stackoverflow found here :
		http://gamedev.stackexchange.com/questions/83159/simple-framerate-counter
	*/
	int frames = 0;
	bool fpspressed = false;
	float starttime = w.GetTimer()->GetMS();
	float fps = 0.0f;

	while (w.UpdateWindow())
	{
		if ((w.GetTimer()->GetMS() - starttime) / 1000 > 0.25 && frames > 10)
		{
			fps = frames / ((w.GetTimer()->GetMS() - starttime) / 1000);
			starttime = w.GetTimer()->GetMS();
			frames = 0;
		}
		frames++;
		
		if (Keyboard::KeyDown(KEY_F))
		{
			fpspressed = true;
		}
		if (fpspressed)
		{
			cout << (int) fps << endl;
			fpspressed = false;
		}

		float msec = w.GetTimer()->GetTimedMS() / 1000;
		if (!r.Updating() && (r.gamestate == -1)) //This if statement puts the pointer back to the whiteball after the balls have stopped moving
		{
			pointer.SetModelMatrix(Matrix4::Translation(ballArray[21].circleCenter));
			r.gamestate = 1;
		}

		if (!r.Updating())//If all the balls are still, then allow user input
		{
			if (ballArray[21].potted == true) //first check if the whiteball is potted, if it is then take the game back to gamestate 0, which is placing the whiteball
			{
				cout << "You potted the whiteball." << endl;
				r.gamestate = 0;
				ballArray[21].circleCenter = whitePosition;
				ballArray[21].SetModelMatrix(Matrix4::Translation(ballArray[21].circleCenter));
				ballArray[21].potted = false;
			}
			if (r.gamestate == 0) //gamestate 0 is selecting where the whiteball is to be placed in the d section
			{
				pointer.SetModelMatrix(Matrix4::Translation(Vector3(0, 0, 1))); //Hide the pointer when moving/selecting where the white ball goes
				if (Keyboard::KeyDown(KEY_LEFT))//Key left, right, down and up are simple methods that allow the user to move the ball
				{
					if (ballArray[21].circleCenter.x > 0.292f)
					{
						ballArray[21].circleCenter += Vector3(-0.001, 0, 0);
						ballArray[21].SetModelMatrix(Matrix4::Translation(ballArray[21].circleCenter));
					}
				}
				if (Keyboard::KeyDown(KEY_RIGHT))
				{
					if (ballArray[21].circleCenter.x < 0.372f)
					{
						ballArray[21].circleCenter += Vector3(0.001, 0, 0);
						ballArray[21].SetModelMatrix(Matrix4::Translation(ballArray[21].circleCenter));
					}
				}
				if (Keyboard::KeyDown(KEY_UP))
				{
					if (ballArray[21].circleCenter.y < 0.075f)
					{
						ballArray[21].circleCenter += Vector3(0, 0.001, 0);
						ballArray[21].SetModelMatrix(Matrix4::Translation(ballArray[21].circleCenter));
					}
				}
				if (Keyboard::KeyDown(KEY_DOWN))
				{
					if (ballArray[21].circleCenter.y > -0.075f)
					{
						ballArray[21].circleCenter += Vector3(0, -0.001, 0);
						ballArray[21].SetModelMatrix(Matrix4::Translation(ballArray[21].circleCenter));
					}
				}
				if (Keyboard::KeyDown(KEY_SPACE))//Key space checks if the white ball is in a valid position to be placed before placing it.
				{
					if ((ballArray[21].circleCenter.x - semi.x) * (ballArray[21].circleCenter.x - semi.x) + (ballArray[21].circleCenter.y - semi.y) * (ballArray[21].circleCenter.y - semi.y) < semiRadius * semiRadius)
					{//Checks balls are in the semi circle
						if (ArrayCollisionCheck(ballArray)) //Checks the whiteball isnt coliding with another ball so it cant be placed in an illegal spot
						{
							cout << "Placed the whiteball. Please aim and take your shot." << endl;
							r.gamestate = 1;
							pointer.SetModelMatrix(Matrix4::Translation(ballArray[21].circleCenter)); //If everything passes, then move onto next gamestate and set the pointer to be where the whiteball is
						}
						else {
							cout << "You cannot place the whiteball inside of another ball." << endl;
						}
					}
					else {
						cout << "You cannot place the whiteball outside the semi-circle." << endl;
					}
				}
			}
			if (r.gamestate == 1)//Gamestate 1 is the state where the user can aim and select power to the ball then shoot the ball
			{
				//Left and right adjusts the power of the shot, and can be seen visually by the slider moving right and left
				if (Keyboard::KeyDown(KEY_LEFT))
				{
					if (widgetO.rectCenter.x > widgetCenter.x)
					{
						widgetO.rectCenter.x += -0.007f;
						widgetO.SetModelMatrix(Matrix4::Translation(widgetO.rectCenter));
					}
				}
				if (Keyboard::KeyDown(KEY_RIGHT))
				{
					if (widgetO.rectCenter.x < widgetCenter.x + powerBar.w)
					{
						widgetO.rectCenter.x += 0.007f;
						widgetO.SetModelMatrix(Matrix4::Translation(widgetO.rectCenter));
					}
				}
				//Key up and down rotates the line in the Z axis and allows the user to aim.
				if (Keyboard::KeyDown(KEY_UP))
				{
					Matrix4 rotate = Matrix4::Rotation(1.0f, Vector3(0, 0, 1));
					pointer.rotation += 1.0f;
					pointer.SetModelMatrix(pointer.GetModelMatrix() * rotate);
				}
				if (Keyboard::KeyDown(KEY_DOWN))
				{
					Matrix4 rotate = Matrix4::Rotation(-1.0f, Vector3(0, 0, 1));
					pointer.rotation += -1.0f;
					pointer.SetModelMatrix(pointer.GetModelMatrix() * rotate);
				}
				//The space is the shoot button, if power is applied it will give the whiteball initial velocity based on how much power
				if (Keyboard::KeyDown(KEY_SPACE))
				{
					float pwr = widgetO.rectCenter.x - widgetCenter.x;
					if (pwr > 0)
					{
						pwr *= 50; //Amplify the power, as it would be too weak without it

						Vector3 p = Vector3(0 - pointer.length, 0, 0); //p is the starting point of the end of the pointer line

						float th = DegToRad(pointer.rotation);
						float c = cos(th);
						float s = sin(th);
						float px = p.x * c - p.y * s;
						float py = p.x * s + p.y * c;
						p = Vector3(px, py, p.z);
						//use pointer's saved rotation to calculate the new position of pointer using cos and sin. I used : http://stackoverflow.com/questions/2259476/rotating-a-point-about-another-point-2d to help me create this.
						ballArray[21].v = p * pwr; //p is the direction, power is the amplifier. Give the whiteball a new initial velocity, the rest is all handled by renderer.cpp and renderobject.cpp
						widgetO.rectCenter = widgetCenter;//Reset the powerbar widget
						widgetO.SetModelMatrix(Matrix4::Translation(widgetO.rectCenter));

						r.gamestate = -1;//-1 == updating
					}
					else {
						cout << "You must apply power to the shot." << endl;
					}
				}
			}
		}
		else {
			pointer.SetModelMatrix(Matrix4::Translation(Vector3(0, 0, 1))); //Sets pointer to invisible when balls moving
		}
		if (Keyboard::KeyDown(KEY_X))
		{
			break;
		}
		r.UpdateScene(msec);
		r.ClearBuffers();
		r.RenderScene();
		r.SwapBuffers();
	}
	delete topMiddle;
	delete botMiddle;
	delete botRight;
	delete topRight;
	delete topLeft;
	delete botLeft;
	delete widgetMesh;
	delete powerMesh;
	delete m;
	delete s;
	delete redBall;
	delete blueBall;
	delete pinkBall;
	delete blackBall;
	delete brownBall;
	delete yellowBall;
	delete greenBall;
	delete whiteBall;
	delete c;
	delete tableShader;
	delete basic;
	delete line;
	delete leftmost;
	delete rightmost;
	delete rightbot;
	delete righttop;
	delete leftbot;
	delete lefttop;
}

