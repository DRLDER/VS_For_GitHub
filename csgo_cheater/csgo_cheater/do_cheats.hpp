#pragma once
#include "help_func.hpp"
#include "inline_hook.hpp"
#include "d3d9.h"
#pragma comment(lib,"d3d9.lib")
#include "stdio.h"

//包含绘制窗口用的头文件
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include"imgui/imgui_impl_win32.h"

//设定D3D项目指针
IDirect3D9* g_direct3d9 = nullptr;

//D3D设备指针
IDirect3DDevice9* g_direct3ddevice9 = nullptr;

// 设定D3D当前参数（内存区域）
D3DPRESENT_PARAMETERS g_present;

//设定下面三个函数的三个inline_hook指针
inline_hook* g_Reset_hook = nullptr;
inline_hook* g_EndScene_hook = nullptr;
inline_hook* g_DrawIndexedPrimitive_hook = nullptr;

//创建存入原始窗口过程指针
WNDPROC g_original_proc = nullptr;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




//调用外部存在的函数定义，extern告诉编译器后面的函数可能在别处定义，在此沿用
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


LRESULT CALLBACK self_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM IParam)
{

	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, IParam))
		return true;

	//子窗口回转主窗口使用CallWindowProcW
	return CallWindowProcW(g_original_proc, hWnd, uMsg, wParam, IParam);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//初始化界面
void initialize_imgui(IDirect3DDevice9* direct3ddevice9)
{

	//初始化参数
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	//界面主题
	ImGui::StyleColorsDark();

	//初始化参数
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	//设置空指针，防止在目录下生成文件（如配置文件等）
	io.LogFilename = nullptr;
	io.IniFilename = nullptr;

	//设置在什么窗口上设置此绘制窗口
	ImGui_ImplWin32_Init(FindWindowW(L"Direct3DWindowClass", nullptr));
	ImGui_ImplDX9_Init(direct3ddevice9);

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//下面就是劫持参数的几个函数

//设置自己的重置函数，源于IDirect3DDevice9中的reset方法,当游戏窗口发生变化时我们进行设置
HRESULT _stdcall self_Reset(IDirect3DDevice9* direct3ddevice9,D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	//printf("DEBUG:DLL in self_Reset!\n");

	//由于跳转到我们的代码区，要先将原跳转代码恢复，此函数单独执行，因为是在线程中，和游戏线程不冲突
	g_Reset_hook->restore_address();
	
	//使imgui无效
	ImGui_ImplDX9_InvalidateDeviceObjects();

	//当游戏屏幕发生改变时，此函数被调用的核心
	HRESULT result = direct3ddevice9->Reset(pPresentationParameters);

	//使imgui有效
	ImGui_ImplDX9_CreateDeviceObjects();

	//由于上面恢复成了原跳转地址，这里就要在修改为自己的跳转地址，以让下一执行原reset时进入我们的reset
	g_Reset_hook->modify_address();

	//返回执行状态
	return result;

}



//这里用来画人物的方框
HRESULT _stdcall self_EndScene(IDirect3DDevice9* direct3ddevice9)
{
	//设定静态全局变量，放其他文件使用（可以在别的文件里取相同名变量不冲突）
	static bool first_call = true;

	if (first_call)
	{
		first_call = false;
		//在我们的d3d设备上初始化imgui
		initialize_imgui(direct3ddevice9);
		//设置原始窗口过程，使用SetWindowLongA
		g_original_proc= (WNDPROC)SetWindowLongA(FindWindowW(L"Direct3DWindowClass", nullptr),GWL_WNDPROC,(LONG)self_proc);
	}

	//printf("DEBUG:DLL in self_EndScene!\n");

	//地址恢复
	g_EndScene_hook->restore_address();

	//设置框架
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//imgui界面开始
	ImGui::Begin("new window");

	//界面内容
	ImGui::Text("this is test window");

	//imgui界面结束
	ImGui::End();

	//结束框架构建
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	//当游戏内在绘制时，此函数被调用的核心
	HRESULT result = direct3ddevice9->EndScene();
	
	//地址转入我们的地址
	g_EndScene_hook->modify_address();

	//返回执行状态
	return result;
}



//人物模型透视
HRESULT _stdcall  self_DrawIndexedPrimitive(IDirect3DDevice9* direct3ddevice9, D3DPRIMITIVETYPE type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{

	return 1;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




//初始化d3d9
unsigned int _stdcall initial_d3d9(void* data)
{
//#ifdef _DEBUG
//	AllocConsole();
//	SetConsoleTitleA("test");
//	freopen("CON", "w", stdout);
//#endif 


	//初始化D3D设备，传递给g_direct3d9
	g_direct3d9 =  Direct3DCreate9(D3D_SDK_VERSION);

	//检错，看是否初始化失败
	check_error(g_direct3d9,"Direct3DCreate9失败");

	//初始化成功后，清空初始化设备区域
	memset(&g_present, 0, sizeof(g_present));

	//在初始化区域内设定相应属性
	g_present.Windowed = TRUE;
	g_present.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_present.BackBufferFormat = D3DFMT_UNKNOWN;
	g_present.EnableAutoDepthStencil = TRUE;
	g_present.AutoDepthStencilFormat = D3DFMT_D16;

	//前面所需的初始已完成，此处创建启动设备
	HRESULT result= g_direct3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, FindWindowW(L"Direct3DWindowClass", nullptr), 
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, &g_present,&g_direct3ddevice9);

	//检测创建设备是否出错
	check_error(result == 0, "CreateDevice函数失败！");

	//将原先的项目指针转成表
	int* direct3d9_table = (int*)*(int*)g_direct3d9;

	//将原先的设备指针转成表
	int* direct3ddevice9_table = (int*)*(int*)g_direct3ddevice9;

	//建立内敛钩子——重置钩子，作为当游戏窗口变动时执行操作的工具
	g_Reset_hook = new inline_hook(direct3ddevice9_table[16], (int)self_Reset);

	//绘制钩子，当游戏在绘制角色时发生操作的工具
	g_EndScene_hook = new inline_hook(direct3ddevice9_table[42], (int)self_EndScene);

	//跳转至我们的reset函数
	g_Reset_hook->modify_address();

	//跳转至我们的EndScene函数
	g_EndScene_hook->modify_address();

	return 0;

}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////






//设置卸载函数
void un_load()
{
	//恢复到程序源跳转地址
	g_Reset_hook->restore_address();
	g_EndScene_hook->restore_address();
}