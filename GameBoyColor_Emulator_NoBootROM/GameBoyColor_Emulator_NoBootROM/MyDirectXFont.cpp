#include "MyDirectXFont.h"
//
//MyDirectXFont::MyDirectXFont(MyDirectXSystem *myDirectXSystem, int font_size, bool is_diagonal, TCHAR *font_name, bool is_bold)
//{
//	if (FAILED(D3DXCreateFont(
//		myDirectXSystem->get_pDevice3D(),
//		font_size,
//		0,
//		(is_bold == false) ? FW_REGULAR : FW_BOLD,
//		NULL,
//		(is_diagonal == true) ? TRUE : FALSE,
//		SHIFTJIS_CHARSET,
//		OUT_DEFAULT_PRECIS,
//		PROOF_QUALITY,
//		DEFAULT_PITCH | FF_DONTCARE,
//		font_name,
//		&font
//	)))
//	{
//		MessageBox(NULL, _T("フォントのロードの際にエラーが発生しました"), _T("ERROR"), MB_OK | MB_ICONERROR);
//
//		__app_safe_exit_error__();
//
//		return;
//	}
//}
//
//MyDirectXFont::~MyDirectXFont()
//{
//	if (font != nullptr) {
//		font->Release();
//	}
//}
//