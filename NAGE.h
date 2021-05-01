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
	SOLID = 0x2588,
	THREEQUARTERS = 0x2593,
	HALF = 0x2592,
	QUARTER = 0x2591
};

struct KeyState
{
	bool pressed;
	bool released;
	bool held;
};

namespace NAGE
{

	template<typename T>
	struct Vector2
	{
		union
		{
			T xy[3];
			struct
			{
				T x, y, w;
			};
		};

		Vector2() : x(0), y(0), w(1.f) {}
		Vector2(T _x, T _y) : x(_x), y(_y), w(1.f) {}
		Vector2(const Vector2& vec) : x(vec.x), y(vec.y), w(1.f) {}
		Vector2& operator= (const Vector2& vec) = default;
		T operator[] (const size_t& sz) const { return this->xy[sz]; }

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

		T mag() { return T(std::sqrt(x * x + y * y)); }
		T mag2() { return x * x + y * y; }

		T dot(const Vector2& rhs) { return this->x * rhs.x + this->y * rhs.y; }
		T cross(const Vector2& rhs) { return this->x * rhs.y - this->y * rhs.x; }

		Vector2 norm() { T r = 1 / mag(); return Vector2(x * r, y * r); }

		std::string str() { return "(" + std::to_string(x) + ", " + std::to_string(y) + ")"; }
	};

	template<typename T>
	struct Vector3
	{
		union
		{
			T xyz[4];
			struct
			{
				T x, y, z, w;
			};
		};

		Vector3() : x(0), y(0), z(0), z(1.f) {}
		Vector3(T _x, T _y, T _z) : x(_x), y(_y), z(_z), w(1.f) {}
		Vector3(const Vector3& vec) : x(vec.x), y(vec.y), z(vec.z), w(1.f) {}
		Vector3& operator= (const Vector3& vec) = default;
		T operator[](const size_t& sz) const { return this->xyz[sz]; }

		Vector3 operator+(const Vector3& rhs) const { return Vector3(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z); }
		Vector3 operator-(const Vector3& rhs) const { return Vector3(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z); }

		Vector3 operator*(const T& rhs) const { return Vector3(this->x * rhs, this->y * rhs, this->z * rhs); }
		Vector3 operator/(const T& rhs) const { return Vector3(this->x / rhs, this->y / rhs, this->z / rhs); }

		Vector3& operator+=(const Vector3& rhs) const { this->x + rhs.x; this->y + rhs.y; this->z + rhs.z; return *this; }
		Vector3& operator-=(const Vector3& rhs) const { this->x - rhs.x; this->y - rhs.y; this->z - rhs.z; return *this; }

		Vector3& operator*=(const T& rhs) const { this->x* rhs; this->y* rhs; this->z* rhs; return *this; }
		Vector3& operator/=(const T& rhs) const { this->x / rhs; this->y / rhs; this->z / rhs; return *this; }

		Vector3 operator+() const { return { +x, +y, +z }; }
		Vector3 operator-() const { return { -x, -y, -z }; }

		T mag() { return T(std::sqrt(x*x + y*y + z*z)); }
		T mag2() { return x*x + y*y + z*z;}

		T dot(const Vector3& rhs) { return this->x * rhs.x + this->y * rhs.y + this->z * rhs.z; }
		Vector3 cross(const Vector3& rhs) { return Vector3(this->y*rhs.z - this->z*rhs.y, this->z * rhs.x - this->x * rhs.z, this->x*rhs.y - this->y*rhs.z); }

		Vector3 norm() { T r = 1 / mag(); return Vector3(x * r, y * r, z * r); }

		std::string str() { return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")"; }
	
		friend Vector3 plane_intersection(Vector3<float>& plane_pos, Vector3<float>& plane_normal, Vector3<float>& line_start, Vector3<float>& line_end)
		{
			plane_normal = plane_normal.norm();
			float plane_d = -plane_normal.dot(plane_pos);
			float ad = line_start.dot(plane_normal);
			float bd = line_end.dot(plane_normal);
			float t = (-plane_d - ad) / (bd - ad);
			Vector3<float> start_to_end = line_end - line_start;
			Vector3<float> intersection_line = start_to_end * t;
			return line_start + intersection_line;
		}
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
		T operator()(const int c, const int r) { return matrix[c][r]; }

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

		Vector3<T> operator*(const Vector3<T>& rhs) const
		{
			Vector3<T> result = rhs;
			for (int c = 0; c < 3; c++)
				for (int r = 0; r < 3; r++)
					result[c] += rhs[r] * matrix[c][r];
			return result;
		}

		Matrix3& operator*=(const Matrix3& mat) 
		{
			for (int c = 0; c < 3; c++)
				for (int r = 0; r < 3; r++)
					this->matrix[c][r] = this->matrix[0][r] * mat.matrix[c][0] + this->matrix[1][r] * mat.matrix[c][1] + this->matrix[2][r] * mat.matrix[c][2];
			return *this;
		}

		Matrix3& operator*=(const T& scalar)
		{
			for (int c = 0; c < 3; c++)
				for (int r = 0; r < 3; r++)
					this->matrix[c][r] *= scalar;
			return *this;
		}


		Matrix3 Transpose() const
		{
			Matrix3 result = *this;
			for (int c = 0; c < 3; c++)
				for (int r = 0; r < 3; r++)
					result.matrix[c][r] = this->matrix[r][c];
			return result;
		}

		Matrix3 Cofactors() const
		{
			Matrix3 result = *this;
			result.matrix[0][1] *= -1;
			result.matrix[1][0] *= -1;
			result.matrix[1][2] *= -1;
			result.matrix[2][1] *= -1;
			return result;
		}

		Matrix3 Minors() const
		{
			Matrix3 result;
			for (int c = 0; c < 3; c++)
				for (int r = 0; r < 3; r++)
					result.matrix[c][r] = (this->matrix[(c + 1) % 3][(r + 1) % 3] * this->matrix[(c + 2) % 3][(r + 2) % 3] - this->matrix[(c + 1) % 3][(r + 2) % 3] * this->matrix[(c + 2) % 3][(r + 1) % 3]);
			return result.Cofactors();
		}

		Matrix3 Invert() const
		{
			Matrix3 result;
			float det = this->Determinant();
			if (det != 0)
			{
				result = this->Minors();
				result = result.Cofactors();
				result = result.Transpose();

				result *= 1.0f / det;

				return result;
			}
		}

		float Determinant() const
		{
			float sum = 0.f;
			for (int c = 0; c < 3; c++)
				sum += (this->matrix[0][c] * (this->matrix[1][(c + 1) % 3] * this->matrix[2][(c + 2) % 3] - this->matrix[2][(c + 1) % 3] * this->matrix[1][(c + 2) % 3]));
			return sum;
		}

		Matrix3 Rotate(const float theta) const
		{
			Matrix3 result;

			result.matrix[0][0] = cosf(theta);	result.matrix[0][1] = sinf(theta);	result.matrix[0][2] = 0.0f;
			result.matrix[1][0] = -sinf(theta);	result.matrix[1][1] = cosf(theta);	result.matrix[1][2] = 0.0f;
			result.matrix[2][0] = 0.0f;			result.matrix[2][1] = 0.0f;			result.matrix[2][2] = 1.0f;

			return result * this;
		}

		Matrix3 Translate(const Vector3<float>& vec) { return Translate(vec.x, vec.y); }
		Matrix3 Translate(const float ox, const float oy) const
		{
			Matrix3 result;

			result.matrix[0][0] = 1.0f;	result.matrix[0][1] = 0.0f;	result.matrix[0][2] = ox;
			result.matrix[1][0] = 0.0f;	result.matrix[1][1] = 1.0f;	result.matrix[1][2] = oy;
			result.matrix[2][0] = 0.0f;	result.matrix[2][1] = 0.0f;	result.matrix[2][2] = 1.0f;

			return result * this;
		}

		Matrix3 Shear(const Vector3<float>& vec) { return Shear(vec.x, vec.y); }
		Matrix3 Shear(const float ox, const float oy) const
		{
			Matrix3 result;
			result.matrix[0][0] = 1.0f;	result.matrix[0][1] = ox;		result.matrix[0][2] = 0.0f;
			result.matrix[1][0] = oy;		result.matrix[1][1] = 1.0f;	result.matrix[1][2] = 0.0f;
			result.matrix[2][0] = 0.0f;	result.matrix[2][1] = 0.0f;	result.matrix[2][2] = 1.0f;

			return result * this;
		}

		Matrix3 Scale(const Vector3<float>& vec) { return Scale(vec.x, vec.y); }
		Matrix3 Scale(const float ox, const float oy) const
		{
			Matrix3 result;
			result.matrix[0][0] = ox;	result.matrix[0][1] = 0.0f;	result.matrix[0][2] = 0.0f;
			result.matrix[1][0] = 0.0f;	result.matrix[1][1] = oy;	result.matrix[1][2] = 0.0f;
			result.matrix[2][0] = 0.0f;	result.matrix[2][1] = 0.0f;	result.matrix[2][2] = 1.0f;

			return result * this;
		}

		std::string str() const
		{
			std::string str = "";
			for (int c = 0; c < 3; c++)
			{
				str += "[ ";
				for (int r = 0; r < 3; r++)
				{
					str += std::to_string(this->matrix[c][r]) + " ";
				}
				str += " ]\n";
			}
			return str;
		}
	};

	template<typename T>
	struct Matrix4
	{
		T matrix[4][4];
		Matrix4() { Reset(); }
		Matrix4(const Matrix4& mat)
		{
			for (int c = 0; c < 4; c++)
				for (int r = 0; r < 4; r++)
					this->matrix[c][r] = mat.matrix[c][r];
		}
		Matrix4& operator=(const Matrix4& mat) = default;
		T operator()(const int c, const int r) { return matrix[c][r]; }

		void Reset()
		{
			matrix[0][0] = 1.0f; matrix[0][1] = 0.0f; matrix[0][2] = 0.0f; matrix[0][3] = 0.0f;
			matrix[1][0] = 0.0f; matrix[1][1] = 1.0f; matrix[1][2] = 0.0f; matrix[1][3] = 0.0f;
			matrix[2][0] = 0.0f; matrix[2][1] = 0.0f; matrix[2][2] = 1.0f; matrix[2][3] = 0.0f;
			matrix[3][0] = 0.0f; matrix[3][1] = 0.0f; matrix[3][2] = 0.0f; matrix[3][3] = 1.0f;
		}

		Matrix4 operator*(const Matrix4& mat) const
		{
			Matrix4 result;
			for (int c = 0; c < 4; c++)
				for (int r = 0; r < 4; r++)
					result.matrix[c][r] = this->matrix[0][r] * mat.matrix[c][0] + this->matrix[1][r] * mat.matrix[c][1] + this->matrix[2][r] * mat.matrix[c][2];
			return result;
		}

		Matrix4 operator*(const T& scalar) const
		{
			Matrix4 result;
			for (int c = 0; c < 4; c++)
				for (int r = 0; r < 4; r++)
					result[c][r] *= scalar;
			return result;
		}

		Vector3<T> operator*(const Vector3<T>& rhs) const
		{
			Vector3<T> result = rhs;
			for (int c = 0; c < 4; c++)
				for (int r = 0; r < 4; r++)
					result[c] += rhs[r] * matrix[c][r];
			return result;
		}

		Matrix4& operator*=(const Matrix4& mat)
		{
			for (int c = 0; c < 4; c++)
				for (int r = 0; r < 4; r++)
					this->matrix[c][r] = this->matrix[0][r] * mat.matrix[c][0] + this->matrix[1][r] * mat.matrix[c][1] + this->matrix[2][r] * mat.matrix[c][2];
			return *this;
		}

		Matrix4& operator*=(const T& scalar)
		{
			for (int c = 0; c < 4; c++)
				for (int r = 0; r < 4; r++)
					this->matrix[c][r] *= scalar;
			return *this;
		}


		Matrix4 Transpose() const
		{
			Matrix4 result = *this;
			for (int c = 0; c < 4; c++)
				for (int r = 0; r < 4; r++)
					result.matrix[c][r] = this->matrix[r][c];
			return result;
		}

		Matrix4 RotateX(const float theta) const
		{
			Matrix4 result;
			result.matrix[0][0] = 1.0f;
			result.matrix[1][1] = cosf(theta);
			result.matrix[1][2] = sinf(theta);
			result.matrix[2][1] = -sinf(theta);
			result.matrix[2][2] = cosf(theta);
			result.matrix[3][3] = 1.0f;
			return *this * result;
		}

		Matrix4 RotateY(const float theta) const
		{
			Matrix4 result;
			result.matrix[0][0] = cosf(theta);
			result.matrix[0][2] = sinf(theta);
			result.matrix[2][0] = -sinf(theta);
			result.matrix[1][1] = 1.0f;
			result.matrix[2][2] = cosf(theta);
			result.matrix[3][3] = 1.0f;
			return *this * result;
		}

		Matrix4 RotateZ(const float theta) const
		{
			Matrix4 result;
			result.matrix[0][0] = cosf(theta);
			result.matrix[0][1] = sinf(theta);
			result.matrix[1][0] = -sinf(theta);
			result.matrix[1][1] = cosf(theta);
			result.matrix[2][2] = 1.0f;
			result.matrix[3][3] = 1.0f;
			return *this * result;
		}

		Matrix4 Translate(const Vector3<float>& vec) { return Translate(vec.x, vec.y, vec.z); }
		Matrix4 Translate(const float ox, const float oy, const float oz) const
		{
			Matrix4 result;
			result.matrix[0][0] = 1.0f;
			result.matrix[1][1] = 1.0f;
			result.matrix[2][2] = 1.0f;
			result.matrix[3][3] = 1.0f;
			result.matrix[3][0] = ox;
			result.matrix[3][1] = oy;
			result.matrix[3][2] = oz;
			return *this * result;
		}

		friend Matrix4 Projection(float fov_deg, float aspect_ratio, float z_near, float z_far)
		{
			float fov_rad = 1.0f / tanf(fov_deg * 0.5f / 180.0f * 3.14159f);
			Matrix4 result;
			result.matrix[0][0] = aspect_ratio * fov_rad;
			result.matrix[1][1] = fov_rad;
			result.matrix[2][2] = z_far / (z_far - z_near);
			result.matrix[3][2] = (-z_far * z_near) / (z_far - z_near);
			result.matrix[2][3] = 1.0f;
			result.matrix[3][3] = 0.0f;
			return result;
		}

		friend Matrix4 PointAt(Vector3<float>& pos, Vector3<float>& target, Vector3<float>& up)
		{
			Vector3<float> new_forward = target - pos;
			new_forward = new_forward.norm();
			Vector3<float> new_up = up - (new_forward * up.dot(new_forward));
			new_up = new_up.norm();
			Vector3<float> new_right = new_up.cross(new_forward);

			Matrix4 result;
			result.matrix[0][0] = new_right.x;	result.matrix[0][1] = new_right.y;	result.matrix[0][2] = new_right.z;	result.matrix[0][3] = 0.0f;
			result.matrix[1][0] = new_up.x;		result.matrix[1][1] = new_up.y;		result.matrix[1][2] = new_up.z;		result.matrix[1][3] = 0.0f;
			result.matrix[2][0] = new_forward.x;result.matrix[2][1] = new_forward.y;result.matrix[2][2] = new_forward.z;result.matrix[2][3] = 0.0f;
			result.matrix[3][0] = pos.x;		result.matrix[3][1] = pos.y;		result.matrix[3][2] = pos.z;		result.matrix[3][3] = 1.0f;
			return *this * result;
		}

		// Only for Rotation/Translation Matrices
		Matrix4 Invert() const 
		{
			Matrix4 result;
			result.matrix[0][0] = this->matrix[0][0];	result.matrix[0][1] = this->matrix[1][0]; result.matrix[0][2] = this->matrix[2][0]; result.matrix[0][3] = 0.0f;
			result.matrix[1][0] = this->matrix[0][1];	result.matrix[1][1] = this->matrix[1][1]; result.matrix[1][2] = this->matrix[2][1]; result.matrix[1][3] = 0.0f;
			result.matrix[2][0] = this->matrix[0][2];	result.matrix[2][1] = this->matrix[1][2]; result.matrix[2][2] = this->matrix[2][2]; result.matrix[2][3] = 0.0f;
			result.matrix[3][0] = -(this->matrix[3][0] * result.matrix[0][0] + this->matrix[3][1] * result.matrix[1][0] + this->matrix[3][2] * result.matrix[2][0]);
			result.matrix[3][1] = -(this->matrix[3][0] * result.matrix[0][1] + this->matrix[3][1] * result.matrix[1][1] + this->matrix[3][2] * result.matrix[2][1]);
			result.matrix[3][2] = -(this->matrix[3][0] * result.matrix[0][2] + this->matrix[3][1] * result.matrix[1][2] + this->matrix[3][2] * result.matrix[2][2]);
			result.matrix[3][3] = 1.0f;
			return result;
		}

		Matrix4 ShearX(const float oy, const float oz) const
		{
			Matrix4 result;
			result.matrix[1][0] = oy;
			result.matrix[2][0] = oz;
			return *this * result;
		}

		Matrix4 ShearY(const float ox, const float oz) const
		{
			Matrix4 result; 
			result.matrix[0][1] = ox;
			result.matrix[2][1] = oz;
			return *this * result;
		}

		Matrix4 ShearZ(const float ox, const float oy) const
		{
			Matrix4 result;
			result.matrix[0][2] = ox;
			result.matrix[1][2] = oy;
			return *this * result;
		}


		Matrix4 Scale(const Vector3<float>& vec) { return Scale(vec.x, vec.y, vec.z); }
		Matrix4 Scale(const float ox, const float oy, const float oz) const
		{
			Matrix4 result;
			result.matrix[0][0] = ox;
			result.matrix[1][1] = oy;
			result.matrix[2][2] = oz;
			return *this * result;
		}

		std::string str() const
		{
			std::string str = "";
			for (int c = 0; c < 4; c++)
			{
				str += "[ ";
				for (int r = 0; r < 4; r++)
				{
					str += std::to_string(this->matrix[c][r]) + " ";
				}
				str += " ]\n";
			}
			return str;
		}
	};

	class Engine
	{
	private:
		HANDLE console;
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

			rect_window = { 0, 0, 1, 1 };
			SetConsoleWindowInfo(console, TRUE, &rect_window);

			COORD coord = { static_cast<short>(screen_width), static_cast<short>(screen_height) };

			SetConsoleScreenBufferSize(console, coord);
			SetConsoleActiveScreenBuffer(console);

			CONSOLE_FONT_INFOEX cfi;
			cfi.cbSize = sizeof(cfi);
			cfi.nFont = 0;
			cfi.dwFontSize.X = pixel_size;
			cfi.dwFontSize.Y = pixel_size;
			cfi.FontFamily = FF_DONTCARE;
			cfi.FontWeight = FW_NORMAL;

			wcscpy_s(cfi.FaceName, L"Fixedsys");

			SetCurrentConsoleFontEx(console, false, &cfi);

			rect_window = { 0, 0, static_cast<short>(screen_width) - 1, static_cast<short>(screen_height) - 1 };
			SetConsoleWindowInfo(console, TRUE, &rect_window);

			screen_buffer = new CHAR_INFO[screen_width * screen_height]{ 0 };

			tp1 = std::chrono::system_clock::now();
			tp2 = std::chrono::system_clock::now();

			loop_running = true;
			std::thread t = std::thread(&Engine::Update, this);
			t.join();
		}

		~Engine() { delete[] screen_buffer; }

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
				WriteConsoleOutput(console, screen_buffer, { static_cast<short>(screen_width), static_cast<short>(screen_height) }, { 0,0 }, &rect_window);
			}
			else
			{
				std::cout << "Error - NAGE not initialised\n";
			}
		}

		virtual void Draw(const Vector2<int>& vec1, short colour = WHITE, short pixel_type = SOLID)
		{
			Draw(vec1.x, vec1.y, colour, pixel_type);
		}

		virtual void Draw(int x, int y, short colour = WHITE, short pixel_type = SOLID)
		{
			screen_buffer[y * screen_width + x].Char.UnicodeChar = pixel_type;
			screen_buffer[y * screen_width + x].Attributes = colour;
		}

		void Fill(const Vector2<int>& vec1, const Vector2<int>& vec2, short colour = WHITE, short pixel_type = SOLID)
		{
			Fill(vec1.x, vec1.y, vec2.x, vec2.y, colour, pixel_type);
		}

		void Fill(int x1, int y1, int x2, int y2, short colour = WHITE, short pixel_type = SOLID)
		{
			Clip(x1, y1);
			Clip(x2, y2);

			for (int x = x1; x < x2; x++)
				for (int y = y1; y < y2; y++)
					Draw(x, y, colour, pixel_type);
		}

		void Clear(short colour = BLACK, short pixel_type = SOLID)
		{
			Fill(0, 0, screen_width, screen_height, colour, pixel_type);
		}

		void Clip(Vector2<int>& vec) { Clip(vec.x, vec.y); }

		void Clip(int& x, int& y)
		{
			if (x < 0) x = 0;
			if (x >= screen_width) x = screen_width;
			if (y < 0) y = 0;
			if (y >= screen_height) y = screen_height;
		}

		void DrawString(const Vector2<int>& vec1, std::wstring text, short colour = WHITE, bool alpha = 0)
		{
			DrawString(vec1.x, vec1.y, text, colour, alpha);
		}

		void DrawString(int x, int y, std::wstring text, short colour = WHITE, bool alpha = 0)
		{
			for (size_t i = 0; i < text.size(); i++)
			{
				if (alpha && text[i] == L' ') continue;
				else
				{
					screen_buffer[y * screen_width + x + i].Char.UnicodeChar = text[i];
					screen_buffer[y * screen_width + x + i].Attributes = colour;
				}
			}
		}

		void DrawLine(const Vector2<int>& vec1, const Vector2<int>& vec2, short colour = WHITE, short pixel_type = SOLID, int width = 1)
		{
			DrawLine(vec1.x, vec1.y, vec2.x, vec2.y, colour, pixel_type, width);
		}

		void DrawLine(int x1, int y1, int x2, int y2, short colour = WHITE, short pixel_type = SOLID, int width = 1)
		{	//  DDA
			int dx, dy, steps;
			float x, y, x_inc, y_inc;

			for (int j = 0; j < width; j++)
			{

				dx = x2 - x1;
				dy = y2 - y1;

				steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

				x_inc = static_cast<float>(dx) / static_cast<float>(steps);
				y_inc = static_cast<float>(dy) / static_cast<float>(steps);

				x = static_cast<float>(x1 + j);
				y = static_cast<float>(y1 + j);

				for (int i = 0; i <= steps; i++)
				{
					Draw(static_cast<int>(x + 0.5f), static_cast<int>(y + 0.5f), colour, pixel_type);
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

				Draw(x, y, c, colour);

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
					Draw(x, y, c, colour);
				}
			}
			else
			{
				if (dy >= 0)
					{ x = x1; y = y1; ye = y2; }
				else
					{ x = x2; y = y2; ye = y1; }

				Draw(x, y, c, colour);

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
					Draw(x, y, c, colour);
				}
			}
			*/
		}

		CHAR_INFO GetShade(float lum)
		{
			short bg_col, fg_col;
			wchar_t sym;
			int pixel_bw = (int)(13.0f * lum);
			switch (pixel_bw)
			{
			case 0: bg_col = BLACK; fg_col = BLACK; sym = SOLID; break;

			case 1: bg_col = BLACK; fg_col = DARK_GREY; sym = QUARTER; break;
			case 2: bg_col = BLACK; fg_col = DARK_GREY; sym = HALF; break;
			case 3: bg_col = BLACK; fg_col = DARK_GREY; sym = THREEQUARTERS; break;
			case 4: bg_col = BLACK; fg_col = DARK_GREY; sym = SOLID; break;

			case 5: bg_col = DARK_GREY; fg_col = GREY; sym = QUARTER; break;
			case 6: bg_col = DARK_GREY; fg_col = GREY; sym = HALF; break;
			case 7: bg_col = DARK_GREY; fg_col = GREY; sym = THREEQUARTERS; break;
			case 8: bg_col = DARK_GREY; fg_col = GREY; sym = SOLID; break;

			case 9:  bg_col = GREY; fg_col = WHITE; sym = QUARTER; break;
			case 10: bg_col = GREY; fg_col = WHITE; sym = HALF; break;
			case 11: bg_col = GREY; fg_col = WHITE; sym = THREEQUARTERS; break;
			case 12: bg_col = GREY; fg_col = WHITE; sym = SOLID; break;
			default:
				bg_col = BLACK; fg_col = BLACK; sym = SOLID;
			}

			CHAR_INFO c;
			c.Attributes = bg_col | fg_col;
			c.Char.UnicodeChar = sym;
			return c;
		}

		float GetDt() { return dt; }
		std::wstring GetName() { return name; }

		int ScreenWidth() { return screen_width; }
		int ScreenHeight() { return screen_height; }

		KeyState Key(int key_id) { return keys[key_id]; }
	};

}

#endif // !NAGE_H