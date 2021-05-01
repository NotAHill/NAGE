#include "NAGE.h"
#include <string>

int main()
{
	NAGE::Matrix3<int> mat;
	auto& m = mat.matrix;

	m[0][0] = 1.f; m[0][1] = 3.f; m[0][2] = 1.f;
	m[1][0] = 0.f; m[1][1] = 4.f; m[1][2] = 1.f;
	m[2][0] = 2.f; m[2][1] = -1.f; m[2][2] = 0.f;
	
	std::cout << mat.str() << std::endl;
	std::cout << mat.Determinant() << std::endl;
	
	mat = mat.Minors();

	std::cout << mat.str() << std::endl;

	mat = mat.Cofactors();

	std::cout << mat.str() << std::endl;

	mat = mat.Transpose();

	std::cout << mat.str() << std::endl;


	return 0;
}