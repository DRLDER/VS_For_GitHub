#pragma warning(disable:4996)
//防止一些警告成为严重错误
#define DEBUG_STRING

//优先调用Windows.h,否则TlHelp32会出错
#include "Windows.h"
#include "TlHelp32.h"
#include "Psapi.h"

//调试时使用
#ifdef DEBUG_STRING
#include "stdio.h"
#endif // DEBUG_STRING



///////////////////////////////////////////////////////////////////////////////////////////////////

//错误提示
void IfError(bool state, const char* text = nullptr)
{
	//如果正常直接返回
	if (state)return;

	//状态异常执行以下代码
	char buffer[1024];
	//使用0来填充一片内存区域
	ZeroMemory(buffer, 1024);
	//将错误的提示文本填充到buffer中并输出到屏幕
	wsprintf(buffer, "ERROR:%s", text);

}

//错误提示（弹窗）
void warning(const char* text, bool state = false)
{
	if (state) return;

	char buffer[5000];
	ZeroMemory(buffer, 5000);
	wsprintf(buffer, "警告 : %s", text);

#ifdef DEBUG_STRING
	printf("%s \n", buffer);
#endif // DEBUG_STRING

	MessageBox(nullptr, buffer, nullptr, MB_OK);
}

///////////////////////////////////////////////////////////////////////////////////////////////////


//定义模组信息结构体
struct module_information
{
	//定义模组相关信息变量
	HANDLE module_handle;
	char module_name[1024];
	char* module_data;
	int module_address;
	int module_size;

	//申请内存函数
	void alloc(int size)
	{
		//初始模组大小
		module_size = size;

		//申请内存，申请成功的首地址返回
		module_data = (char*)VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		//检错，返回值错误，则显示分配失败
		IfError(module_data, "Allocated failed!\n");
	}

	//释放内存函数
	void release()
	{
		//如果存在分配的内存首地址，则释放该区域，因为MEM_RELEASE释放片区域，释放空间为0
		if (module_data) VirtualFree(module_data, 0, MEM_RELEASE);
		//指针置空
		module_data = nullptr;
	}

};
///////////////////////////////////////////////////////////////////////////////////////////////////

//获取进程ID
DWORD GetprocessID(const char* process_name)
{
	//首先，获取的进程ID需要CreateToolhelp32Snapshot来对进程使用的模块，堆，线程执行快照
	HANDLE SNAP = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);

	//判断是否拍到快照，正确继续，错误提示
	IfError(SNAP != INVALID_HANDLE_VALUE, "CreateToolhelp32Snapshot crashed!\n");

	//定义PROCESSENTRY32类型，保存进程相关信息结构体
	PROCESSENTRY32 ProcessInfo;

	//置空内存区域
	ZeroMemory(&ProcessInfo,sizeof(ProcessInfo));

	//初始化设置ProcessInfo的结构体大小，否则会报错
	ProcessInfo.dwSize = sizeof(ProcessInfo);

	//定义存入目标进程名的存储区
	char target[1024];

	//置0
	ZeroMemory(target, 1024);

	//将目标进程名放入存储区
	strncpy(target, process_name,strlen(process_name));

	//将进程名大写
	_strupr(target);

	//获取第一个进程信息,并存入ProcessInfo
	BOOL state = Process32First(SNAP,&ProcessInfo);

	//匹配进程名
	while (state)
	{
		//判别进程名是否相同
		if (strncmp(_strupr(ProcessInfo.szExeFile), target, strlen(target)) == 0)
		{
#ifdef DEBUG_STRING
			printf("ProcessName : %s \n", ProcessInfo.szExeFile);
			printf("ProcessID : %d \n", ProcessInfo.th32ProcessID);
			printf("\n");
#endif // DEBUG_STRING

			//获取到相应的进程信息，进程ID..后我们就结束句柄
			CloseHandle(SNAP);

			//返回进程ID
			return ProcessInfo.th32ProcessID;

		}
		//跳转到下一个进程，进行匹配
		state = Process32Next(SNAP, &ProcessInfo);
	}
	//关闭进程句柄
	CloseHandle(SNAP);
	warning("!!!Unable to find processID!!!\n");
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

//获取进程句柄
HANDLE GetprocessHandle(DWORD ProcessID, DWORD Access = PROCESS_ALL_ACCESS)
{
	//获取句柄
	HANDLE ProcessHandle = OpenProcess(Access,FALSE,ProcessID);
	IfError(ProcessHandle,"Open process handle failed!\n");

#ifdef DEBUG_STRING
	printf("ProcessID : %8x \n", (unsigned int)ProcessID);
	printf("ProcessHandle : %8x \n", (unsigned int)ProcessHandle);
	printf("\n");
#endif // DEBUG_STRING

	return ProcessHandle;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

//读取内存
DWORD read_memory(HANDLE process, int address, void* recv, int size)
{
	DWORD read_size;
	ReadProcessMemory(process, (LPCVOID)address, recv, size, &read_size);
	return read_size;
}

//写入内存
DWORD write_memory(HANDLE process, int address, void* data, int size)
{
	DWORD write_size;
	WriteProcessMemory(process, (LPVOID)address, data, size, &write_size);
	return write_size;
}

///////////////////////////////////////////////////////////////////////////////////////////////////


//获取模块信息
void GetModuleInfo(HANDLE ProcessHandle, DWORD ProcessID, const char* ModuleName, struct module_information& Info)
{
	//进程模块快照，与进程快照类似
	HANDLE SNAP = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, ProcessID);
	
	//检错
	IfError(SNAP!= INVALID_HANDLE_VALUE,"CreateToolhelp32Snapshot crashed!\n");

	//定义模组信息存储点
	MODULEENTRY32 ModuleInfo;

	//清空一块内存区域
	ZeroMemory(&ModuleInfo, sizeof(ModuleInfo));

	//设置模组大小初始值，否则报错
	ModuleInfo.dwSize = sizeof(ModuleInfo);

	//定义目标模组存储名空间
	//下面的操作类似取进程ID
	char target[1024];

	ZeroMemory(target,1024);

	strncpy(target, ModuleName, strlen(ModuleName));

	_strupr(target);

	BOOL state = Module32First(SNAP, &ModuleInfo);

	while(state)
	{
		if (strncmp(_strupr(ModuleInfo.szModule), target, strlen(target) )== 0)
		{
			//传递模组基址地址值
			Info.module_address = (int)ModuleInfo.modBaseAddr;

			//传递模组句柄
			Info.module_handle = ModuleInfo.hModule;

			//分配内存空间
			Info.alloc(ModuleInfo.modBaseSize);

			//置空模块名区域
			ZeroMemory(Info.module_name, sizeof(Info.module_name));

			//复制模组名
			strncpy(Info.module_name,ModuleInfo.szModule,strlen(ModuleInfo.szModule));

			DWORD Size = read_memory(ProcessHandle,Info.module_address,Info.module_data,Info.module_size);

			IfError(Size, "read memory error! @N1\n");

#ifdef DEBUG_STRING
			printf("ModuleName : %s \n", ModuleInfo.szModule);
			printf("ModuleAddress : %8x \n", (unsigned int)ModuleInfo.modBaseAddr);
			printf("ModuleSize : %8x \n", ModuleInfo.modBaseSize);
			printf("RealReadSize : %8x \n", Size);
			printf("\n");
#endif // DEBUG_STRING

			CloseHandle(SNAP);
			return;
		}
		state = Module32Next(SNAP,&ModuleInfo);
	}
	CloseHandle(SNAP);
	warning("!!!Unable to find Module!!!\n");
}


///////////////////////////////////////////////////////////////////////////////////////////////////
