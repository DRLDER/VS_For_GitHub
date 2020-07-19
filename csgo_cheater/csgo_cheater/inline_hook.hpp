#pragma once
#include "Windows.h"

//此处位5个字节的意思，包含操作码一字节和操作数四个字节
constexpr int byte_length = 5;


class inline_hook {

private:

	//这里是重定义，为了我简写
	using uchar = unsigned char;
	using dword = DWORD;


	//定义存储原始跳转汇编代码的无符号字符数组
	uchar mine_original_byte[byte_length];

	//定义存储自定义跳转代码的无符号字符数组
	uchar mine_self_byte[byte_length];

	//定义存储跳转地址的原跳转地址
	int mine_original_address;

	//定义存储自定义跳转地址的跳转地址
	int mine_self_address;

	//建立内存访问修改保护，用来修改访问的内存片区域属性
	//这句话自己写的时候出现了问题！！（注意attributes的类型为dword）
	dword modify_memery_attributes(int address,dword attributes=PAGE_EXECUTE_READWRITE)
	{
		//存放原内存属性
		dword old_attributes;

		//内存保护函数
		VirtualProtect(reinterpret_cast<void*>(address), byte_length, attributes, &old_attributes);

		//返回原内存属性
		return old_attributes;
	}



public:
	//构造函数的作用就是将我们的original_byte提取出来，并将self_byte构造出来
	inline_hook(int original_address, int self_address) :mine_original_address(original_address), mine_self_address(self_address)
	{
		//此处e9表示的操作码长跳转的意思，
		mine_self_byte[0] = '\xe9';

		//然后计算偏移
		int offset = self_address - (original_address + byte_length);

		//将我们自己的跳转地址压入已有操作码的自定义跳转语句中，最终构成一句跳转语句
		memcpy(&mine_self_byte[1], &offset, byte_length - 1);

		//这里访问源代码内存空间，所以使用以下内存片保护
		dword attributes = modify_memery_attributes(original_address);




		//将原地址中存储的操作语句，压入我们存储原始语句的语句数组中，方便以后执行完我们的代码后，可以跳转回原代码执行以不发生程序的崩溃
		//这句话自己写的时候出现了问题！！（记得强行转换）
		memcpy(mine_original_byte, reinterpret_cast<void*>(original_address), byte_length);


		//保存好有原始语句的数组后，我们可以恢复内存空间的属性
		modify_memery_attributes(original_address, attributes);


	}
	//此函数的作用就是为了在原跳转代码前将跳转代码转成我们自己的代码
	void modify_address()
	{

		dword attributes = modify_memery_attributes(mine_original_address);

		//这里就是将我们自己的跳转代码注入原跳转代码地址处，以执行我们的相应操作
		memcpy(reinterpret_cast<void*>(mine_original_address), mine_self_byte, byte_length);

		modify_memery_attributes(mine_original_address, attributes);



	}

	void restore_address()
	{
		dword attributes = modify_memery_attributes(mine_original_address);

		//这里就是将存储的原跳转代码注入原跳转代码地址处，以恢复游戏的正常运行
		memcpy(reinterpret_cast<void*>(mine_original_address),mine_original_byte, byte_length);

		modify_memery_attributes(mine_original_address, attributes);
	}
	
};