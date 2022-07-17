#pragma once

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d9.lib")

#include <tchar.h>
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "DebugUtility.h"
#include "SystemDefine.h"
#include "MyDirectXSystem.h"

#define FVF_CUSTOM (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

class MyDirectXChar
{
private:
	bool skip_flag = false;

	float char_width;
	float char_height;

	MyDirectXSystem* myDirectXSystem;

	LPDIRECT3DTEXTURE9 pTexture;
	//IDirect3DVertexBuffer9* pVertex;

	VERTEX_XYZ_RHW_COLOR_UV base_vertex[4];
public:
	MyDirectXChar(MyDirectXSystem* myDirectXSystem, TCHAR* draw_char, LOGFONT* lf);
	~MyDirectXChar();

	void draw(float x, float y, UINT color);

	float get_width();
	float get_height();
};
