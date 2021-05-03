#include "NAGE.h"

typedef NAGE::Vector3<float> vec3;
typedef NAGE::Matrix4<float> mat4;
typedef NAGE::Triangle<float> triangle;
typedef NAGE::Mesh<float> mesh;

int main()
{
	NAGE::Engine game(600, 600, 5, L"3D Test");	// Initialise Engine
	mat4 proj = NAGE::Projection(90.0f, (float)game.ScreenHeight() / (float)game.ScreenWidth(), 0.1f, 1000.0f);	// Set Up projection matrix
	float dt, theta = 0.0f;
	mesh cube;	

	cube.tris = {	// Vertices of cube
		{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },                                                    
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },                                                    
		{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },                                                      
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },                                                
		{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },                                                
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },
	};

	while (!game.Key(VK_ESCAPE).pressed)	// Main loop
	{
		game.Clear(BLACK);
		dt = game.GetDt();
		theta += 1.0f * dt;	

		for (auto& tri : cube.tris)
		{
			triangle triproj;

			/*
			1) Apply 3d transformation to original vertices / triangle
			2) Normalise triangle to get direction
			3) Apply projection matrix to convert 3D -> 2D
			4) Scale normalised triangle to center of screen
			5) Draw triangle
			*/

			triproj = proj * (NAGE::RotateZ(theta) * NAGE::RotateX(theta * 0.5f) * tri + vec3{ 0.0f,0.0f,3.0f }).norm();
			
			triproj = triproj + vec3{ 1.0f, 1.0f, 0.0f };
			triproj = triproj * vec3{ 0.5f * (float)game.ScreenWidth(), 0.5f * (float)game.ScreenHeight(), 1.0f };

			game.DrawTriangle(triproj);
		}

		game.Update();
	}


	return 0;
}