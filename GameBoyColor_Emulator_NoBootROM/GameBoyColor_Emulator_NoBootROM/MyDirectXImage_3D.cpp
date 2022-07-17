#include "MyDirectXImage_3D.h"

MyDirectXImage_3D::MyDirectXImage_3D(IDirect3DDevice9* pDevice3D, const char *texture_filename)
	: pDevice3D(pDevice3D)
{
	//if (FAILED(pDevice3D->CreateVertexBuffer(
	//	4 * sizeof(VERTEX_XYZ_COLOR_UV),
	//	0,
	//	D3DFVF_CUSTOMVERTEX,
	//	D3DPOOL_DEFAULT,
	//	&pVB,
	//	NULL
	//)))
	//{
	//	MessageBox(NULL, _T("頂点バッファの作成の際にエラーが発生しました"), _T("ERROR"), MB_OK | MB_ICONERROR);
	//
	//	__app_safe_exit_error__();
	//
	//	return;
	//}

	file_info* image_fi;
	image_fi = MaterialArchiveFile::get_instance_ptr()->get_file_info_ptr(texture_filename);
	if (image_fi == nullptr) {
		MessageBox(NULL, _T("テクスチャの作成の際にエラーが発生しました"), _T("ERROR"), MB_OK | MB_ICONERROR);

		__app_safe_exit_error__();

		return;
	}
	BYTE* image_data_buffer;
	image_data_buffer = (BYTE*)malloc((size_t)image_fi->file_size);
	if (image_data_buffer == nullptr) {
		MessageBox(NULL, _T("テクスチャの作成の際にエラーが発生しました"), _T("ERROR"), MB_OK | MB_ICONERROR);

		__app_safe_exit_error__();

		return;
	}
	MaterialArchiveFile::get_instance_ptr()->read_file_data(image_fi->data_top_index, image_fi->file_size, image_data_buffer);

	XOR::xor_256byte(image_data_buffer, image_fi->file_size);

	if (FAILED(D3DXCreateTextureFromFileInMemory(pDevice3D, image_data_buffer, image_fi->file_size, &pTexture))) {
		MessageBox(NULL, _T("テクスチャの作成の際にエラーが発生しました"), _T("ERROR"), MB_OK | MB_ICONERROR);

		__app_safe_exit_error__();

		return;
	}

	free(image_data_buffer);

	//if (FAILED(D3DXCreateTextureFromFile(pDevice3D, texture_filename, &pTexture))) {
	//	MessageBox(NULL, _T("テクスチャの作成の際にエラーが発生しました"), _T("ERROR"), MB_OK | MB_ICONERROR);
	//
	//	__app_safe_exit_error__();
	//
	//	return;
	//}
}

MyDirectXImage_3D::~MyDirectXImage_3D()
{
	//if (pVB != NULL) {
	//	pVB->Release();
	//}

	if (pTexture != NULL) {
		pTexture->Release();
	}
}

void MyDirectXImage_3D::draw(VERTEX_XYZ_COLOR_UV Vertices[4], FLOAT angle_x, FLOAT angle_y, FLOAT angle_z, D3DXVECTOR3 translation_vector, D3DXVECTOR3 camera_location) {
	DWORD cull_state_bak;
	pDevice3D->GetRenderState(D3DRS_CULLMODE, &cull_state_bak);
	//カリングを設定する
	pDevice3D->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	DWORD lighting_state_bak;
	pDevice3D->GetRenderState(D3DRS_LIGHTING, &lighting_state_bak);
	pDevice3D->SetRenderState(D3DRS_LIGHTING, FALSE);

	//Zバッファーを無効化する
	pDevice3D->SetRenderState(D3DRS_ZENABLE, FALSE);
	

	D3DXMATRIXA16 mat_backup1;
	D3DXMATRIXA16 mat_backup2;
	D3DXMATRIXA16 mat_backup3;
	pDevice3D->GetTransform(D3DTS_WORLD, &mat_backup1);
	pDevice3D->GetTransform(D3DTS_VIEW, &mat_backup2);
	pDevice3D->GetTransform(D3DTS_PROJECTION, &mat_backup3);


	//VOID* pVertices;
	//
	//pVB->Lock(0, 4 * sizeof(VERTEX_XYZ_COLOR_UV), (void**)&pVertices, 0);
	//memcpy(pVertices, Vertices, 4 * sizeof(VERTEX_XYZ_COLOR_UV));
	//pVB->Unlock();


	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matRotateX;
	D3DXMATRIXA16 matRotateY;
	D3DXMATRIXA16 matRotateZ;
	D3DXMATRIXA16 mat_tmp1;
	D3DXMATRIXA16 mat_tmp2;
	D3DXMATRIXA16 matTrans;

	D3DXMatrixIdentity(&matWorld);

	////回転軸となる向きを設定する
	//D3DXVECTOR3 to_direction;
	//to_direction.x = 1.0f;
	//to_direction.y = 1.0f;
	//to_direction.z = 0.0f;
	//D3DXMatrixRotationAxis(&matWorld, &to_direction, timeGetTime() / 1000.0f);
	//pDevice3D->SetTransform(D3DTS_WORLD, &matWorld);

	//回転する
	D3DXMatrixRotationX(&matRotateX, angle_x);
	D3DXMatrixRotationY(&matRotateY, angle_y);
	D3DXMatrixRotationZ(&matRotateZ, angle_z);

	D3DXMatrixMultiply(&mat_tmp1, &matRotateX, &matRotateY);
	D3DXMatrixMultiply(&mat_tmp2, &mat_tmp1, &matRotateZ);

	D3DXMatrixTranslation(&matTrans, translation_vector.x, translation_vector.y, translation_vector.z);
	D3DXMatrixMultiply(&matWorld, &mat_tmp2, &matTrans);

	pDevice3D->SetTransform(D3DTS_WORLD, &matWorld);

	D3DXMATRIXA16 matView;
	D3DXVECTOR3 vLookatPt, vUpVec;
	
	vLookatPt.x = 0.0f;
	vLookatPt.y = 0.0f;
	vLookatPt.z = 0.0f;
	vUpVec.x = 0.0f;
	vUpVec.y = 1.0f;
	vUpVec.z = 0.0f;
	D3DXMatrixLookAtLH(&matView, &camera_location, &vLookatPt, &vUpVec);
	pDevice3D->SetTransform(D3DTS_VIEW, &matView);
	
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0f, 1.0f, 100.0f);
	pDevice3D->SetTransform(D3DTS_PROJECTION, &matProj);
	
	pDevice3D->SetTexture(0, pTexture);
	pDevice3D->SetFVF(D3DFVF_CUSTOMVERTEX);
	pDevice3D->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, Vertices, sizeof(VERTEX_XYZ_COLOR_UV));

	//pDevice3D->SetStreamSource(
	//	0,
	//	pVB,
	//	0,
	//	sizeof(VERTEX_XYZ_COLOR_UV)
	//);
	//
	//pDevice3D->SetFVF(D3DFVF_CUSTOMVERTEX);
	//
	//pDevice3D->DrawPrimitive(
	//	D3DPT_TRIANGLEFAN,
	//	0,
	//	2
	//);

	pDevice3D->SetTransform(D3DTS_WORLD, &mat_backup1);
	pDevice3D->SetTransform(D3DTS_VIEW, &mat_backup2);
	pDevice3D->SetTransform(D3DTS_PROJECTION, &mat_backup3);


	//Zバッファーを再度有効にする
	pDevice3D->SetRenderState(D3DRS_ZENABLE, TRUE);
	pDevice3D->SetRenderState(D3DRS_CULLMODE, cull_state_bak);
	pDevice3D->SetRenderState(D3DRS_LIGHTING, lighting_state_bak);
}
