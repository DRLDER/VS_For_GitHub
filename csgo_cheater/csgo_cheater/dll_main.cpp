#include "do_cheats.hpp"
#include "process.h"


int _stdcall DllMain(void* _DllHandle, unsigned long _Reason, void* _Reserved)
{
	//这里主要是激活initial_d3d9,创建了一个线程,当有dll执行时运行
	//_beginthreadex作为创建线程作用
	if (_Reason == DLL_PROCESS_ATTACH) _beginthreadex(nullptr, 0,initial_d3d9,nullptr,0,nullptr);
	
	
	//这里用于卸载DLL，在un_load函数中实现地址回转原地址防止程序崩溃
	if (_Reason == DLL_PROCESS_DETACH) un_load();


	return 1;

}