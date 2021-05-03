#include "NAGE.h"
#include <algorithm>

typedef NAGE::Vector3<float> vec3;	// Aliases
typedef NAGE::Matrix4<float> mat4;
typedef NAGE::Triangle<float> triangle;
typedef NAGE::Mesh<float> mesh;

int main()
{
	NAGE::Engine game(600, 600, 5, L"3D Test");	// Initialise Engine
	mat4 proj = NAGE::Projection(90.0f, (float)game.ScreenHeight() / (float)game.ScreenWidth(), 0.1f, 1000.0f);	// Set Up projection matrix
	float dt, theta = 0.0f;
	mesh cube;
	vec3 camera;

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

		std::vector<triangle> triangles_to_raster;

		for (auto& tri : cube.tris)
		{
			triangle triproj, tritrans;

			/*
			1) Apply 3d transformation to original vertices / triangle
			2) Normalise triangle to get direction
			3) Apply projection matrix to convert 3D -> 2D
			4) Scale normalised triangle to center of screen
			5) Draw triangle
			*/

			tritrans = (NAGE::RotateZ(theta) * NAGE::RotateX(theta * 0.5f) * tri + vec3{ 0.0f,0.0f,3.0f }).norm();
			
			vec3 normal, l1, l2;
			l1 = tritrans[1] - tritrans[0];
			l2 = tritrans[2] - tritrans[0];

			normal = cross(l1, l2).norm();

			if (dot(normal, (tritrans[0] - camera)) < 0.0f)
			{
				vec3 light_dir = { 0.0f, 0.0f, -1.0f };
				float dp = dot(normal, light_dir.norm());
				CHAR_INFO c = game.GetShade(dp);
				tritrans.col = c.Attributes;
				tritrans.sym = c.Char.UnicodeChar;

				triproj = proj * tritrans.norm();
				triproj.col = tritrans.col;
				triproj.sym = tritrans.sym;

				triproj = triproj + vec3{ 1.0f, 1.0f, 0.0f };
				triproj = triproj * vec3{ 0.5f * (float)game.ScreenWidth(), 0.5f * (float)game.ScreenHeight(), 1.0f };	// Z coordinates don't matter

				triangles_to_raster.push_back(triproj);
			}
		}

		std::sort(triangles_to_raster.begin(), triangles_to_raster.end(), [](triangle& t1, triangle& t2)
			{
				float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
				float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
				return z1 > z2;
			});

		for (auto& tri : triangles_to_raster)
			game.DrawTriangle(tri.p[0].x, tri.p[0].y,
							  tri.p[1].x, tri.p[1].y,
							  tri.p[2].x, tri.p[2].y,
							  tri.col, tri.sym);

		game.Update();
	}
	return 0;
}