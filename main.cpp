#include "NAGE.h"
#include <iostream>

int main()
{
	Engine game(800, 600, 10);

	bool looping = true;
	while (looping)
	{
		for (int i = 0; i < game.ScreenWidth(); i++)
			for (int j = 0; j < game.ScreenHeight(); j++)
				game.Draw(i, j, PIXEL_SOLID, rand() % 16);

		game.Update();
	}

	return 0;
}