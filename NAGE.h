/*
Welcome to not_a_hill's

N A G E
o   a n
t   m g
	e i
	  n
	  e

*/


#pragma once
#pragma comment(lib, "winmm.lib")

#ifndef NAGE_H
#define NAGE_H

#include <windows.h>
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <cmath>

#include <thread>
#include <atomic>
#include <condition_variable>

#define PI 3.14159
#define CONCAT(x, y) x##y

enum COLOUR
{
	BLACK = 0x0000,
	DARK_BLUE = 0x0001,
	DARK_GREEN = 0x0002,
	DARK_CYAN = 0x0003,
	DARK_RED = 0x0004,
	DARK_MAGENTA = 0x0005,
	DARK_YELLOW = 0x0006,
	GREY = 0x0007,
	DARK_GREY = 0x0008,
	BLUE = 0x0009,
	GREEN = 0x000A,
	CYAN = 0x000B,
	RED = 0x000C,
	MAGENTA = 0x000D,
	YELLOW = 0x000E,
	WHITE = 0x000F
};

enum PIXEL_TYPE
{
	PIXEL_SOLID = 0x2588,
	PIXEL_THREEQUARTERS = 0x2593,
	PIXEL_HALF = 0x2592,
	PIXEL_QUARTER = 0x2591
};

struct KeyState
{
	bool pressed;
	bool released;
	bool held;
};

template<typename T>
struct Vector2
{
private:
	int w = 1;

public:
	T x = 0;
	T y = 0;

	Vector2() : x(0), y(0), w(1) {}
	Vector2(T _x, T _y) : x(_x), y(_y), w(1) {}
	Vector2(const Vector2& vec) : x(vec.x), y(vec.y), w(1) {}
	Vector2& operator= (const Vector2& vec) = default;

	Vector2 operator+(const Vector2& rhs) const { return Vector2(this->x + rhs.x, this->y + rhs.y); }
	Vector2 operator-(const Vector2& rhs) const { return Vector2(this->x - rhs.x, this->y - rhs.y); }

	Vector2 operator*(const T& rhs) const { return Vector2(this->x * rhs, this->y * rhs); }
	Vector2 operator/(const T& rhs) const { return Vector2(this->x / rhs, this->y / rhs); }

	Vector2& operator+=(const Vector2& rhs) const { this->x + rhs.x; this->y + rhs.y; return *this; }
	Vector2& operator-=(const Vector2& rhs) const { this->x - rhs.x; this->y - rhs.y; return *this; }

	Vector2& operator*=(const T& rhs) const { this->x* rhs; this->y* rhs; return *this; }
	Vector2& operator/=(const T& rhs) const { this->x / rhs; this->y / rhs; return *this; }

	Vector2 operator+() const { return { +x, +y }; }
	Vector2 operator-() const { return { -x, -y }; }
};

template<typename T>
struct Matrix3
{
	T matrix[3][3];
	Matrix3() { Reset(); }
	Matrix3(const Matrix3& mat)
	{
		for (int c = 0; c < 3; c++)
			for (int r = 0; r < 3; r++)
				this->matrix[c][r] = mat.matrix[c][r];
	}
	Matrix3& operator=(const Matrix3& mat) = default;
	T& operator()(const int c, const int r) const { return this->matrix[c][r]; }

	void Reset()
	{
		matrix[0][0] = 1.0f; matrix[0][1] = 0.0f; matrix[0][2] = 0.0f;
		matrix[1][0] = 0.0f; matrix[1][1] = 1.0f; matrix[1][2] = 0.0f;
		matrix[2][0] = 0.0f; matrix[2][1] = 0.0f; matrix[2][2] = 1.0f;
	}

	Matrix3 operator*(const Matrix3& mat) const
	{
		Matrix3 result;
		for (int c = 0; c < 3; c++)
			for (int r = 0; r < 3; r++)
				result.matrix[c][r] = this->matrix[0][r] * mat.matrix[c][0] + this->matrix[1][r] * mat.matrix[c][1] + this->matrix[2][r] * mat.matrix[c][2];
		return result;
	}

	Matrix3 operator*(const T& scalar) const
	{
		Matrix3 result;
		for (int c = 0; c < 3; c++)
			for (int r = 0; r < 3; r++)
				result[c][r] *= scalar;
		return result;
	}

	Vector2<T> operator*(const Vector2<T>& rhs) const
	{
		Vector2<T> result = rhs;
		for (int c = 0; c < 3; c++)
			for (int r = 0; r < 3; r++)
				result[c] += rhs[r] * matrix[c][r];
		return result;
	}

	Matrix3& operator*=(const Matrix3& mat) const
	{
		for (int c = 0; c < 3; c++)
			for (int r = 0; r < 3; r++)
				this->matrix[c][r] = this->matrix[0][r] * mat.matrix[c][0] + this->matrix[1][r] * mat.matrix[c][1] + this->matrix[2][r] * mat.matrix[c][2];
		return *this;
	}


	Matrix3 Transpose() const
	{
		Matrix3 result = this;
		for (int c = 0; c < 3; c++)
			for (int r = 0; r < 3; r++)
				this->matrix[r][c] = result.matrix[c][r];
		return this;
	}

	Matrix3 Cofactors() const
	{
		matrix[0][1] *= -1;
		matrix[1][0] *= -1;
		matrix[1][2] *= -1;
		matrix[2][1] *= -1;
		return this;
	}

	Matrix3 Minors() const
	{
		Matrix3 result;
		for (int c = 0; c < 3; c++)
			for (int r = 0; r < 3; r++)
				this->matrix[c][r] = this->matrix[(c + 1) % 3][(r + 1) % 3] * this->matrix[(c + 2) % 3][(r + 2) % 3] - this->matrix[(c + 1) % 3][(r + 2) % 3] * this->matrix[(c + 2) % 3][(r + 1) % 3];
		return this;
	}

	Matrix3 Invert() const
	{
		float det = this->Determinant();
		if (det != 0)
		{
			this->matrix = this->Minors();
			this->matrix = this->Cofactors();
			this->matrix = this->Transpose();

			return this * (1.0f / det);
		}
	}

	float Determinant() const
	{
		float sum = 0;
		for (int c = 0; c < 3; c++)
			sum += (this->matrix[0][c] * (c == 1 ? -1 : 1) * (this->matrix[1][(c + 1) % 3] * this->matrix[2][(c + 2) % 3] - this->matrix[2][(c + 1) % 3] * this->matrix[1][(c + 2) % 3]));
		return this;
	}

	Matrix3 Rotate(const float theta) const
	{
		Matrix3 result;

		result.matrix[0][0] = cosf(theta);	result.matrix[0][1] = sinf(theta);	result.matrix[0][2] = 0.0f;
		result.matrix[1][0] = -sinf(theta);	result.matrix[1][1] = cosf(theta);	result.matrix[1][2] = 0.0f;
		result.matrix[2][0] = 0.0f;			result.matrix[2][1] = 0.0f;			result.matrix[2][2] = 1.0f;

		return result * this;
	}

	Matrix3 Translate(const Vector2<float>& vec) { return Translate(vec.x, vec.y); }
	Matrix3 Translate(const float ox, const float oy) const
	{
		Matrix3 result;

		result.matrix[0][0] = 1.0f;	result.matrix[0][1] = 0.0f;	result.matrix[0][2] = ox;
		result.matrix[1][0] = 0.0f;	result.matrix[1][1] = 1.0f;	result.matrix[1][2] = oy;
		result.matrix[2][0] = 0.0f;	result.matrix[2][1] = 0.0f;	result.matrix[2][2] = 1.0f;

		return result * this;
	}

	Matrix3 Shear(const Vector2<float>& vec) { return Shear(vec.x, vec.y); }
	Matrix3 Shear(const float ox, const float oy) const
	{
		Matrix3 result;
		result.matrix[0][0] = 1.0f;	result.matrix[0][1] = ox;		result.matrix[0][2] = 0.0f;
		result.matrix[1][0] = oy;		result.matrix[1][1] = 1.0f;	result.matrix[1][2] = 0.0f;
		result.matrix[2][0] = 0.0f;	result.matrix[2][1] = 0.0f;	result.matrix[2][2] = 1.0f;

		return result * this;
	}

	Matrix3 Scale(const Vector2<float>& vec) { return Scale(vec.x, vec.y); }
	Matrix3 Scale(const float ox, const float oy) const
	{
		Matrix3 result;
		result.matrix[0][0] = ox;	result.matrix[0][1] = 0.0f;	result.matrix[0][2] = 0.0f;
		result.matrix[1][0] = 0.0f;	result.matrix[1][1] = oy;	result.matrix[1][2] = 0.0f;
		result.matrix[2][0] = 0.0f;	result.matrix[2][1] = 0.0f;	result.matrix[2][2] = 1.0f;

		return result * this;
	}
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
	Engine(int width, int height, int pixel_size, std::wstring _name = L"Unnamed")
	{
		name = _name;

		screen_width = width / pixel_size;
		screen_height = height / pixel_size;

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
		cfi.dwFontSize.X = pixel_size;
		cfi.dwFontSize.Y = pixel_size;
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

			for (int i = 0; i < 256; i++)
			{
				key_new_state[i] = GetAsyncKeyState(i);
				keys[i].pressed = keys[i].released = false;

				if (key_new_state[i] != key_old_state[i])
				{
					if (key_new_state[i] & 0x8000)
					{
						keys[i].pressed = !keys[i].held;
						keys[i].held = true;
					}
					else
					{
						keys[i].released = false;
						keys[i].held = false;
					}
				}
				key_old_state[i] = key_new_state[i];
			}

			wchar_t s[256];
			swprintf_s(s, 256, L"NAGE - %s - FPS: %3.2f", name.c_str(), 1.0f / dt);
			SetConsoleTitle(s);
			WriteConsoleOutput(console, screen_buffer, { (short)screen_width, (short)screen_height }, { 0,0 }, &rect_window);
		}
		else
		{
			std::cout << "Error - NAGE not initialised\n";
		}
	}

	virtual void Draw(const Vector2<int>& vec1, short c = PIXEL_SOLID, short col = WHITE)
	{
		Draw(vec1.x, vec1.y, c, col);
	}

	virtual void Draw(int x, int y, short c = PIXEL_SOLID, short col = WHITE)
	{
		Clip(x, y);
		screen_buffer[y * screen_width + x].Char.UnicodeChar = c;
		screen_buffer[y * screen_width + x].Attributes = col;
	}

	void Fill(const Vector2<int>& vec1, const Vector2<int>& vec2, short c = PIXEL_SOLID, short col = WHITE)
	{
		Fill(vec1.x, vec1.y, vec2.x, vec2.y, c, col);
	}

	void Fill(int x1, int y1, int x2, int y2, short c = PIXEL_SOLID, short col = WHITE)
	{
		Clip(x1, y1);
		Clip(x2, y2);

		for (int x = x1; x < x2; x++)
			for (int y = y1; y < y2; y++)
				Draw(x, y, c, col);
	}

	void Clear(short col = BLACK)
	{
		Fill(0, 0, screen_width, screen_height, PIXEL_SOLID, col);
	}

	void Clip(Vector2<int>& vec) { Clip(vec.x, vec.y); }

	void Clip(int& x, int& y)
	{
		if (x < 0) x = 0;
		if (x >= screen_width) x = screen_width;
		if (y < 0) y = 0;
		if (y >= screen_height) y = screen_height;
	}

	void DrawString(const Vector2<int>& vec1, std::wstring c, short col = WHITE, bool alpha = 0)
	{
		DrawString(vec1.x, vec1.y, c, col, alpha);
	}

	void DrawString(int x, int y, std::wstring c, short col = WHITE, bool alpha = 0)
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

	void DrawLine(const Vector2<int>& vec1, const Vector2<int>& vec2, short c = PIXEL_SOLID, short col = WHITE, int width = 1)
	{
		DrawLine(vec1.x, vec1.y, vec2.x, vec2.y, c, col, width);
	}

	void DrawLine(int x1, int y1, int x2, int y2, short c = PIXEL_SOLID, short col = WHITE, int width = 1)
	{	//  DDA
		int dx, dy, steps;
		float x, y, x_inc, y_inc;

		for (int j = 0; j < width; j++)
		{

			dx = x2 - x1;
			dy = y2 - y1;

			steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

			x_inc = (float)dx / (float)steps;
			y_inc = (float)dy / (float)steps;

			x = x1 + j;
			y = y1 + j;

			for (int i = 0; i <= steps; i++)
			{
				Draw((int)std::roundf(x), (int)std::roundf(y), c, col);
				x += x_inc;
				y += y_inc;
			}
		}

		/* Bresenhams
		int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
		dx = x2 - x1; dy = y2 - y1;
		dx1 = abs(dx); dy1 = abs(dy);
		px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;
		if (dy1 <= dx1)
		{
			if (dx >= 0)
				{ x = x1; y = y1; xe = x2; }
			else
				{ x = x2; y = y2; xe = x1;}

			Draw(x, y, c, col);

			for (i = 0; x<xe; i++)
			{
				x = x + 1;
				if (px<0)
					px = px + 2 * dy1;
				else
				{
					if ((dx<0 && dy<0) || (dx>0 && dy>0)) y = y + 1; else y = y - 1;
					px = px + 2 * (dy1 - dx1);
				}
				Draw(x, y, c, col);
			}
		}
		else
		{
			if (dy >= 0)
				{ x = x1; y = y1; ye = y2; }
			else
				{ x = x2; y = y2; ye = y1; }

			Draw(x, y, c, col);

			for (i = 0; y<ye; i++)
			{
				y = y + 1;
				if (py <= 0)
					py = py + 2 * dx1;
				else
				{
					if ((dx<0 && dy<0) || (dx>0 && dy>0)) x = x + 1; else x = x - 1;
					py = py + 2 * (dx1 - dy1);
				}
				Draw(x, y, c, col);
			}
		}
		*/
	}

	float GetDt() { return dt; }
	std::wstring GetName() { return name; }

	int ScreenWidth() { return screen_width; }
	int ScreenHeight() { return screen_height; }

	KeyState Key(int key_id) { return keys[key_id]; }
};

#endif // !NAGE_H