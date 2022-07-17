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

#include "SystemDefine.h"
#include "MyDirectXSystem.h"
#include "MyDirectXImage.h"
#include "MyDirectXFont.h"
#include "MyFont.h"

class MyDirectXDraw
{
private:
	MyDirectXDraw();
	~MyDirectXDraw();

	static void fix_vertex_pos__VERTEX_XYZ_RHW_COLOR(VERTEX_XYZ_RHW_COLOR* vertex);
	static void fix_vertex_pos__VERTEX_XYZ_RHW_COLOR_UV(VERTEX_XYZ_RHW_COLOR_UV* vertex);
public:
	static void set_sampler_state__font(MyDirectXSystem* myDirectXSystem);
	static void set_sampler_state__2d(MyDirectXSystem* myDirectXSystem);
	static void set_sampler_state__2d_2(MyDirectXSystem* myDirectXSystem);
	static void set_sampler_state__3d(MyDirectXSystem* myDirectXSystem);
	static void set_sampler_state__3d_2(MyDirectXSystem* myDirectXSystem);

	static void set_default_state(MyDirectXSystem* myDirectXSystem);
	static void disable_alpha(MyDirectXSystem *myDirectXSystem);
	static void enable_alpha(MyDirectXSystem *myDirectXSystem);


	static void draw_box_leftup(MyDirectXSystem *myDirectXSystem, float start_x, float start_y, float end_x, float end_y, DWORD color);

	static void draw_texture_base_leftup_enable_size(MyDirectXSystem* myDirectXSystem, LPDIRECT3DTEXTURE9 pTexture, float texture_src_width, float texture_src_height, float x, float y, double percent, UINT color = 0xFFFFFFFF);
	static void draw_texture_base_leftup(MyDirectXSystem* myDirectXSystem, LPDIRECT3DTEXTURE9 pTexture, float texture_src_width, float texture_src_height, float x, float y, UINT color = 0xFFFFFFFF);

	static void draw_image_base_leftup(MyDirectXSystem* myDirectXSystem, MyDirectXImage* image, float x, float y, UINT color = 0xFFFFFFFF);
	static void draw_image_base_rightup(MyDirectXSystem* myDirectXSystem, MyDirectXImage* image, float x, float y, UINT color = 0xFFFFFFFF);
	static void draw_image_base_rightdown(MyDirectXSystem* myDirectXSystem, MyDirectXImage* image, float x, float y, UINT color = 0xFFFFFFFF);
	static void draw_image_base_center(MyDirectXSystem* myDirectXSystem, MyDirectXImage* image, float x, float y, UINT color = 0xFFFFFFFF);
	static void draw_cut_image_base_leftup(MyDirectXSystem* myDirectXSystem, MyDirectXImage* image, float x, float y, RECT draw_rect, UINT color = 0xFFFFFFFF);
	static void draw_cut_image_base_center(MyDirectXSystem* myDirectXSystem, MyDirectXImage* image, float x, float y, RECT draw_rect, const int width, const int height, UINT color = 0xFFFFFFFF);
	static void draw_image_base_leftup_settable_angle(MyDirectXSystem* myDirectXSystem, MyDirectXImage* image, float x, float y, double angle_x, double angle_y, double angle_z, UINT color = 0xFFFFFFFF);
	static void draw_image_base_center_settable_angle_enable_size(MyDirectXSystem* myDirectXSystem, MyDirectXImage* image, float x, float y, double angle_x, double angle_y, double angle_z, double size_percent, UINT color = 0xFFFFFFFF);
	static void draw_image_base_center_settable_angle(MyDirectXSystem* myDirectXSystem, MyDirectXImage* image, float x, float y, double angle_x, double angle_y, double angle_z, UINT color = 0xFFFFFFFF);
	static void draw_image_base_center_enable_size(MyDirectXSystem* myDirectXSystem, MyDirectXImage* image, float x, float y, double size_percent, UINT color = 0xFFFFFFFF);

	//static void __OLD_draw_image_base_leftup__(LPD3DXSPRITE sprite, MyDirectXImage* image, float x, float y, UINT color = 0xFFFFFFFF);
	//static void __OLD_draw_image_base_leftup_settable_angle__(LPD3DXSPRITE sprite, MyDirectXImage *image, float x, float y, double angle_x, double angle_y, double angle_z, UINT color = 0xFFFFFFFF);
	//static void __OLD_draw_image_base_rightup__(LPD3DXSPRITE sprite, MyDirectXImage *image, float x, float y, UINT color = 0xFFFFFFFF);
	//static void __OLD_draw_image_base_rightdown__(LPD3DXSPRITE sprite, MyDirectXImage *image, float x, float y, UINT color = 0xFFFFFFFF);
	//static void __OLD_draw_image_base_center__(LPD3DXSPRITE sprite, MyDirectXImage *image, float x, float y, UINT color = 0xFFFFFFFF);
	//static void __OLD_draw_image_base_center_settable_angle__(LPD3DXSPRITE sprite, MyDirectXImage *image, float x, float y, double angle_x, double angle_y, double angle_z, UINT color = 0xFFFFFFFF);
	//static void __OLD_draw_image_base_center_enable_size__(LPD3DXSPRITE sprite, MyDirectXImage* image, float x, float y, double size_percent, UINT color = 0xFFFFFFFF);
	//static void __OLD_draw_image_base_center_settable_angle_enable_size__(LPD3DXSPRITE sprite, MyDirectXImage* image, float x, float y, double angle_x, double angle_y, double angle_z, double size_percent, UINT color = 0xFFFFFFFF);

	/*
	êÿÇËî≤Ç´ï`âÊ
	*/
	//static void __OLD_draw_cut_image_base_leftup__(LPD3DXSPRITE sprite, MyDirectXImage *image, float x, float y, RECT draw_rect, UINT color = 0xFFFFFFFF);
	//static void __OLD_draw_cut_image_base_center__(LPD3DXSPRITE sprite, MyDirectXImage *image, float x, float y, RECT draw_rect, const int width, const int height, UINT color = 0xFFFFFFFF);
	
	//static void draw_font_base_leftup(LPD3DXSPRITE sprite, MyDirectXFont *font, TCHAR *str, float x, float y, UINT color = FONT_COLOR_BLACK);
	//static void draw_font_base_rightup(LPD3DXSPRITE sprite, MyDirectXFont *font, TCHAR *str, float x, float y, UINT color = FONT_COLOR_BLACK);
	//static void draw_font_base_center(LPD3DXSPRITE sprite, MyDirectXFont *font, TCHAR *str, float x, float y, UINT color = FONT_COLOR_BLACK);
};
