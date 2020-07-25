#pragma once

#include "D3DDrawEngine.h"


///////////////////////////////////////////////////////////////////////////////////////////////////

//定义初级全局变量
DWORD g_ProcessId;
HANDLE g_ProcessHandle;

int g_MatrixAddr;
int g_AngleAddr;
int g_SelfAddr;
int g_PlayersAddr;

HWND g_game_hwnd;
HWND g_transparent_hwnd;

const int g_players_count = 32;
///////////////////////////////////////////////////////////////////////////////////////////////////

//定义中级全局变量
char ProcessName[] = "hl2.exe";


///////////////////////////////////////////////////////////////////////////////////////////////////

struct PlayerList
{
	bool effective;//是否有效
	int aimbot_len;//自瞄长度
	bool self;//是自己
	float location[3];//身体位置
	float head_bone[3];//头骨位置
	int camp;//阵营
	int blood;//血量
};

///////////////////////////////////////////////////////////////////////////////////////////////////

//初始化相关地址，基址获取，我们需要静态进程名
void InitializeAddress(const char* process_name)
{
	//首先就是获取进程id
	DWORD ProcessID= GetprocessID(process_name);

	//获取进程句柄
	HANDLE ProcessHandle = GetprocessHandle(ProcessID);
	
	//全局变量赋值
	g_ProcessId = ProcessID;
	g_ProcessHandle = ProcessHandle;

	//定义模组信息获取结构存储点
	struct module_information engine_module;
	struct module_information server_module;
	struct module_information client_module;
	//获取模组信息
	GetModuleInfo(ProcessHandle, ProcessID, "engine.dll", engine_module);
	GetModuleInfo(ProcessHandle, ProcessID, "client.dll", client_module);

	//获取基址
	int matrix_address = engine_module.module_address + 0x5A78EC;//自己矩阵基址

	int angle_address = engine_module.module_address + 0x4791B4;//自己角度基址

	int self_address=client_module.module_address+0x4D3904;//4D3914
	//read_memory(ProcessHandle, client_module.module_address + 0x4D3904,&self_address,sizeof(int));

	int players_address= client_module.module_address + 0x4D3904; //包含自己的玩家地址
	

#ifdef DEBUG_STRING
	printf("SelfMatrix :%8x \n", matrix_address);
	printf("SelfAngle :%8x \n", angle_address);
	printf("SelfAddress :%8x \n", self_address);
	printf("PlayerAddress :%8x \n", players_address);
#endif // DEBUG_STRING

	//全局变量赋值
	g_MatrixAddr = matrix_address;
	g_AngleAddr = angle_address;
	g_SelfAddr = self_address;
	g_PlayersAddr = players_address;

}
///////////////////////////////////////////////////////////////////////////////////////////////////

void GetPlayerList(PlayerList* Players)
{
	//获取进程句柄
	HANDLE process = g_ProcessHandle;

	for (int i = 0; i < g_players_count; i++)
	{
		int PlayerBaseAddr;

		//获取玩家偏移后信息结构
		read_memory(process, (g_PlayersAddr + i * 0x10), &PlayerBaseAddr, sizeof(int));

		//如果地址为空，我们跳出一次循环
		if (PlayerBaseAddr == 0)continue;

		//我们获取血量
		read_memory(process, PlayerBaseAddr + 0x94, &Players[i].blood, sizeof(int));

		//如果血量为空，我们也跳出一次循环
		if (Players[i].blood <= 1)continue;

		//设置玩家有效标记
		Players[i].effective = true;

		//设置人物直线距离
		Players[i].aimbot_len = 9999;
		
		//设置骨骼点首地址存储区
		int bone_base_address;

		if (read_memory(process, (PlayerBaseAddr + 0x578), &bone_base_address, sizeof(int)))
		{
			//获取骨骼点
			read_memory(process, (bone_base_address + 171 * sizeof(float)), &Players[i].head_bone[0], sizeof(float));
			read_memory(process, (bone_base_address + 175 * sizeof(float)), &Players[i].head_bone[1], sizeof(float));
			read_memory(process, (bone_base_address + 179 * sizeof(float)), &Players[i].head_bone[2], sizeof(float));		
		}
		
		//获取位置
		read_memory(process, PlayerBaseAddr + 0x338, Players[i].location, sizeof(Players[i].location));
		
		//获取阵营
		read_memory(process, PlayerBaseAddr + 0x9C, &Players[i].camp, sizeof(int));
	}

}

//////////////////////////////////////////////////////////////////////////////////////////////////

//获取自己的位置
void get_self_location(float* location)
{
	int location_base_address;
	read_memory(g_ProcessHandle, g_SelfAddr, &location_base_address, sizeof(int));
	if (location_base_address)
		read_memory(g_ProcessHandle, location_base_address + 0x338, location, sizeof(float) * 3);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

//转化为矩阵信息
bool to_rect_info(float matrix[][4], float* location, int window_width, int window_heigt, int& x, int& y, int& w, int& h)
{
	float to_target = matrix[2][0] * location[0]
		+ matrix[2][1] * location[1]
		+ matrix[2][2] * location[2]
		+ matrix[2][3];
	if (to_target < 0.01f)
	{
		x = y = w = h = 0;
		return false;
	}
	to_target = 1.0f / to_target;

	float to_width = window_width + (matrix[0][0] * location[0]
		+ matrix[0][1] * location[1]
		+ matrix[0][2] * location[2]
		+ matrix[0][3]) * to_target * window_width;

	float to_height_h = window_heigt - (matrix[1][0] * location[0]
		+ matrix[1][1] * location[1]
		+ matrix[1][2] * (location[2] + 75.0f)
		+ matrix[1][3]) * to_target * window_heigt;

	float to_height_w = window_heigt - (matrix[1][0] * location[0]
		+ matrix[1][1] * location[1]
		+ matrix[1][2] * (location[2] - 5.0f)
		+ matrix[1][3]) * to_target * window_heigt;

	x = (int)(to_width - (to_height_w - to_height_h) / 4.0f);
	y = (int)(to_height_h);
	w = (int)((to_height_w - to_height_h) / 2.0f);
	h = (int)(to_height_w - to_height_h);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////


//计算自瞄距离
int get_aimbot_len(int window_w, int window_h, int x, int y)
{
	int temp_x = abs(window_w - x);
	int temp_y = abs(window_h - y);
	return (int)sqrt((temp_x * temp_x) + (temp_y * temp_y));
}

//绘制血量
void render_player_blood(float blood, int x, int y, int h)
{
	float value = blood / 100.0f * h;
	render_line(D3DCOLOR_XRGB(250, 0, 255), x, y, x, y + value);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

int GetSelfCamp(PlayerList* Players);

//绘制人物方框
void render_player_box(PlayerList* Players)
{

	//自定义xy坐标和方框长宽
	int window_x, window_y, window_w, window_h;
	get_window_size(g_game_hwnd, window_x, window_y, window_w, window_h);
	window_w /= 2;
	window_h /= 2;

	float matrix[4][4];
	read_memory(g_ProcessHandle, g_MatrixAddr, matrix, sizeof(float) * 4 * 4);

	float self_location[3];
	get_self_location(self_location);

	int self_camp = GetSelfCamp(Players);

	for (int i = 0; i < g_players_count; i++)
	{
		int x, y, w, h;
		if (Players[i].effective && Players[i].blood > 1 && Players[i].self == false && to_rect_info(matrix, Players[i].location, window_w, window_h, x, y, w, h))
		{
			D3DCOLOR color = NULL;


			if (self_camp != Players[i].camp)
			{
				color = D3DCOLOR_XRGB(255, 137, 0);

				Players[i].aimbot_len = get_aimbot_len(window_w, window_h, x + (w / 2), y + (h / 2));

				render_rect(color, x, y, w, h);

				render_player_blood(Players[i].blood, x - 5, y, h);
			}

		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

//设置自己的辨认值
void SetSelfCamp(PlayerList* Players)
{
	Players[0].self = true;
}

//获取自己的阵营
int GetSelfCamp(PlayerList* Players)
{
	return Players[0].camp;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

//获取鼠标左键按下操作
bool get_mouse_left_down()
{
	return GetAsyncKeyState(VK_LBUTTON) & 0x8000;
}
///////////////////////////////////////////////////////////////////////////////////////////////////

//获取最近的敌人
int GetAimIndexHeadLocation(PlayerList* Players)
{
	//初始化索引值
	int index = -1;
	
	//初始化阵营，获取自己的阵营
	int camp = GetSelfCamp(Players);

	//检索所有玩家
	for (int i = 0; i < g_players_count; i++) 
	{
		//如果玩家有效，且阵营为其它阵营
		if (Players[i].effective && camp != Players[i].camp) 
		{
			//获取索引值
			if (index == -1)index = i;

			//如果索引瞄准线长度比现获取目标还要大，那么传递索引值
			else if (Players[index].aimbot_len > Players[i].aimbot_len)index = i;

		}

	}

	return index;
}
///////////////////////////////////////////////////////////////////////////////////////////////////

//获取当前角度
void get_current_angle(float* angle)
{
	//获取当前的角度
	read_memory(g_ProcessHandle, g_AngleAddr, angle, sizeof(float) * 2);
	
}

//获取自瞄角度
void get_aimbot_angle(float* self_location, float* player_location, float* aim_angle)
{
	float x = self_location[0] - player_location[0];
	float y = self_location[1] - player_location[1];
	float z = self_location[2] - player_location[2] + 65.0f;

	const float pi = 3.1415f;
	aim_angle[0] = (float)atan(z / sqrt(x * x + y * y)) / pi * 180.f;
	aim_angle[1] = (float)atan(y / x);

	if (x >= 0.0f && y >= 0.0f) aim_angle[1] = aim_angle[1] / pi * 180.0f - 180.0f;
	else if (x < 0.0f && y >= 0.0f) aim_angle[1] = aim_angle[1] / pi * 180.0f;
	else if (x < 0.0f && y < 0.0f) aim_angle[1] = aim_angle[1] / pi * 180.0f;
	else if (x >= 0.0f && y < 0.0f) aim_angle[1] = aim_angle[1] / pi * 180.f + 180.0f;
}

///////////////////////////////////////////////////////////////////////////////////////////////////


//自瞄操作
void AimbotPlayers(PlayerList* Players, float max_fov = 50)
{
	//首先获取自己的位置
	float self_location[3];
	get_self_location(self_location);

	//获取自瞄的人,自瞄最接近的人
	int AimIndex = GetAimIndexHeadLocation(Players);

	//没找到的情况
	if (AimIndex == -1) return;

	//获取当前的角度
	float current_angle[2];
	get_current_angle(current_angle);

	//printf("current_angle:%f,%f\n", current_angle[0], current_angle[1]);

	//设置自瞄角度
	float aim_angle[2];
	
	//printf("Players[AimIndex].head_bone:%f,%f,%f\n", Players[AimIndex].head_bone[0], Players[AimIndex].head_bone[1], Players[AimIndex].head_bone[2]);

	get_aimbot_angle(self_location, Players[AimIndex].head_bone, aim_angle);

	//printf("aim_angle:%f,%f\n",aim_angle[0], aim_angle[1]);

	if (abs((int)aim_angle[0] - (int)current_angle[0]) > max_fov
		|| abs((int)aim_angle[1] - (int)current_angle[1]) > max_fov)
		return;

	//写入自瞄角度
	write_memory(g_ProcessHandle, g_AngleAddr, aim_angle, sizeof(float) * 2);


}



///////////////////////////////////////////////////////////////////////////////////////////////////


//工作内容
void CheatDoing()
{
	//定义玩家数组，用来存储
	PlayerList Players[g_players_count]{ 0 };

	//获取玩家信息
	GetPlayerList(Players);

	SetSelfCamp(Players);

	render_player_box(Players);

	if (get_mouse_left_down())AimbotPlayers(Players,90);
}


///////////////////////////////////////////////////////////////////////////////////////////////////

//开始工作
void StartCheater() 
{
	//获取程序句柄，地址等信息
	InitializeAddress(ProcessName);
	
	//当执行绘制时
	g_cheating = CheatDoing;

	//寻找需要绘制在那个窗口上
	HWND game_hwnd = FindWindowA("Valve001", "Counter-Strike Source");
	HWND transparent_hwnd = create_transparent_window(game_hwnd);
	
	//赋值给全局变量
	g_game_hwnd = game_hwnd;
	g_transparent_hwnd = transparent_hwnd;

	//初始化设备
	initialize_direct3d9(transparent_hwnd);

	//处理事件
	message_handle(game_hwnd, transparent_hwnd);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
