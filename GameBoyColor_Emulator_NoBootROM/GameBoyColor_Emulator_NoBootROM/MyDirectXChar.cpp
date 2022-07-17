#include "MyDirectXChar.h"

MyDirectXChar::MyDirectXChar(MyDirectXSystem* myDirectXSystem, TCHAR* draw_char, LOGFONT *lf)
    : myDirectXSystem(myDirectXSystem)
{
    HFONT hFont;
    if (!(hFont = CreateFontIndirect(lf))) {
        return;
    }

    // デバイスコンテキスト取得
    // デバイスにフォントを持たせないとGetGlyphOutline関数はエラーとなる
    HDC hdc = GetDC(NULL);
    HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

    // 文字コード取得
    TCHAR* c = draw_char;
    UINT code = 0;
#if _UNICODE
    // unicodeの場合、文字コードは単純にワイド文字のUINT変換です
    code = (UINT)*c;
#else
    // マルチバイト文字の場合、
    // 1バイト文字のコードは1バイト目のUINT変換、
    // 2バイト文字のコードは[先導コード]*256 + [文字コード]です
    if (IsDBCSLeadByte(*c))
        code = (BYTE)c[0] << 8 | (BYTE)c[1];
    else
        code = c[0];
#endif

    int grad = 16;

    // フォントビットマップ取得
    TEXTMETRIC TM;
    GetTextMetrics(hdc, &TM);
    GLYPHMETRICS GM;
    CONST MAT2 Mat = { {0,1},{0,0},{0,0},{0,1} };
    DWORD size = GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &GM, 0, NULL, &Mat);
    BYTE* ptr = new BYTE[size];
    GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &GM, size, ptr, &Mat);

    // デバイスコンテキストとフォントハンドルの開放
    SelectObject(hdc, oldFont);
    DeleteObject(hFont);
    ReleaseDC(NULL, hdc);


    // 頂点情報
    //float a = 1.0f; // テクスチャの縮尺

    float fTexW = (GM.gmBlackBoxX + 3) / 4 * 4; // テクスチャの横幅
    float fTexH = GM.gmBlackBoxY; // テクスチャの高さ

    char_width = GM.gmCellIncX;// *a;
    char_height = TM.tmHeight;// *a;

    /*
    空白はスキップする
    */
    if (IsDBCSLeadByte(*draw_char)) {//2バイト文字のとき
        TCHAR* cmp_str = _T("　");
        if (draw_char[0] == cmp_str[0] &&
            draw_char[1] == cmp_str[1])
        {
            M_debug_printf("************************************\n");
            M_debug_printf("空白(2byte)\n");
            M_debug_printf("************************************\n");

            skip_flag = true;
        }
    }
    else if (draw_char[0] == ' ')
    {
        M_debug_printf("************************************\n");
        M_debug_printf("空白(1byte)\n");
        M_debug_printf("************************************\n");

        skip_flag = true;
    }

    float start_x = GM.gmptGlyphOrigin.x;
    float start_y = TM.tmAscent - GM.gmptGlyphOrigin.y;

#define GARBAGE 0xFFFFFFFF//0xDEADBEEF
    base_vertex[0].x = fTexW + start_x;
    base_vertex[0].y = 0.0f + start_y;
    base_vertex[0].z = 0.0f;
    base_vertex[0].rhw = 1.0f;
    base_vertex[0].color = GARBAGE;
    base_vertex[0].u = 1.0f;
    base_vertex[0].v = 0.0f;

    base_vertex[1].x = fTexW + start_x;
    base_vertex[1].y = fTexH + start_y;
    base_vertex[1].z = 0.0f;
    base_vertex[1].rhw = 1.0f;
    base_vertex[1].color = GARBAGE;
    base_vertex[1].u = 1.0f;
    base_vertex[1].v = 1.0f;

    base_vertex[2].x = 0.0f + start_x;
    base_vertex[2].y = 0.0f + start_y;
    base_vertex[2].z = 0.0f;
    base_vertex[2].rhw = 1.0f;
    base_vertex[2].color = GARBAGE;
    base_vertex[2].u = 0.0f;
    base_vertex[2].v = 0.0f;

    base_vertex[3].x = 0.0f + start_x;
    base_vertex[3].y = fTexH + start_y;
    base_vertex[3].z = 0.0f;
    base_vertex[3].rhw = 1.0f;
    base_vertex[3].color = GARBAGE;
    base_vertex[3].u = 0.0f;
    base_vertex[3].v = 1.0f;

    //// 頂点バッファ作成
    //if (FAILED(myDirectXSystem->get_pDevice3D()->CreateVertexBuffer(sizeof(VERTEX_XYZ_RHW_COLOR_UV) * 4, D3DUSAGE_WRITEONLY, FVF_CUSTOM,
    //    D3DPOOL_MANAGED, &pVertex, NULL))) {
    //    delete[] ptr;
    //    return;
    //}
    //
    //// 頂点情報の書き込み
    //void* pData;
    //if (FAILED(pVertex->Lock(0, sizeof(VERTEX_XYZ_RHW_COLOR_UV) * 4, (void**)&pData, 0))) {
    //    return;
    //}
    //memcpy(pData, base_vertex, sizeof(VERTEX_XYZ_RHW_COLOR_UV) * 4);
    //pVertex->Unlock();

    //// テクスチャ作成
    //if (FAILED(myDirectXSystem->get_pDevice3D()->CreateTexture(GM.gmCellIncX, TM.tmHeight, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pTexture, NULL)))
    //{
    //    if (FAILED(myDirectXSystem->get_pDevice3D()->CreateTexture(GM.gmCellIncX, TM.tmHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture, NULL)))
    //    {
    //        delete[] ptr;
    //        return;
    //    }
    //}
    //
    //// テクスチャにフォントビットマップ書き込み
    //D3DLOCKED_RECT LockedRect;
    //if (FAILED(pTexture->LockRect(0, &LockedRect, NULL, D3DLOCK_DISCARD)))
    //{
    //    if (FAILED(pTexture->LockRect(0, &LockedRect, NULL, 0)))
    //    {
    //        delete[] ptr;
    //        return;
    //    }
    //}
    //
    //// フォント情報の書き込み
    //// iOfs_x, iOfs_y : 書き出し位置(左上)
    //// iBmp_w, iBmp_h : フォントビットマップの幅高
    //// Level : α値の段階 (GGO_GRAY4_BITMAPなので17段階)
    //int iOfs_x = GM.gmptGlyphOrigin.x;
    //int iOfs_y = TM.tmAscent - GM.gmptGlyphOrigin.y;
    //int iBmp_w = GM.gmBlackBoxX + (4 - (GM.gmBlackBoxX % 4)) % 4;
    //int iBmp_h = GM.gmBlackBoxY;
    //int Level = 17;
    //int x, y;
    //DWORD Alpha, Color;
    //FillMemory(LockedRect.pBits, LockedRect.Pitch * TM.tmHeight, 0);
    //for (y = iOfs_y; y < iOfs_y + iBmp_h; y++) {
    //    for (x = iOfs_x; x < iOfs_x + GM.gmBlackBoxX; x++) {
    //        Alpha = (255 * ptr[x - iOfs_x + iBmp_w * (y - iOfs_y)]) / (Level - 1);
    //        Color = 0x00ffffff | (Alpha << 24);
    //        memcpy((BYTE*)LockedRect.pBits + LockedRect.Pitch * y + 4 * x, &Color, sizeof(DWORD));
    //    }
    //}
    //
    //pTexture->UnlockRect(0);
    //delete[] ptr;

    //pVertex->Release();

    // テクスチャ作成
    int fontWidth = (GM.gmBlackBoxX + 3) / 4 * 4;
    int fontHeight = GM.gmBlackBoxY;
    if (FAILED(myDirectXSystem->get_pDevice3D()->CreateTexture(fontWidth, fontHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture, NULL)))
    {
        delete[] ptr;
        return;
    }

    // テクスチャにフォントビットマップ情報を書き込み
    D3DLOCKED_RECT lockedRect;
    if (FAILED(pTexture->LockRect(0, &lockedRect, NULL, 0)))
    {
        delete[] ptr;
        return;
    }
    DWORD* pTexBuf = (DWORD*)lockedRect.pBits;   // テクスチャメモリへのポインタ

    for (int y = 0; y < fontHeight; y++) {
        for (int x = 0; x < fontWidth; x++) {
            DWORD alpha = ptr[x + fontWidth * y] * 255 / grad;
            pTexBuf[y * fontWidth + x] = (alpha << 24) | 0x00ffffff;
        }
    }

    pTexture->UnlockRect(0);  // アンロック
    delete[] ptr;
}

MyDirectXChar::~MyDirectXChar()
{
    if (pTexture != nullptr) {
        pTexture->Release();
    }
    //if (pVertex != nullptr) {
    //    pVertex->Release();
    //}
}

float MyDirectXChar::get_width()
{
    return char_width;
}

float MyDirectXChar::get_height()
{
    return char_height;
}

void MyDirectXChar::draw(float x, float y, UINT color)
{
    if (skip_flag == true) {
        return;
    }

    int i;

    VERTEX_XYZ_RHW_COLOR_UV vertex[4];

    memcpy(vertex, base_vertex, sizeof(VERTEX_XYZ_RHW_COLOR_UV) * 4);

    for (i = 0; i < 4; i++) {
        vertex[i].x += x;
        vertex[i].y += y;
        vertex[i].color = color;


        //ラスタライズルール
        vertex[i].x = (int)(vertex[i].x) + 0.5f;
        vertex[i].y = (int)(vertex[i].y) + 0.5f;


        //M_debug_printf("vertex[%d].x = %f\n", i, vertex[i].x);
        //M_debug_printf("vertex[%d].y = %f\n", i, vertex[i].y);
    }

    myDirectXSystem->get_pDevice3D()->SetTexture(0, pTexture);
    myDirectXSystem->get_pDevice3D()->SetFVF(FVF_CUSTOM);
    myDirectXSystem->get_pDevice3D()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertex, sizeof(VERTEX_XYZ_RHW_COLOR_UV));
}
