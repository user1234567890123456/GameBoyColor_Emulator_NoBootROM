#pragma comment(lib, "comctl32.lib")

#include <windows.h>
#include <commctrl.h>
#include <tchar.h>

#include <iostream>
#include <fstream>

#include <vector>

#include "resource.h"
#include "resource1.h"

#include "SystemDefine.h"
//#include "MaterialArchiveFile.h"
#include "MyDirectXDraw.h"
#include "MyDirectXFont.h"
#include "MyDirectXSystem.h"
#include "MyDirectXImage.h"
#include "GameManager.h"
#include "Key.h"
#include "Fps.h"
#include "My_Random.h"
#include "My_Input.h"
#include "CRC.h"
#include "XOR.h"
#include "Mutex.h"
#include "Main.h"

using namespace std;

bool Main::ready_read_ROM_flag = false;
TCHAR Main::ROMFilePath[] = {0};

bool Main::ROM_loaded_flag = false;
uint8_t Main::Cartridge_Type = 0;
uint32_t Main::PGM_size = 0;
uint32_t Main::SRAM_size = 0;
Main::GAME_HARDWARE_TYPE Main::game_hardware_type = Main::GAME_HARDWARE_TYPE::GAMEBOY;

bool Main::Sound_Channel1_Mute_Flag = false;
bool Main::Sound_Channel2_Mute_Flag = false;
bool Main::Sound_Channel3_Mute_Flag = false;
bool Main::Sound_Channel4_Mute_Flag = false;

bool Main::Show_FPS_Flag = true;

bool Main::Show_DEBUG_INFO_Flag = false;


//bool Main::Cheat_Window_Exist_Flag = false;


//int frame_process_type = 0;
//bool fullscreen_result = true;


HINSTANCE global_hInstance;

HWND main_hwnd;


#define CHEAT_BK_COLOR_R 0xAE
#define CHEAT_BK_COLOR_G 0xB2
#define CHEAT_BK_COLOR_B 0xB1

#define CHEAT_RESULT_BK_COLOR_R 0xD1
#define CHEAT_RESULT_BK_COLOR_G 0xD6
#define CHEAT_RESULT_BK_COLOR_B 0xD5

#define BUTTON_ID_CHEATCODE_APPLY 100
#define BUTTON_ID_RESIDENT_CHEATCODE_ADD 101
#define BUTTON_ID_RESIDENT_CHEATCODE_DELETE 102
#define BUTTON_ID_MEMORY_SEARCH_START 103
#define BUTTON_ID_MEMORY_NEXTSEARCH_START 104

HWND cheat_hwnd;

HWND cheat_textedit;
HWND cheat_apply_button;

HWND resident_cheat_textedit;
HWND apply_resident_cheat_listview;
HWND resident_cheat_apply_button;
HWND resident_cheat_delete_button;

HWND search_result_listview;
HWND search_value_text;
HWND search_8bit_radiobutton;
HWND search_16bit_radiobutton;
HWND search_start_button;
HWND next_search_equal_radiobutton;
HWND next_search_not_equal_radiobutton;
HWND next_search_biggar_radiobutton;
HWND next_search_smaller_radiobutton;
HWND next_search_unknown_radiobutton;
HWND next_search_cmp_prevvalue_equal_radiobutton;
HWND next_search_cmp_prevvalue_not_equal_radiobutton;
HWND next_search_start_button;


vector<resident_cheat_info> resident_cheat_info_array;

vector<found_info> found_address_info_list;



/*
プロトタイプ宣言
*/
LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK CheatWinProc(HWND, UINT, WPARAM, LPARAM);

void clear_resident_cheat_code_list_ptr_array();
int cheat_code_parse(TCHAR*, vector<uint32_t>& cheat_code_list);
uint8_t charhex_to_uint8(char);
char uint8_to_charhex(uint8_t);

void update_resident_cheat_list();
void get_resident_cheat_code_str(TCHAR*, uint32_t*, size_t);

int parse_search_4byte_value(uint16_t*, TCHAR*, uint32_t);
void clear_found_address_info_list();

void process_first_search_result();

void update_sound_mute_setting();


void __app_safe_exit_error__() {
	Mutex::get_instance_ptr()->delete_app_mutex();
	exit(EXIT_FAILURE);
}

void __app_safe_exit_normal__() {
	Mutex::get_instance_ptr()->delete_app_mutex();
	exit(0);
}

/*
##############################################
#ここからプログラムが始まる
##############################################
*/
int _stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR command, int commandShow)
{
	//複数同時起動できないようにする
	Mutex::Init();
	Mutex::get_instance_ptr()->create_app_mutex();

//################################################

	global_hInstance = hInstance;

	CRC::Init();


#ifdef GAMEBOYCOLOR_EMULATOR_DEBUG
	//if (full_result == IDNO) {//NO
//	if (fullscreen_result == false) {//NO
		AllocConsole();
		FILE* fp;
		freopen_s(&fp, "CONOUT$", "w", stdout);
		freopen_s(&fp, "CONIN$", "r", stdin);

		M_debug_printf("\n");
		M_debug_printf("=======================================\n");
		M_debug_printf("GameBoyColor_Emulator Debug Console\n");
		M_debug_printf("=======================================\n");
		M_debug_printf("\n");
//	}
#endif

//	M_debug_printf("###################################\n");
//	M_debug_printf("###################################\n");
//	M_debug_printf("frame_process_type = %d\n", frame_process_type);
//	M_debug_printf("fullscreen_result = %s\n", (fullscreen_result ? "true" : "false"));
//	M_debug_printf("app_exit_flag = %s\n", (app_exit_flag ? "true" : "false"));
//	M_debug_printf("###################################\n");
//	M_debug_printf("###################################\n");


	//カーソルを変更する
	HCURSOR hCursor;
	hCursor = LoadCursor(NULL, IDC_ARROW);//標準矢印カーソル
	SetCursor(hCursor);
///	//if (full_result == IDNO) {//NO
///	if (fullscreen_result == false) {//NO
///		//カーソルを変更する
///		HCURSOR hCursor;
///		hCursor = LoadCursor(NULL, IDC_ARROW);//標準矢印カーソル
///		SetCursor(hCursor);
///	}
///	else {//YES
///		ShowCursor(FALSE);//カーソルを表示しない
///	}


	const TCHAR* WINDOW_DEF_STR = _T("WINDOW");
	const TCHAR* WINDOW_TITLE_STR = _T(TITLE);

	/*
	WNDCLASSEX構造体の設定
	*/
	WNDCLASSEX wnd_ex;
	wnd_ex.cbSize = sizeof(WNDCLASSEX);
	wnd_ex.style = CS_HREDRAW | CS_VREDRAW;
	wnd_ex.lpfnWndProc = WinProc;
	wnd_ex.cbClsExtra = 0;
	wnd_ex.cbWndExtra = 0;
	wnd_ex.hInstance = hInstance;
	wnd_ex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wnd_ex.hCursor = NULL;
	wnd_ex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wnd_ex.lpszMenuName = "IDR_MENU1";
	wnd_ex.lpszClassName = WINDOW_DEF_STR;
	wnd_ex.hIconSm = NULL;

	/*
	ウインドウの登録をする
	*/
	if (RegisterClassEx(&wnd_ex) == FALSE) {
		//エラーが発生した場合プログラムを終了する
		MessageBox(NULL, _T("ウインドウの初期化の際にエラーが発生しました"), _T("ERROR"), MB_OK | MB_ICONERROR);

		__app_safe_exit_error__();

		return EXIT_FAILURE;
	}

	/*
	ウインドウの描画範囲をWINDOW_WIDTHとWINDOW_HEIGHTの大きさにする
	*/
	RECT window_rect;
	SetRect(&window_rect, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	AdjustWindowRectEx(&window_rect, WS_CAPTION | WS_SYSMENU | WS_VISIBLE, true, 0);

	main_hwnd = CreateWindowEx(
		0,
		WINDOW_DEF_STR,
		WINDOW_TITLE_STR,
		WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		window_rect.right - window_rect.left,
		window_rect.bottom - window_rect.top,
		NULL,
		NULL,
		hInstance,
		NULL);

	SetWindowPos(main_hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);

///	//if (full_result == IDNO) {//フルスクリーンでない時
///	if (fullscreen_result == false) {//フルスクリーンでない時
///
///		/*
///		ウインドウの描画範囲をWINDOW_WIDTHとWINDOW_HEIGHTの大きさにする
///		*/
///		RECT window_rect;
///		SetRect(&window_rect, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
///		AdjustWindowRectEx(&window_rect, WS_CAPTION | WS_SYSMENU | WS_VISIBLE, false, 0);
///
///		h_wnd = CreateWindowEx(
///			0,
///			WINDOW_DEF_STR,
///			WINDOW_TITLE_STR,
///			WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
///			CW_USEDEFAULT,
///			CW_USEDEFAULT,
///			window_rect.right - window_rect.left,
///			window_rect.bottom - window_rect.top,
///			NULL,
///			NULL,
///			hInstance,
///			NULL);
///
///		SetWindowPos(h_wnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
///	}
///	else {//フルスクリーンの時
///
///		//解像度の変更
///		DEVMODE mode = {};
///		BOOL result = EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &mode);
///		mode.dmPelsWidth = WINDOW_WIDTH;
///		mode.dmPelsHeight = WINDOW_HEIGHT;
///		ChangeDisplaySettings(&mode, CDS_FULLSCREEN);
///
///		int display_width = GetSystemMetrics(SM_CXSCREEN);
///		int display_height = GetSystemMetrics(SM_CYSCREEN);
///
///		h_wnd = CreateWindowEx(
///			0,
///			WINDOW_DEF_STR,
///			WINDOW_TITLE_STR,
///			WS_VISIBLE | WS_POPUP,
///			0,
///			0,
///			display_width,
///			display_height,
///			NULL,
///			NULL,
///			hInstance,
///			NULL);
///
///		SetWindowPos(h_wnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
///	}

	if (main_hwnd == NULL) {
		MessageBox(NULL, _T("ウインドウの初期化の際にエラーが発生しました"), _T("ERROR"), MB_OK | MB_ICONERROR);

		__app_safe_exit_error__();

		return EXIT_FAILURE;
	}

	//==============================================================================
	//==============================================================================
	//==============================================================================

	const TCHAR* CHEAT_WINDOW_DEF_STR = _T("CHEAT_WINDOW");
	const TCHAR* CHEAT_WINDOW_TITLE_STR = _T("チート/メモリのウインドウ");

	WNDCLASSEX cheat_wnd_ex;
	cheat_wnd_ex.cbSize = sizeof(WNDCLASSEX);
	cheat_wnd_ex.style = CS_HREDRAW | CS_VREDRAW;
	cheat_wnd_ex.lpfnWndProc = CheatWinProc;
	cheat_wnd_ex.cbClsExtra = 0;
	cheat_wnd_ex.cbWndExtra = 0;
	cheat_wnd_ex.hInstance = hInstance;
	cheat_wnd_ex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	cheat_wnd_ex.hCursor = NULL;
	cheat_wnd_ex.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(CHEAT_BK_COLOR_R, CHEAT_BK_COLOR_G, CHEAT_BK_COLOR_B));
	cheat_wnd_ex.lpszMenuName = NULL;
	cheat_wnd_ex.lpszClassName = CHEAT_WINDOW_DEF_STR;
	cheat_wnd_ex.hIconSm = NULL;

	/*
	ウインドウの登録をする
	*/
	if (RegisterClassEx(&cheat_wnd_ex) == FALSE) {
		//エラーが発生した場合プログラムを終了する
		MessageBox(NULL, _T("サブウインドウの初期化の際にエラーが発生しました"), _T("ERROR"), MB_OK | MB_ICONERROR);

		__app_safe_exit_error__();

		return EXIT_FAILURE;
	}

	window_rect;
	SetRect(&window_rect, 0, 0, 800, 630);
	AdjustWindowRectEx(&window_rect, WS_CAPTION | WS_SYSMENU | WS_VISIBLE, false, /*WS_EX_TOOLWINDOW*/0);

	cheat_hwnd = CreateWindowEx(
		/*WS_EX_TOOLWINDOW*/0,
		CHEAT_WINDOW_DEF_STR,
		CHEAT_WINDOW_TITLE_STR,
		WS_CAPTION | WS_SYSMENU | WS_VISIBLE/*WS_OVERLAPPEDWINDOW | WS_VISIBLE*/,
		CW_USEDEFAULT, CW_USEDEFAULT,
		window_rect.right - window_rect.left,
		window_rect.bottom - window_rect.top,
		NULL, NULL,
		hInstance, NULL
	);
	if (cheat_hwnd == NULL) {
		MessageBox(NULL, _T("サブウインドウの初期化の際にエラーが発生しました"), _T("ERROR"), MB_OK | MB_ICONERROR);

		__app_safe_exit_error__();

		return EXIT_FAILURE;
	}

	ShowWindow(cheat_hwnd, SW_HIDE);


	//MaterialArchiveFile::Init();


	D3DPRESENT_PARAMETERS g_D3DParam;
	/*
	DirectX初期化
	*/
	MyDirectXSystem myDirectXSystem(main_hwnd, WINDOW_WIDTH, WINDOW_HEIGHT, 0, &g_D3DParam, TRUE/*((fullscreen_result == false) ? TRUE : FALSE)*/);

	//LPD3DXSPRITE sprite;
	//if (FAILED(D3DXCreateSprite(myDirectXSystem.get_pDevice3D(), &sprite))) {
	//	MessageBox(NULL, _T("DirectXの初期化の際にエラーが発生しました"), _T("ERROR"), MB_OK | MB_ICONERROR);
	//
	//	__app_safe_exit_error__();
	//
	//	return EXIT_FAILURE;
	//}



	My_Random::_make_singleton();

	Key key;
	//GameManager* game_manager = new GameManager(&myDirectXSystem, &key/*, sprite*/);
	GameManager::Init(&myDirectXSystem, &key);

	Fps fps(&myDirectXSystem/*, sprite*/);

	/*
	いろいろ初期化
	*/

	My_Input::_make_singleton(main_hwnd, hInstance);


	//リフレッシュレートを計算する
	HDC hdc = GetDC(main_hwnd);
	int refresh_rate = GetDeviceCaps(hdc, VREFRESH);
	M_debug_printf("################################\n");
	M_debug_printf("refresh_rate = %d\n", refresh_rate);
	M_debug_printf("################################\n");

	bool wait_flag = false;
//	if (frame_process_type == 0) {
//		if (refresh_rate <= 60) {
//			wait_flag = false;
//		}
//		else {
//			wait_flag = true;
//		}
//	}
//	else if (frame_process_type == 1) {
//		wait_flag = true;
//	}
//	else {
//		wait_flag = true;
//	}
	if (refresh_rate <= 60) {
		wait_flag = false;
	}
	else {
		wait_flag = true;
	}


	M_debug_printf("################################\n");
	M_debug_printf("wait_flag = %s\n", (wait_flag ? "true" : "false"));
	M_debug_printf("################################\n");


	/*
	メッセージループ処理
	*/
	MSG msg = {};
	while (msg.message != WM_QUIT) {
		//fps.update(); //ここだと正確に測れない

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			if (SUCCEEDED(myDirectXSystem.get_pDevice3D()->BeginScene()) == TRUE) {
				My_Input::get_instance_ptr()->update_process();

				//背景色
				DWORD ClearColor = 0xFFFFFF;
				//画面をクリアーする
				myDirectXSystem.get_pDevice3D()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, ClearColor, 1.0f, 0);

				//__SPRITE__
				//sprite->Begin(D3DXSPRITE_ALPHABLEND);

				fps.update();

				MyDirectXDraw::set_default_state(&myDirectXSystem);
				MyDirectXDraw::enable_alpha(&myDirectXSystem);
				MyDirectXDraw::set_sampler_state__2d(&myDirectXSystem);

				//######################################################################################################
				//#
				//#ここでゲームの描画や処理をする
				//#
				//######################################################################################################

				key.key_state_update();

				//game_manager->execute_game_process();
				GameManager::get_instance_ptr()->execute_game_process();

				//######################################################################################################
				//#
				//#ここまでゲームの描画や処理をする
				//#
				//######################################################################################################


				if (Main::Show_FPS_Flag == true) {
					fps.draw_fps();
				}

				//__SPRITE__
				//sprite->End();

				myDirectXSystem.get_pDevice3D()->EndScene();

				//画面に反映する
				HRESULT present_result_1 = myDirectXSystem.get_pDevice3D()->Present(NULL, NULL, NULL, NULL);
				if (present_result_1 == D3DERR_DEVICELOST) {
					HRESULT present_result_2 = myDirectXSystem.get_pDevice3D()->TestCooperativeLevel();

					if (present_result_2 != D3D_OK) {
						if (present_result_2 == D3DERR_DEVICENOTRESET) {
							if (myDirectXSystem.get_pDevice3D()->Reset(&g_D3DParam) != D3D_OK) {
								__app_safe_exit_error__();
							}
						}
					}

				}

				if (present_result_1 != D3D_OK) {//Presentが失敗したとき
					if (wait_flag == false) {
						fps.wait();//高速動作を防ぐ
					}
				}
				
				
				if (wait_flag == true) {
					fps.wait();
				}

			}
		}
	}

	/*
	いろいろ解放する
	*/

	//delete game_manager;
	GameManager::End();

	My_Input::_delete_singleton();

	My_Random::_delete_singleton();

	CRC::End();

	//MaterialArchiveFile::End();

#ifdef GAMEBOYCOLOR_EMULATOR_DEBUG
	//if (full_result == IDNO) {
//	if (fullscreen_result == false) {
		FreeConsole();
//	}
#endif

	Mutex::get_instance_ptr()->delete_app_mutex();
	Mutex::End();

	return 0;

}


/*
コールバック関数(ゲーム画面)
*/
LRESULT CALLBACK WinProc(HWND h_wnd, UINT u_msg, WPARAM w_param, LPARAM l_param) {
	static HMENU hmenu;
	static MENUITEMINFO menuInfo;

	static OPENFILENAME ofn = { 0 };
	static TCHAR strCustom[256] = TEXT("All files\0*.*\0\0");

	switch (u_msg) {
	case WM_DESTROY:
		PostQuitMessage(0);

		return 0;
	/*case WM_KEYDOWN:
		if (w_param == VK_ESCAPE) {//Escキーを押したとき終了する
			PostMessage(h_wnd, WM_CLOSE, 0, 0);
		}

		return 0;*/
	case WM_CREATE:
		hmenu = GetMenu(h_wnd);
		menuInfo.cbSize = sizeof(MENUITEMINFO);
		menuInfo.fState = MFS_UNCHECKED;

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = h_wnd;
		ofn.lpstrFilter = //TEXT("ゲームボーイ(カラー)のROMのやつ (*.gb)\0*.gb\0")
						  TEXT("ゲームボーイ(カラー)のROMのやつ (*.gb;*.gbc)\0*.gb;*.gbc\0")
						  TEXT("すべてのファイル (*.*)\0*.*\0\0");
		ofn.lpstrCustomFilter = strCustom;
		ofn.nMaxCustFilter = 256;
		ofn.nFilterIndex = 0;
		ofn.lpstrFile = Main::ROMFilePath;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_FILEMUSTEXIST;

		menuInfo.fMask = MIIM_STATE;
		menuInfo.fState = MFS_CHECKED;
		SetMenuItemInfo(hmenu, ID_40008, FALSE, &menuInfo);

		menuInfo.fMask = MIIM_STATE;
		menuInfo.fState = MFS_UNCHECKED;
		SetMenuItemInfo(hmenu, ID_40009, FALSE, &menuInfo);

		menuInfo.fMask = MIIM_STATE;
		menuInfo.fState = MFS_UNCHECKED;
		SetMenuItemInfo(hmenu, ID_40011, FALSE, &menuInfo);

		return 0;
	case WM_COMMAND:
		switch (LOWORD(w_param)) {
		case ID_40003:
			if (Main::ready_read_ROM_flag == false) {
				if (GetOpenFileName(&ofn) == TRUE) {//キャンセルなどをしていないとき
					Main::ready_read_ROM_flag = true;

					ListView_DeleteAllItems(search_result_listview);
					clear_found_address_info_list();
					ListView_DeleteAllItems(apply_resident_cheat_listview);
					clear_resident_cheat_code_list_ptr_array();
				}
			}
			else {
				MessageBox(cheat_hwnd, _T("ROM読み込みの際にエラーが発生しました\n再度お試しください"), _T("情報"), MB_ICONWARNING | MB_APPLMODAL);
			}

			return 0;
		case ID_40006:
			if (Main::ROM_loaded_flag == true) {
				char cart_type_name[256];
				if (Main::Cartridge_Type == 0x00) {
					strcpy_s(cart_type_name, 256, "ROM ONLY");
				}
				else if (Main::Cartridge_Type == 0x01) {
					strcpy_s(cart_type_name, 256, "MBC1");
				}
				else if (Main::Cartridge_Type == 0x02) {
					strcpy_s(cart_type_name, 256, "MBC1+RAM");
				}
				else if (Main::Cartridge_Type == 0x03) {
					strcpy_s(cart_type_name, 256, "MBC1+RAM+BATTERY");
				}
				else if (Main::Cartridge_Type == 0x05) {
					strcpy_s(cart_type_name, 256, "MBC2");
				}
				else if (Main::Cartridge_Type == 0x06) {
					strcpy_s(cart_type_name, 256, "MBC2+BATTERY");
				}
				else if (Main::Cartridge_Type == 0x08) {
					strcpy_s(cart_type_name, 256, "ROM+RAM 1");
				}
				else if (Main::Cartridge_Type == 0x09) {
					strcpy_s(cart_type_name, 256, "ROM+RAM+BATTERY 1");
				}
				else if (Main::Cartridge_Type == 0x0B) {
					strcpy_s(cart_type_name, 256, "MMM01");
				}
				else if (Main::Cartridge_Type == 0x0C) {
					strcpy_s(cart_type_name, 256, "MMM01+RAM");
				}
				else if (Main::Cartridge_Type == 0x0D) {
					strcpy_s(cart_type_name, 256, "MMM01+RAM+BATTERY");
				}
				else if (Main::Cartridge_Type == 0x0F) {
					strcpy_s(cart_type_name, 256, "MBC3+TIMER+BATTERY");
				}
				else if (Main::Cartridge_Type == 0x10) {
					strcpy_s(cart_type_name, 256, "MBC3+TIMER+RAM+BATTERY 2");
				}
				else if (Main::Cartridge_Type == 0x11) {
					strcpy_s(cart_type_name, 256, "MBC3");
				}
				else if (Main::Cartridge_Type == 0x12) {
					strcpy_s(cart_type_name, 256, "MBC3+RAM 2");
				}
				else if (Main::Cartridge_Type == 0x13) {
					strcpy_s(cart_type_name, 256, "MBC3+RAM+BATTERY 2");
				}
				else if (Main::Cartridge_Type == 0x19) {
					strcpy_s(cart_type_name, 256, "MBC5");
				}
				else if (Main::Cartridge_Type == 0x1A) {
					strcpy_s(cart_type_name, 256, "MBC5+RAM");
				}
				else if (Main::Cartridge_Type == 0x1B) {
					strcpy_s(cart_type_name, 256, "MBC5+RAM+BATTERY");
				}
				else if (Main::Cartridge_Type == 0x1C) {
					strcpy_s(cart_type_name, 256, "MBC5+RUMBLE");
				}
				else if (Main::Cartridge_Type == 0x1D) {
					strcpy_s(cart_type_name, 256, "MBC5+RUMBLE+RAM");
				}
				else if (Main::Cartridge_Type == 0x1E) {
					strcpy_s(cart_type_name, 256, "MBC5+RUMBLE+RAM+BATTERY");
				}
				else if (Main::Cartridge_Type == 0x20) {
					strcpy_s(cart_type_name, 256, "MBC6");
				}
				else if (Main::Cartridge_Type == 0x22) {
					strcpy_s(cart_type_name, 256, "MBC7+SENSOR+RUMBLE+RAM+BATTERY");
				}
				else if (Main::Cartridge_Type == 0xFC) {
					strcpy_s(cart_type_name, 256, "POCKET CAMERA");
				}
				else if (Main::Cartridge_Type == 0xFD) {
					strcpy_s(cart_type_name, 256, "BANDAI TAMA5");
				}
				else if (Main::Cartridge_Type == 0xFE) {
					strcpy_s(cart_type_name, 256, "HuC3");
				}
				else if (Main::Cartridge_Type == 0xFF) {
					strcpy_s(cart_type_name, 256, "HuC1+RAM+BATTERY");
				}
				else {
					strcpy_s(cart_type_name, 256, "UNKNOWN");
				}
				char hardware_type_name[256];
				if (Main::game_hardware_type == Main::GAME_HARDWARE_TYPE::GAMEBOY) {
					strcpy_s(hardware_type_name, 256, "ゲームボーイ");
				}
				else if (Main::game_hardware_type == Main::GAME_HARDWARE_TYPE::GAMEBOY_COLOR) {
					strcpy_s(hardware_type_name, 256, "ゲームボーイカラー");
				}
				else {
					strcpy_s(hardware_type_name, 256, "UNKNOWN");
				}
				char tmp_str_buffer[1024];
				sprintf_s(tmp_str_buffer, "カードリッジのタイプ : [0x%02x] %s\nプログラムROMサイズ : %dKB\nSRAMのサイズ : %dKB\n動作モード : %s", Main::Cartridge_Type, cart_type_name, Main::PGM_size, Main::SRAM_size, hardware_type_name);
				MessageBox(cheat_hwnd, _T(tmp_str_buffer), _T("ROM情報"), MB_ICONINFORMATION | MB_APPLMODAL);
			}
			else {
				MessageBox(cheat_hwnd, _T("まだROMが読み込まれていません"), _T("ROM情報"), MB_ICONINFORMATION | MB_APPLMODAL);
			}

			return 0;
		case ID_40008:
			Main::Show_FPS_Flag = !Main::Show_FPS_Flag;

			if (Main::Show_FPS_Flag == false) {
				menuInfo.fState = MFS_UNCHECKED;
			}
			else {
				menuInfo.fState = MFS_CHECKED;
			}
			SetMenuItemInfo(hmenu, ID_40008, FALSE, &menuInfo);

			return 0;
		case ID_40009:
			Main::Show_DEBUG_INFO_Flag = !Main::Show_DEBUG_INFO_Flag;

			if (Main::Show_DEBUG_INFO_Flag == false) {
				menuInfo.fState = MFS_UNCHECKED;
			}
			else {
				menuInfo.fState = MFS_CHECKED;
			}
			SetMenuItemInfo(hmenu, ID_40009, FALSE, &menuInfo);

			return 0;
		case ID_40007:
			if (My_Input::get_instance_ptr() != nullptr){
				My_Input::get_instance_ptr()->find_and_update_pad();

				MessageBox(cheat_hwnd, _T("ゲームパッドの更新が完了しました"), _T("情報"), MB_ICONINFORMATION | MB_APPLMODAL);
			}
			else {
				MessageBox(cheat_hwnd, _T("ゲームパッドの更新の際にエラーが発生しました\n再度お試しください"), _T("情報"), MB_ICONWARNING | MB_APPLMODAL);
			}

			return 0;
		case ID_40010:
			//if (Main::Cheat_Window_Exist_Flag == true) {
			//	return 0;
			//}

			ShowWindow(cheat_hwnd, SW_SHOW);

			SetWindowPos(cheat_hwnd, HWND_TOP,
				0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE);

			//Main::Cheat_Window_Exist_Flag = true;

			return 0;
		case ID_40011:
			Main::Sound_Channel1_Mute_Flag = true;
			Main::Sound_Channel2_Mute_Flag = true;
			Main::Sound_Channel3_Mute_Flag = true;
			Main::Sound_Channel4_Mute_Flag = true;

			menuInfo.fState = MFS_CHECKED;
			SetMenuItemInfo(hmenu, ID_40013, FALSE, &menuInfo);
			SetMenuItemInfo(hmenu, ID_40014, FALSE, &menuInfo);
			SetMenuItemInfo(hmenu, ID_40015, FALSE, &menuInfo);
			SetMenuItemInfo(hmenu, ID_40016, FALSE, &menuInfo);

			update_sound_mute_setting();

			return 0;
		case ID_40012:
			Main::Sound_Channel1_Mute_Flag = false;
			Main::Sound_Channel2_Mute_Flag = false;
			Main::Sound_Channel3_Mute_Flag = false;
			Main::Sound_Channel4_Mute_Flag = false;

			menuInfo.fState = MFS_UNCHECKED;
			SetMenuItemInfo(hmenu, ID_40013, FALSE, &menuInfo);
			SetMenuItemInfo(hmenu, ID_40014, FALSE, &menuInfo);
			SetMenuItemInfo(hmenu, ID_40015, FALSE, &menuInfo);
			SetMenuItemInfo(hmenu, ID_40016, FALSE, &menuInfo);

			update_sound_mute_setting();

			return 0;
		case ID_40013:
			Main::Sound_Channel1_Mute_Flag = !Main::Sound_Channel1_Mute_Flag;
			if (Main::Sound_Channel1_Mute_Flag == true) {
				menuInfo.fState = MFS_CHECKED;
			}
			else {
				menuInfo.fState = MFS_UNCHECKED;
			}

			SetMenuItemInfo(hmenu, ID_40013, FALSE, &menuInfo);

			update_sound_mute_setting();

			return 0;
		case ID_40014:
			Main::Sound_Channel2_Mute_Flag = !Main::Sound_Channel2_Mute_Flag;
			if (Main::Sound_Channel2_Mute_Flag == true) {
				menuInfo.fState = MFS_CHECKED;
			}
			else {
				menuInfo.fState = MFS_UNCHECKED;
			}

			SetMenuItemInfo(hmenu, ID_40014, FALSE, &menuInfo);

			update_sound_mute_setting();

			return 0;
		case ID_40015:
			Main::Sound_Channel3_Mute_Flag = !Main::Sound_Channel3_Mute_Flag;
			if (Main::Sound_Channel3_Mute_Flag == true) {
				menuInfo.fState = MFS_CHECKED;
			}
			else {
				menuInfo.fState = MFS_UNCHECKED;
			}

			SetMenuItemInfo(hmenu, ID_40015, FALSE, &menuInfo);

			update_sound_mute_setting();

			return 0;
		case ID_40016:
			Main::Sound_Channel4_Mute_Flag = !Main::Sound_Channel4_Mute_Flag;
			if (Main::Sound_Channel4_Mute_Flag == true) {
				menuInfo.fState = MFS_CHECKED;
			}
			else {
				menuInfo.fState = MFS_UNCHECKED;
			}

			SetMenuItemInfo(hmenu, ID_40016, FALSE, &menuInfo);

			update_sound_mute_setting();

			return 0;
		}

		break;
	}

	return DefWindowProc(h_wnd, u_msg, w_param, l_param);
}


LRESULT CALLBACK CheatWinProc(HWND h_wnd, UINT u_msg, WPARAM w_param, LPARAM l_param) {
	HFONT hFont;

	HDC hdc;
	PAINTSTRUCT ps;

	LPTSTR strText;

	LVCOLUMN col;

	switch (u_msg) {
	case WM_COMMAND:
		if (
			(HWND)l_param == search_8bit_radiobutton ||
			(HWND)l_param == search_16bit_radiobutton)
		{
			//M_debug_printf("Select search bit mode\n");

			ListView_DeleteAllItems(search_result_listview);
			clear_found_address_info_list();

			return 0;
		}
		else if (LOWORD(w_param) == BUTTON_ID_CHEATCODE_APPLY) {
			const int CHEAT_CODE_TEXT_LENGTH = GetWindowTextLength(cheat_textedit);
			strText = (LPTSTR)malloc((CHEAT_CODE_TEXT_LENGTH + 1) * sizeof(TCHAR));
			GetWindowText(cheat_textedit, strText, CHEAT_CODE_TEXT_LENGTH + 1);
			strText[CHEAT_CODE_TEXT_LENGTH] = _T('\0');

			//M_debug_printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
			//M_debug_printf("[text_length = %d]\n<text>\n%s\n", CHEAT_CODE_TEXT_LENGTH, strText);
			//M_debug_printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");

			vector<uint32_t> tmp_cheat_code_array;
			int result = cheat_code_parse(strText, tmp_cheat_code_array);

			if (CHEAT_CODE_TEXT_LENGTH == 0) {
				MessageBox(cheat_hwnd, _T("チートコードを入力してください"), _T("情報"), MB_ICONINFORMATION | MB_APPLMODAL);
			}
			else if (result == 0) {
				//for (int i = 0; i < tmp_cheat_code_array.size(); i++) {
				//	M_debug_printf("[i = %d] = 0x%08X\n", i, tmp_cheat_code_array[i]);
				//}

				GameBoyColor* gameboy_ptr = GameManager::get_instance_ptr()->get_gameboy();
				if (gameboy_ptr != nullptr) {
					gameboy_ptr->apply_cheat_code_list(tmp_cheat_code_array);
				}
				else {
					MessageBox(cheat_hwnd, _T("ゲームがロードされていません"), _T("情報"), MB_ICONWARNING | MB_APPLMODAL | MB_APPLMODAL);
				}
			}
			else {
				MessageBox(cheat_hwnd, _T("入力されたチートコードが不正です"), _T("情報"), MB_ICONWARNING | MB_APPLMODAL | MB_APPLMODAL);
			}

			free(strText);

			return 0;
		}
		else if (LOWORD(w_param) == BUTTON_ID_RESIDENT_CHEATCODE_ADD) {
			const int RESIDENT_CHEAT_CODE_TEXT_LENGTH = GetWindowTextLength(resident_cheat_textedit);
			strText = (LPTSTR)malloc((RESIDENT_CHEAT_CODE_TEXT_LENGTH + 1) * sizeof(TCHAR));
			GetWindowText(resident_cheat_textedit, strText, RESIDENT_CHEAT_CODE_TEXT_LENGTH + 1);
			strText[RESIDENT_CHEAT_CODE_TEXT_LENGTH] = _T('\0');

			//M_debug_printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
			//M_debug_printf("#RESIDENT [text_length = %d]\n<text>\n%s\n", RESIDENT_CHEAT_CODE_TEXT_LENGTH, strText);
			//M_debug_printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");

			vector<uint32_t> tmp_resident_cheat_code_array;
			int result = cheat_code_parse(strText, tmp_resident_cheat_code_array);

			if (RESIDENT_CHEAT_CODE_TEXT_LENGTH == 0) {
				MessageBox(cheat_hwnd, _T("チートコードを入力してください"), _T("情報"), MB_ICONINFORMATION | MB_APPLMODAL);
			}
			else if (result == 0) {
				//for (int i = 0; i < tmp_resident_cheat_code_array.size(); i++) {
				//	M_debug_printf("#RESIDENT [i = %d] = 0x%08X\n", i, tmp_resident_cheat_code_array[i]);
				//}

				GameBoyColor* gameboy_ptr = GameManager::get_instance_ptr()->get_gameboy();
				if (gameboy_ptr != nullptr) {
					uint32_t* tmp_code_list = (uint32_t*)malloc(tmp_resident_cheat_code_array.size() * sizeof(uint32_t));
					for (int i = 0; i < tmp_resident_cheat_code_array.size(); i++) {
						tmp_code_list[i] = tmp_resident_cheat_code_array[i];
					}

					resident_cheat_info rci;
					rci.code_total_number = tmp_resident_cheat_code_array.size();
					rci.resident_cheat_code_list_ptr = tmp_code_list;
					resident_cheat_info_array.push_back(rci);

					update_resident_cheat_list();

					gameboy_ptr->update_resident_cheat_code_list(resident_cheat_info_array);
				}
				else {
					MessageBox(cheat_hwnd, _T("ゲームがロードされていません"), _T("情報"), MB_ICONWARNING | MB_APPLMODAL | MB_APPLMODAL);
				}
			}
			else {
				MessageBox(cheat_hwnd, _T("入力されたチートコードが不正です"), _T("情報"), MB_ICONWARNING | MB_APPLMODAL | MB_APPLMODAL);
			}

			free(strText);

			return 0;
		}
		else if (LOWORD(w_param) == BUTTON_ID_RESIDENT_CHEATCODE_DELETE) {
			GameBoyColor* gameboy_ptr = GameManager::get_instance_ptr()->get_gameboy();
			if (gameboy_ptr != nullptr) {
				//for (;;) {
				//	int select_item_index = ListView_GetNextItem(apply_resident_cheat_listview, -1, LVNI_ALL | LVNI_SELECTED);
				//	if (select_item_index == -1) {
				//		break;
				//	}
				//	ListView_DeleteItem(apply_resident_cheat_listview, nItem);
				//}

				int select_item_index = ListView_GetNextItem(apply_resident_cheat_listview, -1, LVNI_ALL | LVNI_SELECTED);

				if (select_item_index != -1) {
					//M_debug_printf("select_item_index = %d\n", select_item_index);
					free(resident_cheat_info_array[select_item_index].resident_cheat_code_list_ptr);

					resident_cheat_info_array.erase(resident_cheat_info_array.begin() + select_item_index);

					update_resident_cheat_list();

					gameboy_ptr->update_resident_cheat_code_list(resident_cheat_info_array);
				}
				else {
					MessageBox(cheat_hwnd, _T("削除する項目が選択されていません"), _T("情報"), MB_ICONWARNING | MB_APPLMODAL);
				}
			}
			else {
				MessageBox(cheat_hwnd, _T("ゲームがロードされていません"), _T("情報"), MB_ICONWARNING | MB_APPLMODAL);
			}

			return 0;
		}
		else if (LOWORD(w_param) == BUTTON_ID_MEMORY_SEARCH_START) {
			const int SEARCH_VALUE_TEXT_LENGTH = GetWindowTextLength(search_value_text);
			strText = (LPTSTR)malloc((SEARCH_VALUE_TEXT_LENGTH + 1) * sizeof(TCHAR));
			GetWindowText(search_value_text, strText, SEARCH_VALUE_TEXT_LENGTH + 1);
			strText[SEARCH_VALUE_TEXT_LENGTH] = _T('\0');

			//M_debug_printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
			//M_debug_printf("[text_length = %d]\n<text>\n%s\n", SEARCH_VALUE_TEXT_LENGTH, strText);
			//M_debug_printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");

			if (SEARCH_VALUE_TEXT_LENGTH == 0) {
				MessageBox(cheat_hwnd, _T("数値を入力してください"), _T("情報"), MB_ICONINFORMATION | MB_APPLMODAL);
			}
			else {
				uint16_t search_value_16bit;
				int result = parse_search_4byte_value(&search_value_16bit, strText, SEARCH_VALUE_TEXT_LENGTH);

				//M_debug_printf("search_value_16bit = 0x%04X\n", search_value_16bit);

				if (result != -1) {
					if (BST_CHECKED == SendMessage(search_8bit_radiobutton, BM_GETCHECK, 0, 0)) {
						//MessageBox(main_window, _T("8bit"), _T("情報"), MB_ICONINFORMATION | MB_APPLMODAL);
						if (search_value_16bit > 0xFF) {
							MessageBox(cheat_hwnd, _T("入力された数値が8bitより大きいです"), _T("情報"), MB_ICONWARNING | MB_APPLMODAL);
						}
						else {
							GameBoyColor* gameboy_ptr = GameManager::get_instance_ptr()->get_gameboy();
							if (gameboy_ptr != nullptr) {
								clear_found_address_info_list();

								gameboy_ptr->first_search_memory((search_value_16bit & 0xFF), found_address_info_list, false);

								process_first_search_result();
							}
							else {
								MessageBox(cheat_hwnd, _T("ゲームがロードされていません"), _T("情報"), MB_ICONWARNING | MB_APPLMODAL);
							}
						}
					}
					else {
						//MessageBox(main_window, _T("16bit"), _T("情報"), MB_ICONINFORMATION | MB_APPLMODAL);

						GameBoyColor* gameboy_ptr = GameManager::get_instance_ptr()->get_gameboy();
						if (gameboy_ptr != nullptr) {
							clear_found_address_info_list();

							gameboy_ptr->first_search_memory(search_value_16bit, found_address_info_list, true);

							process_first_search_result();
						}
						else {
							MessageBox(cheat_hwnd, _T("ゲームがロードされていません"), _T("情報"), MB_ICONWARNING | MB_APPLMODAL);
						}
					}
				}
				else {
					MessageBox(cheat_hwnd, _T("入力された数値が不正です"), _T("情報"), MB_ICONWARNING | MB_APPLMODAL);
				}
			}

			free(strText);

			return 0;
		}
		else if (LOWORD(w_param) == BUTTON_ID_MEMORY_NEXTSEARCH_START) {
			if (BST_CHECKED == SendMessage(next_search_unknown_radiobutton, BM_GETCHECK, 0, 0)) {
				GameBoyColor* gameboy_ptr = GameManager::get_instance_ptr()->get_gameboy();
				if (gameboy_ptr != nullptr) {
					clear_found_address_info_list();

					if (BST_CHECKED == SendMessage(search_8bit_radiobutton, BM_GETCHECK, 0, 0)) {
						gameboy_ptr->search_memory_unknown_value(found_address_info_list, false);
					}
					else {
						gameboy_ptr->search_memory_unknown_value(found_address_info_list, true);
					}

					process_first_search_result();
				}
				else {
					MessageBox(cheat_hwnd, _T("ゲームがロードされていません"), _T("情報"), MB_ICONWARNING | MB_APPLMODAL);
				}
			}
			else if (BST_CHECKED == SendMessage(next_search_cmp_prevvalue_equal_radiobutton, BM_GETCHECK, 0, 0)) {
				GameBoyColor* gameboy_ptr = GameManager::get_instance_ptr()->get_gameboy();
				if (gameboy_ptr != nullptr) {
					if (BST_CHECKED == SendMessage(search_8bit_radiobutton, BM_GETCHECK, 0, 0)) {
						gameboy_ptr->search_memory_cmp_prevvalue_equal(found_address_info_list, false);
					}
					else {
						gameboy_ptr->search_memory_cmp_prevvalue_equal(found_address_info_list, true);
					}

					process_first_search_result();
				}
				else {
					MessageBox(cheat_hwnd, _T("ゲームがロードされていません"), _T("情報"), MB_ICONWARNING | MB_APPLMODAL);
				}
			}
			else if (BST_CHECKED == SendMessage(next_search_cmp_prevvalue_not_equal_radiobutton, BM_GETCHECK, 0, 0)) {
				GameBoyColor* gameboy_ptr = GameManager::get_instance_ptr()->get_gameboy();
				if (gameboy_ptr != nullptr) {
					if (BST_CHECKED == SendMessage(search_8bit_radiobutton, BM_GETCHECK, 0, 0)) {
						gameboy_ptr->search_memory_cmp_prevvalue_not_equal(found_address_info_list, false);
					}
					else {
						gameboy_ptr->search_memory_cmp_prevvalue_not_equal(found_address_info_list, true);
					}

					process_first_search_result();
				}
				else {
					MessageBox(cheat_hwnd, _T("ゲームがロードされていません"), _T("情報"), MB_ICONWARNING | MB_APPLMODAL);
				}
			}
			else {
				const int SEARCH_VALUE_TEXT_LENGTH = GetWindowTextLength(search_value_text);
				strText = (LPTSTR)malloc((SEARCH_VALUE_TEXT_LENGTH + 1) * sizeof(TCHAR));
				GetWindowText(search_value_text, strText, SEARCH_VALUE_TEXT_LENGTH + 1);
				strText[SEARCH_VALUE_TEXT_LENGTH] = _T('\0');

				//M_debug_printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
				//M_debug_printf("[text_length = %d]\n<text>\n%s\n", SEARCH_VALUE_TEXT_LENGTH, strText);
				//M_debug_printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");

				if (SEARCH_VALUE_TEXT_LENGTH == 0) {
					MessageBox(cheat_hwnd, _T("数値を入力してください"), _T("情報"), MB_ICONINFORMATION | MB_APPLMODAL);
				}
				else {
					uint16_t search_value_16bit;
					int result = parse_search_4byte_value(&search_value_16bit, strText, SEARCH_VALUE_TEXT_LENGTH);

					//M_debug_printf("search_value_16bit = 0x%04X\n", search_value_16bit);

					if (result != -1) {
						if (BST_CHECKED == SendMessage(search_8bit_radiobutton, BM_GETCHECK, 0, 0)) {
							//MessageBox(main_window, _T("8bit"), _T("情報"), MB_ICONINFORMATION | MB_APPLMODAL);
							if (search_value_16bit > 0xFF) {
								MessageBox(cheat_hwnd, _T("入力された数値が8bitより大きいです"), _T("情報"), MB_ICONWARNING | MB_APPLMODAL);
							}
							else {
								GameBoyColor* gameboy_ptr = GameManager::get_instance_ptr()->get_gameboy();
								if (gameboy_ptr != nullptr) {
									if (BST_CHECKED == SendMessage(next_search_equal_radiobutton, BM_GETCHECK, 0, 0)) {
										gameboy_ptr->search_memory_cmp_equal((search_value_16bit & 0xFF), found_address_info_list, false);
									}
									else if (BST_CHECKED == SendMessage(next_search_not_equal_radiobutton, BM_GETCHECK, 0, 0)) {
										gameboy_ptr->search_memory_cmp_not_equal((search_value_16bit & 0xFF), found_address_info_list, false);
									}
									else if (BST_CHECKED == SendMessage(next_search_biggar_radiobutton, BM_GETCHECK, 0, 0)) {
										gameboy_ptr->search_memory_cmp_biggar((search_value_16bit & 0xFF), found_address_info_list, false);
									}
									else {//next_search_smaller_radiobutton
										gameboy_ptr->search_memory_cmp_smaller((search_value_16bit & 0xFF), found_address_info_list, false);
									}

									process_first_search_result();
								}
								else {
									MessageBox(cheat_hwnd, _T("ゲームがロードされていません"), _T("情報"), MB_ICONWARNING | MB_APPLMODAL);
								}
							}
						}
						else {
							GameBoyColor* gameboy_ptr = GameManager::get_instance_ptr()->get_gameboy();
							if (gameboy_ptr != nullptr) {
								if (BST_CHECKED == SendMessage(next_search_equal_radiobutton, BM_GETCHECK, 0, 0)) {
									gameboy_ptr->search_memory_cmp_equal(search_value_16bit, found_address_info_list, true);
								}
								else if (BST_CHECKED == SendMessage(next_search_not_equal_radiobutton, BM_GETCHECK, 0, 0)) {
									gameboy_ptr->search_memory_cmp_not_equal(search_value_16bit, found_address_info_list, true);
								}
								else if (BST_CHECKED == SendMessage(next_search_biggar_radiobutton, BM_GETCHECK, 0, 0)) {
									gameboy_ptr->search_memory_cmp_biggar(search_value_16bit, found_address_info_list, true);
								}
								else {//next_search_smaller_radiobutton
									gameboy_ptr->search_memory_cmp_smaller(search_value_16bit, found_address_info_list, true);
								}

								process_first_search_result();
							}
							else {
								MessageBox(cheat_hwnd, _T("ゲームがロードされていません"), _T("情報"), MB_ICONWARNING | MB_APPLMODAL);
							}
						}
					}
					else {
						MessageBox(cheat_hwnd, _T("入力された数値が不正です"), _T("情報"), MB_ICONWARNING | MB_APPLMODAL);
					}
				}

				free(strText);
			}

			return 0;
		}

		break;
	case WM_PAINT:
		hdc = BeginPaint(h_wnd, &ps);
		SetBkMode(hdc, TRANSPARENT);

		TextOut(hdc, 10, 10, _T("チートコード(16進数、複数行可能)  例)ABCD1234"), lstrlen(_T("チートコード(16進数、複数行可能)  例)ABCD1234")));
		TextOut(hdc, 10, 250, _T("チートコード(常駐します、16進数、複数行可能)"), lstrlen(_T("チートコード(常駐します、16進数、複数行可能)")));
		TextOut(hdc, 10, 430, _T("適用中のチートコード一覧"), lstrlen(_T("適用中のチートコード一覧")));
		TextOut(hdc, 410, 10, _T("メモリのサーチの結果(表示するのは1000件まで)"), lstrlen(_T("メモリのサーチの結果(表示するのは1000件まで)")));
		TextOut(hdc, 410, 220, _T("サーチしたい値(16進数)"), lstrlen(_T("サーチしたい値(16進数)")));
		TextOut(hdc, 600, 220, _T("サーチしたい値のサイズ"), lstrlen(_T("サーチしたい値のサイズ")));
		TextOut(hdc, 410, 320, _T("絞り込みのオプション"), lstrlen(_T("絞り込みのオプション")));
		TextOut(hdc, 410, 520, _T("※値は16進数で記入してください"), lstrlen(_T("※値は16進数で記入してください")));
		TextOut(hdc, 410, 545, _T("※常駐コードがあまりにも多すぎると重くなります"), lstrlen(_T("※常駐コードがあまりにも多すぎると重くなります")));
		TextOut(hdc, 410, 570, _T("※メモリのサーチでは非常に時間がかかる場合が"), lstrlen(_T("※メモリのサーチでは非常に時間がかかる場合が")));
		TextOut(hdc, 410, 590, _T("あります"), lstrlen(_T("あります")));

		EndPaint(h_wnd, &ps);

		return 0;
	case WM_CTLCOLORBTN:
	case WM_CTLCOLORSTATIC:
		/*if ((HWND)l_param == search_result_text) {
			SetBkMode((HDC)w_param, TRANSPARENT);

			return (long)CreateSolidBrush(RGB(CHEAT_RESULT_BK_COLOR_R, CHEAT_RESULT_BK_COLOR_G, CHEAT_RESULT_BK_COLOR_B));
		}
		else */
		if (
			(HWND)l_param == next_search_equal_radiobutton ||
			(HWND)l_param == next_search_not_equal_radiobutton ||
			(HWND)l_param == next_search_biggar_radiobutton ||
			(HWND)l_param == next_search_smaller_radiobutton ||
			(HWND)l_param == next_search_unknown_radiobutton ||
			(HWND)l_param == next_search_cmp_prevvalue_equal_radiobutton ||
			(HWND)l_param == next_search_cmp_prevvalue_not_equal_radiobutton ||
			(HWND)l_param == search_8bit_radiobutton ||
			(HWND)l_param == search_16bit_radiobutton)
		{
			SetBkMode((HDC)w_param, TRANSPARENT);

			return (long)CreateSolidBrush(RGB(CHEAT_BK_COLOR_R, CHEAT_BK_COLOR_G, CHEAT_BK_COLOR_B));
		}

		break;
	case WM_CREATE:
		InitCommonControls();

		hFont = CreateFont(20, 0, 0, 0,
			FW_NORMAL, FALSE, FALSE, 0,
			SHIFTJIS_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, FIXED_PITCH, _T("ＭＳ ゴシック")/*_T("ＭＳ Ｐゴシック")*/);

		cheat_textedit = CreateWindow(
			_T("EDIT"),
			NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER |
			WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL |
			ES_LEFT | ES_MULTILINE,
			10, 30,
			380, 150,
			h_wnd,
			NULL,
			global_hInstance,
			NULL);
		SendMessage(cheat_textedit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));


		cheat_apply_button = CreateWindow(
			_T("button"),
			_T("チートコードを適用する"),
			WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
			10, 190,
			380, 30,
			h_wnd,
			(HMENU)BUTTON_ID_CHEATCODE_APPLY,
			global_hInstance,
			NULL);

		resident_cheat_textedit = CreateWindow(
			_T("EDIT"),
			NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER |
			WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL |
			ES_LEFT | ES_MULTILINE,
			10, 270,
			380, 150,
			h_wnd,
			NULL,
			global_hInstance,
			NULL);
		SendMessage(resident_cheat_textedit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));

		apply_resident_cheat_listview = CreateWindowEx(
			0,
			WC_LISTVIEW,
			0,
			WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_NOCOLUMNHEADER | LVS_SINGLESEL,
			10, 450,
			380, 130,
			h_wnd,
			NULL,
			global_hInstance,
			NULL);
		col.mask = LVCF_FMT | LVCF_WIDTH;
		col.fmt = LVCFMT_LEFT;
		col.cx = 380;
		ListView_InsertColumn(apply_resident_cheat_listview, 0, &col);

		resident_cheat_apply_button = CreateWindow(
			_T("button"),
			_T("コードの追加"),
			WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
			10, 590,
			180, 30,
			h_wnd,
			(HMENU)BUTTON_ID_RESIDENT_CHEATCODE_ADD,
			global_hInstance,
			NULL);

		resident_cheat_delete_button = CreateWindow(
			_T("button"),
			_T("コードの削除"),
			WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
			210, 590,
			180, 30,
			h_wnd,
			(HMENU)BUTTON_ID_RESIDENT_CHEATCODE_DELETE,
			global_hInstance,
			NULL);

		/*search_result_text = CreateWindow(
			_T("EDIT"),
			NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER |
			WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL |
			ES_LEFT | ES_MULTILINE | ES_READONLY,
			410, 40,
			380, 180,
			h_wnd,
			NULL,
			global_hInstance,
			NULL);
		SendMessage(search_result_text, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		*/
		search_result_listview = CreateWindowEx(
			0,
			WC_LISTVIEW,
			0,
			WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_NOCOLUMNHEADER,
			410, 30,
			380, 180,
			h_wnd,
			NULL,
			global_hInstance,
			NULL);
		col.mask = LVCF_FMT | LVCF_WIDTH;
		col.fmt = LVCFMT_LEFT;
		col.cx = 380;
		ListView_InsertColumn(search_result_listview, 0, &col);

		search_value_text = CreateWindow(
			_T("EDIT"),
			NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER |
			ES_AUTOHSCROLL | ES_AUTOVSCROLL |
			ES_LEFT,
			410, 240,
			180, 30,
			h_wnd,
			NULL,
			global_hInstance,
			NULL);
		SendMessage(search_value_text, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));

		search_8bit_radiobutton = CreateWindow(
			_T("button"),
			_T("8bit"),
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,
			620, 240,
			60, 30,
			h_wnd,
			NULL,
			global_hInstance,
			NULL);
		search_16bit_radiobutton = CreateWindow(
			_T("button"),
			_T("16bit"),
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
			720, 240,
			60, 30,
			h_wnd,
			NULL,
			global_hInstance,
			NULL);
		SendMessage(search_8bit_radiobutton, BM_SETCHECK, BST_CHECKED, 0);

		search_start_button = CreateWindow(
			_T("button"),
			_T("メモリのサーチ(1回目)を開始する"),
			WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
			410, 280,
			380, 30,
			h_wnd,
			(HMENU)BUTTON_ID_MEMORY_SEARCH_START,
			global_hInstance,
			NULL);

		next_search_equal_radiobutton = CreateWindow(
			_T("button"),
			_T("上の指定値と同じ"),
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,
			410, 340,
			190, 30,
			h_wnd,
			NULL,
			global_hInstance,
			NULL);
		next_search_not_equal_radiobutton = CreateWindow(
			_T("button"),
			_T("上の指定値と違う"),
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
			600, 340,
			200, 30,
			h_wnd,
			NULL,
			global_hInstance,
			NULL);
		next_search_biggar_radiobutton = CreateWindow(
			_T("button"),
			_T("上の指定値より大きい"),
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
			410, 370,
			190, 30,
			h_wnd,
			NULL,
			global_hInstance,
			NULL);
		next_search_smaller_radiobutton = CreateWindow(
			_T("button"),
			_T("上の指定値より小さい"),
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
			600, 370,
			200, 30,
			h_wnd,
			NULL,
			global_hInstance,
			NULL);
		next_search_unknown_radiobutton = CreateWindow(
			_T("button"),
			_T("未知の値"),
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
			410, 400,
			190, 30,
			h_wnd,
			NULL,
			global_hInstance,
			NULL);
		next_search_cmp_prevvalue_equal_radiobutton = CreateWindow(
			_T("button"),
			_T("前回の値と同じ"),
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
			600, 400,
			200, 30,
			h_wnd,
			NULL,
			global_hInstance,
			NULL);
		next_search_cmp_prevvalue_not_equal_radiobutton = CreateWindow(
			_T("button"),
			_T("前回の値と違う"),
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
			410, 430,
			190, 30,
			h_wnd,
			NULL,
			global_hInstance,
			NULL);
		SendMessage(next_search_equal_radiobutton, BM_SETCHECK, BST_CHECKED, 0);

		next_search_start_button = CreateWindow(
			_T("button"),
			_T("メモリのサーチ(絞り込み)を開始する"),
			WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
			410, 470,
			380, 30,
			h_wnd,
			(HMENU)BUTTON_ID_MEMORY_NEXTSEARCH_START,
			global_hInstance,
			NULL);

		return 0;
	case WM_CLOSE:
		ShowWindow(cheat_hwnd, SW_HIDE);

		//Main::Cheat_Window_Exist_Flag = false;

		return 0;
	}

	return DefWindowProc(h_wnd, u_msg, w_param, l_param);
}

void clear_resident_cheat_code_list_ptr_array() {
	for (int i = 0; i < resident_cheat_info_array.size(); i++) {
		free(resident_cheat_info_array[i].resident_cheat_code_list_ptr);
	}

	resident_cheat_info_array.clear();
	resident_cheat_info_array.shrink_to_fit();
}

/*
不正な文字があったら-1を返す
*/
int cheat_code_parse(TCHAR* cheat_code_str, vector<uint32_t>& cheat_code_list) {
	for (;;) {
		uint32_t cheat_code_1line = 0x00000000;
		for (int i = 0; i < 8; i++) {
			if ((i == 0 && *cheat_code_str == _T('\n')) ||
				(i == 0 && *cheat_code_str == _T('\r'))) {
				cheat_code_str++;
				break;
			}
			if (i == 0 && *cheat_code_str == _T('\0')) {
				return 0;
			}
			if (i != 0 && *cheat_code_str == _T('\0')) {
				return -1;
			}

			uint8_t tmp_hex_1 = charhex_to_uint8((char)(*cheat_code_str));
			if (tmp_hex_1 == 0xFF) {
				return -1;
			}
			cheat_code_1line |= (tmp_hex_1 & 0b1111);
			if (i != 7) {//一番下の桁を処理し終わったらもうシフトしない
				cheat_code_1line <<= 4;
			}
			else {
				cheat_code_list.push_back(cheat_code_1line);
			}

			cheat_code_str++;
		}
	}

	return 0;
}

/*
16進数の文字を4bitの整数に変換する
もし16進数以外の文字ならば0xFFを返す
*/
uint8_t charhex_to_uint8(char charhex) {
	switch (charhex) {
	case '0':
		return 0x00;
	case '1':
		return 0x01;
	case '2':
		return 0x02;
	case '3':
		return 0x03;
	case '4':
		return 0x04;
	case '5':
		return 0x05;
	case '6':
		return 0x06;
	case '7':
		return 0x07;
	case '8':
		return 0x08;
	case '9':
		return 0x09;
	case 'a':
	case 'A':
		return 0x0A;
	case 'b':
	case 'B':
		return 0x0B;
	case 'c':
	case 'C':
		return 0x0C;
	case 'd':
	case 'D':
		return 0x0D;
	case 'e':
	case 'E':
		return 0x0E;
	case 'f':
	case 'F':
		return 0x0F;
	}

	return 0xFF;
}

/*
4bitの整数を16進数の文字に変換する
もし変換に失敗したら'#'を返す
*/
char uint8_to_charhex(uint8_t value_4bit) {
	switch (value_4bit) {
	case 0x0:
		return '0';
	case 0x1:
		return '1';
	case 0x2:
		return '2';
	case 0x3:
		return '3';
	case 0x4:
		return '4';
	case 0x5:
		return '5';
	case 0x6:
		return '6';
	case 0x7:
		return '7';
	case 0x8:
		return '8';
	case 0x9:
		return '9';
	case 0xA:
		return 'A';
	case 0xB:
		return 'B';
	case 0xC:
		return 'C';
	case 0xD:
		return 'D';
	case 0xE:
		return 'E';
	case 0xF:
		return 'F';
	}

	return '#';
}

/*
テキストを最大4byteの値に変換する
*/
int parse_search_4byte_value(uint16_t* ret_value, TCHAR* search_value_str, uint32_t text_length) {
	*ret_value = 0;

	if (text_length > 4) {
		return -1;
	}

	for (int i = 0; i < text_length; i++) {
		uint8_t tmp_4bit_value = charhex_to_uint8((char)search_value_str[i]);

		if (tmp_4bit_value == 0xFF) {
			return -1;
		}

		*ret_value |= (tmp_4bit_value & 0b1111);
		if (i != (text_length - 1)) {
			*ret_value <<= 4;
		}
	}

	return 0;
}

void clear_found_address_info_list() {
	found_address_info_list.clear();
	found_address_info_list.shrink_to_fit();
}

void process_first_search_result() {
	//for (int i = 0; i < found_address_info_list.size(); i++) {
	//	M_debug_printf("[i = %d] = 0x%04X\n", i, found_address_info_list[i].address);
	//}

	ListView_DeleteAllItems(search_result_listview);

	TCHAR item_name_buffer[256];
	LVITEM item = { 0 };
	for (int i = 0; i < found_address_info_list.size(); i++) {
		if (i >= 1000) {//表示するのは小さいアドレスから1000件まで
			break;
		}

		item.mask = LVIF_TEXT;
		wsprintf(item_name_buffer, "address = 0x%04X   prev_value = 0x%X   value = 0x%X", found_address_info_list[i].address, found_address_info_list[i].prev_value, found_address_info_list[i].value);
		item.pszText = item_name_buffer;
		item.iItem = i;
		ListView_InsertItem(search_result_listview, &item);
	}

	TCHAR tmp_msg_buf[256];
	wsprintf(tmp_msg_buf, "サーチ終了 : %d個発見!", found_address_info_list.size());
	MessageBox(cheat_hwnd, tmp_msg_buf, _T("情報"), MB_ICONINFORMATION | MB_APPLMODAL);
}

void update_resident_cheat_list() {
	ListView_DeleteAllItems(apply_resident_cheat_listview);

	for (int i = 0; i < resident_cheat_info_array.size(); i++) {
		size_t code_total_num = resident_cheat_info_array[i].code_total_number;

		size_t item_str_size = code_total_num * 8 * sizeof(TCHAR) + 1 * sizeof(TCHAR);
		TCHAR* item_str = (TCHAR*)malloc(item_str_size);
		memset(item_str, 0, item_str_size);

		get_resident_cheat_code_str(item_str, resident_cheat_info_array[i].resident_cheat_code_list_ptr, code_total_num * 8);

		LVITEM item = { 0 };
		item.mask = LVIF_TEXT;
		item.pszText = item_str;
		item.iItem = i;
		ListView_InsertItem(apply_resident_cheat_listview, &item);

		free(item_str);
	}
}

void get_resident_cheat_code_str(TCHAR* ret_buf, uint32_t* resident_cheat_code_ptr, size_t total_code_size) {
	for (int i = 0; i < (total_code_size / 8); i++) {
		for (int j = 0; j < 8; j++) {
			uint8_t target_4bit_value = ((uint8_t*)resident_cheat_code_ptr)[(i * 8) / 2 + (3 - (j / 2))];
			if (j % 2 == 0) {
				target_4bit_value >>= 4;
			}
			target_4bit_value &= 0b1111;
			ret_buf[i * 8 + j] = _T(uint8_to_charhex(target_4bit_value));
		}
	}
}

void update_sound_mute_setting() {
	GameBoyColor* gameboy_ptr = GameManager::get_instance_ptr()->get_gameboy();
	if (gameboy_ptr != nullptr) {
		APU* apu_ptr = gameboy_ptr->get_apu_ptr();
		if (apu_ptr != nullptr) {
			if (Main::Sound_Channel1_Mute_Flag == true) {
				apu_ptr->get_channel_1()->get_source_voice_ptr()->SetVolume(0.0f);
			}
			else {
				apu_ptr->get_channel_1()->get_source_voice_ptr()->SetVolume(1.0f);
			}

			if (Main::Sound_Channel2_Mute_Flag == true) {
				apu_ptr->get_channel_2()->get_source_voice_ptr()->SetVolume(0.0f);
			}
			else {
				apu_ptr->get_channel_2()->get_source_voice_ptr()->SetVolume(1.0f);
			}

			if (Main::Sound_Channel3_Mute_Flag == true) {
				apu_ptr->get_channel_3()->get_source_voice_ptr()->SetVolume(0.0f);
			}
			else {
				apu_ptr->get_channel_3()->get_source_voice_ptr()->SetVolume(1.0f);
			}

			if (Main::Sound_Channel4_Mute_Flag == true) {
				apu_ptr->get_channel_4()->get_source_voice_ptr()->SetVolume(0.0f);
			}
			else {
				apu_ptr->get_channel_4()->get_source_voice_ptr()->SetVolume(1.0f);
			}
		}
	}
}
