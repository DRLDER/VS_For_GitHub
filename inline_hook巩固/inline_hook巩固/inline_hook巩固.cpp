#include "windows.h"
#include "stdio.h"

//定义InlineHook类
class inline_hook 
{
private:
	//定义私有变量
	//存放原始跳转代码，存放自己的跳转代码
	BYTE g_OriginalByte[5];

	BYTE g_SelfByte[5] = { 0xE9 };

	//存放原始跳转地址，存放自己代码的跳转代码
	int mine_original_address;

	int mine_self_address;


public:
	//构造类初始化两个跳转代码
	inline_hook(int originaladdress,int selfaddress):mine_original_address(originaladdress),mine_self_address(selfaddress)
	{
		//定义内存属性变量
		DWORD attribute;

		//首先将原始代码内存片区属性修改为可访问修改
		VirtualProtect((void*)mine_original_address, 5, PAGE_EXECUTE_READWRITE, &attribute);

		//原首语句存入
		memcpy(g_OriginalByte, (void*)originaladdress, 5);

		//计算自己的函数首地址和原语句首地址的偏移
		DWORD Offset = (DWORD)selfaddress - (DWORD)originaladdress - 5;

		//还原为原内存属性
		VirtualProtect((void*)mine_original_address, 5, attribute, &attribute);

		VirtualProtect((void*)mine_self_address, 5, PAGE_EXECUTE_READWRITE, &attribute);

		//将自己代码区域的偏移地址压入自己的跳转代码
		memcpy((void*)(g_SelfByte+1), &Offset,4);

		VirtualProtect((void*)mine_self_address, 5, attribute, &attribute);
	}
	//构造结束

	//建立钩子
	void HookIn()
	{
		DWORD attribute;

		VirtualProtect((void*)mine_original_address, 5, PAGE_EXECUTE_READWRITE, &attribute);

		//将我们的跳转地址压入原语句首地址，以跳转到我们的函数中
		memcpy((void* )mine_original_address, g_SelfByte, 5);

		VirtualProtect((void*) mine_original_address, 5, attribute, &attribute);

	}

	//取消钩子
	void UnHook()
	{
		DWORD attribute;

		VirtualProtect((void*)mine_original_address, 5, PAGE_EXECUTE_READWRITE, &attribute);

		//还原原语句首地址代码
		memcpy((void*)mine_original_address, g_OriginalByte, 5);

		VirtualProtect((void*)mine_original_address, 5, attribute, &attribute);
	}
};

//定义全局MSG Hook变量
inline_hook *MessageBoxHook;

//自定义MSGBox
int
WINAPI
myMessageBox(_In_opt_ HWND hWnd,_In_opt_ LPCWSTR lpText,_In_opt_ LPCWSTR lpCaption,_In_ UINT uType)
{
	//还原原首地址代码
	MessageBoxHook->UnHook();
	
	printf("Notice: myMessageBox DLL IN!\n");

	//换成我们劫持后的messagebox，显示的内容改变
	MessageBoxW(hWnd, L"HOOKIN！", L"MSGBOXHOOK", uType);

	//在此下钩子等messagebox
	MessageBoxHook->HookIn();

	return 0;
}



int main(int argc, char* argv[])
{

	//初始化
	MessageBoxHook = new inline_hook((int)MessageBoxW, (int)myMessageBox);
	
	//下钩子等MessageBox
	MessageBoxHook->HookIn();

	MessageBoxW(nullptr, L"1", L"MSG", MB_OK);

	MessageBoxW(nullptr, L"2", L"MSG", MB_OK);

	printf("Notice: It is Out!");

	return 1;
}



