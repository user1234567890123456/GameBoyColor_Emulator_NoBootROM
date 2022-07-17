#pragma once

#include <tchar.h>

#include "MyDirectXImage.h"
#include "MyDirectXSystem.h"

#define SPACE_FONT_R_CODE -2

class MyFont
{
protected:
	MyDirectXSystem* myDirectXSystem;

	const int NUMBER_IMAGE_WIDTH = 11;
	const int NUMBER_IMAGE_HEIGHT = 18;

	MyDirectXImage* number_image;
public:
	MyFont(MyDirectXSystem* myDirectXSystem, const int NUMBER_IMAGE_WIDTH, const int NUMBER_IMAGE_HEIGHT, TCHAR *filename);
	virtual ~MyFont();

	virtual int get_font_index(TCHAR chk_ch) = 0;
	virtual void draw_number_leftup(TCHAR *str, float x, float y, UINT color = 0xFFFFFFFF) = 0;
	virtual void draw_number_rightup(TCHAR *str, float x, float y, UINT color = 0xFFFFFFFF) = 0;

	MyDirectXImage *get_image()
	{
		return number_image;
	}

	int get_number_image_width()
	{
		return NUMBER_IMAGE_WIDTH;
	}

	int get_number_image_height()
	{
		return NUMBER_IMAGE_HEIGHT;
	}
};
