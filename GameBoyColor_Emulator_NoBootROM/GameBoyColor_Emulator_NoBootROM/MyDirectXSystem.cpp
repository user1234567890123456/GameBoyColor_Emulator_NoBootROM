#include "MyDirectXSystem.h"

MyDirectXSystem::MyDirectXSystem(HWND h_wnd, int width, int height, int frame_process_type, D3DPRESENT_PARAMETERS* g_D3DParam, BOOL window_mode_flag)
{
	pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (pD3D9 == NULL) {
		MessageBox(NULL, _T("DirectXの初期化の際にエラーが発生しました"), _T("ERROR"), MB_OK | MB_ICONERROR);

		__app_safe_exit_error__();

		return;
	}

	D3DDISPLAYMODE displayMode;
	if (FAILED(pD3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode))) {
		MessageBox(NULL, _T("DirectXの初期化の際にエラーが発生しました"), _T("ERROR"), MB_OK | MB_ICONERROR);

		__app_safe_exit_error__();

		return;
	}

	if (frame_process_type == 0 || frame_process_type == 2) {//垂直同期ONの時
		M_debug_printf("#########################################\n");
		M_debug_printf("MyDirectXSystem>>>垂直同期ON\n");
		M_debug_printf("#########################################\n");

		*g_D3DParam = {
			(unsigned int)width,
			(unsigned int)height,
			displayMode.Format,
			1,
			D3DMULTISAMPLE_NONE,
			0,
			D3DSWAPEFFECT_DISCARD,
			h_wnd,
			window_mode_flag,
			TRUE,
			D3DFMT_D24S8,
			0,
			0,
			D3DPRESENT_INTERVAL_DEFAULT,
		};
	}
	else {//垂直同期OFFの時
		M_debug_printf("#########################################\n");
		M_debug_printf("MyDirectXSystem>>>垂直同期OFF\n");
		M_debug_printf("#########################################\n");

		*g_D3DParam = {
			(unsigned int)width,
			(unsigned int)height,
			displayMode.Format,
			1,
			D3DMULTISAMPLE_NONE,
			0,
			D3DSWAPEFFECT_DISCARD,
			h_wnd,
			window_mode_flag,
			TRUE,
			D3DFMT_D24S8,
			0,
			0,
			D3DPRESENT_INTERVAL_IMMEDIATE,
		};
	}

	/*
	3Dデバイスを作成
	*/
	if (FAILED(pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_D3DParam->hDeviceWindow,
		D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, g_D3DParam, &pDevice3D)) == TRUE) {
		pD3D9->Release();

		if (FAILED(pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_D3DParam->hDeviceWindow,
			D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, g_D3DParam, &pDevice3D)) == TRUE) {
			pD3D9->Release();

			if (FAILED(pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, g_D3DParam->hDeviceWindow,
				D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, g_D3DParam, &pDevice3D)) == TRUE) {
				pD3D9->Release();

				if (FAILED(pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, g_D3DParam->hDeviceWindow,
					D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, g_D3DParam, &pDevice3D)) == TRUE) {
					/*
					DirectXが使えないとき
					*/

					pD3D9->Release();

					MessageBox(NULL, _T("DirectXの初期化の際にエラーが発生しました"), _T("ERROR"), MB_OK | MB_ICONERROR);

					__app_safe_exit_error__();

					return;
				}
			}
		}
	}

	pDevice3D->SetRenderState(D3DRS_ZENABLE, TRUE);
	//pDevice3D->SetRenderState(D3DRS_LIGHTING, FALSE);//ライティングOFF
}

MyDirectXSystem::~MyDirectXSystem()
{
	if (pD3D9 != nullptr) {
		pD3D9->Release();
	}
	if (pDevice3D != nullptr) {
		pDevice3D->Release();
	}
}
