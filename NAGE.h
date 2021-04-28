#pragma once
#pragma comment(lib, "winmm.lib")

#ifndef NAGE_H
#define NAGE_H

#include <windows.h>
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>

#include <thread>
#include <atomic>
#include <condition_variable>

enum COLOUR
{
	FG_BLACK =			0x0000,
	FG_DARK_BLUE =		0x0001,
	FG_DARK_GREEN =		0x0002,
	FG_DARK_CYAN =		0x0003,
	FG_DARK_RED =		0x0004,
	FG_DARK_MAGENTA =	0x0005,
	FG_DARK_YELLOW =	0x0006,
	FG_GREY =			0x0007, 
	FG_DARK_GREY =		0x0008,
	FG_BLUE =			0x0009,
	FG_GREEN =			0x000A,
	FG_CYAN =			0x000B,
	FG_RED =			0x000C,
	FG_MAGENTA =		0x000D,
	FG_YELLOW =			0x000E,
	FG_WHITE =			0x000F
};

enum PIXEL_TYPE
{
	PIXEL_SOLID =			0x2588,
	PIXEL_THREEQUARTERS =	0x2593,
	PIXEL_HALF =			0x2592,
	PIXEL_QUARTER =			0x2591
};

struct KeyState
{
	bool pressed;
	bool released;
};

class Engine
{
private:
	HANDLE console;
	HANDLE console_in;
	short key_new_state[256] = { 0 }, key_old_state[256] = { 0 };
	KeyState keys[256] = { 0 };
	SMALL_RECT rect_window;
	CHAR_INFO* screen_buffer;
	int screen_width, screen_height;
	std::wstring name;

	std::atomic<bool> loop_running;
	std::condition_variable game_finished;

	std::chrono::system_clock::time_point tp1;
	std::chrono::system_clock::time_point tp2;

	float dt = 0.0f;


public:
	Engine(int width, int height, int scale, std::wstring _name = L"Unnamed Application")
	{
		name = _name;

		screen_width = width / scale;
		screen_height = height / scale;

		console = GetStdHandle(STD_OUTPUT_HANDLE);
		console_in = GetStdHandle(STD_INPUT_HANDLE);

		rect_window = { 0, 0, 1, 1 };
		SetConsoleWindowInfo(console, TRUE, &rect_window);

		COORD coord = { (short)screen_width, (short)screen_height };

		SetConsoleScreenBufferSize(console, coord);
		SetConsoleActiveScreenBuffer(console);

		CONSOLE_FONT_INFOEX cfi;
		cfi.cbSize = sizeof(cfi);
		cfi.nFont = 0;
		cfi.dwFontSize.X = scale;
		cfi.dwFontSize.Y = scale;
		cfi.FontFamily = FF_DONTCARE;
		cfi.FontWeight = FW_NORMAL;

		wcscpy_s(cfi.FaceName, L"Consolas");

		SetCurrentConsoleFontEx(console, false, &cfi);

		rect_window = { 0, 0, (short)screen_width - 1, (short)screen_height - 1 };
		SetConsoleWindowInfo(console, TRUE, &rect_window);

		screen_buffer = new CHAR_INFO[screen_width * screen_height]{ 0 };
		
		tp1 = std::chrono::system_clock::now();
		tp2 = std::chrono::system_clock::now();

		loop_running = true;
		std::thread t = std::thread(&Engine::Update, this);
		t.join();
	}

	~Engine()
	{
		delete[] screen_buffer;
	}

	void Update()
	{
		if (loop_running)
		{
			tp2 = std::chrono::system_clock::now();
			std::chrono::duration<float> delta_time = tp2 - tp1;
			tp1 = tp2;
			dt = delta_time.count();

			wchar_t s[256];
			swprintf_s(s, 256, L"NAGE - %s - FPS: %3.2f", name.c_str(), 1.0f / dt);
			SetConsoleTitle(s);
			WriteConsoleOutput(console, screen_buffer, { (short)screen_width, (short)screen_height }, { 0,0 }, &rect_window);
		}
	}

	virtual void Draw(int x, int y, short c = PIXEL_SOLID, short col = FG_WHITE)
	{
		screen_buffer[y * screen_width + x].Char.UnicodeChar = c;
		screen_buffer[y * screen_width + x].Attributes = col;
	}

	void Fill(int x1, int y1, int x2, int y2, short c = PIXEL_SOLID, short col = FG_WHITE)
	{

		for (int x = x1; x < x2; x++)
			for (int y = y1; y < y2; y++)
				Draw(x, y, c, col);
	}

	void Clear(short c = PIXEL_SOLID, short col = FG_BLACK)
	{
		Fill(0, 0, screen_width, screen_height, c, col);
	}

	void Clip(int& x, int& y)
	{
		if (x < 0) x = 0;
		if (x >= screen_width) x = screen_width;
		if (y < 0) y = 0;
		if (y >= screen_height) y = screen_height;
	}

	void DrawString(int x, int y, std::wstring c, short col = FG_BLACK, bool alpha = 0)
	{
		for (size_t i = 0; i < c.size(); i++)
		{
			if (alpha && c[i] == L' ') continue;
			else
			{
				screen_buffer[y * screen_width + x + i].Char.UnicodeChar = c[i];
				screen_buffer[y * screen_width + x + i].Attributes = col;
			}
		}
	}

	float GetDt() { return dt; }
	std::wstring GetName() { return name; }

	int ScreenWidth() { return screen_width; }
	int ScreenHeight() { return screen_height; }


};



#endif // !NAGE_H
