#pragma once

#include "DebugUtility.h"
#include "MyFont.h"
#include "MyDirectXDraw.h"

class FpsFont : public MyFont
{
public:
	FpsFont(MyDirectXSystem* myDirectXSystem);
	~FpsFont();

	int get_font_index(TCHAR chk_ch) override;
	void draw_number_leftup(TCHAR *str, float x, float y, UINT color = 0xFFFFFFFF) override;
	void draw_number_rightup(TCHAR *str, float x, float y, UINT color = 0xFFFFFFFF) override;
};
