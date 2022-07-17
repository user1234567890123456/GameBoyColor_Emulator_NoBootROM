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

#include "DebugUtility.h"

extern void __app_safe_exit_error__();

class MyDirectXSystem
{
private:
	IDirect3D9 *pD3D9;
	IDirect3DDevice9 *pDevice3D;
public:
	MyDirectXSystem(HWND h_wnd, int width, int height, int frame_process_type, D3DPRESENT_PARAMETERS* g_D3DParam, BOOL window_mode_flag);
	~MyDirectXSystem();

	IDirect3D9 *get_pD3D9()
	{
		return pD3D9;
	}

	IDirect3DDevice9 *get_pDevice3D()
	{
		return pDevice3D;
	}

	void set_pDevice3D(IDirect3DDevice9* pDevice3D)
	{
		this->pDevice3D = pDevice3D;
	}

};
