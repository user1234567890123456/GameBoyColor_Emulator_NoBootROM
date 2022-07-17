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

#include "MyDirectXSystem.h"
#include "MyDirectXFont.h"
#include "MyDirectXDraw.h"
//#include "FpsFont.h"
#include "MyDirectXString.h"

#define FPS 60

class Fps
{
private:
	//LPD3DXSPRITE sprite;

	//MyFont *fps_number;
	MyDirectXString* fps_font;

	double fps = 0.0;
	int frame = 0;
	DWORD start_time = 0;
public:
	Fps(MyDirectXSystem *myDirectXSystem/*, LPD3DXSPRITE sprite*/);
	~Fps();

	void draw_fps();
	void update();
	void wait();
};
