#pragma once
#include "Windows.h"

//检测错误函数
void check_error(bool state, const char* str = nullptr)
{
	if (state)return;

	//设定一个空间，放错误提示
	char buffer[1024*2];
	wsprintf(buffer,"ERROR:%s",str );

	//跳出一个窗口
	MessageBox(nullptr, buffer, nullptr, MB_OK | MB_ICONHAND);
	exit(-1);
}