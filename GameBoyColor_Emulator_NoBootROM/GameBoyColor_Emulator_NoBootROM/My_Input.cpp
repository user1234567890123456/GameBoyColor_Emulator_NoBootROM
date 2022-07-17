#include "My_Input.h"

My_Input* My_Input::singleton = nullptr;

My_Input* My_Input::get_instance_ptr() {
	return singleton;
}

void My_Input::_make_singleton(HWND window_handle, HINSTANCE hInstance) {
	M_debug_printf("My_Input::_make_singleton()...\n");

	if (singleton == nullptr) {
		singleton = new My_Input(window_handle, hInstance);
	}
}

void My_Input::_delete_singleton() {
	M_debug_printf("My_Input::_delete_singleton()...\n");

	delete singleton;
}

BOOL CALLBACK DeviceFindCallBack(LPCDIDEVICEINSTANCE ipddi, LPVOID pvRef)
{
	M_debug_printf("DeviceFindCallBack() START\n");
	
	HRESULT result;
	callback_arg *arg = (callback_arg*)pvRef;

	LPDIRECTINPUT8 lp_d_input_2 = (arg->lp_d_input);
	LPDIRECTINPUTDEVICE8 *lp_device_2 = &(arg->lp_device);

	if ((*lp_device_2) != nullptr) {
		M_debug_printf("DeviceFindCallBack() arg->lp_device->Release()\n");

		(*lp_device_2)->Unacquire();
		(*lp_device_2)->Release();
		(*lp_device_2) = nullptr;
	}

	result = lp_d_input_2->CreateDevice(ipddi->guidInstance, lp_device_2, NULL);
	if (FAILED(result)) {
		M_debug_printf("DeviceFindCallBack() err 1\n");

		return DIENUM_CONTINUE;
	}

	result = (*lp_device_2)->SetDataFormat(&c_dfDIJoystick);
	if (FAILED(result)) {
		M_debug_printf("DeviceFindCallBack() err 2\n");

		return DIENUM_CONTINUE;
	}

	result = (*lp_device_2)->SetCooperativeLevel(arg->window_handle, /*DISCL_EXCLUSIVE*/DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if (FAILED(result)) {
		M_debug_printf("DeviceFindCallBack() err 3\n");

		return DIENUM_CONTINUE;
	}

	DIPROPDWORD diprop;
	memset(&diprop, 0, sizeof(diprop));
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.diph.dwObj = 0;
	diprop.dwData = DIPROPAXISMODE_ABS;

	result = (*lp_device_2)->SetProperty(DIPROP_AXISMODE, &diprop.diph);
	if (FAILED(result)) {
		M_debug_printf("DeviceFindCallBack() err 4\n");

		return DIENUM_CONTINUE;
	}

	DIPROPRANGE diprg;
	memset(&diprg, 0, sizeof(diprg));
	diprg.diph.dwSize = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwHow = DIPH_BYOFFSET;
	diprg.diph.dwObj = DIJOFS_X;
	diprg.lMin = -1000;
	diprg.lMax = 1000;
	result = (*lp_device_2)->SetProperty(DIPROP_RANGE, &diprg.diph);
	if (FAILED(result)) {
		M_debug_printf("DeviceFindCallBack() err 5\n");

		return DIENUM_CONTINUE;
	}

	diprg.diph.dwObj = DIJOFS_Y;
	result = (*lp_device_2)->SetProperty(DIPROP_RANGE, &diprg.diph);
	if (FAILED(result)) {
		M_debug_printf("DeviceFindCallBack() err 6\n");

		return DIENUM_CONTINUE;
	}

	(*lp_device_2)->Acquire();

	(*lp_device_2)->Poll();

	M_debug_printf("DeviceFindCallBack() END\n");

	return DIENUM_CONTINUE;
}

My_Input::My_Input(HWND window_handle, HINSTANCE hInstance)
	: window_handle(window_handle), hInstance(hInstance) {
	M_debug_printf("My_Input::My_Input() START\n");

	init();

	memset(key_byte_data, 0, sizeof(key_byte_data));
	memset(pad_byte_data, 0, sizeof(pad_byte_data));

	M_debug_printf("My_Input::My_Input() END\n");
}

My_Input::~My_Input() {
	M_debug_printf("My_Input::~My_Input() START\n");

	end();

	M_debug_printf("My_Input::~My_Input() END\n");
}

void My_Input::init() {
	HRESULT result;

	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&lpDI, NULL);
	if (FAILED(result)) {
		M_debug_printf("My_Input::init() err 1\n");

		MessageBox(NULL, _T("DirectInputの初期化の際にエラーが発生しました"), _T("ERROR"), MB_OK | MB_ICONERROR);

		__app_safe_exit_error__();

		return;
	}

	init_key();
	init_pad();
}

void My_Input::init_key() {
	HRESULT result;

	result = lpDI->CreateDevice(GUID_SysKeyboard, &lpDIKeyDevice, NULL);
	if (FAILED(result)) {
		M_debug_printf("My_Input::init_key() err 1\n");

		lpDI->Release();

		MessageBox(NULL, _T("DirectInputの初期化の際にエラーが発生しました"), _T("ERROR"), MB_OK | MB_ICONERROR);

		__app_safe_exit_error__();

		return;
	}

	result = lpDIKeyDevice->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result)) {
		M_debug_printf("My_Input::init_key() err 2\n");

		lpDI->Release();
		lpDIKeyDevice->Release();

		MessageBox(NULL, _T("DirectInputの初期化の際にエラーが発生しました"), _T("ERROR"), MB_OK | MB_ICONERROR);

		__app_safe_exit_error__();

		return;
	}

	result = lpDIKeyDevice->SetCooperativeLevel(window_handle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	if (FAILED(result)) {
		M_debug_printf("My_Input::init_key() err 3\n");

		lpDI->Release();
		lpDIKeyDevice->Release();

		MessageBox(NULL, _T("DirectInputの初期化の際にエラーが発生しました"), _T("ERROR"), MB_OK | MB_ICONERROR);

		__app_safe_exit_error__();

		return;
	}

	lpDIKeyDevice->Acquire();
}

void My_Input::init_pad() {
	find_and_update_pad();
}

void My_Input::end() {
	if (lpDIKeyDevice) {
		lpDIKeyDevice->Unacquire();
		lpDIKeyDevice->Release();
	}

	if (c_arg.lp_device != nullptr) {
		c_arg.lp_device->Unacquire();
		c_arg.lp_device->Release();
	}

	if (lpDI != nullptr) {
		lpDI->Release();
	}
}

///*
//今までのpadがあれば解放して新しいpadを探して更新する
//*/
void My_Input::find_and_update_pad() {
	HRESULT result;

	if (c_arg.lp_device != nullptr) {
		c_arg.lp_device->Unacquire();
		c_arg.lp_device->Release();
		c_arg.lp_device = nullptr;
	}

	c_arg = { 0 };
	c_arg.lp_d_input = lpDI;
	c_arg.lp_device = nullptr;
	c_arg.window_handle = window_handle;
	c_arg.callback_count = 0;

	result = lpDI->EnumDevices(DI8DEVTYPE_JOYSTICK, DeviceFindCallBack, &c_arg, DIEDFL_ATTACHEDONLY);
	if (FAILED(result)) {
		M_debug_printf("My_Input::init_pad() err 1\n");
	
		//lpDI->Release();
	
		return;
	}

	result = lpDI->EnumDevices(DI8DEVTYPE_GAMEPAD, DeviceFindCallBack, &c_arg, DIEDFL_ATTACHEDONLY);
	if (FAILED(result)) {
		M_debug_printf("My_Input::init_pad() err 1\n");
	
		//lpDI->Release();
	
		return;
	}
}

void My_Input::update_process() {
	key_update();
	pad_update();
}

void My_Input::key_update() {
	memset(tmp_key_byte_data, 0, sizeof(tmp_key_byte_data));//key_dataをリセット
	HRESULT result;
	result = lpDIKeyDevice->GetDeviceState(sizeof(tmp_key_byte_data), tmp_key_byte_data);
	if (FAILED(result)) {
		lpDIKeyDevice->Acquire();

		memset(tmp_key_byte_data, 0, sizeof(tmp_key_byte_data));
		lpDIKeyDevice->GetDeviceState(sizeof(tmp_key_byte_data), tmp_key_byte_data);
	}

	//###############################################

	for (int i = 0; i < 256; i++) {
		if ((tmp_key_byte_data[i] & 0b10000000) != 0) {
			key_byte_data[i]++;
			if (key_byte_data[i] > 2) {
				key_byte_data[i] = 2;
			}
		}
		else {
			key_byte_data[i] = 0;
		}
	}
}

void My_Input::pad_update() {
	if (c_arg.lp_device != nullptr) {//PADがあるとき
		HRESULT result;

		memset(&pad_data, 0, sizeof(pad_data));
		LPDIRECTINPUTDEVICE8 lpDIPadDevice = c_arg.lp_device;

		result = lpDIPadDevice->GetDeviceState(sizeof(DIJOYSTATE), &pad_data);
		if (FAILED(result)) {
			lpDIPadDevice->Acquire();
			lpDIPadDevice->Poll();

			memset(&pad_data, 0, sizeof(pad_data));
			lpDIPadDevice->GetDeviceState(sizeof(DIJOYSTATE), &pad_data);
		}

		//############################################################################

		memset(pad_bool_data, 0, sizeof(pad_bool_data));

		for (int i = 0; i < 32; i++) {
			if ((pad_data.rgbButtons[i] & 0b10000000) != 0) {
				//M_debug_printf("PAD %d button pressed\n", i);

				pad_bool_data[i] = true;
			}
		}

		DWORD pov = pad_data.rgdwPOV[0];
		if (pad_data.rgdwPOV[0] != 0xFFFFFFFF) {
			//M_debug_printf("PAD pov button pressed\n");

			switch (pad_data.rgdwPOV[0])
			{
			//上
			case 0:
				pad_bool_data[PAD_DATA_INDEX_UP] = true;

				break;
			//右上
			case 4500:
				pad_bool_data[PAD_DATA_INDEX_UP] = true;

				pad_bool_data[PAD_DATA_INDEX_RIGHT] = true;

				break;
			//右
			case 9000:
				pad_bool_data[PAD_DATA_INDEX_RIGHT] = true;

				break;
			//右下
			case 13500:
				pad_bool_data[PAD_DATA_INDEX_RIGHT] = true;

				pad_bool_data[PAD_DATA_INDEX_DOWN] = true;

				break;
			//下
			case 18000:
				pad_bool_data[PAD_DATA_INDEX_DOWN] = true;

				break;
			//左下
			case 22500:
				pad_bool_data[PAD_DATA_INDEX_LEFT] = true;

				pad_bool_data[PAD_DATA_INDEX_DOWN] = true;

				break;
			//左
			case 27000:
				pad_bool_data[PAD_DATA_INDEX_LEFT] = true;

				break;
			//左上
			case 31500:
				pad_bool_data[PAD_DATA_INDEX_LEFT] = true;

				pad_bool_data[PAD_DATA_INDEX_UP] = true;

				break;
			}
		}

		for (int j = 0; j < PAD_DATA_NO; j++) {
			if (pad_bool_data[j] == true) {
				pad_byte_data[j]++;
				if (pad_byte_data[j] > 2) {
					pad_byte_data[j] = 2;
				}
			}
			else {
				pad_byte_data[j] = 0;
			}
		}

		//M_debug_printf("POV = %ld\n", pov);
		//if (pad_byte_data[PAD_DATA_INDEX_UP] == 1) {
		//	M_debug_printf("上");
		//}
		//if (pad_byte_data[PAD_DATA_INDEX_DOWN] == 1) {
		//	M_debug_printf("下");
		//}
		//if (pad_byte_data[PAD_DATA_INDEX_LEFT] == 1) {
		//	M_debug_printf("左");
		//}
		//if (pad_byte_data[PAD_DATA_INDEX_RIGHT] == 1) {
		//	M_debug_printf("右");
		//}

	}
}
