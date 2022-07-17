#include "MyDirectXString.h"

MyDirectXString::MyDirectXString(MyDirectXSystem* myDirectXSystem, LOGFONT* lf)
	: myDirectXSystem(myDirectXSystem)
{
	memcpy(&(this->_lf), lf, sizeof(LOGFONT));
}

MyDirectXString::~MyDirectXString()
{
	for (auto it = char_list.begin(); it != char_list.end(); ++it) {
		delete (it->second);
	}
}

void MyDirectXString::prev_process(TCHAR* string)
{
	for (TCHAR* index_ptr = string; (*index_ptr) != NULL; index_ptr++) {
		TCHAR create_char[3];
		memset(create_char, 0, sizeof(TCHAR) * 3);

		create_char[0] = (*index_ptr);
		if (IsDBCSLeadByte(create_char[0]))//マルチバイト文字の時
		{
			index_ptr++;
			create_char[1] = (*index_ptr);
		}

		uint16_t list_key = (byte)create_char[0] << 8 | (byte)create_char[1];

		if (char_list.count(list_key) == 0) {//存在しないとき
			//M_debug_printf("create char...\n");
			MyDirectXChar* myDirectXChar_Ptr = new MyDirectXChar(myDirectXSystem, create_char, &_lf);
			char_list.emplace(list_key, myDirectXChar_Ptr);
		}
	}
}

void MyDirectXString::abstract_draw_process(TCHAR* string, float x, float y, UINT color)
{
	MyDirectXDraw::set_sampler_state__font(myDirectXSystem);

	for (TCHAR* index_ptr = string; (*index_ptr) != NULL; index_ptr++) {
		TCHAR create_char[3];
		memset(create_char, 0, sizeof(TCHAR) * 3);

		create_char[0] = (*index_ptr);
		if (IsDBCSLeadByte(create_char[0]))//マルチバイト文字の時
		{
			index_ptr++;
			create_char[1] = (*index_ptr);
		}

		uint16_t list_key = (byte)create_char[0] << 8 | (byte)create_char[1];

		MyDirectXChar* myDirectXChar_Ptr = char_list[list_key];
		float char_width = myDirectXChar_Ptr->get_width();
		float char_height = myDirectXChar_Ptr->get_height();

		myDirectXChar_Ptr->draw(x, y, color);

		x += char_width;
	}

	MyDirectXDraw::set_sampler_state__2d(myDirectXSystem);
}

void MyDirectXString::draw_process_base_leftup(TCHAR* string, float x, float y, UINT color)
{
	prev_process(string);

	abstract_draw_process(string, x, y, color);
}

void MyDirectXString::draw_process_base_leftup_with_edge(TCHAR* string, float x, float y, UINT color, UINT edge_color) {
	draw_process_base_leftup(string, x - 1, y - 1, edge_color);
	draw_process_base_leftup(string, x - 1, y + 1, edge_color);
	draw_process_base_leftup(string, x + 1, y - 1, edge_color);
	draw_process_base_leftup(string, x + 1, y + 1, edge_color);
	draw_process_base_leftup(string, x, y, color);
}

void MyDirectXString::draw_process_base_center(TCHAR* string, float x, float y, UINT color)
{
	prev_process(string);

	float string_width = 0.0f;
	float string_height = 0.0f;

	for (TCHAR* index_ptr = string; (*index_ptr) != NULL; index_ptr++) {
		TCHAR create_char[3];
		memset(create_char, 0, sizeof(TCHAR) * 3);

		create_char[0] = (*index_ptr);
		if (IsDBCSLeadByte(create_char[0]))//マルチバイト文字の時
		{
			index_ptr++;
			create_char[1] = (*index_ptr);
		}

		uint16_t list_key = (byte)create_char[0] << 8 | (byte)create_char[1];

		MyDirectXChar* myDirectXChar_Ptr = char_list[list_key];
		float char_width = myDirectXChar_Ptr->get_width();
		float char_height = myDirectXChar_Ptr->get_height();

		string_width += char_width;
		if (string_height > char_height) {
			string_height = char_height;
		}
	}

	abstract_draw_process(string, x - (string_width / 2.0f), y - (string_height / 2.0f), color);
}

void MyDirectXString::draw_process_base_center_with_edge(TCHAR* string, float x, float y, UINT color, UINT edge_color) {
	draw_process_base_center(string, x - 1, y - 1, edge_color);
	draw_process_base_center(string, x - 1, y + 1, edge_color);
	draw_process_base_center(string, x + 1, y - 1, edge_color);
	draw_process_base_center(string, x + 1, y + 1, edge_color);
	draw_process_base_center(string, x, y, color);
}

void MyDirectXString::draw_process_base_rightup(TCHAR* string, float x, float y, UINT color)
{
	prev_process(string);

	float string_width = 0.0f;

	for (TCHAR* index_ptr = string; (*index_ptr) != NULL; index_ptr++) {
		TCHAR create_char[3];
		memset(create_char, 0, sizeof(TCHAR) * 3);

		create_char[0] = (*index_ptr);
		if (IsDBCSLeadByte(create_char[0]))//マルチバイト文字の時
		{
			index_ptr++;
			create_char[1] = (*index_ptr);
		}

		uint16_t list_key = (byte)create_char[0] << 8 | (byte)create_char[1];

		MyDirectXChar* myDirectXChar_Ptr = char_list[list_key];
		float char_width = myDirectXChar_Ptr->get_width();
		float char_height = myDirectXChar_Ptr->get_height();

		string_width += char_width;
	}

	abstract_draw_process(string, x - string_width, y, color);
}

void MyDirectXString::draw_process_base_rightup_with_edge(TCHAR* string, float x, float y, UINT color, UINT edge_color) {
	draw_process_base_rightup(string, x - 1, y - 1, edge_color);
	draw_process_base_rightup(string, x - 1, y + 1, edge_color);
	draw_process_base_rightup(string, x + 1, y - 1, edge_color);
	draw_process_base_rightup(string, x + 1, y + 1, edge_color);
	draw_process_base_rightup(string, x, y, color);
}

void MyDirectXString::draw_process_base_leftdown(TCHAR* string, float x, float y, UINT color)
{
	prev_process(string);

	//float string_width = 0.0f;
	float string_height = 0.0f;

	for (TCHAR* index_ptr = string; (*index_ptr) != NULL; index_ptr++) {
		TCHAR create_char[3];
		memset(create_char, 0, sizeof(TCHAR) * 3);

		create_char[0] = (*index_ptr);
		if (IsDBCSLeadByte(create_char[0]))//マルチバイト文字の時
		{
			index_ptr++;
			create_char[1] = (*index_ptr);
		}

		uint16_t list_key = (byte)create_char[0] << 8 | (byte)create_char[1];

		MyDirectXChar* myDirectXChar_Ptr = char_list[list_key];
		float char_width = myDirectXChar_Ptr->get_width();
		float char_height = myDirectXChar_Ptr->get_height();

		//string_width += char_width;
		if (string_height < char_height) {
			string_height = char_height;
		}
	}

	abstract_draw_process(string, x, y - string_height, color);
}

void MyDirectXString::draw_process_base_leftdown_with_edge(TCHAR* string, float x, float y, UINT color, UINT edge_color) {
	draw_process_base_leftdown(string, x - 1, y - 1, edge_color);
	draw_process_base_leftdown(string, x - 1, y + 1, edge_color);
	draw_process_base_leftdown(string, x + 1, y - 1, edge_color);
	draw_process_base_leftdown(string, x + 1, y + 1, edge_color);
	draw_process_base_leftdown(string, x, y, color);
}
