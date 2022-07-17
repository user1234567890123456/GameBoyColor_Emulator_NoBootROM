#include "Fps.h"

Fps::Fps(MyDirectXSystem *myDirectXSystem/*, LPD3DXSPRITE sprite*/)// : sprite(sprite)
{
	//fps_number = new FpsFont(myDirectXSystem);

	int font_size = 22;
	LOGFONT lf = { font_size, 0, 0, 0, 0, 0, 0, 0, SHIFTJIS_CHARSET, OUT_TT_ONLY_PRECIS,
	CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, _T("‚l‚r ‚oƒSƒVƒbƒN") };
	fps_font = new MyDirectXString(myDirectXSystem, &lf);
}

Fps::~Fps()
{
	//delete fps_number;
	delete fps_font;
}

void Fps::draw_fps()
{
	char fps_str[255];
	sprintf_s(fps_str, _T("%.2lfFPS"), fps);
	//fps_number->draw_number_rightup(_T(fps_str), WINDOW_WIDTH, WINDOW_HEIGHT - fps_number->get_number_image_height());
	fps_font->draw_process_base_rightup_with_edge(_T(fps_str), WINDOW_WIDTH - 2, WINDOW_HEIGHT - 22 - 2, GET_COLOR_ALPHA255(255, 255, 255), GET_COLOR_ALPHA255(0, 0, 0));
}

void Fps::update()
{
	if (frame == 0) {
		start_time = timeGetTime();
	}
	else if (frame >= FPS)
	{
		DWORD total_time = timeGetTime() - start_time;

		fps = 1000.0 / (total_time / (double) FPS);

		start_time = timeGetTime();

		frame = 0;
	}

	frame++;
}

void Fps::wait()
{
	DWORD total_time = timeGetTime() - start_time;
	DWORD wait_time = (frame * 1000 / FPS) - total_time;

	if ((signed long) wait_time > 0) {
		Sleep(wait_time);
	}
}
