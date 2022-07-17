#include "MyDirectXDraw.h"

MyDirectXDraw::MyDirectXDraw()
{
}

MyDirectXDraw::~MyDirectXDraw()
{
}


void MyDirectXDraw::set_sampler_state__font(MyDirectXSystem* myDirectXSystem)
{
	myDirectXSystem->get_pDevice3D()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	myDirectXSystem->get_pDevice3D()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	myDirectXSystem->get_pDevice3D()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_GAUSSIANQUAD);
}

void MyDirectXDraw::set_sampler_state__2d(MyDirectXSystem* myDirectXSystem)
{
	myDirectXSystem->get_pDevice3D()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	myDirectXSystem->get_pDevice3D()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	myDirectXSystem->get_pDevice3D()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
}

void MyDirectXDraw::set_sampler_state__2d_2(MyDirectXSystem* myDirectXSystem)
{
	myDirectXSystem->get_pDevice3D()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	myDirectXSystem->get_pDevice3D()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	myDirectXSystem->get_pDevice3D()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
}

void MyDirectXDraw::set_sampler_state__3d(MyDirectXSystem* myDirectXSystem)
{
	myDirectXSystem->get_pDevice3D()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	myDirectXSystem->get_pDevice3D()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	myDirectXSystem->get_pDevice3D()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_GAUSSIANQUAD);
}

void MyDirectXDraw::set_sampler_state__3d_2(MyDirectXSystem* myDirectXSystem)
{
	myDirectXSystem->get_pDevice3D()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	myDirectXSystem->get_pDevice3D()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	myDirectXSystem->get_pDevice3D()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
}

void MyDirectXDraw::set_default_state(MyDirectXSystem* myDirectXSystem)
{
	myDirectXSystem->get_pDevice3D()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	myDirectXSystem->get_pDevice3D()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	myDirectXSystem->get_pDevice3D()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	//透過処理をする
	myDirectXSystem->get_pDevice3D()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	//半透明処理をする
	myDirectXSystem->get_pDevice3D()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
}

void MyDirectXDraw::disable_alpha(MyDirectXSystem *myDirectXSystem)
{
	//アルファーブレンドをしない
	myDirectXSystem->get_pDevice3D()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void MyDirectXDraw::enable_alpha(MyDirectXSystem *myDirectXSystem)
{
	//アルファーブレンドをする
	myDirectXSystem->get_pDevice3D()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
}

void MyDirectXDraw::fix_vertex_pos__VERTEX_XYZ_RHW_COLOR(VERTEX_XYZ_RHW_COLOR* vertex)
{
	//ラスタライズルール
	for (int i = 0; i < 4; i++) {
		vertex[i].x = (float)((int)vertex[i].x) - 0.5f;
		vertex[i].y = (float)((int)vertex[i].y) - 0.5f;
	}
}

void MyDirectXDraw::fix_vertex_pos__VERTEX_XYZ_RHW_COLOR_UV(VERTEX_XYZ_RHW_COLOR_UV* vertex)
{
	//ラスタライズルール
	for (int i = 0; i < 4; i++) {
		vertex[i].x = (float)((int)vertex[i].x) - 0.5f;
		vertex[i].y = (float)((int)vertex[i].y) - 0.5f;
	}
}

void MyDirectXDraw::draw_box_leftup(MyDirectXSystem *myDirectXSystem, float start_x, float start_y, float end_x, float end_y, DWORD color)
{
	VERTEX_XYZ_RHW_COLOR vertex[4];

	vertex[0].x = end_x;
	vertex[0].y = start_y;
	vertex[0].z = 0.0f;
	vertex[0].rhw = 1.0f;
	vertex[0].color = color;

	vertex[1].x = end_x;
	vertex[1].y = end_y;
	vertex[1].z = 0.0f;
	vertex[1].rhw = 1.0f;
	vertex[1].color = color;

	vertex[2].x = start_x;
	vertex[2].y = start_y;
	vertex[2].z = 0.0f;
	vertex[2].rhw = 1.0f;
	vertex[2].color = color;

	vertex[3].x = start_x;
	vertex[3].y = end_y;
	vertex[3].z = 0.0f;
	vertex[3].rhw = 1.0f;
	vertex[3].color = color;

	fix_vertex_pos__VERTEX_XYZ_RHW_COLOR(vertex);

	/*
	テクスチャのリセット(これをしないと色がおかしくなる)
	*/
	myDirectXSystem->get_pDevice3D()->SetTexture(0, NULL);
	myDirectXSystem->get_pDevice3D()->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	myDirectXSystem->get_pDevice3D()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertex, sizeof(VERTEX_XYZ_RHW_COLOR));


	//HRESULT result;
	//IDirect3DVertexBuffer9 *pVertex;
	//result = myDirectXSystem->get_pDevice3D()->CreateVertexBuffer(sizeof(VERTEX_XYZ_RHW_COLOR) * 4, D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_MANAGED, &pVertex, NULL);
	//
	//void *p_data;
	//pVertex->Lock(0, sizeof(VERTEX_XYZ_RHW_COLOR) * 4, &p_data, 0);
	//
	//memcpy(p_data, vertex, sizeof(VERTEX_XYZ_RHW_COLOR) * 4);
	//pVertex->Unlock();
	//
	//myDirectXSystem->get_pDevice3D()->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	//myDirectXSystem->get_pDevice3D()->SetStreamSource(0, pVertex, 0, sizeof(VERTEX_XYZ_RHW_COLOR));
	//myDirectXSystem->get_pDevice3D()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	//
	//pVertex->Release();
}

/*
左上を中心に描画する (テクスチャを)
*/
void MyDirectXDraw::draw_texture_base_leftup(MyDirectXSystem* myDirectXSystem, LPDIRECT3DTEXTURE9 pTexture, float texture_src_width, float texture_src_height, float x, float y, UINT color)
{
	VERTEX_XYZ_RHW_COLOR_UV vertex[4];

	float start_x, end_x;
	float start_y, end_y;

	D3DSURFACE_DESC texture_info;
	pTexture->GetLevelDesc(0, &texture_info);

	float texture_width = (float)(texture_info.Width);
	float texture_height = (float)(texture_info.Height);

	float src_image_width = (float)(texture_src_width);
	float src_image_height = (float)(texture_src_height);

	float max_u = src_image_width / texture_width;
	float max_v = src_image_height / texture_height;

	start_x = x;
	start_y = y;
	end_x = start_x + src_image_width;
	end_y = start_y + src_image_height;

	vertex[0].x = end_x;
	vertex[0].y = start_y;
	vertex[0].z = 0.0f;
	vertex[0].rhw = 1.0f;
	vertex[0].color = color;
	vertex[0].u = max_u;
	vertex[0].v = 0.0f;

	vertex[1].x = end_x;
	vertex[1].y = end_y;
	vertex[1].z = 0.0f;
	vertex[1].rhw = 1.0f;
	vertex[1].color = color;
	vertex[1].u = max_u;
	vertex[1].v = max_v;

	vertex[2].x = start_x;
	vertex[2].y = start_y;
	vertex[2].z = 0.0f;
	vertex[2].rhw = 1.0f;
	vertex[2].color = color;
	vertex[2].u = 0.0f;
	vertex[2].v = 0.0f;

	vertex[3].x = start_x;
	vertex[3].y = end_y;
	vertex[3].z = 0.0f;
	vertex[3].rhw = 1.0f;
	vertex[3].color = color;
	vertex[3].u = 0.0f;
	vertex[3].v = max_v;

	fix_vertex_pos__VERTEX_XYZ_RHW_COLOR_UV(vertex);

	myDirectXSystem->get_pDevice3D()->SetTexture(0, pTexture);
	myDirectXSystem->get_pDevice3D()->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	myDirectXSystem->get_pDevice3D()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertex, sizeof(VERTEX_XYZ_RHW_COLOR_UV));
}

/*
拡大率を設定して左上を中心に描画する (テクスチャを)
*/
void MyDirectXDraw::draw_texture_base_leftup_enable_size(MyDirectXSystem* myDirectXSystem, LPDIRECT3DTEXTURE9 pTexture, float texture_src_width, float texture_src_height, float x, float y, double percent, UINT color)
{
	VERTEX_XYZ_RHW_COLOR_UV vertex[4];

	float start_x, end_x;
	float start_y, end_y;

	D3DSURFACE_DESC texture_info;
	pTexture->GetLevelDesc(0, &texture_info);

	float texture_width = (float)(texture_info.Width);
	float texture_height = (float)(texture_info.Height);

	float src_image_width = (float)(texture_src_width);
	float src_image_height = (float)(texture_src_height);

	float max_u = src_image_width / texture_width;
	float max_v = src_image_height / texture_height;

	start_x = x;
	start_y = y;
	end_x = start_x + (src_image_width * percent);
	end_y = start_y + (src_image_height * percent);

	vertex[0].x = end_x;
	vertex[0].y = start_y;
	vertex[0].z = 0.0f;
	vertex[0].rhw = 1.0f;
	vertex[0].color = color;
	vertex[0].u = max_u;
	vertex[0].v = 0.0f;

	vertex[1].x = end_x;
	vertex[1].y = end_y;
	vertex[1].z = 0.0f;
	vertex[1].rhw = 1.0f;
	vertex[1].color = color;
	vertex[1].u = max_u;
	vertex[1].v = max_v;

	vertex[2].x = start_x;
	vertex[2].y = start_y;
	vertex[2].z = 0.0f;
	vertex[2].rhw = 1.0f;
	vertex[2].color = color;
	vertex[2].u = 0.0f;
	vertex[2].v = 0.0f;

	vertex[3].x = start_x;
	vertex[3].y = end_y;
	vertex[3].z = 0.0f;
	vertex[3].rhw = 1.0f;
	vertex[3].color = color;
	vertex[3].u = 0.0f;
	vertex[3].v = max_v;

	fix_vertex_pos__VERTEX_XYZ_RHW_COLOR_UV(vertex);

	myDirectXSystem->get_pDevice3D()->SetTexture(0, pTexture);
	myDirectXSystem->get_pDevice3D()->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	myDirectXSystem->get_pDevice3D()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertex, sizeof(VERTEX_XYZ_RHW_COLOR_UV));
}

/*
左上を中心に描画する2
*/
void MyDirectXDraw::draw_image_base_leftup(MyDirectXSystem* myDirectXSystem, MyDirectXImage* image, float x, float y, UINT color)
{
	VERTEX_XYZ_RHW_COLOR_UV vertex[4];

	float start_x, end_x;
	float start_y, end_y;

	float texture_width = (float)(image->get_texture_width());
	float texture_height = (float)(image->get_texture_height());

	float src_image_width = (float)(image->get_src_image_width());
	float src_image_height = (float)(image->get_src_image_height());

	float max_u = src_image_width / texture_width;
	float max_v = src_image_height / texture_height;

	start_x = x;
	start_y = y;
	end_x = start_x + src_image_width;
	end_y = start_y + src_image_height;

	vertex[0].x = end_x;
	vertex[0].y = start_y;
	vertex[0].z = 0.0f;
	vertex[0].rhw = 1.0f;
	vertex[0].color = color;
	vertex[0].u = max_u;
	vertex[0].v = 0.0f;

	vertex[1].x = end_x;
	vertex[1].y = end_y;
	vertex[1].z = 0.0f;
	vertex[1].rhw = 1.0f;
	vertex[1].color = color;
	vertex[1].u = max_u;
	vertex[1].v = max_v;

	vertex[2].x = start_x;
	vertex[2].y = start_y;
	vertex[2].z = 0.0f;
	vertex[2].rhw = 1.0f;
	vertex[2].color = color;
	vertex[2].u = 0.0f;
	vertex[2].v = 0.0f;

	vertex[3].x = start_x;
	vertex[3].y = end_y;
	vertex[3].z = 0.0f;
	vertex[3].rhw = 1.0f;
	vertex[3].color = color;
	vertex[3].u = 0.0f;
	vertex[3].v = max_v;

	fix_vertex_pos__VERTEX_XYZ_RHW_COLOR_UV(vertex);

	myDirectXSystem->get_pDevice3D()->SetTexture(0, image->get_texture());
	myDirectXSystem->get_pDevice3D()->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	myDirectXSystem->get_pDevice3D()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertex, sizeof(VERTEX_XYZ_RHW_COLOR_UV));
}

///*
//左上を中心に描画する
//*/
//void MyDirectXDraw::__OLD_draw_image_base_leftup__(LPD3DXSPRITE sprite, MyDirectXImage *image, float x, float y, UINT color)
//{
//	//sprite->Begin(D3DXSPRITE_ALPHABLEND);
//
//	sprite->Draw(image->get_texture(), NULL, NULL, &D3DXVECTOR3(x, y, 0), color);
//	
//	//sprite->End();
//}

///*
//左上を中心に描画する
//回転あり
//*/
//void MyDirectXDraw::__OLD_draw_image_base_leftup_settable_angle__(LPD3DXSPRITE sprite, MyDirectXImage *image, float x, float y, double angle_x, double angle_y, double angle_z, UINT color)
//{
//	//sprite->Begin(D3DXSPRITE_ALPHABLEND);
//
//	D3DXMATRIX matrix_backup;
//	sprite->GetTransform(&matrix_backup);
//
//	int width = image->get_src_image_width();
//	int height = image->get_src_image_height();
//
//	D3DXMATRIX matrix;
//	D3DXMatrixRotationX(&matrix, angle_x);
//	D3DXMatrixRotationY(&matrix, angle_y);
//	D3DXMatrixRotationZ(&matrix, angle_z);
//	matrix._41 = x;
//	matrix._42 = y;
//	matrix._43 = 0;
//	sprite->SetTransform(&matrix);
//
//	sprite->Draw(image->get_texture(), NULL, NULL, &D3DXVECTOR3(x, y, 0), color);
//
//	sprite->SetTransform(&matrix_backup);
//
//	//sprite->End();
//}

/*
右上を中心に描画する2
*/
void MyDirectXDraw::draw_image_base_rightup(MyDirectXSystem* myDirectXSystem, MyDirectXImage* image, float x, float y, UINT color)
{
	float image_width = (float)(image->get_src_image_width());
	draw_image_base_leftup(myDirectXSystem, image, x - image_width, y, color);
}

///*
//右上を中心に描画する
//*/
//void MyDirectXDraw::__OLD_draw_image_base_rightup__(LPD3DXSPRITE sprite, MyDirectXImage *image, float x, float y, UINT color)
//{
//	//sprite->Begin(D3DXSPRITE_ALPHABLEND);
//
//	int width = image->get_src_image_width();
//
//	sprite->Draw(image->get_texture(), NULL, NULL, &D3DXVECTOR3(x - width, y, 0), color);
//
//	//sprite->End();
//}

/*
右下を中心に描画する2
*/
void MyDirectXDraw::draw_image_base_rightdown(MyDirectXSystem* myDirectXSystem, MyDirectXImage* image, float x, float y, UINT color)
{
	float image_width = (float)(image->get_src_image_width());
	float image_height = (float)(image->get_src_image_height());
	draw_image_base_leftup(myDirectXSystem, image, x - image_width, y - image_height, color);
}

///*
//右下を中心に描画する
//*/
//void MyDirectXDraw::__OLD_draw_image_base_rightdown__(LPD3DXSPRITE sprite, MyDirectXImage *image, float x, float y, UINT color)
//{
//	//sprite->Begin(D3DXSPRITE_ALPHABLEND);
//
//	int width = image->get_src_image_width();
//	int height = image->get_src_image_height();
//
//	sprite->Draw(image->get_texture(), NULL, NULL, &D3DXVECTOR3(x - width, y - height, 0), color);
//
//	//sprite->End();
//}

/*
真ん中を中心に描画する2
*/
void MyDirectXDraw::draw_image_base_center(MyDirectXSystem* myDirectXSystem, MyDirectXImage* image, float x, float y, UINT color)
{
	float image_width = (float)(image->get_src_image_width());
	float image_height = (float)(image->get_src_image_height());
	draw_image_base_leftup(myDirectXSystem, image, x - (image_width / 2), y - (image_height / 2), color);
}

///*
//真ん中を中心に描画する
//*/
//void MyDirectXDraw::__OLD_draw_image_base_center__(LPD3DXSPRITE sprite, MyDirectXImage* image, float x, float y, UINT color)
//{
//	//sprite->Begin(D3DXSPRITE_ALPHABLEND);
//
//	int width = image->get_src_image_width();
//	int height = image->get_src_image_height();
//
//	sprite->Draw(image->get_texture(), NULL, NULL, &D3DXVECTOR3(x - width / 2, y - height / 2, 0), color);
//
//	//sprite->End();
//}

//void MyDirectXDraw::__OLD_draw_image_base_center_enable_size__(LPD3DXSPRITE sprite, MyDirectXImage* image, float x, float y, double size_percent, UINT color)
//{
//	//sprite->Begin(D3DXSPRITE_ALPHABLEND);
//
//	D3DXMATRIX matrix_backup;
//	sprite->GetTransform(&matrix_backup);
//	
//	int width = image->get_src_image_width();
//	int height = image->get_src_image_height();
//
//	D3DXMATRIX matrix;
//	D3DXMatrixScaling(&matrix, (float)size_percent, (float)size_percent, 1.0f);
//	matrix._41 = x;
//	matrix._42 = y;
//	sprite->SetTransform(&matrix);
//
//	RECT r;
//	r.bottom = height;
//	r.top = 0;
//	r.right = width;
//	r.left = 0;
//	sprite->Draw(image->get_texture(), &r, &D3DXVECTOR3(width / 2, height / 2, 0), NULL, color);
//
//	sprite->SetTransform(&matrix_backup);
//
//	//sprite->End();
//}

///*
//拡大率を指定して真ん中を中心に描画する
//*/
//void MyDirectXDraw::__OLD_draw_image_base_center_settable_angle_enable_size__(LPD3DXSPRITE sprite, MyDirectXImage* image, float x, float y, double angle_x, double angle_y, double angle_z, double size_percent, UINT color)
//{
//	//sprite->Begin(D3DXSPRITE_ALPHABLEND);
//
//	D3DXMATRIX matrix_backup;
//	sprite->GetTransform(&matrix_backup);
//
//	int width = image->get_src_image_width();
//	int height = image->get_src_image_height();
//
//	D3DXMATRIX matrix;
//	D3DXMatrixRotationX(&matrix, angle_x);
//	D3DXMatrixRotationY(&matrix, angle_y);
//	D3DXMatrixRotationZ(&matrix, angle_z);
//	D3DXMatrixScaling(&matrix, (float)size_percent, (float)size_percent, 1.0f);
//	matrix._41 = x;
//	matrix._42 = y;
//	sprite->SetTransform(&matrix);
//
//	RECT r;
//	r.bottom = height;
//	r.top = 0;
//	r.right = width;
//	r.left = 0;
//	sprite->Draw(image->get_texture(), &r, &D3DXVECTOR3(width / 2, height / 2, 0), NULL, color);
//
//	sprite->SetTransform(&matrix_backup);
//
//	//sprite->End();
//}

///*
//真ん中を中心に描画する
//回転あり
//*/
//void MyDirectXDraw::__OLD_draw_image_base_center_settable_angle__(LPD3DXSPRITE sprite, MyDirectXImage *image, float x, float y, double angle_x, double angle_y, double angle_z, UINT color)
//{
//	//sprite->Begin(D3DXSPRITE_ALPHABLEND);
//
//	D3DXMATRIX matrix_backup;
//	sprite->GetTransform(&matrix_backup);
//
//	int width = image->get_src_image_width();
//	int height = image->get_src_image_height();
//
//	D3DXMATRIX matrix;
//	D3DXMatrixRotationX(&matrix, angle_x);
//	D3DXMatrixRotationY(&matrix, angle_y);
//	D3DXMatrixRotationZ(&matrix, angle_z);
//	matrix._41 = x;
//	matrix._42 = y;
//	sprite->SetTransform(&matrix);
//
//	sprite->Draw(image->get_texture(), NULL, &D3DXVECTOR3(width / 2, height / 2, 0), NULL, color);
//
//	sprite->SetTransform(&matrix_backup);
//
//	//sprite->End();
//}

/*
左上を中心に(切り抜き)描画する2
*/
void MyDirectXDraw::draw_cut_image_base_leftup(MyDirectXSystem* myDirectXSystem, MyDirectXImage* image, float x, float y, RECT draw_rect, UINT color)
{
	VERTEX_XYZ_RHW_COLOR_UV vertex[4];

	float start_x, end_x;
	float start_y, end_y;

	float texture_width = (float)(image->get_texture_width());
	float texture_height = (float)(image->get_texture_height());

	float src_image_width = (float)(image->get_src_image_width());
	float src_image_height = (float)(image->get_src_image_height());

	float draw_width = (float)(draw_rect.right - draw_rect.left);
	float draw_height = (float)(draw_rect.bottom - draw_rect.top);

	//float min_u = 0;
	float max_u = src_image_width / texture_width;
	//float min_v = 0;
	float max_v = src_image_height / texture_height;

	float draw_min_u = max_u * (float)((float)draw_rect.left / src_image_width);
	float draw_max_u = max_u * (float)((float)draw_rect.right / src_image_width);
	float draw_min_v = max_v * (float)((float)draw_rect.top / src_image_height);
	float draw_max_v = max_v * (float)((float)draw_rect.bottom / src_image_height);

	//M_debug_printf("=================================\n");
	//M_debug_printf("max_v = %f\n", max_v);
	//M_debug_printf("draw_max_v = %f\n", draw_max_v);
	//M_debug_printf("=================================\n");

	start_x = x;
	start_y = y;
	end_x = start_x + draw_width;
	end_y = start_y + draw_height;

	vertex[0].x = end_x;
	vertex[0].y = start_y;
	vertex[0].z = 0.0f;
	vertex[0].rhw = 1.0f;
	vertex[0].color = color;
	vertex[0].u = draw_max_u;
	vertex[0].v = draw_min_v;

	vertex[1].x = end_x;
	vertex[1].y = end_y;
	vertex[1].z = 0.0f;
	vertex[1].rhw = 1.0f;
	vertex[1].color = color;
	vertex[1].u = draw_max_u;
	vertex[1].v = draw_max_v;

	vertex[2].x = start_x;
	vertex[2].y = start_y;
	vertex[2].z = 0.0f;
	vertex[2].rhw = 1.0f;
	vertex[2].color = color;
	vertex[2].u = draw_min_u;
	vertex[2].v = draw_min_v;

	vertex[3].x = start_x;
	vertex[3].y = end_y;
	vertex[3].z = 0.0f;
	vertex[3].rhw = 1.0f;
	vertex[3].color = color;
	vertex[3].u = draw_min_u;
	vertex[3].v = draw_max_v;

	fix_vertex_pos__VERTEX_XYZ_RHW_COLOR_UV(vertex);

	myDirectXSystem->get_pDevice3D()->SetTexture(0, image->get_texture());
	myDirectXSystem->get_pDevice3D()->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	myDirectXSystem->get_pDevice3D()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertex, sizeof(VERTEX_XYZ_RHW_COLOR_UV));
}

/*
左上を中心に描画する2
回転あり
*/
void MyDirectXDraw::draw_image_base_leftup_settable_angle(MyDirectXSystem* myDirectXSystem, MyDirectXImage* image, float x, float y, double angle_x, double angle_y, double angle_z, UINT color)
{
	float src_image_width = (float)(image->get_src_image_width());
	float src_image_height = (float)(image->get_src_image_height());
	draw_image_base_center_settable_angle_enable_size(myDirectXSystem, image, x + (src_image_width / 2), y + (src_image_height / 2), angle_x, angle_y, angle_z, color);
}

/*
真ん中を中心に描画する2
回転あり
*/
void MyDirectXDraw::draw_image_base_center_settable_angle(MyDirectXSystem* myDirectXSystem, MyDirectXImage* image, float x, float y, double angle_x, double angle_y, double angle_z, UINT color)
{
	int i;

	VERTEX_XYZ_RHW_COLOR_UV vertex[4];

	float tmp_start_x, tmp_end_x;
	float tmp_start_y, tmp_end_y;

	float texture_width = (float)(image->get_texture_width());
	float texture_height = (float)(image->get_texture_height());

	float src_image_width = (float)(image->get_src_image_width());
	float src_image_height = (float)(image->get_src_image_height());

	float max_u = src_image_width / texture_width;
	float max_v = src_image_height / texture_height;

	tmp_start_x = -(src_image_width / 2);
	tmp_start_y = -(src_image_height / 2);
	tmp_end_x = src_image_width / 2;
	tmp_end_y = src_image_height / 2;

	vertex[0].x = tmp_end_x;
	vertex[0].y = tmp_start_y;
	vertex[0].z = 0.0f;
	vertex[0].rhw = 1.0f;
	vertex[0].color = color;
	vertex[0].u = max_u;
	vertex[0].v = 0.0f;

	vertex[1].x = tmp_end_x;
	vertex[1].y = tmp_end_y;
	vertex[1].z = 0.0f;
	vertex[1].rhw = 1.0f;
	vertex[1].color = color;
	vertex[1].u = max_u;
	vertex[1].v = max_v;

	vertex[2].x = tmp_start_x;
	vertex[2].y = tmp_start_y;
	vertex[2].z = 0.0f;
	vertex[2].rhw = 1.0f;
	vertex[2].color = color;
	vertex[2].u = 0.0f;
	vertex[2].v = 0.0f;

	vertex[3].x = tmp_start_x;
	vertex[3].y = tmp_end_y;
	vertex[3].z = 0.0f;
	vertex[3].rhw = 1.0f;
	vertex[3].color = color;
	vertex[3].u = 0.0f;
	vertex[3].v = max_v;

	for (i = 0; i < 4; i++) {
		float tmp_x = vertex[i].x * cos(angle_z) - vertex[i].y * sin(angle_z);
		float tmp_y = vertex[i].x * sin(angle_z) + vertex[i].y * cos(angle_z);
		vertex[i].x = tmp_x + x;
		vertex[i].y = tmp_y + y;
	}

	fix_vertex_pos__VERTEX_XYZ_RHW_COLOR_UV(vertex);

	myDirectXSystem->get_pDevice3D()->SetTexture(0, image->get_texture());
	myDirectXSystem->get_pDevice3D()->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	myDirectXSystem->get_pDevice3D()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertex, sizeof(VERTEX_XYZ_RHW_COLOR_UV));
}

/*
拡大率を指定して真ん中を中心に描画する2
回転あり
*/
void MyDirectXDraw::draw_image_base_center_settable_angle_enable_size(MyDirectXSystem* myDirectXSystem, MyDirectXImage* image, float x, float y, double angle_x, double angle_y, double angle_z, double size_percent, UINT color)
{
	int i;

	VERTEX_XYZ_RHW_COLOR_UV vertex[4];

	float tmp_start_x, tmp_end_x;
	float tmp_start_y, tmp_end_y;

	float texture_width = (float)(image->get_texture_width());
	float texture_height = (float)(image->get_texture_height());

	float src_image_width = (float)(image->get_src_image_width());
	float src_image_height = (float)(image->get_src_image_height());

	float max_u = src_image_width / texture_width;
	float max_v = src_image_height / texture_height;

	tmp_start_x = -(float)((double)(src_image_width / 2) * size_percent);
	tmp_start_y = -(float)((double)(src_image_height / 2) * size_percent);
	tmp_end_x = (float)((double)(src_image_width / 2) * size_percent);
	tmp_end_y = (float)((double)(src_image_height / 2) * size_percent);

	vertex[0].x = tmp_end_x;
	vertex[0].y = tmp_start_y;
	vertex[0].z = 0.0f;
	vertex[0].rhw = 1.0f;
	vertex[0].color = color;
	vertex[0].u = max_u;
	vertex[0].v = 0.0f;

	vertex[1].x = tmp_end_x;
	vertex[1].y = tmp_end_y;
	vertex[1].z = 0.0f;
	vertex[1].rhw = 1.0f;
	vertex[1].color = color;
	vertex[1].u = max_u;
	vertex[1].v = max_v;

	vertex[2].x = tmp_start_x;
	vertex[2].y = tmp_start_y;
	vertex[2].z = 0.0f;
	vertex[2].rhw = 1.0f;
	vertex[2].color = color;
	vertex[2].u = 0.0f;
	vertex[2].v = 0.0f;

	vertex[3].x = tmp_start_x;
	vertex[3].y = tmp_end_y;
	vertex[3].z = 0.0f;
	vertex[3].rhw = 1.0f;
	vertex[3].color = color;
	vertex[3].u = 0.0f;
	vertex[3].v = max_v;

	for (i = 0; i < 4; i++) {
		float tmp_x = vertex[i].x * cos(angle_z) - vertex[i].y * sin(angle_z);
		float tmp_y = vertex[i].x * sin(angle_z) + vertex[i].y * cos(angle_z);
		vertex[i].x = tmp_x + x;
		vertex[i].y = tmp_y + y;
	}

	fix_vertex_pos__VERTEX_XYZ_RHW_COLOR_UV(vertex);

	myDirectXSystem->get_pDevice3D()->SetTexture(0, image->get_texture());
	myDirectXSystem->get_pDevice3D()->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	myDirectXSystem->get_pDevice3D()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertex, sizeof(VERTEX_XYZ_RHW_COLOR_UV));
}

/*
拡大率を指定して真ん中を中心に描画する2
*/
void MyDirectXDraw::draw_image_base_center_enable_size(MyDirectXSystem* myDirectXSystem, MyDirectXImage* image, float x, float y, double size_percent, UINT color)
{
	draw_image_base_center_settable_angle_enable_size(myDirectXSystem, image, x, y, 0.0f, 0.0f, 0.0f, size_percent, color);
}


///*
//左上を中心に(切り抜き)描画する
//*/
//void MyDirectXDraw::__OLD_draw_cut_image_base_leftup__(LPD3DXSPRITE sprite, MyDirectXImage *image, float x, float y, RECT draw_rect, UINT color)
//{
//	//sprite->Begin(D3DXSPRITE_ALPHABLEND);
//
//	sprite->Draw(image->get_texture(), &draw_rect, NULL, &D3DXVECTOR3(x, y, 0), color);
//
//	//sprite->End();
//}

/*
真ん中を中心に(切り抜き)描画する2
*/
void MyDirectXDraw::draw_cut_image_base_center(MyDirectXSystem* myDirectXSystem, MyDirectXImage* image, float x, float y, RECT draw_rect, const int width, const int height, UINT color)
{
	float draw_width = (float)(draw_rect.right - draw_rect.left);
	float draw_height = (float)(draw_rect.bottom - draw_rect.top);
	draw_cut_image_base_leftup(myDirectXSystem, image, x - (draw_width / 2), y - (draw_height / 2), draw_rect, color);
}

///*
//真ん中を中心に(切り抜き)描画する
//*/
//void MyDirectXDraw::__OLD_draw_cut_image_base_center__(LPD3DXSPRITE sprite, MyDirectXImage *image, float x, float y, RECT draw_rect, const int width, const int height, UINT color)
//{
//	//sprite->Begin(D3DXSPRITE_ALPHABLEND);
//
//	sprite->Draw(image->get_texture(), &draw_rect, NULL, &D3DXVECTOR3(x - width / 2, y - height / 2, 0), color);
//
//	//sprite->End();
//}



///*
//左上を中心に描画する
//*/
//void MyDirectXDraw::draw_font_base_leftup(LPD3DXSPRITE sprite, MyDirectXFont *font, TCHAR *str, float x, float y, UINT color)
//{
//	RECT rect = { x, y, x, y };
//
//	font->get_font()->DrawText(
//		sprite,
//		str,
//		-1,
//		&rect,
//		DT_LEFT | DT_NOCLIP | DT_SINGLELINE,
//		color
//	);
//}
//
///*
//右上を中心に描画する
//*/
//void MyDirectXDraw::draw_font_base_rightup(LPD3DXSPRITE sprite, MyDirectXFont *font, TCHAR *str, float x, float y, UINT color)
//{
//	RECT rect = { x, y, x, y };
//
//	font->get_font()->DrawText(
//		sprite,
//		str,
//		-1,
//		&rect,
//		DT_RIGHT | DT_NOCLIP | DT_SINGLELINE,
//		color
//	);
//}
//
///*
//真ん中を中心に描画する
//*/
//void MyDirectXDraw::draw_font_base_center(LPD3DXSPRITE sprite, MyDirectXFont *font, TCHAR *str, float x, float y, UINT color)
//{
//	RECT rect = { x, y, x, y };
//
//	font->get_font()->DrawText(
//		sprite,
//		str,
//		-1,
//		&rect,
//		DT_CENTER | DT_NOCLIP | DT_SINGLELINE,
//		color
//	);
//}
