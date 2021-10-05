#include <iostream>
#include <Grassland.h>

using namespace std;
using namespace Grassland::Math;

int main()
{
	Mat4x4 mat1(
		1, 2, 3, 4,
		5,7,6,8,
		11,10,12,9,
		16,15,14,13
	),
	mat2 = mat1.inverse();
	Vec4 vec(1, 1, 1, 1);
	cout << mat1 << endl;
	cout << mat1.determinant() << " " << mat1 * mat2 << " " << mat1 * vec << " " << vec * mat1 << mat1 * (mat2 * vec);
	//cout << mat1*vec << vec*mat1 << endl;
}