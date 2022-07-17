#include "MyDirectXImage.h"

MyDirectXImage::MyDirectXImage(TCHAR *FileName, IDirect3DDevice9 *pDevice3D)
{
	file_info* image_fi;
	image_fi = MaterialArchiveFile::get_instance_ptr()->get_file_info_ptr(FileName);
	if (image_fi == nullptr) {
		MessageBox(NULL, _T("画像のロードの際にエラーが発生しました"), _T("ERROR"), MB_OK | MB_ICONERROR);

		__app_safe_exit_error__();

		return;
	}
	BYTE* image_data_buffer;
	image_data_buffer = (BYTE*)malloc((size_t)image_fi->file_size);
	if (image_data_buffer == nullptr) {
		MessageBox(NULL, _T("画像のロードの際にエラーが発生しました"), _T("ERROR"), MB_OK | MB_ICONERROR);

		__app_safe_exit_error__();

		return;
	}
	MaterialArchiveFile::get_instance_ptr()->read_file_data(image_fi->data_top_index, image_fi->file_size, image_data_buffer);

	XOR::xor_256byte(image_data_buffer, image_fi->file_size);

	if (FAILED(D3DXCreateTextureFromFileInMemoryEx(
		pDevice3D,
		image_data_buffer,
		image_fi->file_size,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		D3DX_FILTER_NONE,
		D3DX_FILTER_NONE,
		D3DCOLOR_ARGB(0, 0, 0, 0),
		&src_image_info,
		NULL,
		&texture)))
	{
		MessageBox(NULL, _T("画像のロードの際にエラーが発生しました"), _T("ERROR"), MB_OK | MB_ICONERROR);
		
		free(image_data_buffer);

		__app_safe_exit_error__();
		
		return;
	}

	free(image_data_buffer);

	///*
	//画像の読み込みに失敗した場合
	//*/
	//if (FAILED(D3DXCreateTextureFromFileEx(
	//	pDevice3D,
	//	FileName,
	//	D3DX_DEFAULT,
	//	D3DX_DEFAULT,
	//	D3DX_DEFAULT,
	//	0,
	//	D3DFMT_A8R8G8B8,
	//	D3DPOOL_MANAGED,
	//	D3DX_FILTER_NONE,
	//	D3DX_FILTER_NONE,
	//	D3DCOLOR_ARGB(0, 0, 0, 0),
	//	&texture_info,
	//	NULL,
	//	&texture
	//)) == TRUE)
	//{
	//	MessageBox(NULL, _T("画像のロードの際にエラーが発生しました"), _T("ERROR"), MB_OK | MB_ICONERROR);
	//
	//	__app_safe_exit_error__();
	//
	//	return;
	//}

	texture->GetLevelDesc(0, &texture_info);
}

MyDirectXImage::~MyDirectXImage()
{
	if (texture != nullptr) {
		texture->Release();
	}
}
