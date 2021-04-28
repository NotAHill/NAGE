#include "NAGE.h"
#include <string>

int main()
{
	Engine game(800, 600, 20, L"Test");

	float theta = 0.0f;
	
	int x1 = 10, y1 = 10;
	int x2 = 5, y2 = 5;

	float r = std::sqrtf((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));

	bool looping = true;
	while (looping)
	{
		game.Clear(BLUE);

		if (game.Key(VK_UP).held)
		{
			theta += 1.0f * game.GetDt();
			game.DrawString(0, 0, L"Key: Up Pressed");
		}

		if (game.Key(VK_DOWN).held)
		{
			theta -= 1.0f * game.GetDt();
			game.DrawString(0, 0, L"Key: Down Pressed");
		}

		

		game.DrawLine(x1, y1, (int)(x1 + cosf(theta) * r), (int)(y1 + sinf(theta) * r), PIXEL_SOLID, RED);

		game.Update();
	}

	return 0;
}