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

int main()
{
	//设置bool类型输出格式，以“1”或“0”为输出的结果改为“true”或“false”
	cout.setf(ios_base::boolalpha);

	//检查是否支持SSE2指令集
	if (!XMVerifyCPUSupport()) {
		cout << "Unpatched!\n";
		return 0;
	}





}