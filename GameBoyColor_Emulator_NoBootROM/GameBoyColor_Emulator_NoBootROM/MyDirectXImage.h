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

#include "MaterialArchiveFile.h"
#include "XOR.h"

extern void __app_safe_exit_error__();

class MyDirectXImage
{
private:
	IDirect3DTexture9 *texture;

	D3DXIMAGE_INFO src_image_info;
	D3DSURFACE_DESC texture_info;
public:
	MyDirectXImage(TCHAR *FileName, IDirect3DDevice9 *pDevice3D);
	~MyDirectXImage();

	IDirect3DTexture9 *get_texture()
	{
		return texture;
	}

	int get_texture_width()
	{
		return texture_info.Width;
	}

	int get_texture_height()
	{
		return texture_info.Height;
	}

	int get_src_image_width()
	{
		return src_image_info.Width;
	}

	int get_src_image_height()
	{
		return src_image_info.Height;
	}

};
