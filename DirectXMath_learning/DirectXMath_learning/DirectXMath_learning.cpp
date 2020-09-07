//#include "iostream"
//#include "windows.h"
//#include "DirectXMath.h"
//#include "DirectXPackedVector.h"
//
//using namespace std;
//using namespace DirectX;
//using namespace DirectX::PackedVector;
//
////重载<<运算符
//ostream& XM_CALLCONV operator<<(ostream& os, FXMVECTOR v)
//{
//
//	XMFLOAT3 dest;
//	XMStoreFloat3(&dest, v);
//	os << "(" << dest.x << "," << dest.y << "," << dest.z << ")";
//	return os;
//
//}
//int main()
//{
//	//设置bool类型输出格式，以“1”或“0”为输出的结果改为“true”或“false”
//	cout.setf(ios_base::boolalpha);
//
//	//检查是否支持SSE2指令集
//	if (!XMVerifyCPUSupport()) {
//		cout << "Unpatched!\n";
//		return 0;
//	}
//
//	XMVECTOR p = XMVectorZero();//全置0
//	XMVECTOR q = XMVectorSplatOne();//全置1
//	XMVECTOR u = XMVectorSet(1.0f, 2.0f, 3.0f, 0.0f);//置详细坐标
//	XMVECTOR v = XMVectorReplicate(-2.0f);//全置-2.0f
//	XMVECTOR w = XMVectorSplatZ(u);//全置坐标U的z轴数值
//
//	cout << "p=" << p << endl;
//	cout << "q=" << q << endl;
//	cout << "u=" << u << endl;
//	cout << "v=" << v << endl;
//	cout << "w=" << w << endl;
//
//}



//#include "iostream"
//#include "windows.h"
//#include "DirectXMath.h"
//#include "DirectXPackedVector.h"
//
//using namespace std;
//using namespace DirectX;
//using namespace DirectX::PackedVector;
//
////重载<<运算符
//ostream& XM_CALLCONV operator<<(ostream& os, FXMVECTOR v)
//{
//
//	XMFLOAT3 dest;
//	XMStoreFloat3(&dest, v);
//	os << "(" << dest.x << "," << dest.y << "," << dest.z << ")";
//	return os;
//
//}
//
//int main()
//{
//	//设置bool类型输出格式，以“1”或“0”为输出的结果改为“true”或“false”
//	cout.setf(ios_base::boolalpha);
//
//	//检查是否支持SSE2指令集
//	if (!XMVerifyCPUSupport()) {
//		cout << "Unpatched!\n";
//		return 0;
//	}
//
//	XMVECTOR n = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
//	XMVECTOR u = XMVectorSet(1.0f, 2.0f, 3.0f, 0.0f);
//	XMVECTOR v = XMVectorSet(-2.0f, 0.0f, -3.0f, 0.0f);
//	XMVECTOR w = XMVectorSet(0.707f, 0.707f, 0.0f, 0.0f);
//
//	//加减乘
//
//	XMVECTOR a = u + v;
//
//	XMVECTOR b = u - v;
//
//	XMVECTOR c = 10.0f * u;
//
//	// ||u||
//	XMVECTOR L = XMVector3Length(u);
//
//	// d=u/||u||
//	XMVECTOR d = XMVector4Normalize(u);
//
//	// s=u dot v 点积，计算值将存入vector中的每一坐标，值均相等
//	XMVECTOR s = XMVector3Dot(u,v);
//
//	//e=u x v 叉乘
//	XMVECTOR e = XMVector3Cross(u,v);
//
//	//求出proj_n(w) 和 perp_n(w)
//	XMVECTOR projW;
//	XMVECTOR perpW;
//	XMVector3ComponentsFromNormal(&projW, &perpW, w, n);
//
//	//projW+perpW==w? 判断正交向量位移后是否可得原向量w
//	bool equal = XMVector3Equal(projW + perpW, w) != 0;
//	bool notequal = XMVector3NotEqual(projW + perpW, w) != 0;
//
//	//projW与perpW之间的夹角应为90度
//	
//	//获取角度虚向量
//	XMVECTOR angleVec = XMVector3AngleBetweenVectors(projW, perpW);
//
//	//将角度向量转化为弧度（浮点数）
//	float angleRadians = XMVectorGetX(angleVec);
//
//	//将弧度转换为角度（浮点数）
//	float angleDegrees = XMConvertToDegrees(angleRadians);
//
//	cout << "u=" << u << endl;
//	cout << "v=" << v << endl;
//	cout << "w=" << w<< endl;
//	cout << "n=" << n << endl;
//	cout << "a=u+v=" << a << endl;
//	cout << "b=u-v=" << b << endl;
//	cout << "c=10*u=" << c << endl;
//	cout << "d=u/||u||=" << d << endl;
//	cout << "e=u x v=" << e << endl;
//	cout << "L=||u||=" << L << endl;
//	cout << "s=u.v=" << s << endl;
//	cout << "projW=" << projW << endl;
//	cout << "perpW=" << perpW << endl;
//	cout << "projW+perpW==w=" << equal << endl;
//	cout << "projW+perpW!=w=" << notequal << endl;
//	cout << "angle=" << angleDegrees << endl;
//
//
//}

#include "iostream"
#include "windows.h"
#include "DirectXMath.h"
#include "DirectXPackedVector.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;

//重载<<运算符
ostream& XM_CALLCONV operator<<(ostream& os, FXMVECTOR v)
{

	XMFLOAT3 dest;
	XMStoreFloat3(&dest, v);
	os << "(" << dest.x << "," << dest.y << "," << dest.z << ")";
	return os;

}

//本次只展示相关的变换函数

//缩放矩阵
XMMATRIX XM_CALLCONV XMMatrixScaling(float ScaleX, float ScaleY, float ScaleZ);

//用一个3D向量中的分量来构建缩放矩阵
XMMATRIX XM_CALLCONV XMMatrixScalingFromVector(FXMVECTOR Scale);

//构建一个绕x轴旋转的矩阵Rx,yz轴类似
XMMATRIX XM_CALLCONV XMMatrixRotationX(float Angle);

//构建一个绕任意轴旋转的矩阵Rn
XMMATRIX XM_CALLCONV XMMatrixRotationAxis(FXMMATRIX Axis, float Angle);

//构建一个平移矩阵
XMMATRIX XM_CALLCONV XMMatrixTranslation(float OffsetX, float OffsetY, float OffsetZ);

//用一个3D向量中的分量来构建平移矩阵
XMMATRIX XM_CALLCONV XMMatrixTranslationFromVector(FXMVECTOR Offset);

//计算向量与矩阵的乘积，默认Vw=1，还有点矩阵乘积Vm=0
XMMATRIX XM_CALLCONV XMVector3Transform(FXMVECTOR V, CXMMATRIX M);

XMMATRIX XM_CALLCONV XMVector3TransformNormal(FXMVECTOR V, CXMMATRIX M);