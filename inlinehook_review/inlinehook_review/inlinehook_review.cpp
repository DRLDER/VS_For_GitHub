#include "windows.h"
#include "stdio.h"

//define the class of inlinehook
class inline_hook {


	//first for private part;
private:
	//define the private variate 

	//save the original word, and the self jump word

	BYTE g_OriginalByte[5];

	BYTE g_selfByte[5] = {0xE9};

	//now save the address of the original or self word

	int mine_original_address;
	int mine_self_address;


	//second for public part;
public:

	//build inline_hook word

	inline_hook(int originaladdress, int selfaddress) :mine_original_address(originaladdress), mine_self_address(selfaddress) {

		DWORD attribute;//save the old protection attribute 

		//change the in-memery attribute to in_read

		VirtualProtect((void*)mine_original_address,5,PAGE_EXECUTE_READWRITE,&attribute);

		//original byte save in

		memcpy(g_OriginalByte,(void*)originaladdress,5);

		//calculate the offset of selfaddress and originaladdress

		DWORD offset = (DWORD)selfaddress - (DWORD)originaladdress - 5;

		//change the in_mem attribute to the original in_mem attribute

		VirtualProtect((void*)mine_original_address, 5, attribute, &attribute);

		//change the in_mem attribute of self word

		VirtualProtect((void*)mine_self_address, 5, PAGE_EXECUTE_READWRITE, &attribute);

		memcpy((void*)(g_selfByte+1),&offset,4);

		VirtualProtect((void*)mine_self_address, 5, attribute, &attribute);

	}

	//build the hook

	void Hookin() {
		DWORD attribute;
		VirtualProtect((void*)mine_original_address, 5, PAGE_EXECUTE_READWRITE, &attribute);

		memcpy((void*)mine_original_address, g_selfByte, 5);

		VirtualProtect((void*)mine_original_address, 5, attribute, &attribute);

	}

	//clear the hook

	void Unhook() {
		DWORD attribute;

		VirtualProtect((void*)mine_original_address, 5, PAGE_EXECUTE_READWRITE, &attribute);

		//restore the attribute

		memcpy((void*)mine_original_address, g_OriginalByte, 5);

		VirtualProtect((void*)mine_original_address, 5, attribute, &attribute);

	}


};

//define the genaral MSG HOOK valuate
inline_hook* MessageBoxAHook;


int
WINAPI
myMessageBoxA(
	_In_opt_ HWND hWnd,
	_In_opt_ LPCSTR lpText,
	_In_opt_ LPCSTR lpCaption,
	_In_ UINT uType) {
	MessageBoxAHook->Unhook();

	printf("Notice: The core function MessageBoxA Hook in!");

	MessageBoxAHook->Hookin();

	return 0;

}



int main() {
	MessageBoxAHook = new inline_hook((int)MessageBoxA, (int)myMessageBoxA);

	MessageBoxAHook->Hookin();

	MessageBoxA(nullptr, "1", "MSG", MB_OK);

	printf("Notice: It is Out!");

	return 0;
}