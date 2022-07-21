/*
####################################################

このヘッダファイルではプログラム全体で使う定数などを定義する

####################################################
*/

#pragma once

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3dxof.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")
#include <comdef.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <mmsystem.h>
#include <tchar.h>


#define __VERSION_STR__ _T("VERSION 1.00 nobootrom")

#define __MUTEX_NAME__ "dwVc8ei39pMQCvx1k4lIku2s7kSMYR1i"


//#define GAMEBOYCOLOR_EMULATOR_DEBUG

#define TITLE "ゲームボーイカラーのエミュレーター"
//#define TITLE "ゲームボーイカラーのエミュレーター(BootROMなしのやつ)"
//#define TITLE "GameboyColor Emulator"

#define GBX_WIDTH 160
#define GBX_HEIGHT 144

#define WINDOW_WIDTH (GBX_WIDTH * 4)//WINDOW_WIDTH (GBX_WIDTH * 4)
#define WINDOW_HEIGHT (GBX_HEIGHT * 4)


#define ALL_MIX_INPUT_NUMBER 8

#define INPUT_MY_ID_A 0
#define INPUT_MY_ID_B 1
#define INPUT_MY_ID_SELECT 2
#define INPUT_MY_ID_START 3
#define INPUT_MY_ID_UP 4
#define INPUT_MY_ID_DOWN 5
#define INPUT_MY_ID_LEFT 6
#define INPUT_MY_ID_RIGHT 7


#define ALL_KEY_NUMBER 256

/*
A
B
SELECT
START
UP
DOWN
LEFT
RIGHT
*/
//#define KEY_INPUT_NUMBER 8


#define SETTING_FILENAME "setting.savedata"
#define SETTING_FILENAME_PATH "Setting\\setting.savedata"


//#define LAUNCH_FILENAME_PATH "launch.data"


#define GET_COLOR_ALPHA255(r, g, b) ((0xFF * 0x1000000) + (r * 0x10000) + (g * 0x100) + b)

#define GET_5BIT_COLOR_ALPHA255(r, g, b) ((0xFF * 0x1000000) + ((r << 3) * 0x10000) + ((g << 3) * 0x100) + (b << 3))

struct my_rgb {
	int r;
	int g;
	int b;
};

/*
フォントの色定義
*/
#define FONT_COLOR_BLACK 0xFF000000
#define FONT_COLOR_WHITE 0xFFFFFFFF
#define FONT_COLOR_RED 0xFFFF0000
#define FONT_COLOR_GREEN 0xFF00FF00
#define FONT_COLOR_BLUE 0xFF0000FF

struct VERTEX_XYZ_COLOR_UV
{
	float x;
	float y;
	float z;
	DWORD color;
	float u;
	float v;
};

struct VERTEX_XYZ_RHW_COLOR {
	float x;
	float y;
	float z;
	float rhw;
	DWORD color;
};

struct VERTEX_XYZ_RHW_COLOR_UV {
	float x;
	float y;
	float z;
	float rhw;
	DWORD color;
	float u;
	float v;
};

#define ONE_TIME_FRAME 59//0～59

#define ONE_ANGLE (D3DX_PI / 180)

const uint32_t INIT_BGM_VOLUME = 30;
const uint32_t INIT_SE_VOLUME = 20;
const bool INIT_GAMEPLAY_SE_FLAG = true;

struct found_info {
	uint16_t address;
	uint16_t value;
	uint16_t prev_value;
};

struct resident_cheat_info {
	size_t code_total_number;
	uint32_t* resident_cheat_code_list_ptr;
};
