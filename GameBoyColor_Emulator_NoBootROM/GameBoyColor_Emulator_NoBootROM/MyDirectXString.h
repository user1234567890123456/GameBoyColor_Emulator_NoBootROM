#pragma once

#include <map>
#include <tchar.h>

#include "MyDirectXChar.h"
#include "MyDirectXDraw.h"

using namespace std;

class MyDirectXString
{
private:
	map<uint16_t, MyDirectXChar*> char_list;

	MyDirectXSystem* myDirectXSystem;
	LOGFONT _lf;

	void prev_process(TCHAR* string);

	void abstract_draw_process(TCHAR* string, float x, float y, UINT color);
public:
	MyDirectXString(MyDirectXSystem* myDirectXSystem, LOGFONT* lf);
	~MyDirectXString();

	void draw_process_base_leftup(TCHAR* string, float x, float y, UINT color);
	void draw_process_base_leftup_with_edge(TCHAR* string, float x, float y, UINT color, UINT edge_color);
	void draw_process_base_center(TCHAR* string, float x, float y, UINT color);
	void draw_process_base_center_with_edge(TCHAR* string, float x, float y, UINT color, UINT edge_color);
	void draw_process_base_rightup(TCHAR* string, float x, float y, UINT color);
	void draw_process_base_rightup_with_edge(TCHAR* string, float x, float y, UINT color, UINT edge_color);
	void draw_process_base_leftdown(TCHAR* string, float x, float y, UINT color);
	void draw_process_base_leftdown_with_edge(TCHAR* string, float x, float y, UINT color, UINT edge_color);
};
