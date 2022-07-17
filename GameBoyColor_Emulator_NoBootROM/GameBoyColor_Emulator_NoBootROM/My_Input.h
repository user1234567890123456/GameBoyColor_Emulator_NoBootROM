#pragma once

#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>

#include <iostream>
#include <fstream>

#include "DebugUtility.h"

using namespace std;

struct callback_arg {
	LPDIRECTINPUT8 lp_d_input;
	LPDIRECTINPUTDEVICE8 lp_device;
	HWND window_handle;
	int callback_count;
};

extern void __app_safe_exit_error__();

class My_Input
{
private:
	static My_Input* singleton;

	BYTE mix_byte_data[ALL_KEY_NUMBER];//キー入力とパッド入力を合わせたもの

	BYTE tmp_key_byte_data[ALL_KEY_NUMBER];
	BYTE key_byte_data[ALL_KEY_NUMBER];

	DIJOYSTATE pad_data;

	//32 = button
	//4 = up,down,left,right
#define PAD_BUTTON_DATA_NO 32
#define PAD_BUTTON_POV_DATA_NO 4
#define PAD_DATA_NO (PAD_BUTTON_DATA_NO + PAD_BUTTON_POV_DATA_NO)

#define PAD_DATA_INDEX_UP (32)
#define PAD_DATA_INDEX_DOWN (32 + 1)
#define PAD_DATA_INDEX_LEFT (32 + 2)
#define PAD_DATA_INDEX_RIGHT (32 + 3)

#define PAD_A_NO 3
#define PAD_B_NO 2
#define PAD_START_NO 10
#define PAD_SELECT_NO 11

	bool pad_bool_data[PAD_DATA_NO];
	BYTE pad_byte_data[PAD_DATA_NO];

	HWND window_handle;
	HINSTANCE hInstance;

	callback_arg c_arg = { 0 };

	LPDIRECTINPUT8 lpDI = nullptr;
	LPDIRECTINPUTDEVICE8 lpDIKeyDevice = nullptr;
	//LPDIRECTINPUTDEVICE8 lpDIPadDevice = nullptr;

	void init();
	void end();

	void init_key();
	void init_pad();

	void key_update();
	void pad_update();
	
	My_Input(HWND window_handle, HINSTANCE hInstance);
	~My_Input();
public:
	static My_Input* get_instance_ptr();

	static void _make_singleton(HWND window_handle, HINSTANCE hInstance);
	static void _delete_singleton();

	void find_and_update_pad();

	void update_process();

	void get_raw_kay_data(BYTE *key_out_data) {
		memcpy(key_out_data, key_byte_data, sizeof(key_byte_data));
	}

	void get_raw_pad_data(BYTE* pad_out_data) {
		memcpy(pad_out_data, pad_byte_data, sizeof(pad_byte_data));
	}
};
