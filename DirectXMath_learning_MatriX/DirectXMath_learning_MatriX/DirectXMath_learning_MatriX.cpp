//////////////////////////////////////////////////////////////////////////////////////////////

////设置矩阵的函数
//#include "DirectXMath.h"
//DirectX::XMMATRIX XMMatrixSet(float m00, float m01, float m02, float m03,
//    float m10, float m11, float m12, float m13,
//    float m20, float m21, float m22, float m23,
//    float m30, float m31, float m32, float m33);
//


//////////////////////////////////////////////////////////////////////////////////////////////

//使用函数操作开始
#include "windows.h"
#include "iostream"
#include "DirectXMath.h"
#include "DirectXPackedVector.h"
using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;

//首先重载“<<”运算符，之前写过不多赘述。
ostream& XM_CALLCONV operator << (ostream& os, FXMVECTOR v) {
	XMFLOAT4 dest;
	XMStoreFloat4(&dest, v);
	os << "(" << dest.x << "," << dest.y << "," << dest.z << "," << dest.w << ")";
	return os;
}

//上面重载vector类型，这里重载matri类型
ostream& XM_CALLCONV operator <<(ostream& os, FXMMATRIX m) {
	for (int i = 0; i < 4; i++) {
		os << XMVectorGetX(m.r[i]) << "\t";
		os << XMVectorGetY(m.r[i]) << "\t";
		os << XMVectorGetZ(m.r[i]) << "\t";
		os << XMVectorGetW(m.r[i]) ;
		os << endl;
	}
	return os;
}

int main() {
	//同样的一上来先看是否支持SSE2指令集
	if (!XMVerifyCPUSupport()) {
		cout << "directx math not supported!" << endl;
		return 0;
	}

	//进入正题
	XMMATRIX A(1.0f,0.0f,0.0f,0.0f,
							0.0f,2.0f,0.0f,0.0f,
							0.0f,0.0f,4.0f,1.0f,
							1.0f,2.0f,3.0f,1.0f);

	//输入单位矩阵
	XMMATRIX B = XMMatrixIdentity();
	
	//简单乘法
	XMMATRIX C = A * B;

	//转置
	XMMATRIX D = XMMatrixTranspose(A);

	//detA
	XMVECTOR det = XMMatrixDeterminant(A);

	//逆矩阵
	XMMATRIX E = XMMatrixInverse(&det, A);

	//验证逆矩阵与原矩阵相乘是否为单位矩阵
	XMMATRIX F = A * E;

	cout << "A=" << endl << A << endl;
	cout << "B=" << endl <<B << endl;
	cout << "C=A*B" << endl << C << endl;
	cout << "D=transpose(A)=" << endl << D << endl;
	cout << "det=determinant(A)=" << det << endl << endl;
	cout << "E=inverse(A)=" << endl << E << endl;
	cout << "F=A*E=" << endl << F << endl;



}