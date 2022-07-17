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

#include "SystemDefine.h"
#include "MaterialArchiveFile.h"
#include "XOR.h"

extern void __app_safe_exit_error__();

class MyDirectXImage_3D
{
private:
	IDirect3DDevice9* pDevice3D;

	//LPDIRECT3DVERTEXBUFFER9 pVB = NULL;

	LPDIRECT3DTEXTURE9 pTexture = NULL;

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

public:
	MyDirectXImage_3D(IDirect3DDevice9* pDevice3D, const char* texture_filename);
	~MyDirectXImage_3D();

	void draw(VERTEX_XYZ_COLOR_UV Vertices[4], FLOAT angle_x, FLOAT angle_y, FLOAT angle_z, D3DXVECTOR3 translation_vector, D3DXVECTOR3 camera_location);
};
