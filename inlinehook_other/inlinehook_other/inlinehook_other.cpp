//inline hook 实际上就是指 通过改变目标函数头部的代码来使改变后的代码跳转至我们自己设置的一个函数里，产生hook.

#include <windows.h>
#include <stdio.h>

//定义一个与MessageBoxA类型一致的函数指针。
typedef int (WINAPI* MessageBox_type)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
MessageBox_type RealMessageBox = MessageBoxA;

//自定义的MessageBox，每调用MessageBox都要跳到myMessageBox来处理
_declspec(naked) void WINAPI myMessageBox(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
    __asm
    {
        PUSH ebp
        mov ebp, esp    //平衡栈顶
        push esi    //由于编译后的代码会有cmp esi esp来比较堆栈。所以这里在执行功能代码前保存一下esi.
    }

    //在此加入HOOK之后要执行的功能代码。
    //此处举例打印出系统API：MessageBox的参数
    printf("hwnd:%d  lpText:%s  lpCaption:%s  uType:%d  \n", hWnd, lpText, lpCaption, uType); //由于myMessageBox是直接在API MessageBoxA的头部跳转过来的，MessageBoxA的栈空间没被改变，所以其参数可被myMessageBox直接使用。

    __asm
    {
        pop esi    //恢复esi.
        mov ebx, RealMessageBox
        add ebx, 5    //JMP dword ptr 占5个字节(远地址跳转)如：E9 90604000; RealMessageBox为API MessageBoxA的地址，所以此处为JMP dword ptr [MessageBoxA的硬编码地址+5]
        jmp ebx        //跳回到API MessageBoxA的真实地址。
    }
}

#pragma pack(1) //规定数据对齐系数的最大值为1个字节。
typedef struct _JMPCODE    //用于覆盖API MessageBoxA的头部的5个字节。
{
    BYTE jmp;
    DWORD addr; //跳转到钩子函数myMessageBox。
}JMPCODE, * PJMPCODE;

VOID HookMessageBoxA() //通过修改API MessageBoxA 函数入口来进行Inline HOOK
{
    JMPCODE jcode;
    jcode.jmp = 0xe9;//jmp的硬编码
    jcode.addr = (DWORD)myMessageBox - (DWORD)RealMessageBox - 5;

    ::WriteProcessMemory(GetCurrentProcess(), MessageBoxA, &jcode, sizeof(JMPCODE), NULL);//修改API MessageBoxA前5个字节实现跳转到自定义的钩子函数myMessageBox。
    //GetCurrentProcess() 获取当前进程句柄（内存基址）。
}

int main(int argc, char* argv[])
{
    HookMessageBoxA();  //hook操作
    ::MessageBoxA(NULL, "InlineHOOK Test.", "Title here", MB_OK); //直接加双冒号::表示使用全局函数，当全局函数没有该函数时即指API函数。
    return 0;
}

