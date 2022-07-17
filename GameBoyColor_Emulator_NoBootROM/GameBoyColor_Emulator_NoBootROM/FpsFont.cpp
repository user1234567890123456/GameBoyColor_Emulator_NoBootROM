#include "FpsFont.h"

FpsFont::FpsFont(MyDirectXSystem* myDirectXSystem)
	: MyFont(myDirectXSystem, 18, 18, _T("Resource\\Image\\fps_font.png"))
{
}

FpsFont::~FpsFont()
{
}

int FpsFont::get_font_index(TCHAR chk_ch)
{
	if (chk_ch == '0') {
		return 0;
	}
	else if (chk_ch == '1') {
		return 1;
	}
	else if (chk_ch == '2') {
		return 2;
	}
	else if (chk_ch == '3') {
		return 3;
	}
	else if (chk_ch == '4') {
		return 4;
	}
	else if (chk_ch == '5') {
		return 5;
	}
	else if (chk_ch == '6') {
		return 6;
	}
	else if (chk_ch == '7') {
		return 7;
	}
	else if (chk_ch == '8') {
		return 8;
	}
	else if (chk_ch == '9') {
		return 9;
	}
	else if (chk_ch == '.') {
		return 10;
	}
	else {
		M_debug_printf("エラー発生 @ FpsFont::check_font_index()\n");

		return -1;
	}
}

void FpsFont::draw_number_leftup(TCHAR *str, float x, float y, UINT color)
{
	MyDirectXImage *n_image = get_image();

	float tmp_screen_x = x;

	int i;
	int str_length = strlen(str);
	for (i = 0; i < str_length; i++) {
		int index = get_font_index(str[i]);

		//エラーが発生した場合
		if (index == -1) {
			continue;
		}

		RECT rc;
		rc.top = 0;
		rc.bottom = (LONG)(get_number_image_height());
		rc.left = index * (LONG)(get_number_image_width());
		rc.right = rc.left + (LONG)(get_number_image_width());

		MyDirectXDraw::draw_cut_image_base_leftup(myDirectXSystem, n_image, tmp_screen_x, y, rc);

		/*
		*/
		tmp_screen_x += (float)(get_number_image_width());
	}

}

void FpsFont::draw_number_rightup(TCHAR *str, float x, float y, UINT color)
{
	MyDirectXImage *n_image = get_image();

	int i;
	int str_length = strlen(str);

	float tmp_screen_x = x - str_length * get_number_image_width();

	for (i = 0; i < str_length; i++) {
		int index = get_font_index(str[i]);

		//エラーが発生した場合
		if (index == -1) {
			continue;
		}

		RECT rc;
		rc.top = 0;
		rc.bottom = (LONG)(get_number_image_height());
		rc.left = index * (LONG)(get_number_image_width());
		rc.right = rc.left + (LONG)(get_number_image_width());

		MyDirectXDraw::draw_cut_image_base_leftup(myDirectXSystem, n_image, tmp_screen_x, y, rc);

		/*
		*/
		tmp_screen_x += (float)(get_number_image_width());
	}

}
