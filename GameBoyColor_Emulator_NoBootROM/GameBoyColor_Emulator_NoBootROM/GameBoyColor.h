#pragma once

using namespace std;

#include <vector>
#include <algorithm>

#include <io.h>
#pragma comment(lib, "shlwapi.lib")
#include <shlwapi.h>

#include "Main.h"
#include "Key.h"
#include "MyDirectXSystem.h"
#include "MyDirectXDraw.h"
#include "APU.h"
#include "MyDirectXString.h"

#define INSTRUCTION_NUM 256

#define CPU_FREQ 4194304//CPUの周波数(hz)

class GameBoyColor
{
private:
	uint64_t frame_counter = 0;


	uint32_t rom_crc32 = 0;


	vector<uint32_t> resident_cheat_code_list;


	APU* apu;


	MyDirectXSystem* myDirectXSystem;


	MyDirectXString* debug_info_font_1;



	//エミュレータ内のRAMをコピーする際に使う関数
	//バンクが存在するなどの理由でこれを使わないとRAM内は正常にコピーできない
	void My_Emulator_RAM_memcpy(uint16_t emu_RAM_dst_address, uint16_t emu_RAM_src_address, uint16_t size) {
		for (int i = 0; i < size; i++) {
			write_RAM_8bit(emu_RAM_dst_address + i, read_RAM_8bit(emu_RAM_src_address + i));
		}
	}



	bool FATAL_ERROR_FLAG = false;//ロードなどで続行不能なエラーが発生したか



	Main::GAME_HARDWARE_TYPE hardware_type;



	bool SET_CPU_Clock_2x_Flag__CGB = false;//CPUのSTOP命令でセットするモード
	bool CURRENT_CPU_Clock_2x_Flag__CGB = false;//CPUが倍速モードか



#define SAVEDATA_FILE_EXT_NAME ".savdat"
	FILE* savedata_fp = nullptr;


	Key* key;


	bool tmp_CPU_HALT_Flag = false;


#define GB_PALETTE_0 0xFF9bbc0f
#define GB_PALETTE_1 0xFF8bac0f
#define GB_PALETTE_2 0xFF306230
#define GB_PALETTE_3 0xFF0f380f
	const DWORD GB_PALETTE_LIST[4] = {
			GB_PALETTE_0,
			GB_PALETTE_1,
			GB_PALETTE_2,
			GB_PALETTE_3,
	};

	DWORD get_bg_window_palette(uint8_t _2bit_color_no) {
		uint8_t color_data;

		if (_2bit_color_no == 0) {
			color_data = (gbx_ram.RAM[0xFF47] & 0b00000011);
		}
		else if (_2bit_color_no == 1) {
			color_data = ((gbx_ram.RAM[0xFF47] >> 2) & 0b00000011);
		}
		else if (_2bit_color_no == 2) {
			color_data = ((gbx_ram.RAM[0xFF47] >> 4) & 0b00000011);
		}
		else {
			color_data = ((gbx_ram.RAM[0xFF47] >> 6) & 0b00000011);
		}

		return GB_PALETTE_LIST[color_data];
	}

	DWORD get_sprite_palette(uint8_t _2bit_color_no, bool palette_OBP1_flag) {
		uint8_t valid_palette_data;
		if (palette_OBP1_flag == false) {
			valid_palette_data = gbx_ram.RAM[0xFF48];
		}
		else {
			valid_palette_data = gbx_ram.RAM[0xFF49];
		}

		uint8_t color_data;

		if (_2bit_color_no == 0) {
			//color_data = (valid_palette_data & 0b00000011);

			return 0x00000000;//色番号が下位2bitのときは透明を返す
		}
		else if (_2bit_color_no == 1) {
			color_data = ((valid_palette_data >> 2) & 0b00000011);
		}
		else if (_2bit_color_no == 2) {
			color_data = ((valid_palette_data >> 4) & 0b00000011);
		}
		else {
			color_data = ((valid_palette_data >> 6) & 0b00000011);
		}

		return GB_PALETTE_LIST[color_data];
	}

	//======================
	//======================

	/*
	0   BGP0
			- 色番号0の色データ(2バイト, 後述)
			- 色番号1
			- 色番号2
			- 色番号3
	8   BGP1
			- 色番号0
			- 色番号1
			- 色番号2
			- 色番号3
	16  BGP2
			- 色番号0
			- 色番号1
			- 色番号2
			- 色番号3
		...
	56  BGP7
			- 色番号0
			- 色番号1
			- 色番号2
			- 色番号3
	*/
	uint8_t cgb_color_list_bg_window_index = 0;
	bool cgb_color_list_bg_window_auto_inc_flag = false;
	uint8_t cgb_color_list_bg_window[0x40];
	uint16_t get_bg_window_palette__cgb(uint8_t _3bit_palette_no, uint8_t _2bit_color_no) {
		uint8_t* palette_top_ptr = &(cgb_color_list_bg_window[(_3bit_palette_no & 0b00000111) * 8]);

		return ((palette_top_ptr[(_2bit_color_no & 0b00000011) * 2]) | ((palette_top_ptr[(_2bit_color_no & 0b00000011) * 2 + 1]) << 8));
	}

	uint8_t cgb_color_list_sprite_index = 0;
	bool cgb_color_list_sprite_auto_inc_flag = false;
	uint8_t cgb_color_list_sprite[0x40];
	uint16_t get_sprite_palette__cgb(uint8_t _3bit_palette_no, uint8_t _2bit_color_no) {
		uint8_t* palette_top_ptr = &(cgb_color_list_sprite[(_3bit_palette_no & 0b00000111) * 8]);

		return ((palette_top_ptr[(_2bit_color_no & 0b00000011) * 2]) | ((palette_top_ptr[(_2bit_color_no & 0b00000011) * 2 + 1]) << 8));
	}


	uint8_t _8bit_bg_screen_data_160x144[GBX_WIDTH * GBX_HEIGHT];
	uint8_t _8bit_window_screen_data_160x144[GBX_WIDTH * GBX_HEIGHT];

	uint8_t _8bit_backbuffer_data_256x256__mtype0_dtype0[256 * 256];
	uint8_t _8bit_backbuffer_data_256x256__mtype0_dtype1[256 * 256];
	uint8_t _8bit_backbuffer_data_256x256__mtype1_dtype0[256 * 256];
	uint8_t _8bit_backbuffer_data_256x256__mtype1_dtype1[256 * 256];
	
	uint8_t _8bit_sprite_screen_data_160x144[GBX_WIDTH * GBX_HEIGHT];


	void (GameBoyColor::*cpu_instruction_table[INSTRUCTION_NUM])() = {
			&GameBoyColor::cpu_fnc__NOP, //0x00
			&GameBoyColor::cpu_fnc__LD_BC_n16, //0x01
			&GameBoyColor::cpu_fnc__LD_addrBC_A, //0x02
			&GameBoyColor::cpu_fnc__INC_BC, //0x03
			&GameBoyColor::cpu_fnc__INC_B, //0x04
			&GameBoyColor::cpu_fnc__DEC_B, //0x05
			&GameBoyColor::cpu_fnc__LD_B_n8, //0x06
			&GameBoyColor::cpu_fnc__RLCA, //0x07
			&GameBoyColor::cpu_fnc__LD_addrn16_SP, //0x08
			&GameBoyColor::cpu_fnc__ADD_HL_BC, //0x09
			&GameBoyColor::cpu_fnc__LD_A_addrBC, //0x0a
			&GameBoyColor::cpu_fnc__DEC_BC, //0x0b
			&GameBoyColor::cpu_fnc__INC_C, //0x0c
			&GameBoyColor::cpu_fnc__DEC_C, //0x0d
			&GameBoyColor::cpu_fnc__LD_C_n8, //0x0e
			&GameBoyColor::cpu_fnc__RRCA, //0x0f
			&GameBoyColor::cpu_fnc__STOP, //0x10
			&GameBoyColor::cpu_fnc__LD_DE_n16, //0x11
			&GameBoyColor::cpu_fnc__LD_addrDE_A, //0x12
			&GameBoyColor::cpu_fnc__INC_DE, //0x13
			&GameBoyColor::cpu_fnc__INC_D, //0x14
			&GameBoyColor::cpu_fnc__DEC_D, //0x15
			&GameBoyColor::cpu_fnc__LD_D_n8, //0x16
			&GameBoyColor::cpu_fnc__RLA, //0x17
			&GameBoyColor::cpu_fnc__JR_e8, //0x18
			&GameBoyColor::cpu_fnc__ADD_HL_DE, //0x19
			&GameBoyColor::cpu_fnc__LD_A_addrDE, //0x1a
			&GameBoyColor::cpu_fnc__DEC_DE, //0x1b
			&GameBoyColor::cpu_fnc__INC_E, //0x1c
			&GameBoyColor::cpu_fnc__DEC_E, //0x1d
			&GameBoyColor::cpu_fnc__LD_E_n8, //0x1e
			&GameBoyColor::cpu_fnc__RRA, //0x1f
			&GameBoyColor::cpu_fnc__JR_FNZ_e8, //0x20
			&GameBoyColor::cpu_fnc__LD_HL_n16, //0x21
			&GameBoyColor::cpu_fnc__LD_addrHLI_A, //0x22
			&GameBoyColor::cpu_fnc__INC_HL, //0x23
			&GameBoyColor::cpu_fnc__INC_H, //0x24
			&GameBoyColor::cpu_fnc__DEC_H, //0x25
			&GameBoyColor::cpu_fnc__LD_H_n8, //0x26
			&GameBoyColor::cpu_fnc__DAA, //0x27
			&GameBoyColor::cpu_fnc__JR_FZ_e8, //0x28
			&GameBoyColor::cpu_fnc__ADD_HL_HL, //0x29
			&GameBoyColor::cpu_fnc__LD_A_addrHLI, //0x2a
			&GameBoyColor::cpu_fnc__DEC_HL, //0x2b
			&GameBoyColor::cpu_fnc__INC_L, //0x2c
			&GameBoyColor::cpu_fnc__DEC_L, //0x2d
			&GameBoyColor::cpu_fnc__LD_L_n8, //0x2e
			&GameBoyColor::cpu_fnc__CPL, //0x2f
			&GameBoyColor::cpu_fnc__JR_FNC_e8, //0x30
			&GameBoyColor::cpu_fnc__LD_SP_n16, //0x31
			&GameBoyColor::cpu_fnc__LD_addrHLD_A, //0x32
			&GameBoyColor::cpu_fnc__INC_SP, //0x33
			&GameBoyColor::cpu_fnc__INC_addrHL, //0x34
			&GameBoyColor::cpu_fnc__DEC_addrHL, //0x35
			&GameBoyColor::cpu_fnc__LD_addrHL_n8, //0x36
			&GameBoyColor::cpu_fnc__SCF, //0x37
			&GameBoyColor::cpu_fnc__JR_FC_e8, //0x38
			&GameBoyColor::cpu_fnc__ADD_HL_SP, //0x39
			&GameBoyColor::cpu_fnc__LD_A_addrHLD, //0x3a
			&GameBoyColor::cpu_fnc__DEC_SP, //0x3b
			&GameBoyColor::cpu_fnc__INC_A, //0x3c
			&GameBoyColor::cpu_fnc__DEC_A, //0x3d
			&GameBoyColor::cpu_fnc__LD_A_n8, //0x3e
			&GameBoyColor::cpu_fnc__CCF, //0x3f
			&GameBoyColor::cpu_fnc__LD_B_B, //0x40
			&GameBoyColor::cpu_fnc__LD_B_C, //0x41
			&GameBoyColor::cpu_fnc__LD_B_D, //0x42
			&GameBoyColor::cpu_fnc__LD_B_E, //0x43
			&GameBoyColor::cpu_fnc__LD_B_H, //0x44
			&GameBoyColor::cpu_fnc__LD_B_L, //0x45
			&GameBoyColor::cpu_fnc__LD_B_addrHL, //0x46
			&GameBoyColor::cpu_fnc__LD_B_A, //0x47
			&GameBoyColor::cpu_fnc__LD_C_B, //0x48
			&GameBoyColor::cpu_fnc__LD_C_C, //0x49
			&GameBoyColor::cpu_fnc__LD_C_D, //0x4a
			&GameBoyColor::cpu_fnc__LD_C_E, //0x4b
			&GameBoyColor::cpu_fnc__LD_C_H, //0x4c
			&GameBoyColor::cpu_fnc__LD_C_L, //0x4d
			&GameBoyColor::cpu_fnc__LD_C_addrHL, //0x4e
			&GameBoyColor::cpu_fnc__LD_C_A, //0x4f
			&GameBoyColor::cpu_fnc__LD_D_B, //0x50
			&GameBoyColor::cpu_fnc__LD_D_C, //0x51
			&GameBoyColor::cpu_fnc__LD_D_D, //0x52
			&GameBoyColor::cpu_fnc__LD_D_E, //0x53
			&GameBoyColor::cpu_fnc__LD_D_H, //0x54
			&GameBoyColor::cpu_fnc__LD_D_L, //0x55
			&GameBoyColor::cpu_fnc__LD_D_addrHL, //0x56
			&GameBoyColor::cpu_fnc__LD_D_A, //0x57
			&GameBoyColor::cpu_fnc__LD_E_B, //0x58
			&GameBoyColor::cpu_fnc__LD_E_C, //0x59
			&GameBoyColor::cpu_fnc__LD_E_D, //0x5a
			&GameBoyColor::cpu_fnc__LD_E_E, //0x5b
			&GameBoyColor::cpu_fnc__LD_E_H, //0x5c
			&GameBoyColor::cpu_fnc__LD_E_L, //0x5d
			&GameBoyColor::cpu_fnc__LD_E_addrHL, //0x5e
			&GameBoyColor::cpu_fnc__LD_E_A, //0x5f
			&GameBoyColor::cpu_fnc__LD_H_B, //0x60
			&GameBoyColor::cpu_fnc__LD_H_C, //0x61
			&GameBoyColor::cpu_fnc__LD_H_D, //0x62
			&GameBoyColor::cpu_fnc__LD_H_E, //0x63
			&GameBoyColor::cpu_fnc__LD_H_H, //0x64
			&GameBoyColor::cpu_fnc__LD_H_L, //0x65
			&GameBoyColor::cpu_fnc__LD_H_addrHL, //0x66
			&GameBoyColor::cpu_fnc__LD_H_A, //0x67
			&GameBoyColor::cpu_fnc__LD_L_B, //0x68
			&GameBoyColor::cpu_fnc__LD_L_C, //0x69
			&GameBoyColor::cpu_fnc__LD_L_D, //0x6a
			&GameBoyColor::cpu_fnc__LD_L_E, //0x6b
			&GameBoyColor::cpu_fnc__LD_L_H, //0x6c
			&GameBoyColor::cpu_fnc__LD_L_L, //0x6d
			&GameBoyColor::cpu_fnc__LD_L_addrHL, //0x6e
			&GameBoyColor::cpu_fnc__LD_L_A, //0x6f
			&GameBoyColor::cpu_fnc__LD_addrHL_B, //0x70
			&GameBoyColor::cpu_fnc__LD_addrHL_C, //0x71
			&GameBoyColor::cpu_fnc__LD_addrHL_D, //0x72
			&GameBoyColor::cpu_fnc__LD_addrHL_E, //0x73
			&GameBoyColor::cpu_fnc__LD_addrHL_H, //0x74
			&GameBoyColor::cpu_fnc__LD_addrHL_L, //0x75
			&GameBoyColor::cpu_fnc__HALT, //0x76
			&GameBoyColor::cpu_fnc__LD_addrHL_A, //0x77
			&GameBoyColor::cpu_fnc__LD_A_B, //0x78
			&GameBoyColor::cpu_fnc__LD_A_C, //0x79
			&GameBoyColor::cpu_fnc__LD_A_D, //0x7a
			&GameBoyColor::cpu_fnc__LD_A_E, //0x7b
			&GameBoyColor::cpu_fnc__LD_A_H, //0x7c
			&GameBoyColor::cpu_fnc__LD_A_L, //0x7d
			&GameBoyColor::cpu_fnc__LD_A_addrHL, //0x7e
			&GameBoyColor::cpu_fnc__LD_A_A, //0x7f
			&GameBoyColor::cpu_fnc__ADD_A_B, //0x80
			&GameBoyColor::cpu_fnc__ADD_A_C, //0x81
			&GameBoyColor::cpu_fnc__ADD_A_D, //0x82
			&GameBoyColor::cpu_fnc__ADD_A_E, //0x83
			&GameBoyColor::cpu_fnc__ADD_A_H, //0x84
			&GameBoyColor::cpu_fnc__ADD_A_L, //0x85
			&GameBoyColor::cpu_fnc__ADD_A_addrHL, //0x86
			&GameBoyColor::cpu_fnc__ADD_A_A, //0x87
			&GameBoyColor::cpu_fnc__ADC_A_B, //0x88
			&GameBoyColor::cpu_fnc__ADC_A_C, //0x89
			&GameBoyColor::cpu_fnc__ADC_A_D, //0x8a
			&GameBoyColor::cpu_fnc__ADC_A_E, //0x8b
			&GameBoyColor::cpu_fnc__ADC_A_H, //0x8c
			&GameBoyColor::cpu_fnc__ADC_A_L, //0x8d
			&GameBoyColor::cpu_fnc__ADC_A_addrHL, //0x8e
			&GameBoyColor::cpu_fnc__ADC_A_A, //0x8f
			&GameBoyColor::cpu_fnc__SUB_A_B, //0x90
			&GameBoyColor::cpu_fnc__SUB_A_C, //0x91
			&GameBoyColor::cpu_fnc__SUB_A_D, //0x92
			&GameBoyColor::cpu_fnc__SUB_A_E, //0x93
			&GameBoyColor::cpu_fnc__SUB_A_H, //0x94
			&GameBoyColor::cpu_fnc__SUB_A_L, //0x95
			&GameBoyColor::cpu_fnc__SUB_A_addrHL, //0x96
			&GameBoyColor::cpu_fnc__SUB_A_A, //0x97
			&GameBoyColor::cpu_fnc__SBC_A_B, //0x98
			&GameBoyColor::cpu_fnc__SBC_A_C, //0x99
			&GameBoyColor::cpu_fnc__SBC_A_D, //0x9a
			&GameBoyColor::cpu_fnc__SBC_A_E, //0x9b
			&GameBoyColor::cpu_fnc__SBC_A_H, //0x9c
			&GameBoyColor::cpu_fnc__SBC_A_L, //0x9d
			&GameBoyColor::cpu_fnc__SBC_A_addrHL, //0x9e
			&GameBoyColor::cpu_fnc__SBC_A_A, //0x9f
			&GameBoyColor::cpu_fnc__AND_A_B, //0xa0
			&GameBoyColor::cpu_fnc__AND_A_C, //0xa1
			&GameBoyColor::cpu_fnc__AND_A_D, //0xa2
			&GameBoyColor::cpu_fnc__AND_A_E, //0xa3
			&GameBoyColor::cpu_fnc__AND_A_H, //0xa4
			&GameBoyColor::cpu_fnc__AND_A_L, //0xa5
			&GameBoyColor::cpu_fnc__AND_A_addrHL, //0xa6
			&GameBoyColor::cpu_fnc__AND_A_A, //0xa7
			&GameBoyColor::cpu_fnc__XOR_A_B, //0xa8
			&GameBoyColor::cpu_fnc__XOR_A_C, //0xa9
			&GameBoyColor::cpu_fnc__XOR_A_D, //0xaa
			&GameBoyColor::cpu_fnc__XOR_A_E, //0xab
			&GameBoyColor::cpu_fnc__XOR_A_H, //0xac
			&GameBoyColor::cpu_fnc__XOR_A_L, //0xad
			&GameBoyColor::cpu_fnc__XOR_A_addrHL, //0xae
			&GameBoyColor::cpu_fnc__XOR_A_A, //0xaf
			&GameBoyColor::cpu_fnc__OR_A_B, //0xb0
			&GameBoyColor::cpu_fnc__OR_A_C, //0xb1
			&GameBoyColor::cpu_fnc__OR_A_D, //0xb2
			&GameBoyColor::cpu_fnc__OR_A_E, //0xb3
			&GameBoyColor::cpu_fnc__OR_A_H, //0xb4
			&GameBoyColor::cpu_fnc__OR_A_L, //0xb5
			&GameBoyColor::cpu_fnc__OR_A_addrHL, //0xb6
			&GameBoyColor::cpu_fnc__OR_A_A, //0xb7
			&GameBoyColor::cpu_fnc__CP_A_B, //0xb8
			&GameBoyColor::cpu_fnc__CP_A_C, //0xb9
			&GameBoyColor::cpu_fnc__CP_A_D, //0xba
			&GameBoyColor::cpu_fnc__CP_A_E, //0xbb
			&GameBoyColor::cpu_fnc__CP_A_H, //0xbc
			&GameBoyColor::cpu_fnc__CP_A_L, //0xbd
			&GameBoyColor::cpu_fnc__CP_A_addrHL, //0xbe
			&GameBoyColor::cpu_fnc__CP_A_A, //0xbf
			&GameBoyColor::cpu_fnc__RET_FNZ, //0xc0
			&GameBoyColor::cpu_fnc__POP_BC, //0xc1
			&GameBoyColor::cpu_fnc__JP_FNZ_n16, //0xc2
			&GameBoyColor::cpu_fnc__JP_n16, //0xc3
			&GameBoyColor::cpu_fnc__CALL_FNZ_n16, //0xc4
			&GameBoyColor::cpu_fnc__PUSH_BC, //0xc5
			&GameBoyColor::cpu_fnc__ADD_A_n8, //0xc6
			&GameBoyColor::cpu_fnc__RST_00h, //0xc7
			&GameBoyColor::cpu_fnc__RET_FZ, //0xc8
			&GameBoyColor::cpu_fnc__RET, //0xc9
			&GameBoyColor::cpu_fnc__JP_FZ_n16, //0xca
			&GameBoyColor::cpu_fnc__PREFIX, //0xcb
			&GameBoyColor::cpu_fnc__CALL_FZ_n16, //0xcc
			&GameBoyColor::cpu_fnc__CALL_n16, //0xcd
			&GameBoyColor::cpu_fnc__ADC_A_n8, //0xce
			&GameBoyColor::cpu_fnc__RST_08h, //0xcf
			&GameBoyColor::cpu_fnc__RET_FNC, //0xd0
			&GameBoyColor::cpu_fnc__POP_DE, //0xd1
			&GameBoyColor::cpu_fnc__JP_FNC_n16, //0xd2
			&GameBoyColor::cpu_fnc__GARBAGE, //0xd3
			&GameBoyColor::cpu_fnc__CALL_FNC_n16, //0xd4
			&GameBoyColor::cpu_fnc__PUSH_DE, //0xd5
			&GameBoyColor::cpu_fnc__SUB_A_n8, //0xd6
			&GameBoyColor::cpu_fnc__RST_10h, //0xd7
			&GameBoyColor::cpu_fnc__RET_FC, //0xd8
			&GameBoyColor::cpu_fnc__RETI, //0xd9
			&GameBoyColor::cpu_fnc__JP_FC_n16, //0xda
			&GameBoyColor::cpu_fnc__GARBAGE, //0xdb
			&GameBoyColor::cpu_fnc__CALL_FC_n16, //0xdc
			&GameBoyColor::cpu_fnc__GARBAGE, //0xdd
			&GameBoyColor::cpu_fnc__SBC_A_n8, //0xde
			&GameBoyColor::cpu_fnc__RST_18h, //0xdf
			&GameBoyColor::cpu_fnc__LDH_addrn16_A, //0xe0
			&GameBoyColor::cpu_fnc__POP_HL, //0xe1
			&GameBoyColor::cpu_fnc__LDH_addrC_A, //0xe2
			&GameBoyColor::cpu_fnc__GARBAGE, //0xe3
			&GameBoyColor::cpu_fnc__GARBAGE, //0xe4
			&GameBoyColor::cpu_fnc__PUSH_HL, //0xe5
			&GameBoyColor::cpu_fnc__AND_A_n8, //0xe6
			&GameBoyColor::cpu_fnc__RST_20h, //0xe7
			&GameBoyColor::cpu_fnc__ADD_SP_e8, //0xe8
			&GameBoyColor::cpu_fnc__JP_HL, //0xe9
			&GameBoyColor::cpu_fnc__LD_addrn16_A, //0xea
			&GameBoyColor::cpu_fnc__GARBAGE, //0xeb
			&GameBoyColor::cpu_fnc__GARBAGE, //0xec
			&GameBoyColor::cpu_fnc__GARBAGE, //0xed
			&GameBoyColor::cpu_fnc__XOR_A_n8, //0xee
			&GameBoyColor::cpu_fnc__RST_28h, //0xef
			&GameBoyColor::cpu_fnc__LDH_A_addrn16, //0xf0
			&GameBoyColor::cpu_fnc__POP_AF, //0xf1
			&GameBoyColor::cpu_fnc__LDH_A_addrC, //0xf2
			&GameBoyColor::cpu_fnc__DI, //0xf3
			&GameBoyColor::cpu_fnc__GARBAGE, //0xf4
			&GameBoyColor::cpu_fnc__PUSH_AF, //0xf5
			&GameBoyColor::cpu_fnc__OR_A_n8, //0xf6
			&GameBoyColor::cpu_fnc__RST_30h, //0xf7
			&GameBoyColor::cpu_fnc__LD_HL_SPpe8, //0xf8
			&GameBoyColor::cpu_fnc__LD_SP_HL, //0xf9
			&GameBoyColor::cpu_fnc__LD_A_addrn16, //0xfa
			&GameBoyColor::cpu_fnc__EI, //0xfb
			&GameBoyColor::cpu_fnc__GARBAGE, //0xfc
			&GameBoyColor::cpu_fnc__GARBAGE, //0xfd
			&GameBoyColor::cpu_fnc__CP_A_n8, //0xfe
			&GameBoyColor::cpu_fnc__RST_38h, //0xff
	};

	bool get_timer_enable_flag() {
		return (((gbx_ram.RAM[0xFF07] & 0b00000100) != 0) ? true : false);
	}

	uint64_t get_timer_count_freq() {
		uint8_t div_type = (gbx_ram.RAM[0xFF07] & 0b00000011);
		if (div_type == 0b00) {
			return 1024;
		}
		else if (div_type == 0b01) {
			return 16;
		}
		else if (div_type == 0b10) {
			return 64;
		}
		else {//div_type == 0b11
			return  256;
		}
	}

	uint64_t cpu_machine_cycle = 0;
	uint64_t total_cpu_machine_cycle__div = 0;
	uint64_t total_cpu_machine_cycle__tima = 0;
	//マシンサイクル(クロックサイクルではない)
	uint32_t instruction_machine_cycle_table[INSTRUCTION_NUM] = {
		1, //0x00
		3, //0x01
		2, //0x02
		2, //0x03
		1, //0x04
		1, //0x05
		2, //0x06
		1, //0x07
		5, //0x08
		2, //0x09
		2, //0x0a
		2, //0x0b
		1, //0x0c
		1, //0x0d
		2, //0x0e
		1, //0x0f
		1, //0x10
		3, //0x11
		2, //0x12
		2, //0x13
		1, //0x14
		1, //0x15
		2, //0x16
		1, //0x17
		3, //0x18
		2, //0x19
		2, //0x1a
		2, //0x1b
		1, //0x1c
		1, //0x1d
		2, //0x1e
		1, //0x1f
		2, //0x20(分岐するとき+1)
		3, //0x21
		2, //0x22
		2, //0x23
		1, //0x24
		1, //0x25
		2, //0x26
		1, //0x27
		2, //0x28(分岐するとき+1)
		2, //0x29
		2, //0x2a
		2, //0x2b
		1, //0x2c
		1, //0x2d
		2, //0x2e
		1, //0x2f
		2, //0x30(分岐するとき+1)
		3, //0x31
		2, //0x32
		2, //0x33
		3, //0x34
		3, //0x35
		3, //0x36
		1, //0x37
		2, //0x38(分岐するとき+1)
		2, //0x39
		2, //0x3a
		2, //0x3b
		1, //0x3c
		1, //0x3d
		2, //0x3e
		1, //0x3f
		1, //0x40
		1, //0x41
		1, //0x42
		1, //0x43
		1, //0x44
		1, //0x45
		2, //0x46
		1, //0x47
		1, //0x48
		1, //0x49
		1, //0x4a
		1, //0x4b
		1, //0x4c
		1, //0x4d
		2, //0x4e
		1, //0x4f
		1, //0x50
		1, //0x51
		1, //0x52
		1, //0x53
		1, //0x54
		1, //0x55
		2, //0x56
		1, //0x57
		1, //0x58
		1, //0x59
		1, //0x5a
		1, //0x5b
		1, //0x5c
		1, //0x5d
		2, //0x5e
		1, //0x5f
		1, //0x60
		1, //0x61
		1, //0x62
		1, //0x63
		1, //0x64
		1, //0x65
		2, //0x66
		1, //0x67
		1, //0x68
		1, //0x69
		1, //0x6a
		1, //0x6b
		1, //0x6c
		1, //0x6d
		2, //0x6e
		1, //0x6f
		2, //0x70
		2, //0x71
		2, //0x72
		2, //0x73
		2, //0x74
		2, //0x75
		1, //0x76
		2, //0x77
		1, //0x78
		1, //0x79
		1, //0x7a
		1, //0x7b
		1, //0x7c
		1, //0x7d
		2, //0x7e
		1, //0x7f
		1, //0x80
		1, //0x81
		1, //0x82
		1, //0x83
		1, //0x84
		1, //0x85
		2, //0x86
		1, //0x87
		1, //0x88
		1, //0x89
		1, //0x8a
		1, //0x8b
		1, //0x8c
		1, //0x8d
		2, //0x8e
		1, //0x8f
		1, //0x90
		1, //0x91
		1, //0x92
		1, //0x93
		1, //0x94
		1, //0x95
		2, //0x96
		1, //0x97
		1, //0x98
		1, //0x99
		1, //0x9a
		1, //0x9b
		1, //0x9c
		1, //0x9d
		2, //0x9e
		1, //0x9f
		1, //0xa0
		1, //0xa1
		1, //0xa2
		1, //0xa3
		1, //0xa4
		1, //0xa5
		2, //0xa6
		1, //0xa7
		1, //0xa8
		1, //0xa9
		1, //0xaa
		1, //0xab
		1, //0xac
		1, //0xad
		2, //0xae
		1, //0xaf
		1, //0xb0
		1, //0xb1
		1, //0xb2
		1, //0xb3
		1, //0xb4
		1, //0xb5
		2, //0xb6
		1, //0xb7
		1, //0xb8
		1, //0xb9
		1, //0xba
		1, //0xbb
		1, //0xbc
		1, //0xbd
		2, //0xbe
		1, //0xbf
		2, //0xc0(分岐するとき+3)
		3, //0xc1
		3, //0xc2(分岐するとき+1)
		4, //0xc3
		3, //0xc4(分岐するとき+3)
		4, //0xc5
		2, //0xc6
		4, //0xc7
		2, //0xc8(分岐するとき+3)
		4, //0xc9
		3, //0xca(分岐するとき+1)
		1, //0xcb
		3, //0xcc(分岐するとき+3)
		6, //0xcd
		2, //0xce
		4, //0xcf
		2, //0xd0(分岐するとき+3)
		3, //0xd1
		3, //0xd2(分岐するとき+1)
		0xDEADBEEF, //0xd3
		3, //0xd4(分岐するとき+3)
		4, //0xd5
		2, //0xd6
		4, //0xd7
		2, //0xd8(分岐するとき+3)
		4, //0xd9
		3, //0xda(分岐するとき+1)
		0xDEADBEEF, //0xdb
		3, //0xdc(分岐するとき+3)
		0xDEADBEEF, //0xdd
		2, //0xde
		4, //0xdf
		3, //0xe0
		3, //0xe1
		2, //0xe2
		0xDEADBEEF, //0xe3
		0xDEADBEEF, //0xe4
		4, //0xe5
		2, //0xe6
		4, //0xe7
		4, //0xe8
		1, //0xe9
		4, //0xea
		0xDEADBEEF, //0xeb
		0xDEADBEEF, //0xec
		0xDEADBEEF, //0xed
		2, //0xee
		4, //0xef
		3, //0xf0
		3, //0xf1
		2, //0xf2
		1, //0xf3
		0xDEADBEEF, //0xf4
		4, //0xf5
		2, //0xf6
		4, //0xf7
		3, //0xf8
		2, //0xf9
		4, //0xfa
		1, //0xfb
		0xDEADBEEF, //0xfc
		0xDEADBEEF, //0xfd
		2, //0xfe
		4, //0xff
	};

	struct ROM_INFO {
		uint8_t CGB_Flag;
		uint8_t SGB_Flag;
		uint8_t Cartridge_Type;
		uint8_t ROM_Type;
		uint8_t SRAM_Type;
	};
	ROM_INFO rom_info;

	struct GBX_REGISTER{
		union {
			uint16_t AF;
			struct {
				union {
					uint8_t Flags;
					struct {
						uint8_t F_unused : 4;
						uint8_t F_C : 1;
						uint8_t F_H : 1;
						uint8_t F_N : 1;
						uint8_t F_Z : 1;
					};
				};
				uint8_t A;
			};
		};

		union {
			uint16_t BC;
			struct {
				uint8_t C;
				uint8_t B;
			};
		};

		union {
			uint16_t DE;
			struct {
				uint8_t E;
				uint8_t D;
			};
		};

		union {
			uint16_t HL;
			struct {
				uint8_t L;
				uint8_t H;
			};
		};

		uint16_t SP;
		uint16_t PC;
	};
	GBX_REGISTER gbx_register;

#define RAM_SIZE 0x10000
	union GBX_RAM{
		uint8_t RAM[RAM_SIZE];

		struct {
			uint8_t _16kib_ROM_bank_00[0x4000];
			uint8_t _16kib_ROM_bank_XX[0x4000];
			uint8_t _8kib_VRAM[0x2000];
			uint8_t _8kib_External_RAM[0x2000];
			uint8_t _4kib_WRAM_1[0x1000];
			uint8_t _4kib_WRAM_2[0x1000];
			uint8_t _4kib_Mirror___C000_DDFF[0x1E00];
			uint8_t sprite_attribute_table[0x00A0];
			uint8_t unused[0x0060];
			uint8_t IO_register[0x0080];
			uint8_t HRAM[0x007F];
			uint8_t Interrupt_enable_register[0x0001];
		};
	};
	GBX_RAM gbx_ram;

	uint8_t* ROM_bank_data_ptr;//ROMが32KB以上のときに使うやつ
	uint8_t* SRAM_bank_data_ptr;//RAMが8KB以上のときに使うやつ

	uint8_t VRAM_bank_no__cgb = 0;
	uint8_t VRAM_bank1_data_ptr__cgb[0x2000];
	uint8_t WRAM_bank_no__cgb = 1;
	uint8_t WRAM_bank1_7_data_ptr__cgb[0x1000 * 7];

	bool IME_Flag = false;

	enum class CART_MBC_TYPE {
		ROM,//MBC1と同じ処理をする
		MBC1,
		MBC2,
		MBC3,
		MBC5,
		HuC1,
		OTHER,//このエミュレーターでは非対応のやつ(一応MBC1と同じ処理をする)
	};
	CART_MBC_TYPE cart_mbc_type;

	uint8_t rom_bank_no__low = 1;//初期はバンク1を指すようにする
	uint8_t rom_bank_no__high = 0;
	uint8_t sram_bank_no = 0;
	bool SRAM_Enable_Flag = false;
	bool RTC_Enable_Flag = false;
	bool IR_Enable_Flag = false;
	enum class BankMode {
		ROM,
		SRAM,
		RTC,
		IR,
	};
	BankMode bank_mode = BankMode::ROM;

	enum class CLOCK_TYPE__MBC3 {
		SECONDS,
		MINUTES,
		HOURS,
		DAY_LOW,
		DAY_HIGH,
	};
	CLOCK_TYPE__MBC3 clock_type__mbc3;

#define RTC_SAVEDATA_SIZE 48
#define RTC_DATA_SIZE 0x05
#define RTC_OFFSET_SECONDS 0x00
#define RTC_OFFSET_MINUTES 0x01
#define RTC_OFFSET_HOURS 0x02
#define RTC_OFFSET_DAY_LOW 0x03
#define RTC_OFFSET_DAY_HIGH 0x04
	uint8_t RTC_data[RTC_DATA_SIZE] = { 0x00 };
	uint8_t latched_RTC_data[RTC_DATA_SIZE] = { 0x00 };
	bool latched_flag = false;

	void RTC_time_lapse__Nsec(time_t N) {
		if (N <= 0) {
			return;
		}
		time_t elapse_sec = (N % 60);
		time_t elapse_min = ((N % (60 * 60)) / (60));
		time_t elapse_hou = ((N % (60 * 60 * 24)) / (60 * 60));
		time_t elapse_day = (N / (60 * 60 * 24));
		if (elapse_day > 512) {
			elapse_day = 512;
		}
		M_debug_printf("######################################################\n");
		M_debug_printf("<RTC_time_lapse__Nsec>\n");
		M_debug_printf("elapse_sec = %lld\n", elapse_sec);
		M_debug_printf("elapse_min = %lld\n", elapse_min);
		M_debug_printf("elapse_hou = %lld\n", elapse_hou);
		M_debug_printf("elapse_day = %lld\n", elapse_day);
		M_debug_printf("######################################################\n");
		RTC_data[RTC_OFFSET_SECONDS] += elapse_sec;
		if (RTC_data[RTC_OFFSET_SECONDS] >= 60) {
			RTC_time_lapse__1min();
		}
		RTC_data[RTC_OFFSET_MINUTES] += elapse_min;
		if (RTC_data[RTC_OFFSET_MINUTES] >= 60) {
			RTC_time_lapse__1hou();
		}
		RTC_data[RTC_OFFSET_HOURS] += elapse_hou;
		if (RTC_data[RTC_OFFSET_HOURS] >= 24) {
			RTC_time_lapse__1day();
		}
		uint16_t RTC_day = RTC_data[RTC_OFFSET_DAY_LOW];
		if ((RTC_data[RTC_OFFSET_DAY_HIGH] & 0b00000001) != 0) {
			RTC_day |= 0b100000000;
		}
		RTC_day += elapse_day;
		RTC_data[RTC_OFFSET_DAY_LOW] = (RTC_day & 0b11111111);
		if (RTC_day >= 256) {
			if (RTC_day > 0b111111111) {
				RTC_data[RTC_OFFSET_DAY_HIGH] |= 0b10000000;
				RTC_data[RTC_OFFSET_DAY_HIGH] &= 0b11000000;
			}
			else {
				RTC_data[RTC_OFFSET_DAY_HIGH] |= 0b00000001;
			}
		}
	}

	void RTC_time_lapse__1sec() {
		RTC_data[RTC_OFFSET_SECONDS]++;
		if (RTC_data[RTC_OFFSET_SECONDS] >= 60) {
			RTC_time_lapse__1min();
		}
	}
	void RTC_time_lapse__1min() {
		RTC_data[RTC_OFFSET_SECONDS] = 0;
		RTC_data[RTC_OFFSET_MINUTES]++;
		if (RTC_data[RTC_OFFSET_MINUTES] >= 60) {
			RTC_time_lapse__1hou();
		}
	}
	void RTC_time_lapse__1hou() {
		RTC_data[RTC_OFFSET_MINUTES] = 0;
		RTC_data[RTC_OFFSET_HOURS]++;
		if (RTC_data[RTC_OFFSET_HOURS] >= 24) {
			RTC_time_lapse__1day();
		}
	}
	void RTC_time_lapse__1day() {
		RTC_data[RTC_OFFSET_HOURS] = 0;
		RTC_data[RTC_OFFSET_DAY_LOW]++;
		if (RTC_data[RTC_OFFSET_DAY_LOW] == 0) {//256日経過したとき(オーバーフローして0になったとき)
			//RTC_data[RTC_OFFSET_DAY_LOW] = 0;

			if ((RTC_data[RTC_OFFSET_DAY_HIGH] & 0b00000001) != 0) {
				RTC_data[RTC_OFFSET_DAY_HIGH] |= 0b10000000;
				RTC_data[RTC_OFFSET_DAY_HIGH] &= 0b11000000;
			}
			else {
				RTC_data[RTC_OFFSET_DAY_HIGH] |= 0b00000001;
			}
		}
	}

	uint8_t* get_read_ROM_address() {
		uint32_t use_rom_bank_no = 0;

		if (cart_mbc_type == CART_MBC_TYPE::ROM ||
			cart_mbc_type == CART_MBC_TYPE::OTHER ||
			cart_mbc_type == CART_MBC_TYPE::MBC1)
		{
			use_rom_bank_no = (rom_bank_no__low & 0b00011111) | ((rom_bank_no__high & 0b00000011) << 5);
		}
		else if (cart_mbc_type == CART_MBC_TYPE::MBC2) {
			use_rom_bank_no = (rom_bank_no__low & 0b00001111);
		}
		else if (cart_mbc_type == CART_MBC_TYPE::MBC3) {
			use_rom_bank_no = (rom_bank_no__low & 0b01111111);
		}
		else if (cart_mbc_type == CART_MBC_TYPE::MBC5) {
			use_rom_bank_no = rom_bank_no__low | ((rom_bank_no__high & 0b00000001) << 8);
		}
		else if (cart_mbc_type == CART_MBC_TYPE::HuC1) {
			use_rom_bank_no = rom_bank_no__low;
		}
		use_rom_bank_no &= ((Main::PGM_size >> 4/*16で割る*/) - 1);//PGMのサイズに必要な範囲内になるようにANDで絞る

		//M_debug_printf("use_rom_bank_no = 0x%02X\n", use_rom_bank_no);

		if (use_rom_bank_no == 0) {
			return &(gbx_ram.RAM[0x0000]);
		}
		else if (use_rom_bank_no == 1) {
			return &(gbx_ram.RAM[0x4000]);
		}
		else {
			return (uint8_t*)(ROM_bank_data_ptr + ((use_rom_bank_no - 2) * 0x4000));
		}
	}

	uint8_t* get_read_SRAM_address() {
		uint8_t use_sram_bank_no = 0;
		if (cart_mbc_type == CART_MBC_TYPE::ROM ||
			cart_mbc_type == CART_MBC_TYPE::OTHER ||
			cart_mbc_type == CART_MBC_TYPE::MBC1 ||
	
			cart_mbc_type == CART_MBC_TYPE::MBC3 ||
			cart_mbc_type == CART_MBC_TYPE::MBC5 ||
			cart_mbc_type == CART_MBC_TYPE::HuC1)
		{
			use_sram_bank_no = (sram_bank_no & ((Main::SRAM_size >> 3/*8で割る*/) - 1));//SRAMのサイズに必要な範囲内になるようにANDで絞る
		}
		else if (cart_mbc_type == CART_MBC_TYPE::MBC2) {
			return &(gbx_ram.RAM[0xA000]);
		}

		if (use_sram_bank_no == 0) {
			return &(gbx_ram.RAM[0xA000]);
		}
		else {
			return (uint8_t*)(SRAM_bank_data_ptr + ((sram_bank_no - 1) * 0x2000));
		}
	}

	uint8_t* getMBC2_RAM_address(uint16_t address) {
		/*
		A000-A1FF -> 512x4ビットの RAM
		A200-BFFF -> A000-A1FFのミラー
		*/
		uint16_t relative_address = address - 0xA000;
		relative_address %= 0x200;//A000-A1FFの範囲に変換する

		return (uint8_t*)(&(gbx_ram.RAM[0xA000]) + relative_address);
	}

	uint8_t* get_read_RAM_address___(uint16_t read_address) {
		if (hardware_type == Main::GAME_HARDWARE_TYPE::GAMEBOY_COLOR) {
			if (0x8000 <= read_address && read_address < 0xA000) {//VRAM - GBCではバンク0/1で切り替え可能
				if (VRAM_bank_no__cgb == 0) {
					return &(gbx_ram.RAM[read_address]);
				}
				else {
					uint16_t relative_addr = read_address - 0x8000;

					return &(VRAM_bank1_data_ptr__cgb[relative_addr]);
				}
			}
			else if (0xD000 <= read_address && read_address < 0xE000) {//WRAM(Bank01~NN) - GBCではバンク切り替え可能(バンク数はカートリッジに依存)
				uint16_t relative_addr = read_address - 0xD000;
				return &(WRAM_bank1_7_data_ptr__cgb[0x1000 * (WRAM_bank_no__cgb - 1) + relative_addr]);
			}
		}

		if (read_address <= 0x3FFF) {//ROMバンク00
			return &(gbx_ram.RAM[read_address]);
		}
		else if (read_address <= 0x7FFF) {//ROMバンク01-7F
			//read_value = gbx_ram.RAM[read_address];

			uint8_t* read_ROM_address = get_read_ROM_address();
			return &(read_ROM_address[read_address - 0x4000]);
		}
		else if (0xA000 <= read_address && read_address <= 0xBFFF) {//RAMバンク00-03（存在する場合）
			if (cart_mbc_type == CART_MBC_TYPE::MBC2) {//MBC2のとき
				return &(*(getMBC2_RAM_address(read_address)));
			}
			else {
				uint8_t* read_SRAM_address = get_read_SRAM_address();
				return &(read_SRAM_address[read_address - 0xA000]);
			}
		}
		else {//通常読み取り
			return &(gbx_ram.RAM[read_address]);
		}

		return nullptr;
	}

	uint16_t HDMA_src_addr = 0;
	uint16_t HDMA_dst_addr = 0;

	bool HBlank_DMA_Flag = false;//HBLANKDMAをしているか
	uint16_t HBlank_DMA_Remain_Size = 0x0000;//HBLANK_DMAの残りのサイズ
	void execute_HBLANK_DMA() {
		if (HBlank_DMA_Flag == false) {//HBLANKDMAをしていないとき
			return;
		}

		//memcpy(&(gbx_ram.RAM[HDMA_dst_addr]), &(gbx_ram.RAM[HDMA_src_addr]), 0x10);
		My_Emulator_RAM_memcpy(HDMA_dst_addr, HDMA_src_addr, 0x10);
		HDMA_src_addr += 0x10;
		HDMA_dst_addr += 0x10;
		HBlank_DMA_Remain_Size -= 0x10;

		if (HBlank_DMA_Remain_Size == 0x0000) {
			HBlank_DMA_Flag = false;
			HBlank_DMA_Remain_Size = 0x0000;
		}
	}

	uint8_t read_RAM_8bit(uint16_t read_address) {
		uint8_t read_value = 0x00;

		if (hardware_type == Main::GAME_HARDWARE_TYPE::GAMEBOY_COLOR) {
			if (0x8000 <= read_address && read_address < 0xA000) {//VRAM - GBCではバンク0/1で切り替え可能
				if (VRAM_bank_no__cgb == 0) {
					read_value = gbx_ram.RAM[read_address];
				}
				else {
					uint16_t relative_addr = read_address - 0x8000;

					read_value = VRAM_bank1_data_ptr__cgb[relative_addr];
				}

				return read_value;
			}
			else if (0xD000 <= read_address && read_address < 0xE000) {//WRAM(Bank01~NN) - GBCではバンク切り替え可能(バンク数はカートリッジに依存)
				uint16_t relative_addr = read_address - 0xD000;
				read_value = WRAM_bank1_7_data_ptr__cgb[0x1000 * (WRAM_bank_no__cgb - 1) + relative_addr];

				return read_value;
			}
			else if (read_address == 0xFF4F) {//VRAMバンク
				read_value = ((VRAM_bank_no__cgb & 0b00000001) | 0b11111110);

				//M_debug_printf("read_RAM_8bit()  VRAM_bank_no__cgb = %d\n", read_value);

				return read_value;
			}
			else if (read_address == 0xFF70) {//WRAMバンク
				read_value = (WRAM_bank_no__cgb & 0b00000111);

				//M_debug_printf("read_RAM_8bit()  WRAM_bank_no__cgb = %d\n", read_value);

				return read_value;
			}
			else if (read_address == 0xFF68) {//BGパレットインデックス
				read_value = (cgb_color_list_bg_window_index | ((cgb_color_list_bg_window_auto_inc_flag == true) ? 0b10000000 : 0b00000000));
				//read_value = cgb_color_list_bg_window_index;

				return read_value;
			}
			else if (read_address == 0xFF69) {//BGパレットデータ
				read_value = cgb_color_list_bg_window[cgb_color_list_bg_window_index];

				return read_value;
			}
			else if (read_address == 0xFF6A) {//OBJパレットインデックス
				read_value = (cgb_color_list_sprite_index | ((cgb_color_list_sprite_auto_inc_flag == true) ? 0b10000000 : 0b00000000));
				//read_value = cgb_color_list_sprite_index;

				return read_value;
			}
			else if (read_address == 0xFF6B) {//OBJパレットデータ
				read_value = cgb_color_list_sprite[cgb_color_list_sprite_index];

				return read_value;
			}
			else if (read_address == 0xFF55) {//HDMA制御レジスタ (R/W)
				if (HBlank_DMA_Flag == false) {//アクティブでないとき
					if (HBlank_DMA_Remain_Size != 0x0000) {//HBLANK_DMAを中断した場合
						read_value = ((HBlank_DMA_Remain_Size / 0x10) - 1);
						read_value |= 0b10000000;//bit7は1とする
					}
					else {
						read_value = 0xFF;
					}
				}
				else {//アクティブなとき
					read_value = (((HBlank_DMA_Remain_Size / 0x10) - 1) & 0b01111111);
				}

				return read_value;
			}
			else if (read_address == 0xFF4D) {//速度切り替えレジスタ
				read_value = 0;

				if (CURRENT_CPU_Clock_2x_Flag__CGB == true) {
					read_value |= 0b10000000;
				}

				read_value != 0b00000001;//常に速度切り替えの準備ができていることにする

				return read_value;
			}
		}

		if (read_address <= 0x3FFF) {//ROMバンク00
			if (cart_mbc_type == CART_MBC_TYPE::MBC1 && bank_mode == BankMode::SRAM && Main::PGM_size >= 1024) {//MBC1のときかつ"RAMバンクモード / アドバンスドROMバンクモード" のときかつROMのサイズが1MB以上の時
				uint32_t use_rom_bank_no = ((rom_bank_no__high & 0b00000011) << 5);
				use_rom_bank_no &= ((Main::PGM_size >> 4/*16で割る*/) - 1);//PGMのサイズに必要な範囲内になるようにANDで絞る

				if (use_rom_bank_no == 0) {
					read_value = gbx_ram.RAM[read_address];
				}
				else {
					read_value = *((uint8_t*)(ROM_bank_data_ptr + ((use_rom_bank_no - 2) * 0x4000) + read_address));
				}
			}
			else {
				read_value = gbx_ram.RAM[read_address];
			}
		}
		else if (read_address <= 0x7FFF) {//ROMバンク01-7F
			//read_value = gbx_ram.RAM[read_address];
	
			uint8_t* read_ROM_address = get_read_ROM_address();
			read_value = read_ROM_address[read_address - 0x4000];
		}
		else if (0xA000 <= read_address && read_address <= 0xBFFF) {//RAMバンク00-03（存在する場合）
			if (cart_mbc_type == CART_MBC_TYPE::MBC2) {//MBC2のとき
				read_value = *(getMBC2_RAM_address(read_address));
			}
			else if (cart_mbc_type == CART_MBC_TYPE::MBC3 && bank_mode == BankMode::RTC) {//MBC3でRTC読み取りのとき
				//M_debug_printf("[MBC3] 読み込み A000-BFFF - RTCレジスタ\n");
				//M_debug_printf("PC:0x%04x A:0x%02x, BC:0x%04x, DE:0x%04x, HL:0x%04x, Flags:0x%02x, SP:0x%04x\n",
				//	gbx_register.PC, gbx_register.A, gbx_register.BC, gbx_register.DE, gbx_register.HL, gbx_register.Flags, gbx_register.SP);

				uint8_t* RTC_data_ptr;
				if (latched_flag == true) {
					RTC_data_ptr = latched_RTC_data;
				}
				else {
					RTC_data_ptr = RTC_data;
				}
				if (clock_type__mbc3 == CLOCK_TYPE__MBC3::SECONDS) {
					read_value = (RTC_data_ptr[RTC_OFFSET_SECONDS] & 0b00111111);
				}
				else if (clock_type__mbc3 == CLOCK_TYPE__MBC3::MINUTES) {
					read_value = (RTC_data_ptr[RTC_OFFSET_MINUTES] & 0b00111111);
				}
				else if (clock_type__mbc3 == CLOCK_TYPE__MBC3::HOURS) {
					read_value = (RTC_data_ptr[RTC_OFFSET_HOURS] & 0b00011111);
				}
				else if (clock_type__mbc3 == CLOCK_TYPE__MBC3::DAY_LOW) {
					read_value = RTC_data_ptr[RTC_OFFSET_DAY_LOW];
				}
				else {//DAY_HIGH
					read_value = (RTC_data_ptr[RTC_OFFSET_DAY_HIGH] & 0b11000001);
				}
			}
			else if (cart_mbc_type == CART_MBC_TYPE::HuC1 && bank_mode == BankMode::IR) {//ROMのタイプがHuC1かつIRモードのとき) {
				/*
				TODO
				IRの読み取りを実装する
				*/
	
				//0xC1（赤外線を受信した）または0xC0（受信しなかった）
				read_value = 0xC0;//受信しなかったことにしておく
			}
			else {
				uint8_t* read_SRAM_address = get_read_SRAM_address();
				read_value = read_SRAM_address[read_address - 0xA000];
			}
		}
		else if (read_address == 0xFF00) {//ジョイパッド
			if ((gbx_ram.RAM[0xFF00] & 0b00010000) == 0) {//方向キー
	
				uint8_t b_down = (key->get_input_state__GBX__(INPUT_MY_ID_DOWN) != 0) ? 1 : 0;
				uint8_t b_up = (key->get_input_state__GBX__(INPUT_MY_ID_UP) != 0) ? 1 : 0;
				uint8_t b_left = (key->get_input_state__GBX__(INPUT_MY_ID_LEFT) != 0) ? 1 : 0;
				uint8_t b_right = (key->get_input_state__GBX__(INPUT_MY_ID_RIGHT) != 0) ? 1 : 0;
	
				read_value = (gbx_ram.RAM[0xFF00] & 0b00110000);
				read_value |= 0b11000000;//未使用部分は1にする
				read_value |= ((~((b_down << 3) | (b_up << 2) | (b_left << 1) | b_right)) & 0b00001111);
			}
			else if ((gbx_ram.RAM[0xFF00] & 0b00100000) == 0) {//アクションキー
				uint8_t b_start = (key->get_input_state__GBX__(INPUT_MY_ID_START) != 0) ? 1 : 0;
				uint8_t b_select = (key->get_input_state__GBX__(INPUT_MY_ID_SELECT) != 0) ? 1 : 0;
				uint8_t b_b = (key->get_input_state__GBX__(INPUT_MY_ID_B) != 0) ? 1 : 0;
				uint8_t b_a = (key->get_input_state__GBX__(INPUT_MY_ID_A) != 0) ? 1 : 0;
	
				read_value = (gbx_ram.RAM[0xFF00] & 0b00110000);
				read_value |= 0b11000000;//未使用部分は1にする
				read_value |= ((~((b_start << 3) | (b_select << 2) | (b_b << 1) | b_a)) & 0b00001111);
			}
			else {
				read_value = gbx_ram.RAM[0xFF00] & 0b00110000;
				//read_value |= 0b00001111;
				read_value |= 0b11001111;//未使用部分は1にする
			}
		}
		//=================================================================================
		else if (read_address == 0xFF10) {
			read_value = apu->get_channel_1()->CH1__0xFF10;
		}
		else if (read_address == 0xFF11) {
			read_value = apu->get_channel_1()->CH1__0xFF11;
		}
		else if (read_address == 0xFF12) {
			read_value = apu->get_channel_1()->CH1__0xFF12;
		}
		else if (read_address == 0xFF13) {
			read_value = apu->get_channel_1()->CH1__0xFF13;
		}
		else if (read_address == 0xFF14) {
			read_value = apu->get_channel_1()->CH1__0xFF14;
		}
		//=================================================================================
		else if (read_address == 0xFF16) {
			read_value = apu->get_channel_2()->CH2__0xFF16;
		}
		else if (read_address == 0xFF17) {
			read_value = apu->get_channel_2()->CH2__0xFF17;
		}
		else if (read_address == 0xFF18) {
			read_value = apu->get_channel_2()->CH2__0xFF18;
		}
		else if (read_address == 0xFF19) {
			read_value = apu->get_channel_2()->CH2__0xFF19;
		}
		//=================================================================================
		else if (read_address == 0xFF1A) {
			read_value = apu->get_channel_3()->CH3__0xFF1A;
		}
		else if (read_address == 0xFF1B) {
			read_value = apu->get_channel_3()->CH3__0xFF1B;
		}
		else if (read_address == 0xFF1C) {
			read_value = apu->get_channel_3()->CH3__0xFF1C;
		}
		else if (read_address == 0xFF1D) {
			read_value = apu->get_channel_3()->CH3__0xFF1D;
		}
		else if (read_address == 0xFF1E) {
			read_value = apu->get_channel_3()->CH3__0xFF1E;
		}
		else if (0xFF30 <= read_address && read_address <= 0xFF3F) {
			read_value = apu->get_channel_3()->CH3__0xFF30_0xFF3F[read_address - 0xFF30];
		}
		//=================================================================================
		else if (read_address == 0xFF20) {
			read_value = apu->get_channel_4()->CH4__0xFF20;
		}
		else if (read_address == 0xFF21) {
			read_value = apu->get_channel_4()->CH4__0xFF21;
		}
		else if (read_address == 0xFF22) {
			read_value = apu->get_channel_4()->CH4__0xFF22;
		}
		else if (read_address == 0xFF23) {
			read_value = apu->get_channel_4()->CH4__0xFF23;
		}
		//=================================================================================
		else if (read_address == 0xFF24) {//サウンド
			read_value = gbx_ram.RAM[read_address];
		}
		else if (read_address == 0xFF25) {//サウンド
			read_value = gbx_ram.RAM[read_address];
		}
		else if (read_address == 0xFF26) {//サウンド
			read_value = 0x00;
			if (apu->all_channel_enable_flag == true) {
				read_value |= 0b10000000;

				if (apu->get_channel_1()->is_playing() == true) {
					read_value |= 0b00000001;
				}
				if (apu->get_channel_2()->is_playing() == true) {
					read_value |= 0b00000010;
				}
				if (apu->get_channel_3()->is_playing() == true) {
					read_value |= 0b00000100;
				}
				if (apu->get_channel_4()->is_playing() == true) {
					read_value |= 0b00001000;
				}
			}
		}
		//=================================================================================
		else if (read_address == 0xFF01) {//通信データレジスタ (R/W)
			read_value = 0x00;

			//M_debug_printf("読み込み 通信データレジスタ (R/W)\n");
		}
		else if (read_address == 0xFF02) {//通信制御レジスタ (R/W)
			read_value = 0x00;

			//M_debug_printf("読み込み 通信制御レジスタ (R/W)\n");
		}
		else {//通常読み取り
			read_value = gbx_ram.RAM[read_address];
		}
	
		return read_value;
	}
	
	uint16_t read_RAM_16bit(uint16_t read_address) {
		//return (gbx_ram.RAM[read_address] | (gbx_ram.RAM[read_address + 1] << 8));
	
		return (read_RAM_8bit(read_address) | (read_RAM_8bit(read_address + 1) << 8));
	}

	void write_RAM_8bit(uint16_t write_address, uint8_t value) {
		if (hardware_type == Main::GAME_HARDWARE_TYPE::GAMEBOY_COLOR) {
			if (0x8000 <= write_address && write_address < 0xA000) {//VRAM - GBCではバンク0/1で切り替え可能
				if (VRAM_bank_no__cgb == 0) {
					gbx_ram.RAM[write_address] = value;
				}
				else {
					uint16_t relative_addr = write_address - 0x8000;

					VRAM_bank1_data_ptr__cgb[relative_addr] = value;
				}

				return;
			}
			else if (0xD000 <= write_address && write_address < 0xE000) {//WRAM(Bank01~NN) - GBCではバンク切り替え可能(バンク数はカートリッジに依存)
				uint16_t relative_addr = write_address - 0xD000;
				WRAM_bank1_7_data_ptr__cgb[0x1000 * (WRAM_bank_no__cgb - 1) + relative_addr] = value;

				return;
			}
			else if (write_address == 0xFF4F) {//VRAMバンク
				VRAM_bank_no__cgb = (value & 0b00000001);

				//M_debug_printf("write_RAM_8bit()  VRAM_bank_no__cgb = %d\n", VRAM_bank_no__cgb);

				return;
			}
			else if (write_address == 0xFF70) {//WRAMバンク
				WRAM_bank_no__cgb = (value & 0b00000111);
				if (WRAM_bank_no__cgb == 0) {
					WRAM_bank_no__cgb = 1;
				}

				//M_debug_printf("write_RAM_8bit()  WRAM_bank_no__cgb = %d\n", WRAM_bank_no__cgb);

				return;
			}
			else if (write_address == 0xFF68) {//BGパレットインデックス
				cgb_color_list_bg_window_index = (value & 0b00111111);

				if ((value & 0b10000000) != 0) {
					cgb_color_list_bg_window_auto_inc_flag = true;
				}
				else {
					cgb_color_list_bg_window_auto_inc_flag = false;
				}

				return;
			}
			else if (write_address == 0xFF69) {//BGパレットデータ
				cgb_color_list_bg_window[cgb_color_list_bg_window_index] = value;

				if (cgb_color_list_bg_window_auto_inc_flag == true) {
					cgb_color_list_bg_window_index++;

					if (cgb_color_list_bg_window_index >= 0x40) {
						cgb_color_list_bg_window_index = 0;
					}
				}

				return;
			}
			else if (write_address == 0xFF6A) {//OBJパレットインデックス
				cgb_color_list_sprite_index = (value & 0b00111111);

				if ((value & 0b10000000) != 0) {
					cgb_color_list_sprite_auto_inc_flag = true;
				}
				else {
					cgb_color_list_sprite_auto_inc_flag = false;
				}

				return;
			}
			else if (write_address == 0xFF6B) {//OBJパレットデータ
				cgb_color_list_sprite[cgb_color_list_sprite_index] = value;

				if (cgb_color_list_sprite_auto_inc_flag == true) {
					cgb_color_list_sprite_index++;

					if (cgb_color_list_sprite_index >= 0x40) {
						cgb_color_list_sprite_index = 0;
					}
				}

				return;
			}
			else if (write_address == 0xFF51) {//HDMAソースレジスタ上位8bit (W)
				HDMA_src_addr = (value << 8) | (HDMA_src_addr & 0b11111111);

				return;
			}
			else if (write_address == 0xFF52) {//HDMAソースレジスタ下位8bit (W)
				HDMA_src_addr = (HDMA_src_addr & 0b1111111100000000) | (value & 0b11110000);

				return;
			}
			else if (write_address == 0xFF53) {//HDMAターゲットレジスタ上位8bit (W)
				HDMA_dst_addr = ((value & 0b00011111) << 8) | (HDMA_dst_addr & 0b11111111);
				HDMA_dst_addr |= 0b1000000000000000;

				return;
			}
			else if (write_address == 0xFF54) {//HDMAターゲットレジスタ下位8bit (W)
				HDMA_dst_addr = (HDMA_dst_addr & 0b1111111100000000) | (value & 0b11110000);

				return;
			}
			else if (write_address == 0xFF55) {//HDMA制御レジスタ (R/W)
				uint16_t transfer_size = ((value & 0b01111111) + 1) * 0x10;

				if ((value & 0b10000000) == 0) {//(汎用DMA, GDMA)
					HBlank_DMA_Flag = false;

					//memcpy(&(gbx_ram.RAM[HDMA_dst_addr]), &(gbx_ram.RAM[HDMA_src_addr]), transfer_size);
					My_Emulator_RAM_memcpy(HDMA_dst_addr, HDMA_src_addr, transfer_size);

					//M_debug_printf("GDMA dst = 0x%04x, src = 0x%04x, size = 0x%x\n", HDMA_dst_addr, HDMA_src_addr, transfer_size);
				}
				else {//(HBlank DMA)
					HBlank_DMA_Flag = true;

					HBlank_DMA_Remain_Size = transfer_size;

					//M_debug_printf("HBlank_DMA dst = 0x%04x, src = 0x%04x, size = 0x%x\n", HDMA_dst_addr, HDMA_src_addr, transfer_size);
				}

				return;
			}
			else if (write_address == 0xFF4D) {//速度切り替えレジスタ
				//STOP命令で変更するCPUのモードをセットする
				if ((value & 0b00000001) != 0) {
					SET_CPU_Clock_2x_Flag__CGB = true;
				}
				else {
					SET_CPU_Clock_2x_Flag__CGB = false;
				}

				return;
			}

		}

		if (write_address <= 0x1FFF) {//RAMの有効化
			if (cart_mbc_type == CART_MBC_TYPE::ROM ||
				cart_mbc_type == CART_MBC_TYPE::OTHER ||
				cart_mbc_type == CART_MBC_TYPE::MBC1 ||
					
				cart_mbc_type == CART_MBC_TYPE::MBC5)
			{
				if ((value & 0b00001111) == 0x0A) {
					SRAM_Enable_Flag = true;
				}
				else {
					SRAM_Enable_Flag = false;
				}
			}
			else if (cart_mbc_type == CART_MBC_TYPE::MBC2) {
				if ((write_address & 0b0000000100000000) == 0) {//上位アドレスバイトの最下位ビットが0のとき
					if (value == 0x0A) {
						SRAM_Enable_Flag = true;

						//M_debug_printf("Enable SRAM\n");
					}
					else {
						SRAM_Enable_Flag = false;

						//M_debug_printf("Disable SRAM\n");
					}
				}
				else {//それ以外のとき
					rom_bank_no__low = (value & 0b00001111);
					if (rom_bank_no__low == 0) {
						rom_bank_no__low = 1;
					}
				}
			}
			else if (cart_mbc_type == CART_MBC_TYPE::MBC3) {
				//M_debug_printf("[MBC3] 書き込み 0000-1FFF - RAM/タイマー有効化フラグ\n");
				//M_debug_printf("PC:0x%04x A:0x%02x, BC:0x%04x, DE:0x%04x, HL:0x%04x, Flags:0x%02x, SP:0x%04x\n",
				//	gbx_register.PC, gbx_register.A, gbx_register.BC, gbx_register.DE, gbx_register.HL, gbx_register.Flags, gbx_register.SP);

				if (value == 0x0A) {
					SRAM_Enable_Flag = true;
					RTC_Enable_Flag = true;
				}
				else {
					SRAM_Enable_Flag = false;
					RTC_Enable_Flag = false;
				}
			}
			else if (cart_mbc_type == CART_MBC_TYPE::HuC1) {
				if (value == 0x0E) {
					bank_mode = BankMode::IR;
				}
				else {
					bank_mode = BankMode::SRAM;
				}
			}
		}
		else if (write_address <= 0x3FFF) {//ROMバンク番号
			//M_debug_printf("write_address <= 0x3FFF [value = 0x%02x]\n", value);
			//system("pause");
	
			if (cart_mbc_type == CART_MBC_TYPE::ROM ||
				cart_mbc_type == CART_MBC_TYPE::OTHER ||
				cart_mbc_type == CART_MBC_TYPE::MBC1)
			{
				rom_bank_no__low = (value & 0b00011111);

				//M_debug_printf("[write_address <= 0x3FFF] rom_bank_no__low = %d, rom_bank_no__high = %d, bank_mode = %s\n", rom_bank_no__low, rom_bank_no__high, (bank_mode == BankMode::ROM) ? "ROM_MODE" : "OTHER_MODE");

				if (rom_bank_no__low == 0) {
					rom_bank_no__low = 1;
				}
			}
			else if (cart_mbc_type == CART_MBC_TYPE::MBC2) {
				if ((write_address & 0b0000000100000000) == 0) {//上位アドレスバイトの最下位ビットが0のとき
					if (value == 0x0A) {
						SRAM_Enable_Flag = true;

						//M_debug_printf("Enable SRAM\n");
					}
					else {
						SRAM_Enable_Flag = false;

						//M_debug_printf("Disable SRAM\n");
					}
				}
				else {//それ以外のとき
					rom_bank_no__low = (value & 0b00001111);
					if (rom_bank_no__low == 0) {
						rom_bank_no__low = 1;
					}
				}
			}
			else if (cart_mbc_type == CART_MBC_TYPE::MBC3) {
				//M_debug_printf("[MBC3] 書き込み 2000-3FFF - ROMバンク番号\n");
				//M_debug_printf("PC:0x%04x A:0x%02x, BC:0x%04x, DE:0x%04x, HL:0x%04x, Flags:0x%02x, SP:0x%04x\n",
				//	gbx_register.PC, gbx_register.A, gbx_register.BC, gbx_register.DE, gbx_register.HL, gbx_register.Flags, gbx_register.SP);

				rom_bank_no__low = (value & 0b01111111);
				if (rom_bank_no__low == 0) {
					rom_bank_no__low = 1;
				}
			}
			else if (cart_mbc_type == CART_MBC_TYPE::MBC5) {
				if (write_address <= 0x2FFF) {
					rom_bank_no__low = value;
				}
				else {
					rom_bank_no__high = (value & 0b00000001);
				}
			}
			else if (cart_mbc_type == CART_MBC_TYPE::HuC1) {
				rom_bank_no__low = value;
			}
		}
		else if (write_address <= 0x5FFF) {//RAMバンク番号-または-ROMバンク番号の上位ビット
			//M_debug_printf("write_address <= 0x5FFF [value = 0x%02x]\n", value);
			//system("pause");
	
	
			if (cart_mbc_type == CART_MBC_TYPE::ROM ||
				cart_mbc_type == CART_MBC_TYPE::OTHER ||
				cart_mbc_type == CART_MBC_TYPE::MBC1)
			{
				if (bank_mode == BankMode::ROM) {
					rom_bank_no__high = (value & 0b00000011);
				}
				else if (bank_mode == BankMode::SRAM) {
					if (Main::PGM_size >= 1024) {
						rom_bank_no__high = (value & 0b00000011);
					}
					else {
						sram_bank_no = (value & 0b00000011);
					}
				}

				//M_debug_printf("[write_address <= 0x5FFF] rom_bank_no__low = %d, rom_bank_no__high = %d, bank_mode = %s\n", rom_bank_no__low, rom_bank_no__high, (bank_mode == BankMode::ROM) ? "ROM_MODE" : "OTHER_MODE");
			}
			else if (cart_mbc_type == CART_MBC_TYPE::MBC2) {
				//何もしない
			}
			else if (cart_mbc_type == CART_MBC_TYPE::MBC3) {
				//M_debug_printf("[MBC3] 書き込み 4000-5FFF - RAMバンク/RTCレジスタの選択レジスタ\n");
				//M_debug_printf("PC:0x%04x A:0x%02x, BC:0x%04x, DE:0x%04x, HL:0x%04x, Flags:0x%02x, SP:0x%04x\n",
				//	gbx_register.PC, gbx_register.A, gbx_register.BC, gbx_register.DE, gbx_register.HL, gbx_register.Flags, gbx_register.SP);

				if (0x00 <= value && value <= 0x03) {
					bank_mode = BankMode::SRAM;
					sram_bank_no = (value & 0b00000011);
				}
				else {
					bank_mode = BankMode::RTC;
	
					if (value == 0x08) {
						clock_type__mbc3 = CLOCK_TYPE__MBC3::SECONDS;
					}
					else if (value == 0x09) {
						clock_type__mbc3 = CLOCK_TYPE__MBC3::MINUTES;
					}
					else if (value == 0x0A) {
						clock_type__mbc3 = CLOCK_TYPE__MBC3::HOURS;
					}
					else if (value == 0x0B) {
						clock_type__mbc3 = CLOCK_TYPE__MBC3::DAY_LOW;
					}
					else if (value == 0x0C) {
						clock_type__mbc3 = CLOCK_TYPE__MBC3::DAY_HIGH;
					}
				}
			}
			else if (cart_mbc_type == CART_MBC_TYPE::MBC5) {
				if (0x00 <= value && value <= 0x0F) {
					sram_bank_no = value;
				}
			}
			else if (cart_mbc_type == CART_MBC_TYPE::HuC1) {
				sram_bank_no = value;
			}
		}
		else if (write_address <= 0x7FFF) {//ROM / RAMモード選択
			//M_debug_printf("write_address <= 0x7FFF [value = 0x%02x]\n", value);
			//system("pause");
	
			if (cart_mbc_type == CART_MBC_TYPE::ROM ||
				cart_mbc_type == CART_MBC_TYPE::OTHER ||
				cart_mbc_type == CART_MBC_TYPE::MBC1)
			{
				if (value == 0) {
					bank_mode = BankMode::ROM;
				}
				else if (value == 1) {
					bank_mode = BankMode::SRAM;
				}
			}
			else if (cart_mbc_type == CART_MBC_TYPE::MBC2) {
				//何もしない
			}
			else if (cart_mbc_type == CART_MBC_TYPE::MBC3) {
				//M_debug_printf("[MBC3] 書き込み 6000-7FFF - ラッチクロックデータ\n");
				//M_debug_printf("PC:0x%04x A:0x%02x, BC:0x%04x, DE:0x%04x, HL:0x%04x, Flags:0x%02x, SP:0x%04x\n",
				//	gbx_register.PC, gbx_register.A, gbx_register.BC, gbx_register.DE, gbx_register.HL, gbx_register.Flags, gbx_register.SP);

				if (value == 0x01) {
					latched_flag = false;
				}
				else if (value == 0x00) {
					latched_flag = true;
					memcpy(latched_RTC_data, RTC_data, RTC_DATA_SIZE);
				}
			}
			else if (cart_mbc_type == CART_MBC_TYPE::MBC5) {
				//何もしない
			}
			else if (cart_mbc_type == CART_MBC_TYPE::HuC1) {
				//何もしない
			}
		}
		else if (0xA000 <= write_address && write_address <= 0xBFFF) {//RAMバンク00-03（存在する場合）
			//gbx_ram.RAM[write_address] = value;

			if (cart_mbc_type == CART_MBC_TYPE::MBC2) {//MBC2のとき
				*(getMBC2_RAM_address(write_address)) = value;
			}
			else if (cart_mbc_type == CART_MBC_TYPE::MBC3) {//ROMのタイプがMBC3のとき
				//M_debug_printf("[MBC3] 書き込み A000-BFFF - RTCレジスタ\n");
				//M_debug_printf("PC:0x%04x A:0x%02x, BC:0x%04x, DE:0x%04x, HL:0x%04x, Flags:0x%02x, SP:0x%04x\n",
				//	gbx_register.PC, gbx_register.A, gbx_register.BC, gbx_register.DE, gbx_register.HL, gbx_register.Flags, gbx_register.SP);

				if (bank_mode == BankMode::RTC) {//RTCモードのとき
					if (RTC_Enable_Flag == true) {
						if (clock_type__mbc3 == CLOCK_TYPE__MBC3::SECONDS) {
							RTC_data[RTC_OFFSET_SECONDS] = (value & 0b00111111);
						}
						else if (clock_type__mbc3 == CLOCK_TYPE__MBC3::MINUTES) {
							RTC_data[RTC_OFFSET_MINUTES] = (value & 0b00111111);
						}
						else if (clock_type__mbc3 == CLOCK_TYPE__MBC3::HOURS) {
							RTC_data[RTC_OFFSET_HOURS] = (value & 0b00011111);
						}
						else if (clock_type__mbc3 == CLOCK_TYPE__MBC3::DAY_LOW) {
							RTC_data[RTC_OFFSET_DAY_LOW] = value;
						}
						else {//DAY_HIGH
							RTC_data[RTC_OFFSET_DAY_HIGH] = (value & 0b11000001);

							M_debug_printf("<RTC WRITE> RTC_data[RTC_OFFSET_DAY_HIGH] = 0x%02X\n", RTC_data[RTC_OFFSET_DAY_HIGH]);
						}
					}
				}
				else {//SRAMモードのとき
					if (SRAM_Enable_Flag == true) {
						uint8_t* read_SRAM_address = get_read_SRAM_address();
						read_SRAM_address[write_address - 0xA000] = value;
					}
				}
			}
			else if (cart_mbc_type == CART_MBC_TYPE::HuC1 && bank_mode == BankMode::IR) {//ROMのタイプがHuC1かつIRモードのとき
				if (value == 0x00) {
					IR_Enable_Flag = false;
				}
				else if (value == 0x01) {
					IR_Enable_Flag = true;
				}
			}
			else {
				uint8_t* read_SRAM_address = get_read_SRAM_address();
				read_SRAM_address[write_address - 0xA000] = value;
			}
		}
		else if (write_address == 0xFF00) {//ジョイパッド
			gbx_ram.RAM[0xFF00] = (value & 0b00110000);
		}
		else if (write_address == 0xFF04) {
			gbx_ram.RAM[write_address] = 0x00;
		}
		else if (write_address == 0xFF46) {//DMA
			uint16_t src_address = value << 8;
			//memcpy((void*)(&(gbx_ram.RAM[0xFE00])), (void*)(&(gbx_ram.RAM[src_address])), 40 * 4);
			My_Emulator_RAM_memcpy(0xFE00, src_address, 40 * 4);
	
			//サイクル数はすすめない //cpu_machine_cycle += 160;//160 M-cycle かかる
		}
		//=================================================================================
		else if (write_address == 0xFF10) {
			apu->get_channel_1()->CH1__0xFF10 = value;
		}
		else if (write_address == 0xFF11) {
			apu->get_channel_1()->CH1__0xFF11 = value;
		}
		else if (write_address == 0xFF12) {
			apu->get_channel_1()->CH1__0xFF12 = value;
		}
		else if (write_address == 0xFF13) {
			apu->get_channel_1()->CH1__0xFF13 = value;
		}
		else if (write_address == 0xFF14) {
			apu->get_channel_1()->CH1__0xFF14 = value;

			if ((value & 0b10000000) != 0) {//リセットする場合
				apu->get_channel_1()->CH1_envelope_volume = ((apu->get_channel_1()->CH1__0xFF12 >> 4) & 0b1111);
				apu->get_channel_1()->CH1_envelope_counter = 0.0;

				apu->get_channel_1()->CH1_length_counter = 0.0;

				apu->get_channel_1()->set_sound_enable_flag(true);

				apu->get_channel_1()->init_CH1_sweep_freq_f();
			}
		}
		//=================================================================================
		else if (write_address == 0xFF16) {
			apu->get_channel_2()->CH2__0xFF16 = value;
		}
		else if (write_address == 0xFF17) {
			apu->get_channel_2()->CH2__0xFF17 = value;
		}
		else if (write_address == 0xFF18) {
			apu->get_channel_2()->CH2__0xFF18 = value;
		}
		else if (write_address == 0xFF19) {
			apu->get_channel_2()->CH2__0xFF19 = value;

			if ((value & 0b10000000) != 0) {//リセットする場合
				apu->get_channel_2()->CH2_envelope_volume = ((apu->get_channel_2()->CH2__0xFF17 >> 4) & 0b1111);
				apu->get_channel_2()->CH2_envelope_counter = 0.0;

				apu->get_channel_2()->CH2_length_counter = 0.0;

				apu->get_channel_2()->set_sound_enable_flag(true);
			}
		}
		//=================================================================================
		else if (write_address == 0xFF1A) {
			apu->get_channel_3()->CH3__0xFF1A = value;

			if ((value & 0b10000000) != 0) {//波形メモリチャネル有効フラグが有効な場合
				apu->get_channel_3()->set_sound_enable_flag(true);
			}
			else {//波形メモリチャネル有効フラグが有効でない場合
				apu->get_channel_3()->set_sound_enable_flag(false);
			}
		}
		else if (write_address == 0xFF1B) {
			apu->get_channel_3()->CH3__0xFF1B = value;
		}
		else if (write_address == 0xFF1C) {
			apu->get_channel_3()->CH3__0xFF1C = value;
		}
		else if (write_address == 0xFF1D) {
			apu->get_channel_3()->CH3__0xFF1D = value;
		}
		else if (write_address == 0xFF1E) {
			apu->get_channel_3()->CH3__0xFF1E = value;

			if ((value & 0b10000000) != 0) {//リセットする場合
				apu->get_channel_3()->CH3_length_counter = 0;

				//apu->get_channel_3()->set_sound_enable_flag(true);
			}
		}
		else if (0xFF30 <= write_address && write_address <= 0xFF3F) {
			apu->get_channel_3()->CH3__0xFF30_0xFF3F[write_address - 0xFF30] = value;
		}
		//=================================================================================
		else if (write_address == 0xFF20) {
			apu->get_channel_4()->CH4__0xFF20 = value;
		}
		else if (write_address == 0xFF21) {
			apu->get_channel_4()->CH4__0xFF21 = value;
		}
		else if (write_address == 0xFF22) {
			apu->get_channel_4()->CH4__0xFF22 = value;

			//M_debug_printf("value = 0x%02x\n", value);
		}
		else if (write_address == 0xFF23) {
			apu->get_channel_4()->CH4__0xFF23 = value;

			/*
			条件をつけるとノイズがなり続けて止まらないことがある(ゲームボーイの仕様？
			条件をコメントアウトすると正常に動作するのでコメントアウトしておく
			*/
			//if ((value & 0b10000000) != 0) {//リセットする場合
				apu->get_channel_4()->CH4_envelope_volume = ((apu->get_channel_4()->CH4__0xFF21 >> 4) & 0b1111);
				apu->get_channel_4()->CH4_envelope_counter = 0.0;

				apu->get_channel_4()->CH4_length_counter = 0.0;

				apu->get_channel_4()->set_sound_enable_flag(true);
			//}
		}
		//=================================================================================
		else if (write_address == 0xFF24) {//サウンド
			gbx_ram.RAM[write_address] = value;

			float left_volume;
			float right_volume;

			//if ((value & 0b10000000) != 0) {//左出力有効フラグが有効なとき
			uint8_t left_volume_3bit_value = ((value >> 4) & 0b111);

			if (left_volume_3bit_value == 7) {
				left_volume = 1.0f;
			}
			else if (left_volume_3bit_value == 0) {
				left_volume = 0.0f;
			}
			else {
				left_volume = ((float)left_volume_3bit_value / 7.0f);
			}
			//}
			//else {//左出力有効フラグが無効なとき
			//	left_volume = 0.0f;
			//}

			//if ((value & 0b00001000) != 0) {//右出力有効フラグが有効なとき
			uint8_t right_volume_3bit_value = (value & 0b111);

			if (right_volume_3bit_value == 7) {
				right_volume = 1.0f;
			}
			else if (right_volume_3bit_value == 0) {
				right_volume = 0.0f;
			}
			else {
				right_volume = ((float)right_volume_3bit_value / 7.0f);
			}
			//}
			//else {//右出力有効フラグが無効なとき
			//	right_volume = 0.0f;
			//}

			//M_debug_printf("value = 0x%02X,  left_volume_3bit_value = %d,  right_volume_3bit_value = %d\n", value, ((value >> 4) & 0b111), (value & 0b111));
			//M_debug_printf("left_volume = %f,  right_volume = %f\n", left_volume, right_volume);
			apu->set_all_Channel_Left_Right_volume(left_volume, right_volume);
		}
		else if (write_address == 0xFF25) {//サウンド
			//M_debug_printf("<0xFF25> value = 0x%02x\n", value);

			gbx_ram.RAM[write_address] = value;

			float left_volume;
			float right_volume;

			uint8_t left_volume_3bit_value = ((gbx_ram.RAM[0xFF24] >> 4) & 0b111);
			if (left_volume_3bit_value == 7) {
				left_volume = 1.0f;
			}
			else if (left_volume_3bit_value == 0) {
				left_volume = 0.0f;
			}
			else {
				left_volume = ((float)left_volume_3bit_value / 7.0f);
			}

			uint8_t right_volume_3bit_value = (gbx_ram.RAM[0xFF24] & 0b111);
			if (right_volume_3bit_value == 7) {
				right_volume = 1.0f;
			}
			else if (right_volume_3bit_value == 0) {
				right_volume = 0.0f;
			}
			else {
				right_volume = ((float)right_volume_3bit_value / 7.0f);
			}

			if ((value & 0b10000000) != 0) {
				apu->get_channel_4()->left_sound_ON_flag = true;
				apu->set_Channel4_Left_volume(left_volume);
			}
			else {
				apu->get_channel_4()->left_sound_ON_flag = false;
				apu->set_Channel4_Left_volume(0.0f);
			}

			if ((value & 0b01000000) != 0) {
				apu->get_channel_3()->left_sound_ON_flag = true;
				apu->set_Channel3_Left_volume(left_volume);
			}
			else {
				apu->get_channel_3()->left_sound_ON_flag = false;
				apu->set_Channel3_Left_volume(0.0f);
			}

			if ((value & 0b00100000) != 0) {
				apu->get_channel_2()->left_sound_ON_flag = true;
				apu->set_Channel2_Left_volume(left_volume);
			}
			else {
				apu->get_channel_2()->left_sound_ON_flag = false;
				apu->set_Channel2_Left_volume(0.0f);
			}

			if ((value & 0b00010000) != 0) {
				apu->get_channel_1()->left_sound_ON_flag = true;
				apu->set_Channel1_Left_volume(left_volume);
			}
			else {
				apu->get_channel_1()->left_sound_ON_flag = false;
				apu->set_Channel1_Left_volume(0.0f);
			}

			if ((value & 0b00001000) != 0) {
				apu->get_channel_4()->right_sound_ON_flag = true;
				apu->set_Channel4_Right_volume(right_volume);
			}
			else {
				apu->get_channel_4()->right_sound_ON_flag = false;
				apu->set_Channel4_Right_volume(0.0f);
			}

			if ((value & 0b00000100) != 0) {
				apu->get_channel_3()->right_sound_ON_flag = true;
				apu->set_Channel3_Right_volume(right_volume);
			}
			else {
				apu->get_channel_3()->right_sound_ON_flag = false;
				apu->set_Channel3_Right_volume(0.0f);
			}

			if ((value & 0b00000010) != 0) {
				apu->get_channel_2()->right_sound_ON_flag = true;
				apu->set_Channel2_Right_volume(right_volume);
			}
			else {
				apu->get_channel_2()->right_sound_ON_flag = false;
				apu->set_Channel2_Right_volume(0.0f);
			}

			if ((value & 0b00000001) != 0) {
				apu->get_channel_1()->right_sound_ON_flag = true;
				apu->set_Channel1_Right_volume(right_volume);
			}
			else {
				apu->get_channel_1()->right_sound_ON_flag = false;
				apu->set_Channel1_Right_volume(0.0f);
			}

		}
		else if (write_address == 0xFF26) {//サウンド
			apu->all_channel_enable_flag = ((value & 0b10000000) != 0) ? true : false;
		}
		//=================================================================================
		else if (write_address == 0xFF01) {//通信データレジスタ (R/W)
			gbx_ram.RAM[write_address] = 0x00;

			//M_debug_printf("書き込み 通信データレジスタ (R/W)\n");
		}
		else if (write_address == 0xFF02) {//通信制御レジスタ (R/W)
			gbx_ram.RAM[write_address] = 0x00;

			//M_debug_printf("書き込み 通信制御レジスタ (R/W)\n");
		}
		else {//通常書き込み
			gbx_ram.RAM[write_address] = value;
		}
	}

	void write_RAM_16bit(uint16_t write_address, uint16_t value) {
		//gbx_ram.RAM[write_address] = (uint8_t)(value & 0b0000000011111111);
		//gbx_ram.RAM[write_address + 1] = (uint8_t)(value >> 8);
	
		write_RAM_8bit(write_address, (uint8_t)(value & 0b0000000011111111));
		write_RAM_8bit(write_address + 1, (uint8_t)(value >> 8));
	}


	//====================================================
	//====================================================

	void create_savedata_file(const char* savedata_filename) {
		bool file_exist_flag = false;
		if (PathFileExists(savedata_filename) == TRUE) {//ファイルが存在した場合
			file_exist_flag = true;
		}

		if (fopen_s(&savedata_fp, savedata_filename, "ab+") != 0) {
			goto create_gamedata_error;
		}

		if (file_exist_flag == false) {//セーブデータファイルを新規作成した場合
			save_gamedata();//初期の状態をセーブする
		}

		fseek(savedata_fp, 0, SEEK_SET);//ファイルのカーソルを先頭に持ってくる

		return;

	create_gamedata_error:
		savedata_fp = nullptr;

		MessageBox(NULL, _T("セーブデータファイルの作成に失敗しました"), _T("ERROR"), MB_OK | MB_ICONERROR);

		FATAL_ERROR_FLAG = true;
	}

	/*
	ゲームのデータをファイルに保存する
	*/
	void save_gamedata() {
		if (savedata_fp == nullptr) {
			return;
		}

		if (cart_mbc_type != CART_MBC_TYPE::MBC2 && cart_mbc_type != CART_MBC_TYPE::MBC3 && Main::SRAM_size == 0) {//MBCが2と3でなくなおかつSRAMのサイズが0のとき
			return;
		}

		//ファイルのサイズを0にする
		int file_handle = _fileno(savedata_fp);
		if (_chsize_s(file_handle, 0) != 0) {
			goto save_gamedata_error;
		}

		fseek(savedata_fp, 0, SEEK_SET);//ファイルのカーソルを先頭に持ってくる

		size_t tmp_write_size;
		if (cart_mbc_type == CART_MBC_TYPE::ROM ||
			cart_mbc_type == CART_MBC_TYPE::MBC1 ||
			cart_mbc_type == CART_MBC_TYPE::MBC5 ||
			cart_mbc_type == CART_MBC_TYPE::HuC1)
		{
			tmp_write_size = 0x2000;
			if (fwrite(&(gbx_ram.RAM[0xA000]), 1, tmp_write_size, savedata_fp) != tmp_write_size) {
				goto save_gamedata_error;
			}
			if (Main::SRAM_size > 8) {
				tmp_write_size = ((Main::SRAM_size - 8) * 1024);
				if (fwrite(SRAM_bank_data_ptr, 1, tmp_write_size, savedata_fp) != tmp_write_size) {
					goto save_gamedata_error;
				}
			}
		}
		else if (cart_mbc_type == CART_MBC_TYPE::MBC2) {
			tmp_write_size = 0x200;
			if (fwrite(&(gbx_ram.RAM[0xA000]), 1, tmp_write_size, savedata_fp) != tmp_write_size) {
				goto save_gamedata_error;
			}
		}
		else if (cart_mbc_type == CART_MBC_TYPE::MBC3) {
			if (Main::SRAM_size != 0) {//SRAMのサイズが0でないとき
				tmp_write_size = 0x2000;
				if (fwrite(&(gbx_ram.RAM[0xA000]), 1, tmp_write_size, savedata_fp) != tmp_write_size) {
					goto save_gamedata_error;
				}
				if (Main::SRAM_size > 8) {
					tmp_write_size = ((Main::SRAM_size - 8) * 1024);
					if (fwrite(SRAM_bank_data_ptr, 1, tmp_write_size, savedata_fp) != tmp_write_size) {
						goto save_gamedata_error;
					}
				}
			}

			/*
			offset  size    desc
			0       4       time seconds
			4       4       time minutes
			8       4       time hours
			12      4       time days
			16      4       time days high
			20      4       latched time seconds
			24      4       latched time minutes
			28      4       latched time hours
			32      4       latched time days
			36      4       latched time days high
			40      8       unix timestamp when saving (64 bits little endian)
			*/
			//RTCレジスタの保存をする
			uint8_t RTC_savedata_data[RTC_SAVEDATA_SIZE] = { 0x00 };
			RTC_savedata_data[0] = RTC_data[RTC_OFFSET_SECONDS];
			RTC_savedata_data[4] = RTC_data[RTC_OFFSET_MINUTES];
			RTC_savedata_data[8] = RTC_data[RTC_OFFSET_HOURS];
			RTC_savedata_data[12] = RTC_data[RTC_OFFSET_DAY_LOW];
			RTC_savedata_data[16] = RTC_data[RTC_OFFSET_DAY_HIGH];
			RTC_savedata_data[20] = latched_RTC_data[RTC_OFFSET_SECONDS];
			RTC_savedata_data[24] = latched_RTC_data[RTC_OFFSET_MINUTES];
			RTC_savedata_data[28] = latched_RTC_data[RTC_OFFSET_HOURS];
			RTC_savedata_data[32] = latched_RTC_data[RTC_OFFSET_DAY_LOW];
			RTC_savedata_data[36] = latched_RTC_data[RTC_OFFSET_DAY_HIGH];
			time_t now_time;
			time(&now_time);
			memcpy((void*)(RTC_savedata_data + 40), &now_time, sizeof(time_t));
			if (fwrite(RTC_savedata_data, 1, RTC_SAVEDATA_SIZE, savedata_fp) != RTC_SAVEDATA_SIZE) {
				goto save_gamedata_error;
			}
			M_debug_printf("*********************************************\n");
			M_debug_printf("<RTC SAVE> now_time = %lld\n", now_time);
			M_debug_printf("*********************************************\n");
		}
		//else if (cart_mbc_type == CART_MBC_TYPE::OTHER) {
		else {//OTHER
			//このエミュレータでは対応していないので何もしない
		}

		return;

	save_gamedata_error:
		MessageBox(NULL, _T("セーブデータの保存に失敗しました"), _T("ERROR"), MB_OK | MB_ICONERROR);

		FATAL_ERROR_FLAG = true;
	}

	void load_gamedata() {
		if (savedata_fp == nullptr) {
			return;
		}

		if (cart_mbc_type != CART_MBC_TYPE::MBC2 && cart_mbc_type != CART_MBC_TYPE::MBC3 && Main::SRAM_size == 0) {//MBCが2と3でなくなおかつSRAMのサイズが0のとき
			return;
		}

		fseek(savedata_fp, 0, SEEK_SET);//ファイルのカーソルを先頭に持ってくる

		size_t tmp_read_size;

		if (cart_mbc_type == CART_MBC_TYPE::ROM ||
			cart_mbc_type == CART_MBC_TYPE::MBC1 ||
			cart_mbc_type == CART_MBC_TYPE::MBC5 ||
			cart_mbc_type == CART_MBC_TYPE::HuC1)
		{
			tmp_read_size = 0x2000;
			if (fread(&(gbx_ram.RAM[0xA000]), 1, tmp_read_size, savedata_fp) != tmp_read_size) {
				goto load_gamedata_error;
			}
			if (Main::SRAM_size > 8) {
				tmp_read_size = ((Main::SRAM_size - 8) * 1024);
				if (fread(SRAM_bank_data_ptr, 1, tmp_read_size, savedata_fp) != tmp_read_size) {
					goto load_gamedata_error;
				}
			}
		}
		else if (cart_mbc_type == CART_MBC_TYPE::MBC2) {
			tmp_read_size = 0x200;
			if (fread(&(gbx_ram.RAM[0xA000]), 1, tmp_read_size, savedata_fp) != tmp_read_size) {
				goto load_gamedata_error;
			}
		}
		else if (cart_mbc_type == CART_MBC_TYPE::MBC3) {
			if (Main::SRAM_size != 0) {//SRAMのサイズが0でないとき
				tmp_read_size = 0x2000;
				if (fread(&(gbx_ram.RAM[0xA000]), 1, tmp_read_size, savedata_fp) != tmp_read_size) {
					goto load_gamedata_error;
				}
				if (Main::SRAM_size > 8) {
					tmp_read_size = ((Main::SRAM_size - 8) * 1024);
					if (fread(SRAM_bank_data_ptr, 1, tmp_read_size, savedata_fp) != tmp_read_size) {
						goto load_gamedata_error;
					}
				}
			}

			/*
			offset  size    desc
			0       4       time seconds
			4       4       time minutes
			8       4       time hours
			12      4       time days
			16      4       time days high
			20      4       latched time seconds
			24      4       latched time minutes
			28      4       latched time hours
			32      4       latched time days
			36      4       latched time days high
			40      8       unix timestamp when saving (64 bits little endian)
			*/
			//RTCレジスタの読み込みをする
			uint8_t RTC_savedata_data[RTC_SAVEDATA_SIZE] = { 0x00 };

			if (fread(RTC_savedata_data, 1, RTC_SAVEDATA_SIZE, savedata_fp) != RTC_SAVEDATA_SIZE) {
				goto load_gamedata_error;
			}

			RTC_data[RTC_OFFSET_SECONDS] = RTC_savedata_data[0];
			RTC_data[RTC_OFFSET_MINUTES] = RTC_savedata_data[4];
			RTC_data[RTC_OFFSET_HOURS] = RTC_savedata_data[8];
			RTC_data[RTC_OFFSET_DAY_LOW] = RTC_savedata_data[12];
			RTC_data[RTC_OFFSET_DAY_HIGH] = RTC_savedata_data[16];

			latched_RTC_data[RTC_OFFSET_SECONDS] = RTC_savedata_data[20];
			latched_RTC_data[RTC_OFFSET_MINUTES] = RTC_savedata_data[24];
			latched_RTC_data[RTC_OFFSET_HOURS] = RTC_savedata_data[28];
			latched_RTC_data[RTC_OFFSET_DAY_LOW] = RTC_savedata_data[32];
			latched_RTC_data[RTC_OFFSET_DAY_HIGH] = RTC_savedata_data[36];

			time_t saved_now_time = *((time_t*)(RTC_savedata_data + 40));

			time_t now_time;
			time(&now_time);

			time_t time_elapsed__sec = now_time - saved_now_time;
			if (time_elapsed__sec > 0) {
				RTC_time_lapse__Nsec(time_elapsed__sec);
			}

			M_debug_printf("*********************************************\n");
			M_debug_printf("<RTC LOAD> now_time = %lld\n", now_time);
			M_debug_printf("<RTC LOAD> saved_now_time = %lld\n", saved_now_time);
			M_debug_printf("<RTC LOAD> time_elapsed__sec = %lld\n", time_elapsed__sec);
			M_debug_printf("\n");
			M_debug_printf("<RTC LOAD> RTC_data[RTC_OFFSET_DAY_LOW] = 0x%02X\n", RTC_data[RTC_OFFSET_DAY_LOW]);
			M_debug_printf("<RTC LOAD> RTC_data[RTC_OFFSET_DAY_HIGH] = 0x%02X\n", RTC_data[RTC_OFFSET_DAY_HIGH]);
			M_debug_printf("<RTC LOAD> RTC_data[RTC_OFFSET_DAY_HIGH] #MSB = %s\n", ((RTC_data[RTC_OFFSET_DAY_HIGH] & 0b10000000) != 0) ? "true" : "false");
			M_debug_printf("\n");
			M_debug_printf("<RTC LOAD> latched_RTC_data[RTC_OFFSET_DAY_LOW] = 0x%02X\n", latched_RTC_data[RTC_OFFSET_DAY_LOW]);
			M_debug_printf("<RTC LOAD> latched_RTC_data[RTC_OFFSET_DAY_HIGH] = 0x%02X\n", latched_RTC_data[RTC_OFFSET_DAY_HIGH]);
			M_debug_printf("<RTC LOAD> latched_RTC_data[RTC_OFFSET_DAY_HIGH] #MSB = %s\n", ((latched_RTC_data[RTC_OFFSET_DAY_HIGH] & 0b10000000) != 0) ? "true" : "false");
			M_debug_printf("*********************************************\n");
		}
		//else if (cart_mbc_type == CART_MBC_TYPE::OTHER) {
		else {//OTHER
			//このエミュレータでは対応していないので何もしない
		}

		return;

	load_gamedata_error:
		MessageBox(NULL, _T("セーブデータの読み込みに失敗しました"), _T("ERROR"), MB_OK | MB_ICONERROR);

		FATAL_ERROR_FLAG = true;
	}

	void close_savedata_file() {
		if (savedata_fp == nullptr) {
			return;
		}

		fclose(savedata_fp);
	}

	//====================================================
	//====================================================

	void push_8bit(uint8_t value) {
		gbx_register.SP--;
		//gbx_ram.RAM[gbx_register.SP] = value;
		write_RAM_8bit(gbx_register.SP, value);
	}

	void push_16bit(uint16_t value) {
		gbx_register.SP--;
		//gbx_ram.RAM[gbx_register.SP] = (uint8_t)(value >> 8);
		write_RAM_8bit(gbx_register.SP, (uint8_t)(value >> 8));
		gbx_register.SP--;
		//gbx_ram.RAM[gbx_register.SP] = (uint8_t)(value & 0b11111111);
		write_RAM_8bit(gbx_register.SP, (uint8_t)(value & 0b11111111));
	}

	uint8_t pop_8bit() {
		//uint8_t value = gbx_ram.RAM[gbx_register.SP];
		uint8_t value = read_RAM_8bit(gbx_register.SP);
		gbx_register.SP++;

		return value;
	}

	uint16_t pop_16bit() {
		//uint8_t low_value = gbx_ram.RAM[gbx_register.SP];
		uint8_t low_value = read_RAM_8bit(gbx_register.SP);
		gbx_register.SP++;
		//uint8_t high_value = gbx_ram.RAM[gbx_register.SP];
		uint8_t high_value = read_RAM_8bit(gbx_register.SP);
		gbx_register.SP++;

		return (uint16_t)(low_value | (high_value << 8));
	}

	//=============================================================================
	//=============================================================================
	
	void calc_Z_Flag__8bit(uint8_t result) {
		if (result == 0) {
			gbx_register.F_Z = 1;
		}
		else {
			gbx_register.F_Z = 0;
		}
	}

	//op1 + op2
	void calc_H_Flag__8bit_add(uint8_t op1, uint8_t op2) {
		if ((op1 & 0b00001111) + (op2 & 0b00001111) > 0b00001111) {
			gbx_register.F_H = 1;
		}
		else {
			gbx_register.F_H = 0;
		}
	}

	//op1 - op2
	void calc_H_Flag__8bit_sub(uint8_t op1, uint8_t op2) {
		//uint8_t result = op1 - op2;
		//if ((((uint8_t)(op1 ^ op2 ^ result)) & 0b00010000) != 0) {
		if ((op1 & 0b00001111) < (op2 & 0b00001111)) {
			gbx_register.F_H = 1;
		}
		else {
			gbx_register.F_H = 0;
		}
	}

	//op1 + op2 + (CFLAG)
	void calc_H_Flag__8bit_adc(uint8_t op1, uint8_t op2, uint8_t C_Flag) {
		if ((op1 & 0b00001111) + (op2 & 0b00001111) + C_Flag > 0b00001111) {
			gbx_register.F_H = 1;
		}
		else {
			gbx_register.F_H = 0;
		}
	}

	//op1 - (op2 + (CFLAG))
	void calc_H_Flag__8bit_sbc(uint8_t op1, uint8_t op2, uint8_t C_Flag) {
		if ((((op1 & 0b00001111) - ((op2 & 0b00001111) + C_Flag)) & 0b10000) != 0) {
			gbx_register.F_H = 1;
		}
		else {
			gbx_register.F_H = 0;
		}
	}

	//op1 + op2
	void calc_C_Flag__8bit_add(uint8_t op1, uint8_t op2) {
		if ((uint16_t)op1 + (uint16_t)op2 > 0b11111111) {
			gbx_register.F_C = 1;
		}
		else {
			gbx_register.F_C = 0;
		}
	}

	//op1 - op2
	void calc_C_Flag__8bit_sub(uint8_t op1, uint8_t op2) {
		if (op2 > op1) {
			gbx_register.F_C = 1;
		}
		else {
			gbx_register.F_C = 0;
		}
	}

	//op1 + op2 + (C_FLAG)
	void calc_C_Flag__8bit_adc(uint8_t op1, uint8_t op2, uint8_t C_Flag) {
		if ((uint16_t)op1 + (uint16_t)op2 + (uint16_t)C_Flag > 0b11111111) {
			gbx_register.F_C = 1;
		}
		else {
			gbx_register.F_C = 0;
		}
	}

	//op1 - (op2 + (C_FLAG))
	void calc_C_Flag__8bit_sbc(uint8_t op1, uint8_t op2, uint8_t C_Flag) {
		if ((((uint16_t)op1 - (uint16_t)((uint16_t)op2 + (uint16_t)C_Flag)) & 0b100000000) != 0) {
			gbx_register.F_C = 1;
		}
		else {
			gbx_register.F_C = 0;
		}
	}

	//op1 + op2 (16bit計算)
	void calc_H_Flag__16bit_add(uint16_t op1, uint16_t op2) {
		if ((op1 & 0b0000111111111111) + (op2 & 0b0000111111111111) > 0b0000111111111111) {
			gbx_register.F_H = 1;
		}
		else {
			gbx_register.F_H = 0;
		}
	}

	//op1 + op2 (16bit計算)
	void calc_C_Flag__16bit_add(uint16_t op1, uint16_t op2) {
		if ((uint32_t)op1 + (uint32_t)op2 > 0b1111111111111111) {
			gbx_register.F_C = 1;
		}
		else {
			gbx_register.F_C = 0;
		}
	}

	//=============================================================================


	//未定義のCPUの命令
	void cpu_fnc__GARBAGE() {
		//M_debug_printf("ERROR cpu_fnc__GARBAGE()...\n");
		//MessageBox(NULL, _T("未定義の命令を実行しました"), _T("情報"), MB_ICONERROR);
	}


	//=============================================================================
	
	//void cpu_fnc__ADC_A_r8() {
	//}

	//0x8F
	void cpu_fnc__ADC_A_A() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		calc_H_Flag__8bit_adc(gbx_register.A, gbx_register.A, bef_C_Flag);
		calc_C_Flag__8bit_adc(gbx_register.A, gbx_register.A, bef_C_Flag);

		gbx_register.A += gbx_register.A;
		gbx_register.A += bef_C_Flag;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0x88
	void cpu_fnc__ADC_A_B() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		calc_H_Flag__8bit_adc(gbx_register.A, gbx_register.B, bef_C_Flag);
		calc_C_Flag__8bit_adc(gbx_register.A, gbx_register.B, bef_C_Flag);

		gbx_register.A += gbx_register.B;
		gbx_register.A += bef_C_Flag;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0x89
	void cpu_fnc__ADC_A_C() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		calc_H_Flag__8bit_adc(gbx_register.A, gbx_register.C, bef_C_Flag);
		calc_C_Flag__8bit_adc(gbx_register.A, gbx_register.C, bef_C_Flag);

		gbx_register.A += gbx_register.C;
		gbx_register.A += bef_C_Flag;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0x8A
	void cpu_fnc__ADC_A_D() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		calc_H_Flag__8bit_adc(gbx_register.A, gbx_register.D, bef_C_Flag);
		calc_C_Flag__8bit_adc(gbx_register.A, gbx_register.D, bef_C_Flag);

		gbx_register.A += gbx_register.D;
		gbx_register.A += bef_C_Flag;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0x8B
	void cpu_fnc__ADC_A_E() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		calc_H_Flag__8bit_adc(gbx_register.A, gbx_register.E, bef_C_Flag);
		calc_C_Flag__8bit_adc(gbx_register.A, gbx_register.E, bef_C_Flag);

		gbx_register.A += gbx_register.E;
		gbx_register.A += bef_C_Flag;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0x8C
	void cpu_fnc__ADC_A_H() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		calc_H_Flag__8bit_adc(gbx_register.A, gbx_register.H, bef_C_Flag);
		calc_C_Flag__8bit_adc(gbx_register.A, gbx_register.H, bef_C_Flag);

		gbx_register.A += gbx_register.H;
		gbx_register.A += bef_C_Flag;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0x8D
	void cpu_fnc__ADC_A_L() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		calc_H_Flag__8bit_adc(gbx_register.A, gbx_register.L, bef_C_Flag);
		calc_C_Flag__8bit_adc(gbx_register.A, gbx_register.L, bef_C_Flag);

		gbx_register.A += gbx_register.L;
		gbx_register.A += bef_C_Flag;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0x8E
	void cpu_fnc__ADC_A_addrHL() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		uint8_t value_8bit = read_RAM_8bit(gbx_register.HL);

		calc_H_Flag__8bit_adc(gbx_register.A, value_8bit, bef_C_Flag);
		calc_C_Flag__8bit_adc(gbx_register.A, value_8bit, bef_C_Flag);

		gbx_register.A += value_8bit;
		gbx_register.A += bef_C_Flag;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0xCE
	void cpu_fnc__ADC_A_n8() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		uint8_t value_8bit = read_RAM_8bit(gbx_register.PC);

		calc_H_Flag__8bit_adc(gbx_register.A, value_8bit, bef_C_Flag);
		calc_C_Flag__8bit_adc(gbx_register.A, value_8bit, bef_C_Flag);

		//if ((((gbx_register.A & 0b1111) + (value_8bit & 0b1111) + bef_C_Flag) & 0b10000) != 0) {
		//	gbx_register.F_H = 1;
		//}
		//else {
		//	gbx_register.F_H = 0;
		//}

		//if (((gbx_register.A + value_8bit + bef_C_Flag) & 0b100000000) != 0) {
		//	gbx_register.F_C = 1;
		//}
		//else {
		//	gbx_register.F_C = 0;
		//}


		gbx_register.A += value_8bit;
		gbx_register.A += bef_C_Flag;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;

		gbx_register.PC++;
	}

	//void cpu_fnc__ADD_A_r8() {
	//}

	//0x87
	void cpu_fnc__ADD_A_A() {
		calc_H_Flag__8bit_add(gbx_register.A, gbx_register.A);
		calc_C_Flag__8bit_add(gbx_register.A, gbx_register.A);

		gbx_register.A += gbx_register.A;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0x80
	void cpu_fnc__ADD_A_B() {
		calc_H_Flag__8bit_add(gbx_register.A, gbx_register.B);
		calc_C_Flag__8bit_add(gbx_register.A, gbx_register.B);

		gbx_register.A += gbx_register.B;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0x81
	void cpu_fnc__ADD_A_C() {
		calc_H_Flag__8bit_add(gbx_register.A, gbx_register.C);
		calc_C_Flag__8bit_add(gbx_register.A, gbx_register.C);

		gbx_register.A += gbx_register.C;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0x82
	void cpu_fnc__ADD_A_D() {
		calc_H_Flag__8bit_add(gbx_register.A, gbx_register.D);
		calc_C_Flag__8bit_add(gbx_register.A, gbx_register.D);

		gbx_register.A += gbx_register.D;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0x83
	void cpu_fnc__ADD_A_E() {
		calc_H_Flag__8bit_add(gbx_register.A, gbx_register.E);
		calc_C_Flag__8bit_add(gbx_register.A, gbx_register.E);

		gbx_register.A += gbx_register.E;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0x84
	void cpu_fnc__ADD_A_H() {
		calc_H_Flag__8bit_add(gbx_register.A, gbx_register.H);
		calc_C_Flag__8bit_add(gbx_register.A, gbx_register.H);

		gbx_register.A += gbx_register.H;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0x85
	void cpu_fnc__ADD_A_L() {
		calc_H_Flag__8bit_add(gbx_register.A, gbx_register.L);
		calc_C_Flag__8bit_add(gbx_register.A, gbx_register.L);

		gbx_register.A += gbx_register.L;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0x86
	void cpu_fnc__ADD_A_addrHL() {
		uint8_t value_8bit = read_RAM_8bit(gbx_register.HL);

		calc_H_Flag__8bit_add(gbx_register.A, value_8bit);
		calc_C_Flag__8bit_add(gbx_register.A, value_8bit);

		gbx_register.A += value_8bit;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0xC6
	void cpu_fnc__ADD_A_n8() {
		uint8_t value_8bit = read_RAM_8bit(gbx_register.PC);

		calc_H_Flag__8bit_add(gbx_register.A, value_8bit);
		calc_C_Flag__8bit_add(gbx_register.A, value_8bit);

		gbx_register.A += value_8bit;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;

		gbx_register.PC++;
	}

	//void cpu_fnc__AND_A_r8() {
	//}

	//0xA7
	void cpu_fnc__AND_A_A() {
		gbx_register.A &= gbx_register.A;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 1;
		gbx_register.F_C = 0;
	}

	//0xA0
	void cpu_fnc__AND_A_B() {
		gbx_register.A &= gbx_register.B;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 1;
		gbx_register.F_C = 0;
	}

	//0xA1
	void cpu_fnc__AND_A_C() {
		gbx_register.A &= gbx_register.C;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 1;
		gbx_register.F_C = 0;
	}

	//0xA2
	void cpu_fnc__AND_A_D() {
		gbx_register.A &= gbx_register.D;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 1;
		gbx_register.F_C = 0;
	}

	//0xA3
	void cpu_fnc__AND_A_E() {
		gbx_register.A &= gbx_register.E;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 1;
		gbx_register.F_C = 0;
	}

	//0xA4
	void cpu_fnc__AND_A_H() {
		gbx_register.A &= gbx_register.H;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 1;
		gbx_register.F_C = 0;
	}

	//0xA5
	void cpu_fnc__AND_A_L() {
		gbx_register.A &= gbx_register.L;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 1;
		gbx_register.F_C = 0;
	}

	//0xA6
	void cpu_fnc__AND_A_addrHL() {
		uint8_t value_8bit = read_RAM_8bit(gbx_register.HL);

		gbx_register.A &= value_8bit;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 1;
		gbx_register.F_C = 0;
	}

	//0xE6
	void cpu_fnc__AND_A_n8() {
		uint8_t value_8bit = read_RAM_8bit(gbx_register.PC);

		gbx_register.A &= value_8bit;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 1;
		gbx_register.F_C = 0;

		gbx_register.PC++;

		//M_debug_printf("AND A, 0x%02x\n", value_8bit);
	}

	//void cpu_fnc__CP_A_r8() {
	//}

	//0xBF
	void cpu_fnc__CP_A_A() {
		uint8_t tmp_8bit = gbx_register.A;

		calc_C_Flag__8bit_sub(tmp_8bit, gbx_register.A);
		calc_H_Flag__8bit_sub(tmp_8bit, gbx_register.A);

		tmp_8bit -= gbx_register.A;

		calc_Z_Flag__8bit(tmp_8bit);
		gbx_register.F_N = 1;
	}

	//0xB8
	void cpu_fnc__CP_A_B() {
		uint8_t tmp_8bit = gbx_register.A;

		calc_C_Flag__8bit_sub(tmp_8bit, gbx_register.B);
		calc_H_Flag__8bit_sub(tmp_8bit, gbx_register.B);

		tmp_8bit -= gbx_register.B;

		calc_Z_Flag__8bit(tmp_8bit);
		gbx_register.F_N = 1;
	}

	//0xB9
	void cpu_fnc__CP_A_C() {
		uint8_t tmp_8bit = gbx_register.A;

		calc_C_Flag__8bit_sub(tmp_8bit, gbx_register.C);
		calc_H_Flag__8bit_sub(tmp_8bit, gbx_register.C);

		tmp_8bit -= gbx_register.C;

		calc_Z_Flag__8bit(tmp_8bit);
		gbx_register.F_N = 1;
	}

	//0xBA
	void cpu_fnc__CP_A_D() {
		uint8_t tmp_8bit = gbx_register.A;

		calc_C_Flag__8bit_sub(tmp_8bit, gbx_register.D);
		calc_H_Flag__8bit_sub(tmp_8bit, gbx_register.D);

		tmp_8bit -= gbx_register.D;

		calc_Z_Flag__8bit(tmp_8bit);
		gbx_register.F_N = 1;
	}

	//0xBB
	void cpu_fnc__CP_A_E() {
		uint8_t tmp_8bit = gbx_register.A;

		calc_C_Flag__8bit_sub(tmp_8bit, gbx_register.E);
		calc_H_Flag__8bit_sub(tmp_8bit, gbx_register.E);

		tmp_8bit -= gbx_register.E;

		calc_Z_Flag__8bit(tmp_8bit);
		gbx_register.F_N = 1;
	}

	//0xBC
	void cpu_fnc__CP_A_H() {
		uint8_t tmp_8bit = gbx_register.A;

		calc_C_Flag__8bit_sub(tmp_8bit, gbx_register.H);
		calc_H_Flag__8bit_sub(tmp_8bit, gbx_register.H);

		tmp_8bit -= gbx_register.H;

		calc_Z_Flag__8bit(tmp_8bit);
		gbx_register.F_N = 1;
	}

	//0xBD
	void cpu_fnc__CP_A_L() {
		uint8_t tmp_8bit = gbx_register.A;

		calc_C_Flag__8bit_sub(tmp_8bit, gbx_register.L);
		calc_H_Flag__8bit_sub(tmp_8bit, gbx_register.L);

		tmp_8bit -= gbx_register.L;

		calc_Z_Flag__8bit(tmp_8bit);
		gbx_register.F_N = 1;
	}

	//0xBE
	void cpu_fnc__CP_A_addrHL() {
		uint8_t tmp_8bit = gbx_register.A;
		uint8_t value_1_8bit = read_RAM_8bit(gbx_register.HL);

		calc_C_Flag__8bit_sub(tmp_8bit, value_1_8bit);
		calc_H_Flag__8bit_sub(tmp_8bit, value_1_8bit);

		tmp_8bit -= value_1_8bit;

		calc_Z_Flag__8bit(tmp_8bit);
		gbx_register.F_N = 1;
	}

	//0xFE
	void cpu_fnc__CP_A_n8() {
		uint8_t tmp_8bit = gbx_register.A;
		uint8_t value_1_8bit = read_RAM_8bit(gbx_register.PC);

		calc_C_Flag__8bit_sub(tmp_8bit, value_1_8bit);
		calc_H_Flag__8bit_sub(tmp_8bit, value_1_8bit);

		tmp_8bit -= value_1_8bit;

		calc_Z_Flag__8bit(tmp_8bit);
		gbx_register.F_N = 1;

		gbx_register.PC++;

	}

	//void cpu_fnc__DEC_r8() {
	//}

	//0x3D
	void cpu_fnc__DEC_A() {
		calc_H_Flag__8bit_sub(gbx_register.A, 1);

		gbx_register.A--;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 1;
	}

	//0x05
	void cpu_fnc__DEC_B() {
		calc_H_Flag__8bit_sub(gbx_register.B, 1);

		gbx_register.B--;

		calc_Z_Flag__8bit(gbx_register.B);
		gbx_register.F_N = 1;
	}

	//0x0D
	void cpu_fnc__DEC_C() {
		calc_H_Flag__8bit_sub(gbx_register.C, 1);

		gbx_register.C--;

		calc_Z_Flag__8bit(gbx_register.C);
		gbx_register.F_N = 1;
	}

	//0x15
	void cpu_fnc__DEC_D() {
		calc_H_Flag__8bit_sub(gbx_register.D, 1);

		gbx_register.D--;

		calc_Z_Flag__8bit(gbx_register.D);
		gbx_register.F_N = 1;
	}

	//0x1D
	void cpu_fnc__DEC_E() {
		calc_H_Flag__8bit_sub(gbx_register.E, 1);

		gbx_register.E--;

		calc_Z_Flag__8bit(gbx_register.E);
		gbx_register.F_N = 1;
	}

	//0x25
	void cpu_fnc__DEC_H() {
		calc_H_Flag__8bit_sub(gbx_register.H, 1);

		gbx_register.H--;

		calc_Z_Flag__8bit(gbx_register.H);
		gbx_register.F_N = 1;
	}

	//0x2D
	void cpu_fnc__DEC_L() {
		calc_H_Flag__8bit_sub(gbx_register.L, 1);

		gbx_register.L--;

		calc_Z_Flag__8bit(gbx_register.L);
		gbx_register.F_N = 1;
	}

	//0x35
	void cpu_fnc__DEC_addrHL() {
		uint8_t value_1_8bit = read_RAM_8bit(gbx_register.HL);

		calc_H_Flag__8bit_sub(value_1_8bit, 1);

		value_1_8bit--;

		calc_Z_Flag__8bit(value_1_8bit);

		write_RAM_8bit(gbx_register.HL, value_1_8bit);

		gbx_register.F_N = 1;
	}

	//void cpu_fnc__INC_r8() {
	//}

	//0x3C
	void cpu_fnc__INC_A() {
		calc_H_Flag__8bit_add(gbx_register.A, 1);

		gbx_register.A++;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
	}

	//0x04
	void cpu_fnc__INC_B() {
		calc_H_Flag__8bit_add(gbx_register.B, 1);

		gbx_register.B++;

		calc_Z_Flag__8bit(gbx_register.B);
		gbx_register.F_N = 0;
	}

	//0x0C
	void cpu_fnc__INC_C() {
		calc_H_Flag__8bit_add(gbx_register.C, 1);

		gbx_register.C++;

		calc_Z_Flag__8bit(gbx_register.C);
		gbx_register.F_N = 0;
	}

	//0x14
	void cpu_fnc__INC_D() {
		calc_H_Flag__8bit_add(gbx_register.D, 1);

		gbx_register.D++;

		calc_Z_Flag__8bit(gbx_register.D);
		gbx_register.F_N = 0;
	}

	//0x1C
	void cpu_fnc__INC_E() {
		calc_H_Flag__8bit_add(gbx_register.E, 1);

		gbx_register.E++;

		calc_Z_Flag__8bit(gbx_register.E);
		gbx_register.F_N = 0;
	}

	//0x24
	void cpu_fnc__INC_H() {
		calc_H_Flag__8bit_add(gbx_register.H, 1);

		gbx_register.H++;

		calc_Z_Flag__8bit(gbx_register.H);
		gbx_register.F_N = 0;
	}

	//0x2C
	void cpu_fnc__INC_L() {
		calc_H_Flag__8bit_add(gbx_register.L, 1);

		gbx_register.L++;

		calc_Z_Flag__8bit(gbx_register.L);
		gbx_register.F_N = 0;
	}

	//0x34
	void cpu_fnc__INC_addrHL() {
		uint8_t value_1_8bit = read_RAM_8bit(gbx_register.HL);

		calc_H_Flag__8bit_add(value_1_8bit, 1);

		value_1_8bit++;

		calc_Z_Flag__8bit(value_1_8bit);

		write_RAM_8bit(gbx_register.HL, value_1_8bit);

		gbx_register.F_N = 0;
	}

	//void cpu_fnc__OR_A_r8() {
	//}

	//0xB7
	void cpu_fnc__OR_A_A() {
		gbx_register.A |= gbx_register.A;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xB0
	void cpu_fnc__OR_A_B() {
		gbx_register.A |= gbx_register.B;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xB1
	void cpu_fnc__OR_A_C() {
		gbx_register.A |= gbx_register.C;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xB2
	void cpu_fnc__OR_A_D() {
		gbx_register.A |= gbx_register.D;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xB3
	void cpu_fnc__OR_A_E() {
		gbx_register.A |= gbx_register.E;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xB4
	void cpu_fnc__OR_A_H() {
		gbx_register.A |= gbx_register.H;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xB5
	void cpu_fnc__OR_A_L() {
		gbx_register.A |= gbx_register.L;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xB6
	void cpu_fnc__OR_A_addrHL() {
		uint8_t value_1_8bit = read_RAM_8bit(gbx_register.HL);

		gbx_register.A |= value_1_8bit;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xF6
	void cpu_fnc__OR_A_n8() {
		uint8_t value_1_8bit = read_RAM_8bit(gbx_register.PC);

		gbx_register.A |= value_1_8bit;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;

		gbx_register.PC++;
	}

	//void cpu_fnc__SBC_A_r8() {
	//}

	//0x9F
	void cpu_fnc__SBC_A_A() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		calc_H_Flag__8bit_sbc(gbx_register.A, gbx_register.A, bef_C_Flag);
		calc_C_Flag__8bit_sbc(gbx_register.A, gbx_register.A, bef_C_Flag);

		gbx_register.A -= (gbx_register.A + bef_C_Flag);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0x98
	void cpu_fnc__SBC_A_B() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		calc_H_Flag__8bit_sbc(gbx_register.A, gbx_register.B, bef_C_Flag);
		calc_C_Flag__8bit_sbc(gbx_register.A, gbx_register.B, bef_C_Flag);

		gbx_register.A -= (gbx_register.B + bef_C_Flag);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0x99
	void cpu_fnc__SBC_A_C() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		calc_H_Flag__8bit_sbc(gbx_register.A, gbx_register.C, bef_C_Flag);
		calc_C_Flag__8bit_sbc(gbx_register.A, gbx_register.C, bef_C_Flag);

		gbx_register.A -= (gbx_register.C + bef_C_Flag);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0x9A
	void cpu_fnc__SBC_A_D() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		calc_H_Flag__8bit_sbc(gbx_register.A, gbx_register.D, bef_C_Flag);
		calc_C_Flag__8bit_sbc(gbx_register.A, gbx_register.D, bef_C_Flag);

		gbx_register.A -= (gbx_register.D + bef_C_Flag);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0x9B
	void cpu_fnc__SBC_A_E() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		calc_H_Flag__8bit_sbc(gbx_register.A, gbx_register.E, bef_C_Flag);
		calc_C_Flag__8bit_sbc(gbx_register.A, gbx_register.E, bef_C_Flag);

		gbx_register.A -= (gbx_register.E + bef_C_Flag);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0x9C
	void cpu_fnc__SBC_A_H() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		calc_H_Flag__8bit_sbc(gbx_register.A, gbx_register.H, bef_C_Flag);
		calc_C_Flag__8bit_sbc(gbx_register.A, gbx_register.H, bef_C_Flag);

		gbx_register.A -= (gbx_register.H + bef_C_Flag);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0x9D
	void cpu_fnc__SBC_A_L() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		calc_H_Flag__8bit_sbc(gbx_register.A, gbx_register.L, bef_C_Flag);
		calc_C_Flag__8bit_sbc(gbx_register.A, gbx_register.L, bef_C_Flag);

		gbx_register.A -= (gbx_register.L + bef_C_Flag);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0x9E
	void cpu_fnc__SBC_A_addrHL() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		uint8_t value_1_8bit = read_RAM_8bit(gbx_register.HL);

		calc_H_Flag__8bit_sbc(gbx_register.A, value_1_8bit, bef_C_Flag);
		calc_C_Flag__8bit_sbc(gbx_register.A, value_1_8bit, bef_C_Flag);

		gbx_register.A -= (value_1_8bit + bef_C_Flag);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0xDE
	void cpu_fnc__SBC_A_n8() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		uint8_t value_1_8bit = read_RAM_8bit(gbx_register.PC);

		calc_H_Flag__8bit_sbc(gbx_register.A, value_1_8bit, bef_C_Flag);
		calc_C_Flag__8bit_sbc(gbx_register.A, value_1_8bit, bef_C_Flag);

		gbx_register.A -= (value_1_8bit + bef_C_Flag);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;

		gbx_register.PC++;
	}

	//void cpu_fnc__SUB_A_r8() {
	//}

	//0x97
	void cpu_fnc__SUB_A_A() {
		calc_H_Flag__8bit_sub(gbx_register.A, gbx_register.A);
		calc_C_Flag__8bit_sub(gbx_register.A, gbx_register.A);

		gbx_register.A -= (gbx_register.A);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0x90
	void cpu_fnc__SUB_A_B() {
		calc_H_Flag__8bit_sub(gbx_register.A, gbx_register.B);
		calc_C_Flag__8bit_sub(gbx_register.A, gbx_register.B);

		gbx_register.A -= (gbx_register.B);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0x91
	void cpu_fnc__SUB_A_C() {
		calc_H_Flag__8bit_sub(gbx_register.A, gbx_register.C);
		calc_C_Flag__8bit_sub(gbx_register.A, gbx_register.C);

		gbx_register.A -= (gbx_register.C);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0x92
	void cpu_fnc__SUB_A_D() {
		calc_H_Flag__8bit_sub(gbx_register.A, gbx_register.D);
		calc_C_Flag__8bit_sub(gbx_register.A, gbx_register.D);

		gbx_register.A -= (gbx_register.D);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0x93
	void cpu_fnc__SUB_A_E() {
		calc_H_Flag__8bit_sub(gbx_register.A, gbx_register.E);
		calc_C_Flag__8bit_sub(gbx_register.A, gbx_register.E);

		gbx_register.A -= (gbx_register.E);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0x94
	void cpu_fnc__SUB_A_H() {
		calc_H_Flag__8bit_sub(gbx_register.A, gbx_register.H);
		calc_C_Flag__8bit_sub(gbx_register.A, gbx_register.H);

		gbx_register.A -= (gbx_register.H);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0x95
	void cpu_fnc__SUB_A_L() {
		calc_H_Flag__8bit_sub(gbx_register.A, gbx_register.L);
		calc_C_Flag__8bit_sub(gbx_register.A, gbx_register.L);

		gbx_register.A -= (gbx_register.L);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0x96
	void cpu_fnc__SUB_A_addrHL() {
		uint8_t value_1_8bit = read_RAM_8bit(gbx_register.HL);

		calc_H_Flag__8bit_sub(gbx_register.A, value_1_8bit);
		calc_C_Flag__8bit_sub(gbx_register.A, value_1_8bit);

		gbx_register.A -= (value_1_8bit);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0xD6
	void cpu_fnc__SUB_A_n8() {
		uint8_t value_1_8bit = read_RAM_8bit(gbx_register.PC);

		calc_H_Flag__8bit_sub(gbx_register.A, value_1_8bit);
		calc_C_Flag__8bit_sub(gbx_register.A, value_1_8bit);

		gbx_register.A -= (value_1_8bit);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;

		gbx_register.PC++;
	}

	//void cpu_fnc__XOR_A_r8() {
	//}

	//0xAF
	void cpu_fnc__XOR_A_A() {
		gbx_register.A ^= gbx_register.A;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xA8
	void cpu_fnc__XOR_A_B() {
		gbx_register.A ^= gbx_register.B;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xA9
	void cpu_fnc__XOR_A_C() {
		gbx_register.A ^= gbx_register.C;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xAA
	void cpu_fnc__XOR_A_D() {
		gbx_register.A ^= gbx_register.D;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xAB
	void cpu_fnc__XOR_A_E() {
		gbx_register.A ^= gbx_register.E;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xAC
	void cpu_fnc__XOR_A_H() {
		gbx_register.A ^= gbx_register.H;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xAD
	void cpu_fnc__XOR_A_L() {
		gbx_register.A ^= gbx_register.L;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xAE
	void cpu_fnc__XOR_A_addrHL() {
		uint8_t value_1_8bit = read_RAM_8bit(gbx_register.HL);

		gbx_register.A ^= value_1_8bit;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xEE
	void cpu_fnc__XOR_A_n8() {
		uint8_t value_1_8bit = read_RAM_8bit(gbx_register.PC);

		gbx_register.A ^= value_1_8bit;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;

		gbx_register.PC++;
	}

	//=============================================================================

	//void cpu_fnc__ADD_HL_r16() {
	//}

	//0x09
	void cpu_fnc__ADD_HL_BC() {
		calc_H_Flag__16bit_add(gbx_register.HL, gbx_register.BC);
		calc_C_Flag__16bit_add(gbx_register.HL, gbx_register.BC);

		gbx_register.HL += gbx_register.BC;

		gbx_register.F_N = 0;
	}

	//0x19
	void cpu_fnc__ADD_HL_DE() {
		calc_H_Flag__16bit_add(gbx_register.HL, gbx_register.DE);
		calc_C_Flag__16bit_add(gbx_register.HL, gbx_register.DE);

		gbx_register.HL += gbx_register.DE;

		gbx_register.F_N = 0;
	}

	//0x29
	void cpu_fnc__ADD_HL_HL() {
		calc_H_Flag__16bit_add(gbx_register.HL, gbx_register.HL);
		calc_C_Flag__16bit_add(gbx_register.HL, gbx_register.HL);

		gbx_register.HL += gbx_register.HL;

		gbx_register.F_N = 0;
	}

	//void cpu_fnc__INC_r16() {
	//}

	//0x03
	void cpu_fnc__INC_BC() {
		gbx_register.BC++;
	}

	//0x13
	void cpu_fnc__INC_DE() {
		gbx_register.DE++;
	}

	//0x23
	void cpu_fnc__INC_HL() {
		gbx_register.HL++;
	}

	//void cpu_fnc__DEC_r16() {
	//}

	//0x0B
	void cpu_fnc__DEC_BC() {
		gbx_register.BC--;
	}

	//0x1B
	void cpu_fnc__DEC_DE() {
		gbx_register.DE--;
	}

	//0x2B
	void cpu_fnc__DEC_HL() {
		gbx_register.HL--;
	}

	//=============================================================================

	////void cpu_fnc__BIT_u3_r8() {
	////
	////}
	////
	////void cpu_fnc__BIT_u3_addrHL() {
	////
	////}
	////
	////void cpu_fnc__SET_u3_r8() {
	////
	////}
	////
	////void cpu_fnc__SET_u3_addrHL() {
	////
	////}
	////
	////void cpu_fnc__RES_u3_r8() {
	////
	////}
	////
	////void cpu_fnc__RES_u3_addrHL() {
	////
	////}
	////
	////void cpu_fnc__SWAP_r8() {
	////
	////}
	////
	////void cpu_fnc__SWAP_addrHL() {
	////
	////}

	//=============================================================================

	////void cpu_fnc__RL_r8() {
	////
	////}
	////
	////void cpu_fnc__RL_addrHL() {
	////
	////}


	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	//0x17
	void cpu_fnc__RLA() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		if ((gbx_register.A & 0b10000000) != 0) {
			gbx_register.F_C = 1;
		}
		else {
			gbx_register.F_C = 0;
		}

		gbx_register.A <<= 1;
		gbx_register.A |= bef_C_Flag;

		gbx_register.F_Z = 0;
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
	}

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


	////void cpu_fnc__RLC_r8() {
	////
	////}
	////
	////void cpu_fnc__RLC_addrHL() {
	////
	////}


	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	//0x07
	void cpu_fnc__RLCA() {
		if ((gbx_register.A & 0b10000000) != 0) {
			gbx_register.F_C = 1;
		}
		else {
			gbx_register.F_C = 0;
		}

		gbx_register.A <<= 1;
		gbx_register.A |= gbx_register.F_C;

		gbx_register.F_Z = 0;
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
	}

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


	////void cpu_fnc__RR_r8() {
	////
	////}
	////
	////void cpu_fnc__RR_addrHL() {
	////
	////}


	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	//0x1F
	void cpu_fnc__RRA() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		if ((gbx_register.A & 0b00000001) != 0) {
			gbx_register.F_C = 1;
		}
		else {
			gbx_register.F_C = 0;
		}

		gbx_register.A >>= 1;
		gbx_register.A |= (bef_C_Flag << 7);

		gbx_register.F_Z = 0;
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
	}

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


	////void cpu_fnc__RRC_r8() {
	////
	////}
	////
	////void cpu_fnc__RRC_addrHL() {
	////
	////}


	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	//0x0F
	void cpu_fnc__RRCA() {
		if ((gbx_register.A & 0b00000001) != 0) {
			gbx_register.F_C = 1;
		}
		else {
			gbx_register.F_C = 0;
		}

		gbx_register.A >>= 1;
		gbx_register.A |= (gbx_register.F_C << 7);

		gbx_register.F_Z = 0;
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
	}

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


	////void cpu_fnc__SLA_r8() {
	////
	////}
	////
	////void cpu_fnc__SLA_addrHL() {
	////
	////}
	////
	////void cpu_fnc__SRA_r8() {
	////
	////}
	////
	////void cpu_fnc__SRA_addrHL() {
	////
	////}
	////
	////void cpu_fnc__SRL_r8() {
	////
	////}
	////
	////void cpu_fnc__SRL_addrHL() {
	////
	////}

	//=============================================================================

	//void cpu_fnc__LD_r8_r8() {
	//
	//}

	//0x7F
	void cpu_fnc__LD_A_A() {
		gbx_register.A = gbx_register.A;
	}

	//0x78
	void cpu_fnc__LD_A_B() {
		gbx_register.A = gbx_register.B;
	}

	//0x79
	void cpu_fnc__LD_A_C() {
		gbx_register.A = gbx_register.C;
	}

	//0x7A
	void cpu_fnc__LD_A_D() {
		gbx_register.A = gbx_register.D;
	}

	//0x7B
	void cpu_fnc__LD_A_E() {
		gbx_register.A = gbx_register.E;
	}

	//0x7C
	void cpu_fnc__LD_A_H() {
		gbx_register.A = gbx_register.H;
	}

	//0x7D
	void cpu_fnc__LD_A_L() {
		gbx_register.A = gbx_register.L;
	}

	//###

	//0x47
	void cpu_fnc__LD_B_A() {
		gbx_register.B = gbx_register.A;
	}

	//0x40
	void cpu_fnc__LD_B_B() {
		gbx_register.B = gbx_register.B;
	}

	//0x41
	void cpu_fnc__LD_B_C() {
		gbx_register.B = gbx_register.C;
	}

	//0x42
	void cpu_fnc__LD_B_D() {
		gbx_register.B = gbx_register.D;
	}

	//0x43
	void cpu_fnc__LD_B_E() {
		gbx_register.B = gbx_register.E;
	}

	//0x44
	void cpu_fnc__LD_B_H() {
		gbx_register.B = gbx_register.H;
	}

	//0x45
	void cpu_fnc__LD_B_L() {
		gbx_register.B = gbx_register.L;
	}

	//###

	//0x4F
	void cpu_fnc__LD_C_A() {
		gbx_register.C = gbx_register.A;
	}

	//0x48
	void cpu_fnc__LD_C_B() {
		gbx_register.C = gbx_register.B;
	}

	//0x49
	void cpu_fnc__LD_C_C() {
		gbx_register.C = gbx_register.C;
	}

	//0x4A
	void cpu_fnc__LD_C_D() {
		gbx_register.C = gbx_register.D;
	}

	//0x4B
	void cpu_fnc__LD_C_E() {
		gbx_register.C = gbx_register.E;
	}

	//0x4C
	void cpu_fnc__LD_C_H() {
		gbx_register.C = gbx_register.H;
	}

	//0x4D
	void cpu_fnc__LD_C_L() {
		gbx_register.C = gbx_register.L;
	}

	//###

	//0x57
	void cpu_fnc__LD_D_A() {
		gbx_register.D = gbx_register.A;
	}

	//0x50
	void cpu_fnc__LD_D_B() {
		gbx_register.D = gbx_register.B;
	}

	//0x51
	void cpu_fnc__LD_D_C() {
		gbx_register.D = gbx_register.C;
	}

	//0x52
	void cpu_fnc__LD_D_D() {
		gbx_register.D = gbx_register.D;
	}

	//0x53
	void cpu_fnc__LD_D_E() {
		gbx_register.D = gbx_register.E;
	}

	//0x54
	void cpu_fnc__LD_D_H() {
		gbx_register.D = gbx_register.H;
	}

	//0x55
	void cpu_fnc__LD_D_L() {
		gbx_register.D = gbx_register.L;
	}

	//###

	//0x5F
	void cpu_fnc__LD_E_A() {
		gbx_register.E = gbx_register.A;
	}

	//0x58
	void cpu_fnc__LD_E_B() {
		gbx_register.E = gbx_register.B;
	}

	//0x59
	void cpu_fnc__LD_E_C() {
		gbx_register.E = gbx_register.C;
	}

	//0x5A
	void cpu_fnc__LD_E_D() {
		gbx_register.E = gbx_register.D;
	}

	//0x5B
	void cpu_fnc__LD_E_E() {
		gbx_register.E = gbx_register.E;
	}

	//0x5C
	void cpu_fnc__LD_E_H() {
		gbx_register.E = gbx_register.H;
	}

	//0x5D
	void cpu_fnc__LD_E_L() {
		gbx_register.E = gbx_register.L;
	}

	//###

	//0x67
	void cpu_fnc__LD_H_A() {
		gbx_register.H = gbx_register.A;
	}

	//0x60
	void cpu_fnc__LD_H_B() {
		gbx_register.H = gbx_register.B;
	}

	//0x61
	void cpu_fnc__LD_H_C() {
		gbx_register.H = gbx_register.C;
	}

	//0x62
	void cpu_fnc__LD_H_D() {
		gbx_register.H = gbx_register.D;
	}

	//0x63
	void cpu_fnc__LD_H_E() {
		gbx_register.H = gbx_register.E;
	}

	//0x64
	void cpu_fnc__LD_H_H() {
		gbx_register.H = gbx_register.H;
	}

	//0x65
	void cpu_fnc__LD_H_L() {
		gbx_register.H = gbx_register.L;
	}

	//###

	//0x6F
	void cpu_fnc__LD_L_A() {
		gbx_register.L = gbx_register.A;
	}

	//0x68
	void cpu_fnc__LD_L_B() {
		gbx_register.L = gbx_register.B;
	}

	//0x69
	void cpu_fnc__LD_L_C() {
		gbx_register.L = gbx_register.C;
	}

	//0x6A
	void cpu_fnc__LD_L_D() {
		gbx_register.L = gbx_register.D;
	}

	//0x6B
	void cpu_fnc__LD_L_E() {
		gbx_register.L = gbx_register.E;
	}

	//0x6C
	void cpu_fnc__LD_L_H() {
		gbx_register.L = gbx_register.H;
	}

	//0x6D
	void cpu_fnc__LD_L_L() {
		gbx_register.L = gbx_register.L;
	}

	//###

	//void cpu_fnc__LD_r8_n8() {
	//}

	//0x3E
	void cpu_fnc__LD_A_n8() {
		gbx_register.A = read_RAM_8bit(gbx_register.PC);

		gbx_register.PC++;

		//M_debug_printf("LD A, 0x%02x\n", gbx_register.A);
	}

	//0x06
	void cpu_fnc__LD_B_n8() {
		gbx_register.B = read_RAM_8bit(gbx_register.PC);

		gbx_register.PC++;
	}

	//0x0E
	void cpu_fnc__LD_C_n8() {
		gbx_register.C = read_RAM_8bit(gbx_register.PC);

		gbx_register.PC++;
	}

	//0x16
	void cpu_fnc__LD_D_n8() {
		gbx_register.D = read_RAM_8bit(gbx_register.PC);

		gbx_register.PC++;
	}

	//0x1E
	void cpu_fnc__LD_E_n8() {
		gbx_register.E = read_RAM_8bit(gbx_register.PC);

		gbx_register.PC++;
	}

	//0x26
	void cpu_fnc__LD_H_n8() {
		gbx_register.H = read_RAM_8bit(gbx_register.PC);

		gbx_register.PC++;
	}

	//0x2E
	void cpu_fnc__LD_L_n8() {
		gbx_register.L = read_RAM_8bit(gbx_register.PC);

		gbx_register.PC++;
	}

	//void cpu_fnc__LD_r16_n16() {
	//}

	//0x01
	void cpu_fnc__LD_BC_n16() {
		uint16_t value_16bit = read_RAM_16bit(gbx_register.PC);
		gbx_register.BC = value_16bit;

		gbx_register.PC += 2;
	}

	//0x11
	void cpu_fnc__LD_DE_n16() {
		uint16_t value_16bit = read_RAM_16bit(gbx_register.PC);
		gbx_register.DE = value_16bit;

		gbx_register.PC += 2;
	}

	//0x21
	void cpu_fnc__LD_HL_n16() {
		uint16_t value_16bit = read_RAM_16bit(gbx_register.PC);
		gbx_register.HL = value_16bit;

		gbx_register.PC += 2;
	}


	//void cpu_fnc__LD_addrHL_r8() {
	//}

	//0x77
	void cpu_fnc__LD_addrHL_A() {
		write_RAM_8bit(gbx_register.HL, gbx_register.A);
	}

	//0x70
	void cpu_fnc__LD_addrHL_B() {
		write_RAM_8bit(gbx_register.HL, gbx_register.B);
	}

	//0x71
	void cpu_fnc__LD_addrHL_C() {
		write_RAM_8bit(gbx_register.HL, gbx_register.C);
	}

	//0x72
	void cpu_fnc__LD_addrHL_D() {
		write_RAM_8bit(gbx_register.HL, gbx_register.D);
	}

	//0x73
	void cpu_fnc__LD_addrHL_E() {
		write_RAM_8bit(gbx_register.HL, gbx_register.E);
	}

	//0x74
	void cpu_fnc__LD_addrHL_H() {
		write_RAM_8bit(gbx_register.HL, gbx_register.H);
	}

	//0x75
	void cpu_fnc__LD_addrHL_L() {
		write_RAM_8bit(gbx_register.HL, gbx_register.L);
	}


	//0x36
	void cpu_fnc__LD_addrHL_n8() {
		uint8_t value_8bit = read_RAM_8bit(gbx_register.PC);

		write_RAM_8bit(gbx_register.HL, value_8bit);

		gbx_register.PC++;
	}


	//void cpu_fnc__LD_r8_addrHL() {
	//}
	
	//0x7E
	void cpu_fnc__LD_A_addrHL() {
		uint8_t value_8bit = read_RAM_8bit(gbx_register.HL);

		gbx_register.A = value_8bit;
	}

	//0x46
	void cpu_fnc__LD_B_addrHL() {
		uint8_t value_8bit = read_RAM_8bit(gbx_register.HL);

		gbx_register.B = value_8bit;
	}

	//0x4E
	void cpu_fnc__LD_C_addrHL() {
		uint8_t value_8bit = read_RAM_8bit(gbx_register.HL);

		gbx_register.C = value_8bit;
	}

	//0x56
	void cpu_fnc__LD_D_addrHL() {
		uint8_t value_8bit = read_RAM_8bit(gbx_register.HL);

		gbx_register.D = value_8bit;
	}

	//0x5E
	void cpu_fnc__LD_E_addrHL() {
		uint8_t value_8bit = read_RAM_8bit(gbx_register.HL);

		gbx_register.E = value_8bit;
	}

	//0x66
	void cpu_fnc__LD_H_addrHL() {
		uint8_t value_8bit = read_RAM_8bit(gbx_register.HL);

		gbx_register.H = value_8bit;
	}

	//0x6E
	void cpu_fnc__LD_L_addrHL() {
		uint8_t value_8bit = read_RAM_8bit(gbx_register.HL);

		gbx_register.L = value_8bit;
	}


	//void cpu_fnc__LD_addrr16_A() {
	//}

	//0x02
	void cpu_fnc__LD_addrBC_A() {
		write_RAM_8bit(gbx_register.BC, gbx_register.A);
	}

	//0x12
	void cpu_fnc__LD_addrDE_A() {
		write_RAM_8bit(gbx_register.DE, gbx_register.A);
	}

	//void cpu_fnc__LD_addrHL_A()//実装済み

	//0xEA
	void cpu_fnc__LD_addrn16_A() {
		uint16_t addr_16bit = read_RAM_16bit(gbx_register.PC);

		write_RAM_8bit(addr_16bit, gbx_register.A);

		gbx_register.PC += 2;
	}

	//0xE0
	void cpu_fnc__LDH_addrn16_A() {
		uint8_t relative_addr = read_RAM_8bit(gbx_register.PC);

		write_RAM_8bit(0xFF00 + relative_addr, gbx_register.A);

		gbx_register.PC++;

		//M_debug_printf("LD (0x%04x), A\n", 0xFF00 + relative_addr);
	}

	//0xE2
	void cpu_fnc__LDH_addrC_A() {
		write_RAM_8bit(0xFF00 + gbx_register.C, gbx_register.A);
	}


	//void cpu_fnc__LD_A_addrr16() {
	//}

	//0x0A
	void cpu_fnc__LD_A_addrBC() {
		uint8_t value_8bit = read_RAM_8bit(gbx_register.BC);

		gbx_register.A = value_8bit;
	}

	//0x1A
	void cpu_fnc__LD_A_addrDE() {
		uint8_t value_8bit = read_RAM_8bit(gbx_register.DE);

		//printf("check!\n");

		gbx_register.A = value_8bit;
	}

	//void cpu_fnc__LD_A_addrHL()//実装済み


	//0xFA
	void cpu_fnc__LD_A_addrn16() {
		uint16_t addr_16bit = read_RAM_16bit(gbx_register.PC);
		uint8_t value_8bit = read_RAM_8bit(addr_16bit);

		gbx_register.A = value_8bit;

		gbx_register.PC += 2;
	}

	//0xF0
	void cpu_fnc__LDH_A_addrn16() {
		uint8_t relative_addr = read_RAM_8bit(gbx_register.PC);
		gbx_register.A = read_RAM_8bit(0xFF00 + relative_addr);

		gbx_register.PC++;

	}

	//0xF2
	void cpu_fnc__LDH_A_addrC() {
		gbx_register.A = read_RAM_8bit(0xFF00 + gbx_register.C);
	}

	//0x22
	void cpu_fnc__LD_addrHLI_A() {
		write_RAM_8bit(gbx_register.HL, gbx_register.A);

		gbx_register.HL++;
	}

	//0x32
	void cpu_fnc__LD_addrHLD_A() {
		write_RAM_8bit(gbx_register.HL, gbx_register.A);

		gbx_register.HL--;
	}

	//0x2A
	void cpu_fnc__LD_A_addrHLI() {
		gbx_register.A = read_RAM_8bit(gbx_register.HL);

		gbx_register.HL++;
	}

	//0x3A
	void cpu_fnc__LD_A_addrHLD() {
		gbx_register.A = read_RAM_8bit(gbx_register.HL);

		gbx_register.HL--;
	}

	//=============================================================================
	
	//0xC3
	void cpu_fnc__JP_n16() {
		gbx_register.PC = read_RAM_16bit(gbx_register.PC);

		//gbx_register.PC += 2;
	}


	//void cpu_fnc__JP_cc_n16() {
	//}

	//0xCA
	void cpu_fnc__JP_FZ_n16() {//Z
		uint16_t jmp_addr = read_RAM_16bit(gbx_register.PC);

		gbx_register.PC += 2;

		if (gbx_register.F_Z == 1) {
			gbx_register.PC = jmp_addr;

			cpu_machine_cycle++;
		}
	}

	//0xC2
	void cpu_fnc__JP_FNZ_n16() {//NZ
		uint16_t jmp_addr = read_RAM_16bit(gbx_register.PC);

		gbx_register.PC += 2;

		if (gbx_register.F_Z == 0) {
			gbx_register.PC = jmp_addr;

			cpu_machine_cycle++;
		}
	}

	//0xDA
	void cpu_fnc__JP_FC_n16() {//C
		uint16_t jmp_addr = read_RAM_16bit(gbx_register.PC);

		gbx_register.PC += 2;

		if (gbx_register.F_C == 1) {
			gbx_register.PC = jmp_addr;

			cpu_machine_cycle++;
		}
	}

	//0xD2
	void cpu_fnc__JP_FNC_n16() {//NC
		uint16_t jmp_addr = read_RAM_16bit(gbx_register.PC);

		gbx_register.PC += 2;

		if (gbx_register.F_C == 0) {
			gbx_register.PC = jmp_addr;

			cpu_machine_cycle++;
		}
	}


	//0xE9
	void cpu_fnc__JP_HL() {
		gbx_register.PC = gbx_register.HL;
	}


	//0x18
	void cpu_fnc__JR_e8() {
		int8_t relative_addr = read_RAM_8bit(gbx_register.PC);

		gbx_register.PC++;

		gbx_register.PC += relative_addr;
	}


	//void cpu_fnc__JR_cc_e8() {
	//}
	
	//0x28
	void cpu_fnc__JR_FZ_e8() {//Z
		int8_t relative_addr = read_RAM_8bit(gbx_register.PC);

		if (gbx_register.F_Z == 1) {
			gbx_register.PC += relative_addr;

			cpu_machine_cycle++;
		}

		gbx_register.PC++;
	}
	//0x20
	void cpu_fnc__JR_FNZ_e8() {//NZ
		int8_t relative_addr = read_RAM_8bit(gbx_register.PC);

		if (gbx_register.F_Z == 0) {
			gbx_register.PC += relative_addr;

			cpu_machine_cycle++;
		}

		gbx_register.PC++;

	}
	//0x38
	void cpu_fnc__JR_FC_e8() {//C
		int8_t relative_addr = read_RAM_8bit(gbx_register.PC);

		if (gbx_register.F_C == 1) {
			gbx_register.PC += relative_addr;

			cpu_machine_cycle++;
		}

		gbx_register.PC++;
	}
	//0x30
	void cpu_fnc__JR_FNC_e8() {//NC
		int8_t relative_addr = read_RAM_8bit(gbx_register.PC);

		if (gbx_register.F_C == 0) {
			gbx_register.PC += relative_addr;

			cpu_machine_cycle++;
		}

		gbx_register.PC++;
	}


	//0xCD
	void cpu_fnc__CALL_n16() {
		uint16_t call_addr = read_RAM_16bit(gbx_register.PC);

		gbx_register.PC += 2;

		push_16bit(gbx_register.PC);

		gbx_register.PC = call_addr;
	}


	//void cpu_fnc__CALL_cc_n16() {
	//}

	//0xCC
	void cpu_fnc__CALL_FZ_n16() {//Z
		uint16_t call_addr = read_RAM_16bit(gbx_register.PC);

		gbx_register.PC += 2;

		if (gbx_register.F_Z == 1) {
			push_16bit(gbx_register.PC);

			gbx_register.PC = call_addr;

			cpu_machine_cycle += 3;
		}
	}

	//0xC4
	void cpu_fnc__CALL_FNZ_n16() {//NZ
		uint16_t call_addr = read_RAM_16bit(gbx_register.PC);

		gbx_register.PC += 2;

		if (gbx_register.F_Z == 0) {
			push_16bit(gbx_register.PC);

			gbx_register.PC = call_addr;

			cpu_machine_cycle += 3;
		}
	}

	//0xDC
	void cpu_fnc__CALL_FC_n16() {//C
		uint16_t call_addr = read_RAM_16bit(gbx_register.PC);

		gbx_register.PC += 2;

		if (gbx_register.F_C == 1) {
			push_16bit(gbx_register.PC);

			gbx_register.PC = call_addr;

			cpu_machine_cycle += 3;
		}
	}

	//0xD4
	void cpu_fnc__CALL_FNC_n16() {//NC
		uint16_t call_addr = read_RAM_16bit(gbx_register.PC);

		gbx_register.PC += 2;

		if (gbx_register.F_C == 0) {
			push_16bit(gbx_register.PC);

			gbx_register.PC = call_addr;

			cpu_machine_cycle += 3;
		}
	}


	//0xC9
	void cpu_fnc__RET() {
		gbx_register.PC = pop_16bit();
	}


	//void cpu_fnc__RET_cc() {
	//}

	//0xC8
	void cpu_fnc__RET_FZ() {//Z
		if (gbx_register.F_Z == 1) {
			gbx_register.PC = pop_16bit();

			cpu_machine_cycle += 3;
		}
	}

	//0xC0
	void cpu_fnc__RET_FNZ() {//NZ
		if (gbx_register.F_Z == 0) {
			gbx_register.PC = pop_16bit();

			cpu_machine_cycle += 3;
		}
	}

	//0xD8
	void cpu_fnc__RET_FC() {//C
		if (gbx_register.F_C == 1) {
			gbx_register.PC = pop_16bit();

			cpu_machine_cycle += 3;
		}
	}

	//0xD0
	void cpu_fnc__RET_FNC() {//NC
		if (gbx_register.F_C == 0) {
			gbx_register.PC = pop_16bit();

			cpu_machine_cycle += 3;
		}
	}


	//0xD9
	void cpu_fnc__RETI() {
		gbx_register.PC = pop_16bit();

		IME_Flag = true;
	}


	//void cpu_fnc__RST_vec() {
	//}

	void RST_process(uint8_t vec) {
		uint16_t call_addr = (uint16_t)(0x0000 + vec);

		push_16bit(gbx_register.PC);

		gbx_register.PC = call_addr;
	}

	//0xC7
	void cpu_fnc__RST_00h() {
		RST_process(0x00);
	}

	//0xCF
	void cpu_fnc__RST_08h() {
		RST_process(0x08);
	}

	//0xD7
	void cpu_fnc__RST_10h() {
		RST_process(0x10);
	}

	//0xDF
	void cpu_fnc__RST_18h() {
		RST_process(0x18);
	}

	//0xE7
	void cpu_fnc__RST_20h() {
		RST_process(0x20);
	}

	//0xEF
	void cpu_fnc__RST_28h() {
		RST_process(0x28);
	}

	//0xF7
	void cpu_fnc__RST_30h() {
		RST_process(0x30);
	}

	//0xFF
	void cpu_fnc__RST_38h() {
		RST_process(0x38);
	}


	//=============================================================================
	
	//0x39
	void cpu_fnc__ADD_HL_SP() {
		calc_H_Flag__16bit_add(gbx_register.HL, gbx_register.SP);
		calc_C_Flag__16bit_add(gbx_register.HL, gbx_register.SP);

		gbx_register.HL += gbx_register.SP;

		gbx_register.F_N = 0;
	}


	//0xE8
	void cpu_fnc__ADD_SP_e8() {
		uint8_t value_8bit_unsigned = read_RAM_8bit(gbx_register.PC);
		int8_t value_8bit_signed = (int8_t)value_8bit_unsigned;

		//calc_H_Flag__8bit_add(gbx_register.SP & 0b0000000011111111, value_8bit_unsigned);
		//calc_C_Flag__8bit_add(gbx_register.SP & 0b0000000011111111, value_8bit_unsigned);
		//
		//gbx_register.SP = (uint16_t)((int32_t)gbx_register.SP + (int32_t)value_8bit_signed);

		int32_t result = (int32_t)gbx_register.SP + (int32_t)value_8bit_signed;
		uint32_t carry_bit = (uint32_t)gbx_register.SP ^ (uint32_t)value_8bit_signed ^ (uint32_t)result;
		
		gbx_register.SP = (uint16_t)result;

		if ((carry_bit & 0b00010000) != 0) {
			gbx_register.F_H = 1;
		}
		else {
			gbx_register.F_H = 0;
		}

		if ((carry_bit & 0b100000000) != 0) {
			gbx_register.F_C = 1;
		}
		else {
			gbx_register.F_C = 0;
		}

		gbx_register.F_Z = 0;
		gbx_register.F_N = 0;

		gbx_register.PC++;
	}

	//0x33
	void cpu_fnc__INC_SP() {
		gbx_register.SP++;
	}

	//0x3B
	void cpu_fnc__DEC_SP() {
		gbx_register.SP--;
	}

	//0x31
	void cpu_fnc__LD_SP_n16() {
		gbx_register.SP = read_RAM_16bit(gbx_register.PC);

		gbx_register.PC += 2;
	}


	//0x08
	void cpu_fnc__LD_addrn16_SP() {
		uint16_t addr_16bit = read_RAM_16bit(gbx_register.PC);

		write_RAM_8bit(addr_16bit, gbx_register.SP & 0b11111111);
		write_RAM_8bit(addr_16bit + 1, (gbx_register.SP >> 8) & 0b11111111);

		gbx_register.PC += 2;
	}


	//0xF8
	void cpu_fnc__LD_HL_SPpe8() {//LD HL,SP+e8
		uint8_t value_8bit_unsigned = read_RAM_8bit(gbx_register.PC);
		int8_t value_8bit_signed = (int8_t)value_8bit_unsigned;

		//calc_H_Flag__8bit_add(gbx_register.SP & 0b0000000011111111, value_8bit_unsigned);
		//calc_C_Flag__8bit_add(gbx_register.SP & 0b0000000011111111, value_8bit_unsigned);
		//
		//gbx_register.HL = (uint16_t)((int32_t)gbx_register.SP + (int32_t)value_8bit_signed);

		int32_t result = (int32_t)gbx_register.SP + (int32_t)value_8bit_signed;
		uint32_t carry_bit = (uint32_t)gbx_register.SP ^ (uint32_t)value_8bit_signed ^ (uint32_t)result;

		gbx_register.HL = (uint16_t)result;

		if ((carry_bit & 0b00010000) != 0) {
			gbx_register.F_H = 1;
		}
		else {
			gbx_register.F_H = 0;
		}

		if ((carry_bit & 0b100000000) != 0) {
			gbx_register.F_C = 1;
		}
		else {
			gbx_register.F_C = 0;
		}

		gbx_register.F_Z = 0;
		gbx_register.F_N = 0;

		gbx_register.PC++;
	}


	//0xF9
	void cpu_fnc__LD_SP_HL() {
		gbx_register.SP = gbx_register.HL;
	}


	//0xF5
	void cpu_fnc__PUSH_AF() {
		gbx_register.F_unused = 0;//未使用領域は0にしておく

		push_16bit(gbx_register.AF);
	}


	//void cpu_fnc__PUSH_r16() {
	//}

	//0xC5
	void cpu_fnc__PUSH_BC() {
		push_16bit(gbx_register.BC);
	}

	//0xD5
	void cpu_fnc__PUSH_DE() {
		push_16bit(gbx_register.DE);
	}

	//0xE5
	void cpu_fnc__PUSH_HL() {
		push_16bit(gbx_register.HL);
	}


	//0xF1
	void cpu_fnc__POP_AF() {
		gbx_register.AF = pop_16bit();

		if ((gbx_register.AF & 0b0000000010000000) != 0) {
			gbx_register.F_Z = 1;
		}
		else {
			gbx_register.F_Z = 0;
		}
		
		if ((gbx_register.AF & 0b0000000001000000) != 0) {
			gbx_register.F_N = 1;
		}
		else {
			gbx_register.F_N = 0;
		}
		
		if ((gbx_register.AF & 0b0000000000100000) != 0) {
			gbx_register.F_H = 1;
		}
		else {
			gbx_register.F_H = 0;
		}
		
		if ((gbx_register.AF & 0b0000000000010000) != 0) {
			gbx_register.F_C = 1;
		}
		else {
			gbx_register.F_C = 0;
		}

		gbx_register.F_unused = 0;//未使用領域は0にしておく
	}


	//void cpu_fnc__POP_r16() {
	//}

	//0xC1
	void cpu_fnc__POP_BC() {
		gbx_register.BC = pop_16bit();
	}

	//0xD1
	void cpu_fnc__POP_DE() {
		gbx_register.DE = pop_16bit();
	}

	//0xE1
	void cpu_fnc__POP_HL() {
		gbx_register.HL = pop_16bit();
	}


	//=============================================================================

	//0x3F
	void cpu_fnc__CCF() {
		gbx_register.F_C = (gbx_register.F_C == 0) ? 1 : 0;

		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
	}

	//0x2F
	void cpu_fnc__CPL() {
		gbx_register.A = ~gbx_register.A;

		gbx_register.F_N = 1;
		gbx_register.F_H = 1;
	}

	//0x27
	void cpu_fnc__DAA() {
		int32_t tmp = gbx_register.A;
		if (gbx_register.F_N == 0) {
			if (gbx_register.F_C == 1 || tmp > 0x99) {
				tmp += 0x60;
				gbx_register.F_C = 1;
			}
			if (gbx_register.F_H == 1 || (tmp & 0xf) > 0x9) {
				tmp += 0x06;
			}
		}
		else {
			if (gbx_register.F_C == 1) {
				tmp -= 0x60;
			}
			if (gbx_register.F_H == 1) {
				tmp -= 0x06;
			}
		}
		gbx_register.A = (uint8_t)(tmp & 0xff);
		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_H = 0;

	}

	//0xF3
	void cpu_fnc__DI() {
		IME_Flag = false;
	}

	//0xFB
	void cpu_fnc__EI() {
		IME_Flag = true;
	}

	//0x76
	void cpu_fnc__HALT() {
		////M_debug_printf("HALT...\n");
		////
		////gbx_register.PC--;

		//M_debug_printf("HALT...\n");

		uint8_t IE_value = gbx_ram.RAM[0xFFFF];
		uint8_t IF_value = gbx_ram.RAM[0xFF0F];

		if (IME_Flag == true) {
			//if (!(((IE_value & IF_value) & 0b00011111) != 0)) {
				gbx_register.PC--;//PCをすすめずにとどまる

				tmp_CPU_HALT_Flag = true;
			//}
		}
		else {
			if (((IE_value & IF_value) & 0b00011111) != 0) {//割り込みが保留中のとき
				gbx_register.PC++;//ハードウエアのバグで1バイトすすめる
				
			}
			else {
				gbx_register.PC--;//PCをすすめずにとどまる
			}
		}
	}

	//0x00
	void cpu_fnc__NOP() {
	}

	//0x37
	void cpu_fnc__SCF() {
		gbx_register.F_C = 1;

		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
	}

	//0x10
	void cpu_fnc__STOP() {
		//M_debug_printf("STOP...\n");

		//CPUの動作モードを変更する
		CURRENT_CPU_Clock_2x_Flag__CGB = SET_CPU_Clock_2x_Flag__CGB;

		gbx_register.PC++;//stop命令は1バイトとばす

		//M_debug_printf("PC = 0x%04x\n", gbx_register.PC - 2);
		//system("pause");
	}

	//=============================================================================
	

	//0xCB
	void cpu_fnc__PREFIX() {
		uint8_t instruction_code_2 = read_RAM_8bit(gbx_register.PC);

		gbx_register.PC++;

		PREFIX_process(instruction_code_2);

		if (instruction_code_2 == 0x46 ||
			instruction_code_2 == 0x56 ||
			instruction_code_2 == 0x66 ||
			instruction_code_2 == 0x76 ||

			instruction_code_2 == 0x4E ||
			instruction_code_2 == 0x5E ||
			instruction_code_2 == 0x6E ||
			instruction_code_2 == 0x7E)
		{
			cpu_machine_cycle += 2;
		}
		else if (
			instruction_code_2 == 0x06 ||
			instruction_code_2 == 0x16 ||
			instruction_code_2 == 0x26 ||
			instruction_code_2 == 0x36 ||

			instruction_code_2 == 0x0E ||
			instruction_code_2 == 0x1E ||
			instruction_code_2 == 0x2E ||
			instruction_code_2 == 0x3E ||

			instruction_code_2 == 0x86 ||
			instruction_code_2 == 0x96 ||
			instruction_code_2 == 0xA6 ||
			instruction_code_2 == 0xB6 ||
			instruction_code_2 == 0xC6 ||
			instruction_code_2 == 0xD6 ||
			instruction_code_2 == 0xE6 ||
			instruction_code_2 == 0xF6 ||

			instruction_code_2 == 0x8E ||
			instruction_code_2 == 0x9E ||
			instruction_code_2 == 0xAE ||
			instruction_code_2 == 0xBE ||
			instruction_code_2 == 0xCE ||
			instruction_code_2 == 0xDE ||
			instruction_code_2 == 0xEE ||
			instruction_code_2 == 0xFE)
		{
			cpu_machine_cycle += 3;
		}
		else {
			cpu_machine_cycle++;
		}
	}

	void PREFIX_process(uint8_t instruction_code) {
		//target_op_ptrがnullptrなら(HL)
		uint8_t* target_op_ptr = get_target_op_ptr(instruction_code); //操作する"レジスタかメモリ"のポインタ

		if (0x07 >= instruction_code) {//RLC
			PREFIX_process__RLC(target_op_ptr);
		}
		else if (0x0F >= instruction_code) {//RRC
			PREFIX_process__RRC(target_op_ptr);
		}
		else if (0x17 >= instruction_code) {//RL
			PREFIX_process__RL(target_op_ptr);
		}
		else if (0x1F >= instruction_code) {//RR
			PREFIX_process__RR(target_op_ptr);
		}
		else if (0x27 >= instruction_code) {//SLA
			PREFIX_process__SLA(target_op_ptr);
		}
		else if (0x2F >= instruction_code) {//SRA
			PREFIX_process__SRA(target_op_ptr);
		}
		else if (0x37 >= instruction_code) {//SWAP
			PREFIX_process__SWAP(target_op_ptr);
		}
		else if (0x3F >= instruction_code) {//SRL
			PREFIX_process__SRL(target_op_ptr);
		}
		else if (0x47 >= instruction_code) {//BIT 0
			PREFIX_process__BIT_0(target_op_ptr);
		}
		else if (0x4F >= instruction_code) {//BIT 1
			PREFIX_process__BIT_1(target_op_ptr);
		}
		else if (0x57 >= instruction_code) {//BIT 2
			PREFIX_process__BIT_2(target_op_ptr);
		}
		else if (0x5F >= instruction_code) {//BIT 3
			PREFIX_process__BIT_3(target_op_ptr);
		}
		else if (0x67 >= instruction_code) {//BIT 4
			PREFIX_process__BIT_4(target_op_ptr);
		}
		else if (0x6F >= instruction_code) {//BIT 5
			PREFIX_process__BIT_5(target_op_ptr);
		}
		else if (0x77 >= instruction_code) {//BIT 6
			PREFIX_process__BIT_6(target_op_ptr);
		}
		else if (0x7F >= instruction_code) {//BIT 7
			PREFIX_process__BIT_7(target_op_ptr);
		}
		else if (0x87 >= instruction_code) {//RES 0
			PREFIX_process__RES_0(target_op_ptr);
		}
		else if (0x8F >= instruction_code) {//RES 1
			PREFIX_process__RES_1(target_op_ptr);
		}
		else if (0x97 >= instruction_code) {//RES 2
			PREFIX_process__RES_2(target_op_ptr);
		}
		else if (0x9F >= instruction_code) {//RES 3
			PREFIX_process__RES_3(target_op_ptr);
		}
		else if (0xA7 >= instruction_code) {//RES 4
			PREFIX_process__RES_4(target_op_ptr);
		}
		else if (0xAF >= instruction_code) {//RES 5
			PREFIX_process__RES_5(target_op_ptr);
		}
		else if (0xB7 >= instruction_code) {//RES 6
			PREFIX_process__RES_6(target_op_ptr);
		}
		else if (0xBF >= instruction_code) {//RES 7
			PREFIX_process__RES_7(target_op_ptr);
		}
		else if (0xC7 >= instruction_code) {//SET 0
			PREFIX_process__SET_0(target_op_ptr);
		}
		else if (0xCF >= instruction_code) {//SET 1
			PREFIX_process__SET_1(target_op_ptr);
		}
		else if (0xD7 >= instruction_code) {//SET 2
			PREFIX_process__SET_2(target_op_ptr);
		}
		else if (0xDF >= instruction_code) {//SET 3
			PREFIX_process__SET_3(target_op_ptr);
		}
		else if (0xE7 >= instruction_code) {//SET 4
			PREFIX_process__SET_4(target_op_ptr);
		}
		else if (0xEF >= instruction_code) {//SET 5
			PREFIX_process__SET_5(target_op_ptr);
		}
		else if (0xF7 >= instruction_code) {//SET 6
			PREFIX_process__SET_6(target_op_ptr);
		}
		//else if (0xFF >= instruction_code) {//SET 7
		else {//SET 7
			PREFIX_process__SET_7(target_op_ptr);
		}
	}

	/*

	B		= +0x00
	C		= +0x01
	D		= +0x02
	E		= +0x03
	H		= +0x04
	L		= +0x05
	(HL)	= +0x06
	A		= +0x07

	*/
	uint8_t* get_target_op_ptr(uint8_t instruction_code) {
		uint8_t op_index = (uint8_t)(instruction_code % 8); //= 0～7

		if (op_index == 0) {
			return &(gbx_register.B);
		}
		else if (op_index == 1) {
			return &(gbx_register.C);
		}
		else if (op_index == 2) {
			return &(gbx_register.D);
		}
		else if (op_index == 3) {
			return &(gbx_register.E);
		}
		else if (op_index == 4) {
			return &(gbx_register.H);
		}
		else if (op_index == 5) {
			return &(gbx_register.L);
		}
		else if (op_index == 6) {
			//return &(gbx_ram.RAM[gbx_register.HL]);
			//return get_read_RAM_address___(gbx_register.HL);
			return nullptr;
		}
		//else if (op_index == 7) {
		else {
			return &(gbx_register.A);
		}
	}


	void PREFIX_process__RLC(uint8_t* target_op_ptr) {
		if (target_op_ptr != nullptr) {
			if (((*target_op_ptr) & 0b10000000) != 0) {
				gbx_register.F_C = 1;
			}
			else {
				gbx_register.F_C = 0;
			}

			(*target_op_ptr) <<= 1;
			(*target_op_ptr) |= gbx_register.F_C;

			calc_Z_Flag__8bit(*target_op_ptr);
			gbx_register.F_N = 0;
			gbx_register.F_H = 0;
		}
		else {
			if ((read_RAM_8bit(gbx_register.HL) & 0b10000000) != 0) {
				gbx_register.F_C = 1;
			}
			else {
				gbx_register.F_C = 0;
			}

			//(*target_op_ptr) <<= 1;
			//(*target_op_ptr) |= gbx_register.F_C;
			write_RAM_8bit(gbx_register.HL, read_RAM_8bit(gbx_register.HL) << 1);
			write_RAM_8bit(gbx_register.HL, read_RAM_8bit(gbx_register.HL) | gbx_register.F_C);

			calc_Z_Flag__8bit(read_RAM_8bit(gbx_register.HL));
			gbx_register.F_N = 0;
			gbx_register.F_H = 0;
		}
	}

	void PREFIX_process__RRC(uint8_t* target_op_ptr) {
		if (target_op_ptr != nullptr) {
			if (((*target_op_ptr) & 0b00000001) != 0) {
				gbx_register.F_C = 1;
			}
			else {
				gbx_register.F_C = 0;
			}

			(*target_op_ptr) >>= 1;
			(*target_op_ptr) |= (gbx_register.F_C << 7);

			calc_Z_Flag__8bit(*target_op_ptr);
			gbx_register.F_N = 0;
			gbx_register.F_H = 0;
		}
		else {
			if ((read_RAM_8bit(gbx_register.HL) & 0b00000001) != 0) {
				gbx_register.F_C = 1;
			}
			else {
				gbx_register.F_C = 0;
			}

			//(*target_op_ptr) >>= 1;
			//(*target_op_ptr) |= (gbx_register.F_C << 7);
			write_RAM_8bit(gbx_register.HL, read_RAM_8bit(gbx_register.HL) >> 1);
			write_RAM_8bit(gbx_register.HL, read_RAM_8bit(gbx_register.HL) | (gbx_register.F_C << 7));

			calc_Z_Flag__8bit(read_RAM_8bit(gbx_register.HL));
			gbx_register.F_N = 0;
			gbx_register.F_H = 0;
		}
	}

	void PREFIX_process__RL(uint8_t* target_op_ptr) {
		if (target_op_ptr != nullptr) {
			uint8_t bef_C_Flag = gbx_register.F_C;

			if (((*target_op_ptr) & 0b10000000) != 0) {
				gbx_register.F_C = 1;
			}
			else {
				gbx_register.F_C = 0;
			}

			(*target_op_ptr) <<= 1;
			(*target_op_ptr) |= bef_C_Flag;

			calc_Z_Flag__8bit(*target_op_ptr);
			gbx_register.F_N = 0;
			gbx_register.F_H = 0;
		}
		else {
			uint8_t bef_C_Flag = gbx_register.F_C;

			if ((read_RAM_8bit(gbx_register.HL) & 0b10000000) != 0) {
				gbx_register.F_C = 1;
			}
			else {
				gbx_register.F_C = 0;
			}

			//(*target_op_ptr) <<= 1;
			//(*target_op_ptr) |= bef_C_Flag;
			write_RAM_8bit(gbx_register.HL, read_RAM_8bit(gbx_register.HL) << 1);
			write_RAM_8bit(gbx_register.HL, read_RAM_8bit(gbx_register.HL) | bef_C_Flag);

			calc_Z_Flag__8bit(read_RAM_8bit(gbx_register.HL));
			gbx_register.F_N = 0;
			gbx_register.F_H = 0;
		}
	}

	void PREFIX_process__RR(uint8_t* target_op_ptr) {
		if (target_op_ptr != nullptr) {
			uint8_t bef_C_Flag = gbx_register.F_C;

			if (((*target_op_ptr) & 0b00000001) != 0) {
				gbx_register.F_C = 1;
			}
			else {
				gbx_register.F_C = 0;
			}

			(*target_op_ptr) >>= 1;
			(*target_op_ptr) |= (bef_C_Flag << 7);

			calc_Z_Flag__8bit(*target_op_ptr);
			gbx_register.F_N = 0;
			gbx_register.F_H = 0;
		}
		else {
			uint8_t bef_C_Flag = gbx_register.F_C;

			if ((read_RAM_8bit(gbx_register.HL) & 0b00000001) != 0) {
				gbx_register.F_C = 1;
			}
			else {
				gbx_register.F_C = 0;
			}

			//(*target_op_ptr) >>= 1;
			//(*target_op_ptr) |= (bef_C_Flag << 7);
			write_RAM_8bit(gbx_register.HL, read_RAM_8bit(gbx_register.HL) >> 1);
			write_RAM_8bit(gbx_register.HL, read_RAM_8bit(gbx_register.HL) | (bef_C_Flag << 7));

			calc_Z_Flag__8bit(read_RAM_8bit(gbx_register.HL));
			gbx_register.F_N = 0;
			gbx_register.F_H = 0;
		}
	}

	void PREFIX_process__SLA(uint8_t* target_op_ptr) {
		if (target_op_ptr != nullptr) {
			if (((*target_op_ptr) & 0b10000000) != 0) {
				gbx_register.F_C = 1;
			}
			else {
				gbx_register.F_C = 0;
			}

			(*target_op_ptr) <<= 1;

			calc_Z_Flag__8bit(*target_op_ptr);
			gbx_register.F_N = 0;
			gbx_register.F_H = 0;
		}
		else {
			if ((read_RAM_8bit(gbx_register.HL) & 0b10000000) != 0) {
				gbx_register.F_C = 1;
			}
			else {
				gbx_register.F_C = 0;
			}

			//(*target_op_ptr) <<= 1;
			write_RAM_8bit(gbx_register.HL, read_RAM_8bit(gbx_register.HL) << 1);

			calc_Z_Flag__8bit(read_RAM_8bit(gbx_register.HL));
			gbx_register.F_N = 0;
			gbx_register.F_H = 0;
		}
	}

	void PREFIX_process__SRA(uint8_t* target_op_ptr) {
		if (target_op_ptr != nullptr) {
			uint8_t bef_sign = (((*target_op_ptr) & 0b10000000) != 0) ? 1 : 0;

			if (((*target_op_ptr) & 0b00000001) != 0) {
				gbx_register.F_C = 1;
			}
			else {
				gbx_register.F_C = 0;
			}

			(*target_op_ptr) >>= 1;
			(*target_op_ptr) &= 0b01111111;
			(*target_op_ptr) |= (bef_sign << 7);

			calc_Z_Flag__8bit(*target_op_ptr);
			gbx_register.F_N = 0;
			gbx_register.F_H = 0;
		}
		else {
			uint8_t bef_sign = ((read_RAM_8bit(gbx_register.HL) & 0b10000000) != 0) ? 1 : 0;

			if ((read_RAM_8bit(gbx_register.HL) & 0b00000001) != 0) {
				gbx_register.F_C = 1;
			}
			else {
				gbx_register.F_C = 0;
			}

			//(*target_op_ptr) >>= 1;
			//(*target_op_ptr) &= 0b01111111;
			//(*target_op_ptr) |= (bef_sign << 7);
			write_RAM_8bit(gbx_register.HL, read_RAM_8bit(gbx_register.HL) >> 1);
			write_RAM_8bit(gbx_register.HL, read_RAM_8bit(gbx_register.HL) & 0b01111111);
			write_RAM_8bit(gbx_register.HL, read_RAM_8bit(gbx_register.HL) | (bef_sign << 7));

			calc_Z_Flag__8bit(read_RAM_8bit(gbx_register.HL));
			gbx_register.F_N = 0;
			gbx_register.F_H = 0;
		}
	}

	void PREFIX_process__SWAP(uint8_t* target_op_ptr) {
		if (target_op_ptr != nullptr) {
			uint8_t bef_value = (*target_op_ptr);

			(*target_op_ptr) >>= 4;
			(*target_op_ptr) &= 0b00001111;
			(*target_op_ptr) |= ((bef_value << 4) & 0b11110000);

			calc_Z_Flag__8bit(*target_op_ptr);
			gbx_register.F_C = 0;
			gbx_register.F_H = 0;
			gbx_register.F_N = 0;
		}
		else {
			uint8_t bef_value = read_RAM_8bit(gbx_register.HL);

			//(*target_op_ptr) >>= 4;
			//(*target_op_ptr) &= 0b00001111;
			//(*target_op_ptr) |= ((bef_value << 4) & 0b11110000);
			write_RAM_8bit(gbx_register.HL, read_RAM_8bit(gbx_register.HL) >> 4);
			write_RAM_8bit(gbx_register.HL, read_RAM_8bit(gbx_register.HL) & 0b00001111);
			write_RAM_8bit(gbx_register.HL, read_RAM_8bit(gbx_register.HL) | ((bef_value << 4) & 0b11110000));

			calc_Z_Flag__8bit(read_RAM_8bit(gbx_register.HL));
			gbx_register.F_C = 0;
			gbx_register.F_H = 0;
			gbx_register.F_N = 0;
		}
	}

	void PREFIX_process__SRL(uint8_t* target_op_ptr) {
		if (target_op_ptr != nullptr) {
			if (((*target_op_ptr) & 0b00000001) != 0) {
				gbx_register.F_C = 1;
			}
			else {
				gbx_register.F_C = 0;
			}

			(*target_op_ptr) >>= 1;

			calc_Z_Flag__8bit(*target_op_ptr);
			gbx_register.F_N = 0;
			gbx_register.F_H = 0;
		}
		else {
			if ((read_RAM_8bit(gbx_register.HL) & 0b00000001) != 0) {
				gbx_register.F_C = 1;
			}
			else {
				gbx_register.F_C = 0;
			}

			//(*target_op_ptr) >>= 1;
			write_RAM_8bit(gbx_register.HL, read_RAM_8bit(gbx_register.HL) >> 1);

			calc_Z_Flag__8bit(read_RAM_8bit(gbx_register.HL));
			gbx_register.F_N = 0;
			gbx_register.F_H = 0;
		}
	}


	void PREFIX_sub_process__BIT(uint8_t check_bit, uint8_t* target_op_ptr) {
		if (target_op_ptr != nullptr) {
			if (((*target_op_ptr) & (0b00000001 << check_bit)) == 0) {
				gbx_register.F_Z = 1;
			}
			else {
				gbx_register.F_Z = 0;
			}

			gbx_register.F_N = 0;
			gbx_register.F_H = 1;
		}
		else {
			if ((read_RAM_8bit(gbx_register.HL) & (0b00000001 << check_bit)) == 0) {
				gbx_register.F_Z = 1;
			}
			else {
				gbx_register.F_Z = 0;
			}

			gbx_register.F_N = 0;
			gbx_register.F_H = 1;
		}
	}
	void PREFIX_process__BIT_0(uint8_t* target_op_ptr) {
		PREFIX_sub_process__BIT(0, target_op_ptr);
	}
	void PREFIX_process__BIT_1(uint8_t* target_op_ptr) {
		PREFIX_sub_process__BIT(1, target_op_ptr);
	}
	void PREFIX_process__BIT_2(uint8_t* target_op_ptr) {
		PREFIX_sub_process__BIT(2, target_op_ptr);
	}
	void PREFIX_process__BIT_3(uint8_t* target_op_ptr) {
		PREFIX_sub_process__BIT(3, target_op_ptr);
	}
	void PREFIX_process__BIT_4(uint8_t* target_op_ptr) {
		PREFIX_sub_process__BIT(4, target_op_ptr);
	}
	void PREFIX_process__BIT_5(uint8_t* target_op_ptr) {
		PREFIX_sub_process__BIT(5, target_op_ptr);
	}
	void PREFIX_process__BIT_6(uint8_t* target_op_ptr) {
		PREFIX_sub_process__BIT(6, target_op_ptr);
	}
	void PREFIX_process__BIT_7(uint8_t* target_op_ptr) {
		PREFIX_sub_process__BIT(7, target_op_ptr);
	}

	void PREFIX_sub_process__RES(uint8_t clear_bit, uint8_t* target_op_ptr) {
		if (target_op_ptr != nullptr) {
			(*target_op_ptr) &= (uint8_t)(~(0b00000001 << clear_bit));
		}
		else {
			//(*target_op_ptr) &= (uint8_t)(~(0b00000001 << clear_bit));
			write_RAM_8bit(gbx_register.HL, read_RAM_8bit(gbx_register.HL) & (uint8_t)(~(0b00000001 << clear_bit)));
		}
	}
	void PREFIX_process__RES_0(uint8_t* target_op_ptr) {
		PREFIX_sub_process__RES(0, target_op_ptr);
	}
	void PREFIX_process__RES_1(uint8_t* target_op_ptr) {
		PREFIX_sub_process__RES(1, target_op_ptr);
	}
	void PREFIX_process__RES_2(uint8_t* target_op_ptr) {
		PREFIX_sub_process__RES(2, target_op_ptr);
	}
	void PREFIX_process__RES_3(uint8_t* target_op_ptr) {
		PREFIX_sub_process__RES(3, target_op_ptr);
	}
	void PREFIX_process__RES_4(uint8_t* target_op_ptr) {
		PREFIX_sub_process__RES(4, target_op_ptr);
	}
	void PREFIX_process__RES_5(uint8_t* target_op_ptr) {
		PREFIX_sub_process__RES(5, target_op_ptr);
	}
	void PREFIX_process__RES_6(uint8_t* target_op_ptr) {
		PREFIX_sub_process__RES(6, target_op_ptr);
	}
	void PREFIX_process__RES_7(uint8_t* target_op_ptr) {
		PREFIX_sub_process__RES(7, target_op_ptr);
	}

	void PREFIX_sub_process__SET(uint8_t set_bit, uint8_t* target_op_ptr) {
		if (target_op_ptr != nullptr) {
			(*target_op_ptr) |= (0b00000001 << set_bit);
		}
		else {
			//(*target_op_ptr) |= (0b00000001 << set_bit);
			write_RAM_8bit(gbx_register.HL, read_RAM_8bit(gbx_register.HL) | (0b00000001 << set_bit));
		}
	}
	void PREFIX_process__SET_0(uint8_t* target_op_ptr) {
		PREFIX_sub_process__SET(0, target_op_ptr);
	}
	void PREFIX_process__SET_1(uint8_t* target_op_ptr) {
		PREFIX_sub_process__SET(1, target_op_ptr);
	}
	void PREFIX_process__SET_2(uint8_t* target_op_ptr) {
		PREFIX_sub_process__SET(2, target_op_ptr);
	}
	void PREFIX_process__SET_3(uint8_t* target_op_ptr) {
		PREFIX_sub_process__SET(3, target_op_ptr);
	}
	void PREFIX_process__SET_4(uint8_t* target_op_ptr) {
		PREFIX_sub_process__SET(4, target_op_ptr);
	}
	void PREFIX_process__SET_5(uint8_t* target_op_ptr) {
		PREFIX_sub_process__SET(5, target_op_ptr);
	}
	void PREFIX_process__SET_6(uint8_t* target_op_ptr) {
		PREFIX_sub_process__SET(6, target_op_ptr);
	}
	void PREFIX_process__SET_7(uint8_t* target_op_ptr) {
		PREFIX_sub_process__SET(7, target_op_ptr);
	}


	//=============================================================================
	//=============================================================================

	void cpu_init() {
		/*
		gbx_register.AF = 0;
		gbx_register.BC = 0;
		gbx_register.DE = 0;
		gbx_register.HL = 0;
		gbx_register.SP = 0x0000;
		gbx_register.PC = 0x0000;
		*/

		if (hardware_type == Main::GAME_HARDWARE_TYPE::GAMEBOY) {//ゲームボーイ
			//DMG
			gbx_register.A = 0x01;
			gbx_register.F_Z = 1;
			gbx_register.F_N = 0;
			if (read_RAM_8bit(0x014D) == 0x00) {
				gbx_register.F_H = 0;
				gbx_register.F_C = 0;
			}
			else {
				gbx_register.F_H = 1;
				gbx_register.F_C = 1;
			}
			gbx_register.B = 0x00;
			gbx_register.C = 0x13;
			gbx_register.D = 0x00;
			gbx_register.E = 0xD8;
			gbx_register.H = 0x01;
			gbx_register.L = 0x4D;
			gbx_register.PC = 0x0100;
			gbx_register.SP = 0xFFFE;

			
			gbx_ram.RAM[0xFF00] = 0xCF;
			gbx_ram.RAM[0xFF01] = 0x00;
			gbx_ram.RAM[0xFF02] = 0x7E;
			gbx_ram.RAM[0xFF04] = 0xAB;
			gbx_ram.RAM[0xFF05] = 0x00;
			gbx_ram.RAM[0xFF06] = 0x00;
			gbx_ram.RAM[0xFF07] = 0xF8;
			gbx_ram.RAM[0xFF0F] = 0xE1;
			gbx_ram.RAM[0xFF10] = 0x80;
			gbx_ram.RAM[0xFF11] = 0xBF;
			gbx_ram.RAM[0xFF12] = 0xF3;
			gbx_ram.RAM[0xFF13] = 0xFF;
			gbx_ram.RAM[0xFF14] = 0xBF;
			gbx_ram.RAM[0xFF16] = 0x3F;
			gbx_ram.RAM[0xFF17] = 0x00;
			gbx_ram.RAM[0xFF18] = 0xFF;
			gbx_ram.RAM[0xFF19] = 0xBF;
			gbx_ram.RAM[0xFF1A] = 0x7F;
			gbx_ram.RAM[0xFF1B] = 0xFF;
			gbx_ram.RAM[0xFF1C] = 0x9F;
			gbx_ram.RAM[0xFF1D] = 0xFF;
			gbx_ram.RAM[0xFF1E] = 0xBF;
			gbx_ram.RAM[0xFF20] = 0xFF;
			gbx_ram.RAM[0xFF21] = 0x00;
			gbx_ram.RAM[0xFF22] = 0x00;
			gbx_ram.RAM[0xFF23] = 0xBF;
			gbx_ram.RAM[0xFF24] = 0x77;
			gbx_ram.RAM[0xFF25] = 0xF3;
			gbx_ram.RAM[0xFF26] = 0xF1;
			gbx_ram.RAM[0xFF40] = 0x91;
			gbx_ram.RAM[0xFF41] = 0x85;
			gbx_ram.RAM[0xFF42] = 0x00;
			gbx_ram.RAM[0xFF43] = 0x00;
			gbx_ram.RAM[0xFF44] = 0x00;
			gbx_ram.RAM[0xFF45] = 0x00;
			gbx_ram.RAM[0xFF46] = 0xFF;
			gbx_ram.RAM[0xFF47] = 0xFC;
			gbx_ram.RAM[0xFF48] = 0xFF;
			gbx_ram.RAM[0xFF49] = 0xFF;
			gbx_ram.RAM[0xFF4A] = 0x00;
			gbx_ram.RAM[0xFF4B] = 0x00;
			gbx_ram.RAM[0xFF4D] = 0xFF;
			gbx_ram.RAM[0xFF4F] = 0xFF;
			gbx_ram.RAM[0xFF51] = 0xFF;
			gbx_ram.RAM[0xFF52] = 0xFF;
			gbx_ram.RAM[0xFF53] = 0xFF;
			gbx_ram.RAM[0xFF54] = 0xFF;
			gbx_ram.RAM[0xFF55] = 0xFF;
			gbx_ram.RAM[0xFF56] = 0xFF;
			gbx_ram.RAM[0xFF68] = 0xFF;
			gbx_ram.RAM[0xFF69] = 0xFF;
			gbx_ram.RAM[0xFF6A] = 0xFF;
			gbx_ram.RAM[0xFF6B] = 0xFF;
			gbx_ram.RAM[0xFF70] = 0xFF;
			gbx_ram.RAM[0xFFFF] = 0x00;
		}
		else {//ゲームボーイカラー
			//CGB
			gbx_register.A = 0x11;
			gbx_register.F_Z = 1;
			gbx_register.F_N = 0;
			gbx_register.F_H = 0;
			gbx_register.F_C = 0;
			gbx_register.B = 0x00;
			gbx_register.C = 0x00;
			gbx_register.D = 0xFF;
			gbx_register.E = 0x56;
			gbx_register.H = 0x00;
			gbx_register.L = 0x0D;
			gbx_register.PC = 0x0100;
			gbx_register.SP = 0xFFFE;


			//不明なところは0x00にしておく
			gbx_ram.RAM[0xFF00] = 0xCF;
			gbx_ram.RAM[0xFF01] = 0x00;
			gbx_ram.RAM[0xFF02] = 0x7F;
			gbx_ram.RAM[0xFF04] = 0x00;//??
			gbx_ram.RAM[0xFF05] = 0x00;
			gbx_ram.RAM[0xFF06] = 0x00;
			gbx_ram.RAM[0xFF07] = 0xF8;
			gbx_ram.RAM[0xFF0F] = 0xE1;
			gbx_ram.RAM[0xFF10] = 0x80;
			gbx_ram.RAM[0xFF11] = 0xBF;
			gbx_ram.RAM[0xFF12] = 0xF3;
			gbx_ram.RAM[0xFF13] = 0xFF;
			gbx_ram.RAM[0xFF14] = 0xBF;
			gbx_ram.RAM[0xFF16] = 0x3F;
			gbx_ram.RAM[0xFF17] = 0x00;
			gbx_ram.RAM[0xFF18] = 0xFF;
			gbx_ram.RAM[0xFF19] = 0xBF;
			gbx_ram.RAM[0xFF1A] = 0x7F;
			gbx_ram.RAM[0xFF1B] = 0xFF;
			gbx_ram.RAM[0xFF1C] = 0x9F;
			gbx_ram.RAM[0xFF1D] = 0xFF;
			gbx_ram.RAM[0xFF1E] = 0xBF;
			gbx_ram.RAM[0xFF20] = 0xFF;
			gbx_ram.RAM[0xFF21] = 0x00;
			gbx_ram.RAM[0xFF22] = 0x00;
			gbx_ram.RAM[0xFF23] = 0xBF;
			gbx_ram.RAM[0xFF24] = 0x77;
			gbx_ram.RAM[0xFF25] = 0xF3;
			gbx_ram.RAM[0xFF26] = 0xF1;
			gbx_ram.RAM[0xFF40] = 0x91;
			gbx_ram.RAM[0xFF41] = 0x00;//??
			gbx_ram.RAM[0xFF42] = 0x00;
			gbx_ram.RAM[0xFF43] = 0x00;
			gbx_ram.RAM[0xFF44] = 0x00;//??
			gbx_ram.RAM[0xFF45] = 0x00;
			gbx_ram.RAM[0xFF46] = 0x00;
			gbx_ram.RAM[0xFF47] = 0xFC;
			gbx_ram.RAM[0xFF48] = 0x00;
			gbx_ram.RAM[0xFF49] = 0x00;
			gbx_ram.RAM[0xFF4A] = 0x00;
			gbx_ram.RAM[0xFF4B] = 0x00;
			gbx_ram.RAM[0xFF4D] = 0xFF;
			gbx_ram.RAM[0xFF4F] = 0xFF;
			gbx_ram.RAM[0xFF51] = 0xFF;
			gbx_ram.RAM[0xFF52] = 0xFF;
			gbx_ram.RAM[0xFF53] = 0xFF;
			gbx_ram.RAM[0xFF54] = 0xFF;
			gbx_ram.RAM[0xFF55] = 0xFF;
			gbx_ram.RAM[0xFF56] = 0xFF;
			gbx_ram.RAM[0xFF68] = 0x00;//??
			gbx_ram.RAM[0xFF69] = 0x00;//??
			gbx_ram.RAM[0xFF6A] = 0x00;//??
			gbx_ram.RAM[0xFF6B] = 0x00;//??
			gbx_ram.RAM[0xFF70] = 0xFF;
			gbx_ram.RAM[0xFFFF] = 0x00;
		}
	}

	void ppu_init() {
		memset(_8bit_bg_screen_data_160x144, 0, GBX_WIDTH * GBX_HEIGHT);
		memset(_8bit_window_screen_data_160x144, 0, GBX_WIDTH * GBX_HEIGHT);

		memset(_8bit_backbuffer_data_256x256__mtype0_dtype0, 0, 256 * 256);
		memset(_8bit_backbuffer_data_256x256__mtype0_dtype1, 0, 256 * 256);
		memset(_8bit_backbuffer_data_256x256__mtype1_dtype0, 0, 256 * 256);
		memset(_8bit_backbuffer_data_256x256__mtype1_dtype1, 0, 256 * 256);

		memset(_8bit_sprite_screen_data_160x144, 0, GBX_WIDTH * GBX_HEIGHT);
	}

	void init_util() {
		tmp_CPU_HALT_Flag = false;

		cpu_machine_cycle = 0;
		total_cpu_machine_cycle__div = 0;
		total_cpu_machine_cycle__tima = 0;

		//memset(gbx_ram.RAM, 0, RAM_SIZE);//RAMを初期化してはいけない

		IME_Flag = false;

		rom_bank_no__low = 1;//初期はバンク1を指すようにする
		rom_bank_no__high = 0;
		sram_bank_no = 0;
		SRAM_Enable_Flag = false;
		RTC_Enable_Flag = false;
		IR_Enable_Flag = false;
		bank_mode = BankMode::ROM;

		VRAM_bank_no__cgb = 0;
		memset(VRAM_bank1_data_ptr__cgb, 0, 0x2000);
		WRAM_bank_no__cgb = 1;
		memset(WRAM_bank1_7_data_ptr__cgb, 0, 0x1000 * 7);
	}

	//ROMのCRC32を計算する
	int calc_rom_file_crc32(const char* filename, uint32_t* result) {
		FILE* rom_crc32_fp;
		if (fopen_s(&rom_crc32_fp, filename, "rb") != 0) {
			return -1;
		}

		size_t rom_size = (size_t)_filelengthi64(_fileno(rom_crc32_fp));

		uint8_t* rom_data_buffer__crc32 = (uint8_t*)malloc(rom_size);
		if (rom_data_buffer__crc32 == nullptr) {
			goto calc_rom_crc32_error;
		}
		
		if (fread(rom_data_buffer__crc32, sizeof(uint8_t), rom_size, rom_crc32_fp) != rom_size) {
			goto calc_rom_crc32_error;
		}

		*result = CRC::get_instance_ptr()->crc32(rom_data_buffer__crc32, rom_size);

		free(rom_data_buffer__crc32);

		fclose(rom_crc32_fp);

		return 0;

	calc_rom_crc32_error:
		fclose(rom_crc32_fp);

		return -1;
	}

	int read_rom_file(const char* filename) {
		FILE* rom_fp;
		if (fopen_s(&rom_fp, filename, "rb") != 0) {
			return -1;
		}

		if (fseek(rom_fp, 0x143, SEEK_SET) != 0) {
			goto read_rom_error;
		}
		if (fread(&(rom_info.CGB_Flag), sizeof(uint8_t), 1, rom_fp) != 1) {
			goto read_rom_error;
		}

		if (((rom_info.CGB_Flag) & 0b11000000) == 0b11000000/*ゲームボーイカラー専用*/ ||
			((rom_info.CGB_Flag) & 0b11000000) == 0b10000000/*ゲームボーイカラー対応で、古いゲームボーイでも動く*/)
		{
			hardware_type = Main::GAME_HARDWARE_TYPE::GAMEBOY_COLOR;
		}
		else {/*ゲームボーイ専用(白黒のやつ)*/
			hardware_type = Main::GAME_HARDWARE_TYPE::GAMEBOY;
		}
		Main::game_hardware_type = hardware_type;

		if (fseek(rom_fp, 0x146, SEEK_SET) != 0) {
			goto read_rom_error;
		}
		if (fread(&(rom_info.SGB_Flag), sizeof(uint8_t), 1, rom_fp) != 1) {
			goto read_rom_error;
		}

		if (fseek(rom_fp, 0x147, SEEK_SET) != 0) {
			goto read_rom_error;
		}
		if (fread(&(rom_info.Cartridge_Type), sizeof(uint8_t), 1, rom_fp) != 1) {
			goto read_rom_error;
		}
		Main::Cartridge_Type = rom_info.Cartridge_Type;

		if (fseek(rom_fp, 0x148, SEEK_SET) != 0) {
			goto read_rom_error;
		}
		if (fread(&(rom_info.ROM_Type), sizeof(uint8_t), 1, rom_fp) != 1) {
			goto read_rom_error;
		}
		Main::PGM_size = (32 << rom_info.ROM_Type);

		if (fseek(rom_fp, 0x149, SEEK_SET) != 0) {
			goto read_rom_error;
		}
		if (fread(&(rom_info.SRAM_Type), sizeof(uint8_t), 1, rom_fp) != 1) {
			goto read_rom_error;
		}

		M_debug_printf("################################\n");
		M_debug_printf("CGB_Flag = 0x%02x\n", rom_info.CGB_Flag);
		M_debug_printf("SGB_Flag = 0x%02x\n", rom_info.SGB_Flag);
		M_debug_printf("Cartridge_Type = 0x%02x\n", rom_info.Cartridge_Type);
		M_debug_printf("ROM_Type = 0x%02x\n", rom_info.ROM_Type);
		M_debug_printf("SRAM_Type = 0x%02x\n", rom_info.SRAM_Type);
		M_debug_printf("################################\n");

		if (fseek(rom_fp, 0x0, SEEK_SET) != 0) {
			goto read_rom_error;
		}
		if (fread(&(gbx_ram.RAM), sizeof(uint8_t), 0x8000, rom_fp) != 0x8000) {
			goto read_rom_error;
		}

		if (0x08 < rom_info.ROM_Type) {//0x09以降は未対応
			goto read_rom_error;
		}

		if (Main::PGM_size > 32) {//ROMが32KBより大きいときはバンクを使う
			const size_t ROM_BANK_DATA_SIZE = 1024 * (Main::PGM_size - 32);

			ROM_bank_data_ptr = (uint8_t*)malloc(ROM_BANK_DATA_SIZE);
			if (ROM_bank_data_ptr == NULL) {
				goto read_rom_error;
			}

			if (fread(ROM_bank_data_ptr, sizeof(uint8_t), ROM_BANK_DATA_SIZE, rom_fp) != ROM_BANK_DATA_SIZE) {
				goto read_rom_error;
			}
		}

		if (rom_info.SRAM_Type == 0) {//RAM無し
			//なにもしない
			Main::SRAM_size = 0;
		}
		else if (rom_info.SRAM_Type == 1) {//不使用
			//なにもしない
			Main::SRAM_size = 0;
		}
		else if (rom_info.SRAM_Type == 2) {//8 KB (1バンク)
			//1バンクなので追加の領域は必要ない
			Main::SRAM_size = 8;
		}
		else if (rom_info.SRAM_Type == 3) {//32 KB (4バンク)
			Main::SRAM_size = 32;
		}
		else if (rom_info.SRAM_Type == 4) {//128 KB (16バンク)
			Main::SRAM_size = 128;
		}
		else if (rom_info.SRAM_Type == 5) {//64 KB (8バンク)
			Main::SRAM_size = 64;
		}
		else {//それ以外は未対応
			goto read_rom_error;
		}

		if (Main::SRAM_size > 8) {
			const size_t SRAM_BANK_DATA_SIZE = 1024 * (Main::SRAM_size - 8);

			SRAM_bank_data_ptr = (uint8_t*)malloc(SRAM_BANK_DATA_SIZE);
			if (SRAM_bank_data_ptr == NULL) {
				goto read_rom_error;
			}

			memset(SRAM_bank_data_ptr, 0, SRAM_BANK_DATA_SIZE);
		}


		if (Main::Cartridge_Type == 0x00) {
			cart_mbc_type = CART_MBC_TYPE::ROM;
		}
		else if (Main::Cartridge_Type == 0x01) {
			cart_mbc_type = CART_MBC_TYPE::MBC1;
		}
		else if (Main::Cartridge_Type == 0x02) {
			cart_mbc_type = CART_MBC_TYPE::MBC1;
		}
		else if (Main::Cartridge_Type == 0x03) {
			cart_mbc_type = CART_MBC_TYPE::MBC1;
		}
		else if (Main::Cartridge_Type == 0x05) {
			cart_mbc_type = CART_MBC_TYPE::MBC2;
		}
		else if (Main::Cartridge_Type == 0x06) {
			cart_mbc_type = CART_MBC_TYPE::MBC2;
		}
		else if (Main::Cartridge_Type == 0x08) {
			cart_mbc_type = CART_MBC_TYPE::ROM;
		}
		else if (Main::Cartridge_Type == 0x09) {
			cart_mbc_type = CART_MBC_TYPE::ROM;
		}
		else if (Main::Cartridge_Type == 0x0B) {
			cart_mbc_type = CART_MBC_TYPE::OTHER;
		}
		else if (Main::Cartridge_Type == 0x0C) {
			cart_mbc_type = CART_MBC_TYPE::OTHER;
		}
		else if (Main::Cartridge_Type == 0x0D) {
			cart_mbc_type = CART_MBC_TYPE::OTHER;
		}
		else if (Main::Cartridge_Type == 0x0F) {
			cart_mbc_type = CART_MBC_TYPE::MBC3;
		}
		else if (Main::Cartridge_Type == 0x10) {
			cart_mbc_type = CART_MBC_TYPE::MBC3;
		}
		else if (Main::Cartridge_Type == 0x11) {
			cart_mbc_type = CART_MBC_TYPE::MBC3;
		}
		else if (Main::Cartridge_Type == 0x12) {
			cart_mbc_type = CART_MBC_TYPE::MBC3;
		}
		else if (Main::Cartridge_Type == 0x13) {
			cart_mbc_type = CART_MBC_TYPE::MBC3;
		}
		else if (Main::Cartridge_Type == 0x19) {
			cart_mbc_type = CART_MBC_TYPE::MBC5;
		}
		else if (Main::Cartridge_Type == 0x1A) {
			cart_mbc_type = CART_MBC_TYPE::MBC5;
		}
		else if (Main::Cartridge_Type == 0x1B) {
			cart_mbc_type = CART_MBC_TYPE::MBC5;
		}
		else if (Main::Cartridge_Type == 0x1C) {
			cart_mbc_type = CART_MBC_TYPE::MBC5;
		}
		else if (Main::Cartridge_Type == 0x1D) {
			cart_mbc_type = CART_MBC_TYPE::MBC5;
		}
		else if (Main::Cartridge_Type == 0x1E) {
			cart_mbc_type = CART_MBC_TYPE::MBC5;
		}
		else if (Main::Cartridge_Type == 0x20) {
			cart_mbc_type = CART_MBC_TYPE::OTHER;
		}
		else if (Main::Cartridge_Type == 0x22) {
			cart_mbc_type = CART_MBC_TYPE::OTHER;
		}
		else if (Main::Cartridge_Type == 0xFC) {
			cart_mbc_type = CART_MBC_TYPE::OTHER;
		}
		else if (Main::Cartridge_Type == 0xFD) {
			cart_mbc_type = CART_MBC_TYPE::OTHER;
		}
		else if (Main::Cartridge_Type == 0xFE) {
			cart_mbc_type = CART_MBC_TYPE::OTHER;
		}
		else if (Main::Cartridge_Type == 0xFF) {
			cart_mbc_type = CART_MBC_TYPE::HuC1;
		}
		else {
			cart_mbc_type = CART_MBC_TYPE::OTHER;
		}


		fclose(rom_fp);

		return 0;

	read_rom_error:
		fclose(rom_fp);

		return -1;
	}

#define POCKET_MONSTER_CRYSTAL_JPN__CRC32 0x270C4ECC
	/*
	CRC32の計算結果でソフトを判断してソフトによっては動作するようにパッチをあてる

	モバイルシステムGBなどをとばす
	*/
	void patch_read_rom_data(uint32_t rom_crc32) {
		if (rom_crc32 == POCKET_MONSTER_CRYSTAL_JPN__CRC32) {//ポケットモンスタークリスタル(日本版)
			execute_patch__POKEMON_CRYSTAL_JPN();
		}
	}

	//ポケットモンスタークリスタル(日本版)
	void execute_patch__POKEMON_CRYSTAL_JPN() {
		/*
		モバイルシステムGBの初期化？の処理を回避する
		
		<before>
		ROM1:6590	CD C2 31	call 31C2
		ROM1:6593	3E 5B		ld a,5B		;バンク0x5B
		ROM1:6595	21 00 40	ld hl,4000	;ジャンプ先のアドレス0x4000
		ROM1:6598	CF			rst 08		;上2行で指定した場所にジャンプする
		ROM1:6599	C3 85 63	jp 6385

		<after>
		ROM1:6590	CD C2 31	call 31C2
		ROM1:6593	00 			nop
		ROM1:6594	00 			nop
		ROM1:6595	00 			nop
		ROM1:6596	00 			nop
		ROM1:6597	00 			nop
		ROM1:6598	00 			nop
		ROM1:6599	C3 85 63	jp 6385
		*/
		gbx_ram.RAM[0x6593] = 0x00;
		gbx_ram.RAM[0x6594] = 0x00;
		gbx_ram.RAM[0x6595] = 0x00;
		gbx_ram.RAM[0x6596] = 0x00;
		gbx_ram.RAM[0x6597] = 0x00;
		gbx_ram.RAM[0x6598] = 0x00;

		M_debug_printf("execute_patch__POKEMON_CRYSTAL_JPN() END...\n");
	}

	uint8_t* get_backbuffer_data_256x256_ptr(bool tilemap_type1_flag, bool tiledata_type1_flag) {
		if (tilemap_type1_flag == false && tiledata_type1_flag == false) {
			return _8bit_backbuffer_data_256x256__mtype0_dtype0;
		}
		else if (tilemap_type1_flag == false && tiledata_type1_flag == true) {
			return _8bit_backbuffer_data_256x256__mtype0_dtype1;
		}
		else if (tilemap_type1_flag == true && tiledata_type1_flag == false) {
			return _8bit_backbuffer_data_256x256__mtype1_dtype0;
		}
		//else if (tilemap_type1_flag == true && tiledata_type1_flag == true) {
		else {
			return _8bit_backbuffer_data_256x256__mtype1_dtype1;
		}
	}

	void draw_backbuffer_bg_1pixel(uint8_t screen_x, uint8_t screen_y) {
		//M_debug_printf("screen_x = %d, screen_y = %d\n", screen_x, screen_y);

		if ((gbx_ram.RAM[0xFF40] & 0b00000001) == 0) {//背景/ウィンドウが無効であるとき
			return;
		}

		bool tilemap_type1_flag;
		bool tiledata_type1_flag;

		if ((gbx_ram.RAM[0xFF40] & 0b00001000) == 0) {
			tilemap_type1_flag = false;
		}
		else {
			tilemap_type1_flag = true;
		}

		if ((gbx_ram.RAM[0xFF40] & 0b00010000) == 0) {
			tiledata_type1_flag = false;
		}
		else {
			tiledata_type1_flag = true;
		}

		uint8_t* _8bit_bg_backbuffer_data_256x256__ptr = get_backbuffer_data_256x256_ptr(tilemap_type1_flag, tiledata_type1_flag);

		uint8_t scroll_x = gbx_ram.RAM[0xFF43];
		uint8_t scroll_y = gbx_ram.RAM[0xFF42];

		uint8_t color_2bit = _8bit_bg_backbuffer_data_256x256__ptr[(256 * (uint8_t)(scroll_y + screen_y)) + (uint8_t)(scroll_x + screen_x)];
		if ((color_2bit & 0b00000011) != 0) {//背景色でないとき
			backbuffer_isnobackgroundcolor_mask[screen_x + screen_y * GBX_WIDTH] = true;
		}
		//else {
		//	backbuffer_sprite_mask[screen_x + screen_y * GBX_WIDTH] = false;
		//}
		_8bit_bg_screen_data_160x144[screen_x + screen_y * GBX_WIDTH] = color_2bit;
	}

	void draw_backbuffer_bg_1pixel__cgb(uint8_t screen_x, uint8_t screen_y) {
		//M_debug_printf("screen_x = %d, screen_y = %d\n", screen_x, screen_y);

		bool tilemap_type1_flag;
		bool tiledata_type1_flag;

		if ((gbx_ram.RAM[0xFF40] & 0b00001000) == 0) {
			tilemap_type1_flag = false;
		}
		else {
			tilemap_type1_flag = true;
		}

		if ((gbx_ram.RAM[0xFF40] & 0b00010000) == 0) {
			tiledata_type1_flag = false;
		}
		else {
			tiledata_type1_flag = true;
		}

		uint8_t* _8bit_bg_backbuffer_data_256x256__ptr = get_backbuffer_data_256x256_ptr(tilemap_type1_flag, tiledata_type1_flag);

		uint8_t scroll_x = gbx_ram.RAM[0xFF43];
		uint8_t scroll_y = gbx_ram.RAM[0xFF42];

		uint8_t pixel_priority_1bit_palette_3bit_color_2bit = _8bit_bg_backbuffer_data_256x256__ptr[(256 * (uint8_t)(scroll_y + screen_y)) + (uint8_t)(scroll_x + screen_x)];
		if ((pixel_priority_1bit_palette_3bit_color_2bit & 0b00000011) != 0) {//背景色でないとき
			backbuffer_isnobackgroundcolor_mask[screen_x + screen_y * GBX_WIDTH] = true;
		}
		//else {
		//	backbuffer_sprite_mask[screen_x + screen_y * GBX_WIDTH] = false;
		//}
		if ((pixel_priority_1bit_palette_3bit_color_2bit & 0b00100000) != 0) {
			BG_0bit_attribute_flag__cgb[screen_x + screen_y * GBX_WIDTH] = true;
		}
		if ((gbx_ram.RAM[0xFF40] & 0b00000001) != 0) {//背景/ウィンドウのマスター優先度のビットが0のとき
			LCDC_0bit_master_flag__cgb[screen_x + screen_y * GBX_WIDTH] = true;
			//backbuffer_isnobackgroundcolor_mask[screen_x + screen_y * GBX_WIDTH] = false;//優先順位を強制的に下げる
		}
		_8bit_bg_screen_data_160x144[screen_x + screen_y * GBX_WIDTH] = pixel_priority_1bit_palette_3bit_color_2bit;
	}


	/*
	1ラインごとにウインドウの内部情報を更新するためのやつ
	*/
	void update_window_flag__1line() {
		if (window_backbuffer_draw_internal_flag_x == true) {
			window_backbuffer_draw_internal_flag_x = false;

			window_backbuffer_draw_internal_counter_y++;
		}
	}
	bool window_backbuffer_draw_internal_flag_x = false;
	uint32_t window_backbuffer_draw_internal_counter_y = 0;
	void draw_backbuffer_window_1pixel(uint8_t screen_x, uint8_t screen_y) {
		if ((gbx_ram.RAM[0xFF40] & 0b00000001) == 0) {//背景/ウィンドウが無効であるとき
			return;
		}

		if ((gbx_ram.RAM[0xFF40] & 0b00100000) == 0) {//ウィンドウが無効であるとき
			return;
		}

		bool tilemap_type1_flag;
		bool tiledata_type1_flag;

		if ((gbx_ram.RAM[0xFF40] & 0b01000000) == 0) {
			tilemap_type1_flag = false;
		}
		else {
			tilemap_type1_flag = true;
		}

		if ((gbx_ram.RAM[0xFF40] & 0b00010000) == 0) {
			tiledata_type1_flag = false;
		}
		else {
			tiledata_type1_flag = true;
		}

		uint8_t* _8bit_window_backbuffer_data_256x256__ptr = get_backbuffer_data_256x256_ptr(tilemap_type1_flag, tiledata_type1_flag);

		uint8_t window_x = gbx_ram.RAM[0xFF4B] - 7;
		uint8_t window_y = gbx_ram.RAM[0xFF4A];

		if (screen_x < window_x ||
			screen_y < window_y)
		{
			//透明の部分
			
			//_8bit_window_screen_data_160x144[screen_x + screen_y * GBX_WIDTH] = 0xFF;
		
			return;
		}

		uint8_t color_2bit = _8bit_window_backbuffer_data_256x256__ptr[(uint8_t)(screen_x - window_x) + ((uint8_t)window_backbuffer_draw_internal_counter_y/*(screen_y - window_y)*/ * 256)];
		if ((color_2bit & 0b00000011) != 0) {//背景色でないとき
			backbuffer_isnobackgroundcolor_mask[screen_x + screen_y * GBX_WIDTH] = true;
		}
		//else {
		//	backbuffer_sprite_mask[screen_x + screen_y * GBX_WIDTH] = false;
		//}
		_8bit_window_screen_data_160x144[screen_x + screen_y * GBX_WIDTH] = color_2bit;

		window_backbuffer_draw_internal_flag_x = true;//このラインでウインドウが描画されたフラグをたてる
	}

	void draw_backbuffer_window_1pixel__cgb(uint8_t screen_x, uint8_t screen_y) {
		if ((gbx_ram.RAM[0xFF40] & 0b00100000) == 0) {//ウィンドウが無効であるとき
			return;
		}

		bool tilemap_type1_flag;
		bool tiledata_type1_flag;

		if ((gbx_ram.RAM[0xFF40] & 0b01000000) == 0) {
			tilemap_type1_flag = false;
		}
		else {
			tilemap_type1_flag = true;
		}

		if ((gbx_ram.RAM[0xFF40] & 0b00010000) == 0) {
			tiledata_type1_flag = false;
		}
		else {
			tiledata_type1_flag = true;
		}

		uint8_t* _8bit_window_backbuffer_data_256x256__ptr = get_backbuffer_data_256x256_ptr(tilemap_type1_flag, tiledata_type1_flag);

		uint8_t window_x = gbx_ram.RAM[0xFF4B] - 7;
		uint8_t window_y = gbx_ram.RAM[0xFF4A];

		if (screen_x < window_x ||
			screen_y < window_y)
		{
			//透明の部分

			//_8bit_window_screen_data_160x144[screen_x + screen_y * GBX_WIDTH] = 0xFF;

			return;
		}

		uint8_t pixel_priority_1bit_palette_3bit_color_2bit = _8bit_window_backbuffer_data_256x256__ptr[(uint8_t)(screen_x - window_x) + ((uint8_t)window_backbuffer_draw_internal_counter_y/*(screen_y - window_y)*/ * 256)];
		if ((pixel_priority_1bit_palette_3bit_color_2bit & 0b00000011) != 0) {//背景色でないとき
			backbuffer_isnobackgroundcolor_mask[screen_x + screen_y * GBX_WIDTH] = true;
		}
		//else {
		//	backbuffer_sprite_mask[screen_x + screen_y * GBX_WIDTH] = false;
		//}
		if ((pixel_priority_1bit_palette_3bit_color_2bit & 0b00100000) != 0) {
			BG_0bit_attribute_flag__cgb[screen_x + screen_y * GBX_WIDTH] = true;
		}
		if ((gbx_ram.RAM[0xFF40] & 0b00000001) != 0) {//背景/ウィンドウのマスター優先度のビットが0のとき
			LCDC_0bit_master_flag__cgb[screen_x + screen_y * GBX_WIDTH] = true;
			//backbuffer_isnobackgroundcolor_mask[screen_x + screen_y * GBX_WIDTH] = false;//優先順位を強制的に下げる
		}
		_8bit_window_screen_data_160x144[screen_x + screen_y * GBX_WIDTH] = pixel_priority_1bit_palette_3bit_color_2bit;

		window_backbuffer_draw_internal_flag_x = true;//このラインでウインドウが描画されたフラグをたてる
	}

	void _abstruct__create_256_256_backbuffer(uint8_t* backbuffer_data_ptr, bool tilemap_type1_flag, bool tiledata_type1_flag) {
		for (int i = 0; i < 32; i++) {
			for (int j = 0; j < 32; j++) {
				int8_t tile_no;//符号あり
				if (tilemap_type1_flag == false) {
					tile_no = (int8_t)(gbx_ram.RAM[0x9800 + (j + i * 32)]);
				}
				else {
					tile_no = (int8_t)(gbx_ram.RAM[0x9C00 + (j + i * 32)]);
				}

				uint8_t* tile_data_ptr;
				if (tiledata_type1_flag == true) {
					//tile_data_ptr = &(gbx_ram.RAM[0x8000]);

					tile_data_ptr = get_read_RAM_address___(0x8000);
				}
				else {
					//tile_data_ptr = &(gbx_ram.RAM[0x9000]);

					tile_data_ptr = get_read_RAM_address___(0x9000);
				}

				//M_debug_printf("[index = %d]tile_no = %c[%02x]\n", (j + i * 32), tile_no, tile_no);

				for (int y = 0; y < 8; y++) {
					for (int x = 0; x < 8; x++) {
						uint8_t pixel_color_2bit;

						if (tiledata_type1_flag == true) {//タイル番号の符号なしで計算する
							pixel_color_2bit = (((tile_data_ptr[0x10 * (uint8_t)tile_no + y * 2]) >> (7 - x)) & 0b00000001) |
								((((tile_data_ptr[0x10 * (uint8_t)tile_no + y * 2 + 1]) >> (7 - x)) & 0b00000001) << 1);
						}
						else {//タイル番号の符号ありで計算する
							pixel_color_2bit = (((tile_data_ptr[0x10 * tile_no + y * 2]) >> (7 - x)) & 0b00000001) |
								((((tile_data_ptr[0x10 * tile_no + y * 2 + 1]) >> (7 - x)) & 0b00000001) << 1);
						}

						backbuffer_data_ptr[(8 * 256 * i + 256 * y) + (j * 8 + x)] = pixel_color_2bit;
					}
				}
			}

		}
	}

	uint8_t* get_background_map_attribute_data_ptr1() {
		return &(VRAM_bank1_data_ptr__cgb[0x1800]);
	}

	uint8_t* get_background_map_attribute_data_ptr2() {
		return &(VRAM_bank1_data_ptr__cgb[0x1C00]);
	}

	uint8_t* get_tiledata_bank0_ptr() {
		return (&(gbx_ram.RAM[0x8000]));
	}

	uint8_t* get_tiledata_bank1_ptr() {
		return VRAM_bank1_data_ptr__cgb;
	}

	void _abstruct__create_256_256_backbuffer__cgb(uint8_t* backbuffer_data_ptr, bool tilemap_type1_flag, bool tiledata_type1_flag) {
		uint8_t* tile_palette_3bit_data_ptr;
		if (tilemap_type1_flag == false) {
			tile_palette_3bit_data_ptr = get_background_map_attribute_data_ptr1();
		}
		else {
			tile_palette_3bit_data_ptr = get_background_map_attribute_data_ptr2();
		}

		for (int i = 0; i < 32; i++) {
			for (int j = 0; j < 32; j++) {
				uint8_t tilemap_attribute = tile_palette_3bit_data_ptr[j + i * 32];
				uint8_t tile_palette_3bit = (tilemap_attribute & 0b111);
				bool tile_data_bank1_flag = ((tilemap_attribute & 0b00001000) != 0) ? true : false;
				bool reverse_x_flag = ((tilemap_attribute & 0b00100000) != 0) ? true : false;
				bool reverse_y_flag = ((tilemap_attribute & 0b01000000) != 0) ? true : false;
				bool max_priority_flag = ((tilemap_attribute & 0b10000000) != 0) ? true : false;

				int8_t tile_no;//符号あり
				if (tilemap_type1_flag == false) {
					tile_no = (int8_t)(gbx_ram.RAM[0x9800 + (j + i * 32)]);
				}
				else {
					tile_no = (int8_t)(gbx_ram.RAM[0x9C00 + (j + i * 32)]);
				}

				//uint8_t* tile_data_ptr;
				//if (tiledata_type1_flag == true) {
				//	//tile_data_ptr = &(gbx_ram.RAM[0x8000]);
				//
				//	tile_data_ptr = get_read_RAM_address___(0x8000);
				//}
				//else {
				//	//tile_data_ptr = &(gbx_ram.RAM[0x9000]);
				//
				//	tile_data_ptr = get_read_RAM_address___(0x9000);
				//}
				uint8_t* tile_data_ptr;
				if (tile_data_bank1_flag == false) {
					tile_data_ptr = get_tiledata_bank0_ptr();
				}
				else {
					tile_data_ptr = get_tiledata_bank1_ptr();
				}
				//if (tiledata_type1_flag == true) {
				//
				//}
				//else {
				if (tiledata_type1_flag == false) {
					tile_data_ptr += 0x1000;
				}

				//M_debug_printf("[index = %d]tile_no = %c[%02x]\n", (j + i * 32), tile_no, tile_no);

				for (int y = 0; y < 8; y++) {
					for (int x = 0; x < 8; x++) {
						uint8_t pixel_priority_1bit_palette_3bit_color_2bit;

						uint8_t x_offset;
						if (reverse_x_flag == false) {
							x_offset = (7 - x);
						}
						else {
							x_offset = x;
						}
						uint8_t y_offset;
						if (reverse_y_flag == false) {
							y_offset = y;
						}
						else {
							y_offset = (7 - y);
						}
						if (tiledata_type1_flag == true) {//タイル番号の符号なしで計算する
							pixel_priority_1bit_palette_3bit_color_2bit = (((tile_data_ptr[0x10 * (uint8_t)tile_no + y_offset * 2]) >> x_offset) & 0b00000001) |
								((((tile_data_ptr[0x10 * (uint8_t)tile_no + y_offset * 2 + 1]) >> x_offset) & 0b00000001) << 1);
						}
						else {//タイル番号の符号ありで計算する
							pixel_priority_1bit_palette_3bit_color_2bit = (((tile_data_ptr[0x10 * tile_no + y_offset * 2]) >> x_offset) & 0b00000001) |
								((((tile_data_ptr[0x10 * tile_no + y_offset * 2 + 1]) >> x_offset) & 0b00000001) << 1);
						}

						pixel_priority_1bit_palette_3bit_color_2bit |= (tile_palette_3bit << 2);

						if (max_priority_flag == true) {
							pixel_priority_1bit_palette_3bit_color_2bit |= 0b00100000;
						}

						backbuffer_data_ptr[(8 * 256 * i + 256 * y) + (j * 8 + x)] = pixel_priority_1bit_palette_3bit_color_2bit;
					}
				}
			}

		}
	}

	void create_all_256x256_backbuffer() {
		/*
		uint8_t _8bit_backbuffer_data_256x256__mtype0_dtype0[256 * 256];
		uint8_t _8bit_backbuffer_data_256x256__mtype0_dtype1[256 * 256];
		uint8_t _8bit_backbuffer_data_256x256__mtype1_dtype0[256 * 256];
		uint8_t _8bit_backbuffer_data_256x256__mtype1_dtype1[256 * 256];
		*/

		if (hardware_type == Main::GAME_HARDWARE_TYPE::GAMEBOY) {//カラーでないとき
			_abstruct__create_256_256_backbuffer(_8bit_backbuffer_data_256x256__mtype0_dtype0, false, false);
			_abstruct__create_256_256_backbuffer(_8bit_backbuffer_data_256x256__mtype0_dtype1, false, true);
			_abstruct__create_256_256_backbuffer(_8bit_backbuffer_data_256x256__mtype1_dtype0, true, false);
			_abstruct__create_256_256_backbuffer(_8bit_backbuffer_data_256x256__mtype1_dtype1, true, true);
		}
		else {
			_abstruct__create_256_256_backbuffer__cgb(_8bit_backbuffer_data_256x256__mtype0_dtype0, false, false);
			_abstruct__create_256_256_backbuffer__cgb(_8bit_backbuffer_data_256x256__mtype0_dtype1, false, true);
			_abstruct__create_256_256_backbuffer__cgb(_8bit_backbuffer_data_256x256__mtype1_dtype0, true, false);
			_abstruct__create_256_256_backbuffer__cgb(_8bit_backbuffer_data_256x256__mtype1_dtype1, true, true);
		}
	}

	//==================================================================================

	void execute_draw_screenbuffer_1sprite_8x8_data(int32_t sprite_x, int32_t sprite_y, uint8_t tile_no, bool sprite_reverse_x_flag, bool sprite_reverse_y_flag, bool palette_OBP1_flag, bool sprite_max_priority_flag) {
		//uint8_t* tile_data_ptr = &(gbx_ram.RAM[0x8000]);
		uint8_t* tile_data_ptr = get_read_RAM_address___(0x8000);

		for (int y = 0; y < 8; y++) {
			for (int x = 0; x < 8; x++) {
				int32_t offset_X = sprite_x + x;
				int32_t offset_Y = sprite_y + y;

				if (offset_X < 0 || offset_Y < 0 || GBX_WIDTH <= offset_X || GBX_HEIGHT <= offset_Y) {//画面外は描画しない
					continue;
				}

				uint8_t pixel_color_x_shift_bit;
				if (sprite_reverse_x_flag == true) {
					pixel_color_x_shift_bit = x;
				}
				else {
					pixel_color_x_shift_bit = (7 - x);
				}
				uint8_t pixel_color_y_shift_bit;
				if (sprite_reverse_y_flag == true) {
					pixel_color_y_shift_bit = (7 - y);
				}
				else {
					pixel_color_y_shift_bit = y;
				}
				uint8_t pixel_color_2bit;
				pixel_color_2bit = (((tile_data_ptr[0x10 * tile_no + pixel_color_y_shift_bit * 2]) >> pixel_color_x_shift_bit) & 0b00000001) |
					((((tile_data_ptr[0x10 * tile_no + pixel_color_y_shift_bit * 2 + 1]) >> pixel_color_x_shift_bit) & 0b00000001) << 1);

				if (palette_OBP1_flag == true) {//パレットが1のときは最上位ビットを1にしておく
					pixel_color_2bit |= 0b10000000;
				}

				if (sprite_max_priority_flag == false) {//優先順位が低いときは最上位から２番目のビットを1にしておく
					pixel_color_2bit |= 0b01000000;
				}

				if ((pixel_color_2bit & 0b00111111) == 0) {//透明部分はとばす
					continue;
				}

				_8bit_sprite_screen_data_160x144[offset_X + (offset_Y * GBX_WIDTH)] = pixel_color_2bit;
			}
		}
	}

	struct Sprite_Info {
		uint16_t address_index = 0x0000;

		uint32_t x = 0;
		uint32_t y = 0;
		uint8_t tile_no = 0;
		bool sprite_max_priority_flag = false;
		bool sprite_reverse_y_flag = false;
		bool sprite_reverse_x_flag = false;
		bool palette_OBP1_flag = false;
		bool size_8x16_flag = false;

		//アドレスが大きい順に並べ替えるために使う比較関数
		bool operator<(const Sprite_Info& right) const {
			return (address_index < right.address_index) ? false : true;
		}
	};
	vector<Sprite_Info> sprite_info_list;
	void init_sprite_info_list() {
		sprite_info_list.clear();
		sprite_info_list.shrink_to_fit();
	}
	/*
	画面に表示されない16ラインにスプライトがあるかチェックする
	*/
	void check_sprite_upside_16line() {
		for (int y = -16; y < 0; y++) {
			for (int x = -8; x < GBX_WIDTH; x++) {//X座標左端もチェックする
				create_screen_sprite_data__1pixel(x, y);
			}
		}
	}

	void create_screen_sprite_data__1pixel(uint32_t pixel_x, uint32_t pixel_y) {
		if ((gbx_ram.RAM[0xFF40] & 0b00000010) == 0) {//スプライトが無効なとき
			return;
		}

		for (int i = 0; i < 40; i++) {
			Sprite_Info s_info;

			s_info.address_index = (4 * i);//スプライト間の描画優先順位決定のためにアドレスのインデックスを保存しておく

			/*
			s_info.x, s_info.x はスクリーンの座標
			*/
			s_info.y = (gbx_ram.RAM[0xFE00 + (4 * i)] - 16);
			s_info.x = (gbx_ram.RAM[0xFE00 + (4 * i) + 1] - 8);
			if (!(pixel_x == s_info.x && pixel_y == s_info.y)) {
				continue;
			}
			s_info.tile_no = gbx_ram.RAM[0xFE00 + (4 * i) + 2];
			uint8_t attribute = gbx_ram.RAM[0xFE00 + (4 * i) + 3];

			s_info.sprite_max_priority_flag = ((attribute & 0b10000000) != 0) ? false : true;
			s_info.sprite_reverse_y_flag = ((attribute & 0b01000000) != 0) ? true : false;
			s_info.sprite_reverse_x_flag = ((attribute & 0b00100000) != 0) ? true : false;
			s_info.palette_OBP1_flag = ((attribute & 0b00010000) != 0) ? true : false;

			if ((gbx_ram.RAM[0xFF40] & 0b00000100) == 0) {//8x8のとき
				s_info.size_8x16_flag = false;
			}
			else {//8x16のとき
				s_info.size_8x16_flag = true;
			}

			sprite_info_list.push_back(s_info);

		}
	}

	void draw_screenbuffer_sprite_data() {
		//M_debug_printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
		//for (int k = 0; k < sprite_info_list.size(); k++) {
		//	M_debug_printf("BEF sprite_info_list[%d].address_index = 0x%04x\n", k, sprite_info_list[k].address_index);
		//}
		sort(sprite_info_list.begin(), sprite_info_list.end());
		//for (int k = 0; k < sprite_info_list.size(); k++) {
		//	M_debug_printf("AFT sprite_info_list[%d].address_index = 0x%04x\n", k, sprite_info_list[k].address_index);
		//}

		for (int i = 0; i < sprite_info_list.size(); i++) {
			Sprite_Info s_info = sprite_info_list[i];

			if (s_info.size_8x16_flag == false) {//8x8のとき
				execute_draw_screenbuffer_1sprite_8x8_data(s_info.x, s_info.y, s_info.tile_no, s_info.sprite_reverse_x_flag, s_info.sprite_reverse_y_flag, s_info.palette_OBP1_flag, s_info.sprite_max_priority_flag);
			}
			else {//8x16のとき
				if (s_info.sprite_reverse_y_flag == false) {
					execute_draw_screenbuffer_1sprite_8x8_data(s_info.x, s_info.y, (s_info.tile_no & 0b11111110), s_info.sprite_reverse_x_flag, s_info.sprite_reverse_y_flag, s_info.palette_OBP1_flag, s_info.sprite_max_priority_flag);
					execute_draw_screenbuffer_1sprite_8x8_data(s_info.x, s_info.y + 8, (uint8_t)(s_info.tile_no | 0b00000001), s_info.sprite_reverse_x_flag, s_info.sprite_reverse_y_flag, s_info.palette_OBP1_flag, s_info.sprite_max_priority_flag);
				}
				else {
					execute_draw_screenbuffer_1sprite_8x8_data(s_info.x, s_info.y, (uint8_t)(s_info.tile_no | 0b00000001), s_info.sprite_reverse_x_flag, s_info.sprite_reverse_y_flag, s_info.palette_OBP1_flag, s_info.sprite_max_priority_flag);
					execute_draw_screenbuffer_1sprite_8x8_data(s_info.x, s_info.y + 8, (s_info.tile_no & 0b11111110), s_info.sprite_reverse_x_flag, s_info.sprite_reverse_y_flag, s_info.palette_OBP1_flag, s_info.sprite_max_priority_flag);
				}
			}
		}
	}

	//=======================================================================
	//=======================================================================

	void execute_draw_screenbuffer_1sprite_8x8_data__cgb(int32_t sprite_x, int32_t sprite_y, uint8_t tile_no, bool sprite_reverse_x_flag, bool sprite_reverse_y_flag, bool sprite_max_priority_flag, bool tile_data_bank1_flag, uint8_t sprite_palette_no_3bit) {
		//uint8_t* tile_data_ptr = &(gbx_ram.RAM[0x8000]);
		//uint8_t* tile_data_ptr = get_read_RAM_address___(0x8000);
		uint8_t* tile_data_ptr;
		if (tile_data_bank1_flag == false) {
			tile_data_ptr = get_tiledata_bank0_ptr();
		}
		else {
			tile_data_ptr = get_tiledata_bank1_ptr();
		}

		for (int y = 0; y < 8; y++) {
			for (int x = 0; x < 8; x++) {
				int32_t offset_X = sprite_x + x;
				int32_t offset_Y = sprite_y + y;

				if (offset_X < 0 || offset_Y < 0 || GBX_WIDTH <= offset_X || GBX_HEIGHT <= offset_Y) {//画面外は描画しない
					continue;
				}

				uint8_t pixel_color_x_shift_bit;
				if (sprite_reverse_x_flag == true) {
					pixel_color_x_shift_bit = x;
				}
				else {
					pixel_color_x_shift_bit = (7 - x);
				}
				uint8_t pixel_color_y_shift_bit;
				if (sprite_reverse_y_flag == true) {
					pixel_color_y_shift_bit = (7 - y);
				}
				else {
					pixel_color_y_shift_bit = y;
				}
				uint8_t pixel_priority_1bit_palette_3bit_color_2bit;
				pixel_priority_1bit_palette_3bit_color_2bit = (((tile_data_ptr[0x10 * tile_no + pixel_color_y_shift_bit * 2]) >> pixel_color_x_shift_bit) & 0b00000001) |
					((((tile_data_ptr[0x10 * tile_no + pixel_color_y_shift_bit * 2 + 1]) >> pixel_color_x_shift_bit) & 0b00000001) << 1);

				//if (palette_OBP1_flag == true) {//パレットが1のときは最上位ビットを1にしておく
				//	pixel_color_2bit |= 0b10000000;
				//}
				//
				//if (sprite_max_priority_flag == false) {//優先順位が低いときは最上位から２番目のビットを1にしておく
				//	pixel_color_2bit |= 0b01000000;
				//}
				//
				//if ((pixel_color_2bit & 0b00111111) == 0) {//透明部分はとばす
				//	continue;
				//}

				pixel_priority_1bit_palette_3bit_color_2bit |= ((sprite_palette_no_3bit & 0b111) << 2);
				if (sprite_max_priority_flag == true) {//優先順位を記録しておく
					pixel_priority_1bit_palette_3bit_color_2bit |= 0b00100000;
				}
				if ((pixel_priority_1bit_palette_3bit_color_2bit & 0b00000011) == 0) {//透明部分はとばす
					continue;
				}

				_8bit_sprite_screen_data_160x144[offset_X + (offset_Y * GBX_WIDTH)] = pixel_priority_1bit_palette_3bit_color_2bit;
			}
		}
	}


	struct Sprite_Info_CGB {
		uint16_t address_index = 0x0000;

		uint32_t x = 0;
		uint32_t y = 0;
		uint8_t tile_no = 0;
		bool sprite_max_priority_flag = false;
		bool sprite_reverse_y_flag = false;
		bool sprite_reverse_x_flag = false;
		bool tile_data_bank1_flag = false;
		uint8_t sprite_palette_no_3bit = 0;
		bool size_8x16_flag = false;

		//アドレスが大きい順に並べ替えるために使う比較関数
		bool operator<(const Sprite_Info_CGB& right) const {
			return (address_index < right.address_index) ? false : true;
		}
	};
	vector<Sprite_Info_CGB> sprite_info_list__cgb;
	void init_sprite_info_list__cgb() {
		sprite_info_list__cgb.clear();
		sprite_info_list__cgb.shrink_to_fit();
	}
	/*
	画面に表示されない16ラインにスプライトがあるかチェックする
	*/
	void check_sprite_upside_16line__cgb() {
		for (int y = -16; y < 0; y++) {
			for (int x = -8; x < GBX_WIDTH; x++) {//X座標左端もチェックする
				create_screen_sprite_data__1pixel__cgb(x, y);
			}
		}
	}

	void create_screen_sprite_data__1pixel__cgb(uint32_t pixel_x, uint32_t pixel_y) {
		if ((gbx_ram.RAM[0xFF40] & 0b00000010) == 0) {//スプライトが無効なとき
			return;
		}

		for (int i = 0; i < 40; i++) {
			Sprite_Info_CGB s_info;

			s_info.address_index = (4 * i);//スプライト間の描画優先順位決定のためにアドレスのインデックスを保存しておく

			/*
			s_info.x, s_info.x はスクリーンの座標
			*/
			s_info.y = (gbx_ram.RAM[0xFE00 + (4 * i)] - 16);
			s_info.x = (gbx_ram.RAM[0xFE00 + (4 * i) + 1] - 8);
			if (!(pixel_x == s_info.x && pixel_y == s_info.y)) {
				continue;
			}
			s_info.tile_no = gbx_ram.RAM[0xFE00 + (4 * i) + 2];
			uint8_t attribute = gbx_ram.RAM[0xFE00 + (4 * i) + 3];

			s_info.sprite_max_priority_flag = ((attribute & 0b10000000) != 0) ? false : true;
			s_info.sprite_reverse_y_flag = ((attribute & 0b01000000) != 0) ? true : false;
			s_info.sprite_reverse_x_flag = ((attribute & 0b00100000) != 0) ? true : false;
			s_info.tile_data_bank1_flag = ((attribute & 0b00001000) != 0) ? true : false;
			s_info.sprite_palette_no_3bit = (attribute & 0b111);

			if ((gbx_ram.RAM[0xFF40] & 0b00000100) == 0) {//8x8のとき
				s_info.size_8x16_flag = false;
			}
			else {//8x16のとき
				s_info.size_8x16_flag = true;
			}

			sprite_info_list__cgb.push_back(s_info);

		}
	}

	void draw_screenbuffer_sprite_data__cgb() {
		//M_debug_printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
		//for (int k = 0; k < sprite_info_list__cgb.size(); k++) {
		//	M_debug_printf("BEF sprite_info_list__cgb[%d].address_index = 0x%04x\n", k, sprite_info_list__cgb[k].address_index);
		//}
		sort(sprite_info_list__cgb.begin(), sprite_info_list__cgb.end());
		//for (int k = 0; k < sprite_info_list__cgb.size(); k++) {
		//	M_debug_printf("AFT sprite_info_list__cgb[%d].address_index = 0x%04x\n", k, sprite_info_list__cgb[k].address_index);
		//}

		for (int i = 0; i < sprite_info_list__cgb.size(); i++) {
			Sprite_Info_CGB s_info = sprite_info_list__cgb[i];

			if (s_info.size_8x16_flag == false) {//8x8のとき
				execute_draw_screenbuffer_1sprite_8x8_data__cgb(s_info.x, s_info.y, s_info.tile_no, s_info.sprite_reverse_x_flag, s_info.sprite_reverse_y_flag, s_info.sprite_max_priority_flag, s_info.tile_data_bank1_flag, s_info.sprite_palette_no_3bit);
			}
			else {//8x16のとき
				if (s_info.sprite_reverse_y_flag == false) {
					execute_draw_screenbuffer_1sprite_8x8_data__cgb(s_info.x, s_info.y, (s_info.tile_no & 0b11111110), s_info.sprite_reverse_x_flag, s_info.sprite_reverse_y_flag, s_info.sprite_max_priority_flag, s_info.tile_data_bank1_flag, s_info.sprite_palette_no_3bit);
					execute_draw_screenbuffer_1sprite_8x8_data__cgb(s_info.x, s_info.y + 8, (uint8_t)(s_info.tile_no | 0b00000001), s_info.sprite_reverse_x_flag, s_info.sprite_reverse_y_flag, s_info.sprite_max_priority_flag, s_info.tile_data_bank1_flag, s_info.sprite_palette_no_3bit);
				}
				else {
					execute_draw_screenbuffer_1sprite_8x8_data__cgb(s_info.x, s_info.y, (uint8_t)(s_info.tile_no | 0b00000001), s_info.sprite_reverse_x_flag, s_info.sprite_reverse_y_flag, s_info.sprite_max_priority_flag, s_info.tile_data_bank1_flag, s_info.sprite_palette_no_3bit);
					execute_draw_screenbuffer_1sprite_8x8_data__cgb(s_info.x, s_info.y + 8, (s_info.tile_no & 0b11111110), s_info.sprite_reverse_x_flag, s_info.sprite_reverse_y_flag, s_info.sprite_max_priority_flag, s_info.tile_data_bank1_flag, s_info.sprite_palette_no_3bit);
				}
			}
		}
	}

	//==================================================================================

	void update_LCD_STAT() {
		if ((gbx_ram.RAM[0xFF40] & 0b10000000) == 0) {//LCD有効フラグが無効のとき

			////LCDがオフのときはVblank状態にしておく(※これは間違い？)
			////set_LCD_STAT_mode_flag(1);//1: VBlank(※これは間違い？)

			/*
			LCDが無効のときはLCDのSTATレジスタのbit1とbit0を0にしておく必要がある
			*/
			set_LCD_STAT_mode_flag(0);

			return;
		}

		uint8_t bef_STAT_mode = (read_RAM_8bit(0xFF41) & 0b00000011);//以前のモードを保存しておく

		if (ppu_line_y >= 144) {
			set_LCD_STAT_mode_flag(1);//1: VBlank
		}
		else if (0 <= ppu_line_x && ppu_line_x <= 80) {
			set_LCD_STAT_mode_flag(2);//2: Searching OAM
		}
		else if (80 < ppu_line_x && ppu_line_x <= 240) {
			set_LCD_STAT_mode_flag(3);//3: Transferring Data to LCD Controller
		}
		else {
			set_LCD_STAT_mode_flag(0);//0: HBlank
		}

		uint8_t current_STAT_mode = (read_RAM_8bit(0xFF41) & 0b00000011);
		if (bef_STAT_mode != current_STAT_mode) {//モードが変更された場合
			if (current_STAT_mode == 0) {//0: HBlank
				//LCD_STAT割り込み HBLANK
				if ((read_RAM_8bit(0xFF41) & 0b00001000) != 0) {//Bit 3 - Mode 0 HBlank STAT Interrupt source
					//if ((gbx_ram.RAM[0xFF40] & 0b10000000) != 0) {//LCDが有効な時
						gbx_ram.RAM[0xFF0F] |= 0b00000010;//STAT割り込みを要求する
					//}
				}

				if (hardware_type == Main::GAME_HARDWARE_TYPE::GAMEBOY_COLOR) {
					execute_HBLANK_DMA();//HBLANK DMAをする
				}
			}
			else if (current_STAT_mode == 1) {//1: VBlank
				//LCD_STAT割り込み VBLANK
				if ((read_RAM_8bit(0xFF41) & 0b00010000) != 0) {//Bit 4 - Mode 1 VBlank STAT Interrupt source
					//if ((gbx_ram.RAM[0xFF40] & 0b10000000) != 0) {//LCDが有効な時
						gbx_ram.RAM[0xFF0F] |= 0b00000010;//STAT割り込みを要求する
					//}
				}
			}
			else if (current_STAT_mode == 2) {//2: Searching OAM
				//LCD_STAT割り込み OAM
				if ((read_RAM_8bit(0xFF41) & 0b00100000) != 0) {//Bit 5 - Mode 2 OAM STAT Interrupt source
					//if ((gbx_ram.RAM[0xFF40] & 0b10000000) != 0) {//LCDが有効な時
						gbx_ram.RAM[0xFF0F] |= 0b00000010;//STAT割り込みを要求する
					//}
				}
			}
		}
	}

	void set_LCD_STAT_mode_flag(uint8_t mode_flag) {
		write_RAM_8bit(0xFF41, (read_RAM_8bit(0xFF41) & 0b11111100) | (mode_flag & 0b00000011));
	}

	uint32_t c_cycle_mod = 0;//余りのC-Cycle
	uint32_t ppu_line_x = 0;
	uint32_t ppu_line_y = 0;

	void execute_ppu_process(uint64_t c_cycle) {
		if ((gbx_ram.RAM[0xFF40] & 0b10000000) == 0) {//LCD有効フラグが無効のとき
			for (uint64_t k = 0; k < c_cycle; k++) {
				ppu_line_x++;
				if (ppu_line_x >= 456) {
					ppu_line_x = 0;
					ppu_line_y++;
					write_RAM_8bit(0xFF44, ppu_line_y);

					//if (ppu_line_y == 144) {//Vblank開始
					//	if ((gbx_ram.RAM[0xFF40] & 0b10000000) != 0) {//LCDが有効な時
					//		gbx_ram.RAM[0xFF0F] |= 0b00000001;//Vblankの割り込みを要求する
					//	}
					//}

					if (ppu_line_y >= 154) {//Vblank終了
						c_cycle_mod = c_cycle - (k + 1);//余りのC-Cycleを計算する

						return;
					}
				}
			}

			c_cycle_mod = 0;

			return;
		}

		for (uint64_t i = 0; i < c_cycle; i++) {
			//M_debug_printf("ppu_line_x = %d, ppu_line_y = %d\n", ppu_line_x, ppu_line_y);

			if (ppu_line_x == 0 && ppu_line_y == 0) {//初回は上端にあるスプライトのチェックをする
				if (hardware_type == Main::GAME_HARDWARE_TYPE::GAMEBOY) {
					check_sprite_upside_16line();
				}
				else {
					check_sprite_upside_16line__cgb();
				}
			}

			//描画中
			if ((80 <= ppu_line_x && ppu_line_x < 240) && ppu_line_y < 144) {
				if (hardware_type == Main::GAME_HARDWARE_TYPE::GAMEBOY) {
					draw_backbuffer_bg_1pixel(ppu_line_x - 80, ppu_line_y);
					draw_backbuffer_window_1pixel(ppu_line_x - 80, ppu_line_y);
				}
				else {
					draw_backbuffer_bg_1pixel__cgb(ppu_line_x - 80, ppu_line_y);
					draw_backbuffer_window_1pixel__cgb(ppu_line_x - 80, ppu_line_y);
				}
			}
			if (((80 - 8) <= ppu_line_x && ppu_line_x < 240) && ppu_line_y < 144) {//X座標左端もチェックする
				if (hardware_type == Main::GAME_HARDWARE_TYPE::GAMEBOY) {
					create_screen_sprite_data__1pixel(ppu_line_x - 80, ppu_line_y);
				}
				else {
					create_screen_sprite_data__1pixel__cgb(ppu_line_x - 80, ppu_line_y);
				}
			}

			ppu_line_x++;
			if (ppu_line_x >= 456) {
				ppu_line_x = 0;
				ppu_line_y++;
				write_RAM_8bit(0xFF44, ppu_line_y);

				update_window_flag__1line();//ウィンドウの内部情報を1ラインごとに更新する

				//===================================
				//LCD_STAT割り込み LYC=LY
				if ((read_RAM_8bit(0xFF41) & 0b01000000) != 0) {//Bit 6 - LYC=LY STAT Interrupt source
					if (ppu_line_y == read_RAM_8bit(0xFF45)) {
						//if ((gbx_ram.RAM[0xFF40] & 0b10000000) != 0) {//LCDが有効な時
							gbx_ram.RAM[0xFF0F] |= 0b00000010;//STAT割り込みを要求する
						//}
					}
				}
				//LCD_STATのLYC=LYのフラグを更新する
				if (ppu_line_y == read_RAM_8bit(0xFF45)) {
					write_RAM_8bit(0xFF41, read_RAM_8bit(0xFF41) | 0b00000100);
				}
				else {
					write_RAM_8bit(0xFF41, read_RAM_8bit(0xFF41) & 0b11111011);
				}
				//===================================

				if (ppu_line_y == 144) {//Vblank開始
					//初めてのときはVblankの割り込みを要求する
					//if ((gbx_ram.RAM[0xFF40] & 0b10000000) != 0) {//LCDが有効な時
						gbx_ram.RAM[0xFF0F] |= 0b00000001;//Vblankの割り込みを要求する
					//}
				}

				if (ppu_line_y >= 154) {//Vblank終了
					c_cycle_mod = c_cycle - (i + 1);//余りのC-Cycleを計算する

					return;
				}
			}
		}

		c_cycle_mod = 0;//普段は余り0
	}

	bool backbuffer_isnobackgroundcolor_mask[GBX_WIDTH * GBX_HEIGHT] = { false };//背景色でない場合true

	void draw_screen_bg() {
		LPDIRECT3DTEXTURE9 pTexture;
		if (FAILED(myDirectXSystem->get_pDevice3D()->CreateTexture(GBX_WIDTH, GBX_HEIGHT, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture, NULL))) {
			return;
		}
		D3DLOCKED_RECT lockedRect;
		pTexture->LockRect(0, &lockedRect, NULL, 0);
		DWORD* pTextureBuffer = (DWORD*)lockedRect.pBits;

		for (int y = 0; y < GBX_HEIGHT; y++) {
			for (int x = 0; x < GBX_WIDTH; x++) {
				uint8_t color_no = _8bit_bg_screen_data_160x144[y * GBX_WIDTH + x];
				DWORD color = get_bg_window_palette(color_no);

				pTextureBuffer[y * GBX_WIDTH + x] = color;
			}
		}

		pTexture->UnlockRect(0);

		//MyDirectXDraw::draw_texture_base_leftup(myDirectXSystem, pTexture, (float)GBX_WIDTH, (float)GBX_HEIGHT, 0, 0);
		MyDirectXDraw::draw_texture_base_leftup_enable_size(myDirectXSystem, pTexture, (float)GBX_WIDTH, (float)GBX_HEIGHT, 0, 0, 4.0);

		pTexture->Release();
	}

	void draw_screen_window() {
		LPDIRECT3DTEXTURE9 pTexture;
		if (FAILED(myDirectXSystem->get_pDevice3D()->CreateTexture(GBX_WIDTH, GBX_HEIGHT, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture, NULL))) {
			return;
		}
		D3DLOCKED_RECT lockedRect;
		pTexture->LockRect(0, &lockedRect, NULL, 0);
		DWORD* pTextureBuffer = (DWORD*)lockedRect.pBits;

		for (int y = 0; y < GBX_HEIGHT; y++) {
			for (int x = 0; x < GBX_WIDTH; x++) {
				uint8_t color_no = _8bit_window_screen_data_160x144[y * GBX_WIDTH + x];
				DWORD color;
				if (color_no == 0xFF) {//透明部分の時
					color = 0x00000000;
				}
				else {
					color = get_bg_window_palette(color_no);
				}

				pTextureBuffer[y * GBX_WIDTH + x] = color;
			}
		}

		pTexture->UnlockRect(0);

		//MyDirectXDraw::draw_texture_base_leftup(myDirectXSystem, pTexture, (float)GBX_WIDTH, (float)GBX_HEIGHT, 0, 0);
		MyDirectXDraw::draw_texture_base_leftup_enable_size(myDirectXSystem, pTexture, (float)GBX_WIDTH, (float)GBX_HEIGHT, 0, 0, 4.0);

		pTexture->Release();
	}

	void draw_screen_sprite() {
		LPDIRECT3DTEXTURE9 pTexture;
		if (FAILED(myDirectXSystem->get_pDevice3D()->CreateTexture(GBX_WIDTH, GBX_HEIGHT, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture, NULL))) {
			return;
		}
		D3DLOCKED_RECT lockedRect;
		pTexture->LockRect(0, &lockedRect, NULL, 0);
		DWORD* pTextureBuffer = (DWORD*)lockedRect.pBits;

		for (int y = 0; y < GBX_HEIGHT; y++) {
			for (int x = 0; x < GBX_WIDTH; x++) {
				uint8_t color_no = _8bit_sprite_screen_data_160x144[y * GBX_WIDTH + x];
				DWORD color;

				if ((color_no & 0b01000000) != 0) {//優先順位が低いとき
					if (backbuffer_isnobackgroundcolor_mask[y * GBX_WIDTH + x] == true || color_no == 0xFF) {//背景色でない場合
						color = 0x00000000;
					}
					else {
						color = get_sprite_palette(color_no & 0b00111111, ((color_no & 0b10000000) != 0) ? true : false);
					}
				}
				else {
					if (color_no == 0xFF) {//透明部分の時
						color = 0x00000000;
					}
					else {
						color = get_sprite_palette(color_no & 0b00111111, ((color_no & 0b10000000) != 0) ? true : false);
					}
				}

				pTextureBuffer[y * GBX_WIDTH + x] = color;
			}
		}

		pTexture->UnlockRect(0);

		//MyDirectXDraw::draw_texture_base_leftup(myDirectXSystem, pTexture, (float)GBX_WIDTH, (float)GBX_HEIGHT, 0, 0);
		MyDirectXDraw::draw_texture_base_leftup_enable_size(myDirectXSystem, pTexture, (float)GBX_WIDTH, (float)GBX_HEIGHT, 0, 0, 4.0);

		pTexture->Release();
	}

	//====================================================================================

	bool LCDC_0bit_master_flag__cgb[GBX_WIDTH * GBX_HEIGHT] = { false };
	bool BG_0bit_attribute_flag__cgb[GBX_WIDTH * GBX_HEIGHT] = { false };

	void draw_screen_bg__cgb() {
		LPDIRECT3DTEXTURE9 pTexture;
		if (FAILED(myDirectXSystem->get_pDevice3D()->CreateTexture(GBX_WIDTH, GBX_HEIGHT, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture, NULL))) {
			return;
		}
		D3DLOCKED_RECT lockedRect;
		pTexture->LockRect(0, &lockedRect, NULL, 0);
		DWORD* pTextureBuffer = (DWORD*)lockedRect.pBits;

		for (int y = 0; y < GBX_HEIGHT; y++) {
			for (int x = 0; x < GBX_WIDTH; x++) {
				uint8_t pixel_priority_1bit_palette_3bit_color_2bit = _8bit_bg_screen_data_160x144[y * GBX_WIDTH + x];
				uint16_t color_2byte = get_bg_window_palette__cgb((pixel_priority_1bit_palette_3bit_color_2bit >> 2) & 0b111, pixel_priority_1bit_palette_3bit_color_2bit & 0b11);

				uint8_t r_5bit = color_2byte & 0b11111;
				uint8_t g_5bit = (color_2byte >> 5) & 0b11111;
				uint8_t b_5bit = (color_2byte >> 10) & 0b11111;

				pTextureBuffer[y * GBX_WIDTH + x] = GET_5BIT_COLOR_ALPHA255(r_5bit, g_5bit, b_5bit);
			}
		}

		pTexture->UnlockRect(0);

		//MyDirectXDraw::draw_texture_base_leftup(myDirectXSystem, pTexture, (float)GBX_WIDTH, (float)GBX_HEIGHT, 0, 0);
		MyDirectXDraw::draw_texture_base_leftup_enable_size(myDirectXSystem, pTexture, (float)GBX_WIDTH, (float)GBX_HEIGHT, 0, 0, 4.0);

		pTexture->Release();
	}

	void draw_screen_window__cgb() {
		LPDIRECT3DTEXTURE9 pTexture;
		if (FAILED(myDirectXSystem->get_pDevice3D()->CreateTexture(GBX_WIDTH, GBX_HEIGHT, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture, NULL))) {
			return;
		}
		D3DLOCKED_RECT lockedRect;
		pTexture->LockRect(0, &lockedRect, NULL, 0);
		DWORD* pTextureBuffer = (DWORD*)lockedRect.pBits;

		for (int y = 0; y < GBX_HEIGHT; y++) {
			for (int x = 0; x < GBX_WIDTH; x++) {
				uint8_t pixel_priority_1bit_palette_3bit_color_2bit = _8bit_window_screen_data_160x144[y * GBX_WIDTH + x];
				DWORD color;
				if (pixel_priority_1bit_palette_3bit_color_2bit == 0xFF) {//透明部分の時
					color = 0x00000000;
				}
				else {
					uint16_t color_2byte = get_bg_window_palette__cgb((pixel_priority_1bit_palette_3bit_color_2bit >> 2) & 0b111, pixel_priority_1bit_palette_3bit_color_2bit & 0b11);

					uint8_t r_5bit = color_2byte & 0b11111;
					uint8_t g_5bit = (color_2byte >> 5) & 0b11111;
					uint8_t b_5bit = (color_2byte >> 10) & 0b11111;

					color = GET_5BIT_COLOR_ALPHA255(r_5bit, g_5bit, b_5bit);
				}

				pTextureBuffer[y * GBX_WIDTH + x] = color;
			}
		}

		pTexture->UnlockRect(0);

		//MyDirectXDraw::draw_texture_base_leftup(myDirectXSystem, pTexture, (float)GBX_WIDTH, (float)GBX_HEIGHT, 0, 0);
		MyDirectXDraw::draw_texture_base_leftup_enable_size(myDirectXSystem, pTexture, (float)GBX_WIDTH, (float)GBX_HEIGHT, 0, 0, 4.0);

		pTexture->Release();
	}

	void draw_screen_sprite__cgb() {
		LPDIRECT3DTEXTURE9 pTexture;
		if (FAILED(myDirectXSystem->get_pDevice3D()->CreateTexture(GBX_WIDTH, GBX_HEIGHT, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture, NULL))) {
			return;
		}
		D3DLOCKED_RECT lockedRect;
		pTexture->LockRect(0, &lockedRect, NULL, 0);
		DWORD* pTextureBuffer = (DWORD*)lockedRect.pBits;

		for (int y = 0; y < GBX_HEIGHT; y++) {
			for (int x = 0; x < GBX_WIDTH; x++) {
				uint8_t pixel_priority_1bit_palette_3bit_color_2bit = _8bit_sprite_screen_data_160x144[y * GBX_WIDTH + x];
				DWORD color;

				if (LCDC_0bit_master_flag__cgb[y * GBX_WIDTH + x] == false) {
					if (pixel_priority_1bit_palette_3bit_color_2bit == 0xFF) {//透明部分のとき
						color = 0x00000000;
					}
					else {//透明部分でないとき
						//color = get_sprite_palette(color_no & 0b00111111, ((color_no & 0b10000000) != 0) ? true : false);
						uint16_t color_2byte = get_sprite_palette__cgb((pixel_priority_1bit_palette_3bit_color_2bit >> 2) & 0b111, pixel_priority_1bit_palette_3bit_color_2bit & 0b11);
						uint8_t r_5bit = color_2byte & 0b11111;
						uint8_t g_5bit = (color_2byte >> 5) & 0b11111;
						uint8_t b_5bit = (color_2byte >> 10) & 0b11111;

						color = GET_5BIT_COLOR_ALPHA255(r_5bit, g_5bit, b_5bit);
					}
				}
				else if (BG_0bit_attribute_flag__cgb[y * GBX_WIDTH + x] == true) {
					if (backbuffer_isnobackgroundcolor_mask[y * GBX_WIDTH + x] == false) {//背景色のとき
						if (pixel_priority_1bit_palette_3bit_color_2bit == 0xFF) {//透明部分のとき
							color = 0x00000000;
						}
						else {//透明部分でないとき
							//color = get_sprite_palette(color_no & 0b00111111, ((color_no & 0b10000000) != 0) ? true : false);
							uint16_t color_2byte = get_sprite_palette__cgb((pixel_priority_1bit_palette_3bit_color_2bit >> 2) & 0b111, pixel_priority_1bit_palette_3bit_color_2bit & 0b11);
							uint8_t r_5bit = color_2byte & 0b11111;
							uint8_t g_5bit = (color_2byte >> 5) & 0b11111;
							uint8_t b_5bit = (color_2byte >> 10) & 0b11111;

							color = GET_5BIT_COLOR_ALPHA255(r_5bit, g_5bit, b_5bit);
						}
					}
					else {//背景色でないとき
						color = 0x00000000;
					}
				}
				else if ((pixel_priority_1bit_palette_3bit_color_2bit & 0b00100000) == 0) {//優先順位が低いとき
					if (backbuffer_isnobackgroundcolor_mask[y * GBX_WIDTH + x] == true || pixel_priority_1bit_palette_3bit_color_2bit == 0xFF) {//背景色でない場合
						color = 0x00000000;
					}
					else {
						//color = get_sprite_palette(color_no & 0b00111111, ((color_no & 0b10000000) != 0) ? true : false);
						uint16_t color_2byte = get_sprite_palette__cgb((pixel_priority_1bit_palette_3bit_color_2bit >> 2) & 0b111, pixel_priority_1bit_palette_3bit_color_2bit & 0b11);
						uint8_t r_5bit = color_2byte & 0b11111;
						uint8_t g_5bit = (color_2byte >> 5) & 0b11111;
						uint8_t b_5bit = (color_2byte >> 10) & 0b11111;

						color = GET_5BIT_COLOR_ALPHA255(r_5bit, g_5bit, b_5bit);
					}
				}
				else {
					if (pixel_priority_1bit_palette_3bit_color_2bit == 0xFF) {//透明部分の時
						color = 0x00000000;
					}
					else {
						//color = get_sprite_palette(color_no & 0b00111111, ((color_no & 0b10000000) != 0) ? true : false);
						uint16_t color_2byte = get_sprite_palette__cgb((pixel_priority_1bit_palette_3bit_color_2bit >> 2) & 0b111, pixel_priority_1bit_palette_3bit_color_2bit & 0b11);
						uint8_t r_5bit = color_2byte & 0b11111;
						uint8_t g_5bit = (color_2byte >> 5) & 0b11111;
						uint8_t b_5bit = (color_2byte >> 10) & 0b11111;

						color = GET_5BIT_COLOR_ALPHA255(r_5bit, g_5bit, b_5bit);
					}
				}

				pTextureBuffer[y * GBX_WIDTH + x] = color;
			}
		}

		pTexture->UnlockRect(0);

		//MyDirectXDraw::draw_texture_base_leftup(myDirectXSystem, pTexture, (float)GBX_WIDTH, (float)GBX_HEIGHT, 0, 0);
		MyDirectXDraw::draw_texture_base_leftup_enable_size(myDirectXSystem, pTexture, (float)GBX_WIDTH, (float)GBX_HEIGHT, 0, 0, 4.0);

		pTexture->Release();
	}

	//===================================================================

#ifdef GAMEBOYCOLOR_EMULATOR_DEBUG
	void _debug_draw_screen_256x256_backbuffer(uint8_t buffer_type) {
		LPDIRECT3DTEXTURE9 pTexture;
		if (FAILED(myDirectXSystem->get_pDevice3D()->CreateTexture(256, 256, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture, NULL))) {
			return;
		}
		D3DLOCKED_RECT lockedRect;
		pTexture->LockRect(0, &lockedRect, NULL, 0);
		DWORD* pTextureBuffer = (DWORD*)lockedRect.pBits;

		for (int y = 0; y < 256; y++) {
			for (int x = 0; x < 256; x++) {
				uint8_t color_no;
				if (buffer_type == 0) {
					color_no = _8bit_backbuffer_data_256x256__mtype0_dtype0[y * 256 + x];
				}
				else if (buffer_type == 1) {
					color_no = _8bit_backbuffer_data_256x256__mtype0_dtype1[y * 256 + x];
				}
				else if (buffer_type == 2) {
					color_no = _8bit_backbuffer_data_256x256__mtype1_dtype0[y * 256 + x];
				}
				else {
					color_no = _8bit_backbuffer_data_256x256__mtype1_dtype1[y * 256 + x];
				}
				DWORD color = get_bg_window_palette(color_no);

				pTextureBuffer[y * 256 + x] = color;
			}
		}

		pTexture->UnlockRect(0);

		MyDirectXDraw::draw_texture_base_leftup(myDirectXSystem, pTexture, (float)256, (float)256, 0, 0);

		pTexture->Release();
	}

	void _debug_draw_screen_256x256_backbuffer__cgb(uint8_t buffer_type) {
		LPDIRECT3DTEXTURE9 pTexture;
		if (FAILED(myDirectXSystem->get_pDevice3D()->CreateTexture(256, 256, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture, NULL))) {
			return;
		}
		D3DLOCKED_RECT lockedRect;
		pTexture->LockRect(0, &lockedRect, NULL, 0);
		DWORD* pTextureBuffer = (DWORD*)lockedRect.pBits;

		for (int y = 0; y < 256; y++) {
			for (int x = 0; x < 256; x++) {
				uint8_t pixel_priority_1bit_palette_3bit_color_2bit;
				if (buffer_type == 0) {
					pixel_priority_1bit_palette_3bit_color_2bit = _8bit_backbuffer_data_256x256__mtype0_dtype0[y * 256 + x];
				}
				else if (buffer_type == 1) {
					pixel_priority_1bit_palette_3bit_color_2bit = _8bit_backbuffer_data_256x256__mtype0_dtype1[y * 256 + x];
				}
				else if (buffer_type == 2) {
					pixel_priority_1bit_palette_3bit_color_2bit = _8bit_backbuffer_data_256x256__mtype1_dtype0[y * 256 + x];
				}
				else {
					pixel_priority_1bit_palette_3bit_color_2bit = _8bit_backbuffer_data_256x256__mtype1_dtype1[y * 256 + x];
				}
				uint16_t color_2byte = get_sprite_palette__cgb((pixel_priority_1bit_palette_3bit_color_2bit >> 2) & 0b111, pixel_priority_1bit_palette_3bit_color_2bit & 0b11);
				uint8_t r_5bit = color_2byte & 0b11111;
				uint8_t g_5bit = (color_2byte >> 5) & 0b11111;
				uint8_t b_5bit = (color_2byte >> 10) & 0b11111;

				DWORD color;
				color = GET_5BIT_COLOR_ALPHA255(r_5bit, g_5bit, b_5bit);

				pTextureBuffer[y * 256 + x] = color;
			}
		}

		pTexture->UnlockRect(0);

		MyDirectXDraw::draw_texture_base_leftup(myDirectXSystem, pTexture, (float)256, (float)256, 0, 0);

		pTexture->Release();
	}
#endif

	//LCDオフのときの画面の描画
	void draw_screen_LCD_off() {
		LPDIRECT3DTEXTURE9 pTexture;
		if (FAILED(myDirectXSystem->get_pDevice3D()->CreateTexture(GBX_WIDTH, GBX_HEIGHT, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture, NULL))) {
			return;
		}
		D3DLOCKED_RECT lockedRect;
		pTexture->LockRect(0, &lockedRect, NULL, 0);
		DWORD* pTextureBuffer = (DWORD*)lockedRect.pBits;

		for (int y = 0; y < GBX_HEIGHT; y++) {
			for (int x = 0; x < GBX_WIDTH; x++) {
				//pTextureBuffer[y * GBX_WIDTH + x] = 0xFF4e454a;
				if (hardware_type == Main::GAME_HARDWARE_TYPE::GAMEBOY) {
					pTextureBuffer[y * GBX_WIDTH + x] = GB_PALETTE_0;
				}
				else {
					pTextureBuffer[y * GBX_WIDTH + x] = 0xFFFFFFFF;
				}
			}
		}

		pTexture->UnlockRect(0);

		//MyDirectXDraw::draw_texture_base_leftup(myDirectXSystem, pTexture, (float)GBX_WIDTH, (float)GBX_HEIGHT, 0, 0);
		MyDirectXDraw::draw_texture_base_leftup_enable_size(myDirectXSystem, pTexture, (float)GBX_WIDTH, (float)GBX_HEIGHT, 0, 0, 4.0);

		pTexture->Release();
	}

#ifdef GAMEBOYCOLOR_EMULATOR_DEBUG
	void __debug_draw_all_palette__cgb() {
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 4; j++) {
				uint16_t color_2byte = get_bg_window_palette__cgb(i, j);

				uint8_t r_5bit = color_2byte & 0b11111;
				uint8_t g_5bit = (color_2byte >> 5) & 0b11111;
				uint8_t b_5bit = (color_2byte >> 10) & 0b11111;

				DWORD color;
				color = GET_5BIT_COLOR_ALPHA255(r_5bit, g_5bit, b_5bit);

				//M_debug_printf("[%d][%d] r = 0x%02x, g = 0x%02x, b = 0x%02x, COLOR = 0x%08x\n", j, i, r_5bit, g_5bit, b_5bit, color);

				MyDirectXDraw::draw_box_leftup(myDirectXSystem, j * 20, 300 + i * 20, j * 20 + 20, 300 + i * 20 + 20, color);
			}
		}

		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 4; j++) {
				uint16_t color_2byte = get_sprite_palette__cgb(i, j);

				uint8_t r_5bit = color_2byte & 0b11111;
				uint8_t g_5bit = (color_2byte >> 5) & 0b11111;
				uint8_t b_5bit = (color_2byte >> 10) & 0b11111;

				DWORD color;
				color = GET_5BIT_COLOR_ALPHA255(r_5bit, g_5bit, b_5bit);

				//M_debug_printf("[%d][%d] r = 0x%02x, g = 0x%02x, b = 0x%02x, COLOR = 0x%08x\n", j, i, r_5bit, g_5bit, b_5bit, color);

				MyDirectXDraw::draw_box_leftup(myDirectXSystem, 100 + j * 20, 300 + 10 + i * 20, 100 + j * 20 + 20, 300 + 10 + i * 20 + 20, color);
			}
		}
	}
#endif


	//====================================================================================


	/*
	割り込みを実行したときはtrueをかえす
	*/
	bool interrupt_process() {
		uint8_t IE_value = gbx_ram.RAM[0xFFFF];
		uint8_t IF_value = gbx_ram.RAM[0xFF0F];

		if ((IE_value & 0b00000001) != 0 && (IF_value & 0b00000001) != 0) {//VBlank
			execute_interrupt(0x40);

			gbx_ram.RAM[0xFF0F] &= (uint8_t)(~(0b00000001));//対応するIFのbitをクリアする

			return true;
		}
		else if ((IE_value & 0b00000010) != 0 && (IF_value & 0b00000010) != 0) {//LCD STAT
			execute_interrupt(0x48);

			gbx_ram.RAM[0xFF0F] &= (uint8_t)(~(0b00000010));//対応するIFのbitをクリアする

			return true;
		}
		else if ((IE_value & 0b00000100) != 0 && (IF_value & 0b00000100) != 0) {//Timer
			execute_interrupt(0x50);

			gbx_ram.RAM[0xFF0F] &= (uint8_t)(~(0b00000100));//対応するIFのbitをクリアする

			return true;
		}
		else if ((IE_value & 0b00001000) != 0 && (IF_value & 0b00001000) != 0) {//Serial
			execute_interrupt(0x58);

			gbx_ram.RAM[0xFF0F] &= (uint8_t)(~(0b00001000));//対応するIFのbitをクリアする

			return true;
		}
		else if ((IE_value & 0b00010000) != 0 && (IF_value & 0b00010000) != 0) {//Joypad
			execute_interrupt(0x60);

			gbx_ram.RAM[0xFF0F] &= (uint8_t)(~(0b00010000));//対応するIFのbitをクリアする

			return true;
		}

		return false;
	}

	void execute_interrupt(uint8_t handler_addr) {
		if (tmp_CPU_HALT_Flag == true) {//HALT命令で割り込みを待っていたとき
			gbx_register.PC++;//PC+1をPUSHする

			tmp_CPU_HALT_Flag = false;
		}

		push_16bit(gbx_register.PC);

		gbx_register.PC = (uint16_t)handler_addr;

		IME_Flag = false;//IMEを無効にする
	}

	void watch_key_interrupt() {
		if ((gbx_ram.RAM[0xFF00] & 0b00010000) == 0) {//方向キー

			uint8_t b_down = (key->get_input_state__GBX__(INPUT_MY_ID_DOWN) != 0) ? 1 : 0;
			uint8_t b_up = (key->get_input_state__GBX__(INPUT_MY_ID_UP) != 0) ? 1 : 0;
			uint8_t b_left = (key->get_input_state__GBX__(INPUT_MY_ID_LEFT) != 0) ? 1 : 0;
			uint8_t b_right = (key->get_input_state__GBX__(INPUT_MY_ID_RIGHT) != 0) ? 1 : 0;

			if ((b_down + b_up + b_left + b_right) != 0) {//ボタンが1つでも押されていたとき
				gbx_ram.RAM[0xFF0F] |= 0b00010000;//JPADの割り込みを要求する
			}
		}
		else if ((gbx_ram.RAM[0xFF00] & 0b00100000) == 0) {//アクションキー
			uint8_t b_start = (key->get_input_state__GBX__(INPUT_MY_ID_START) != 0) ? 1 : 0;
			uint8_t b_select = (key->get_input_state__GBX__(INPUT_MY_ID_SELECT) != 0) ? 1 : 0;
			uint8_t b_b = (key->get_input_state__GBX__(INPUT_MY_ID_B) != 0) ? 1 : 0;
			uint8_t b_a = (key->get_input_state__GBX__(INPUT_MY_ID_A) != 0) ? 1 : 0;

			if ((b_start + b_select + b_b + b_a) != 0) {//ボタンが1つでも押されていたとき
				gbx_ram.RAM[0xFF0F] |= 0b00010000;//JPADの割り込みを要求する
			}
		}
	}

	void draw_debugmode_info() {
		char str[256];
		debug_info_font_1->draw_process_base_center_with_edge(_T("デバッグ情報"), WINDOW_WIDTH / 2, 20, GET_COLOR_ALPHA255(255, 255, 255), GET_COLOR_ALPHA255(0, 0, 0));
		debug_info_font_1->draw_process_base_leftup_with_edge(_T("レジスタ"), 20, 50, GET_COLOR_ALPHA255(255, 255, 255), GET_COLOR_ALPHA255(0, 0, 0));
		sprintf_s(str, "A = 0x%02X", gbx_register.A);
		debug_info_font_1->draw_process_base_leftup_with_edge(_T(str), 20, 80, GET_COLOR_ALPHA255(255, 255, 255), GET_COLOR_ALPHA255(0, 0, 0));
		sprintf_s(str, "B = 0x%02X", gbx_register.B);
		debug_info_font_1->draw_process_base_leftup_with_edge(_T(str), 20, 105, GET_COLOR_ALPHA255(255, 255, 255), GET_COLOR_ALPHA255(0, 0, 0));
		sprintf_s(str, "C = 0x%02X", gbx_register.C);
		debug_info_font_1->draw_process_base_leftup_with_edge(_T(str), 20, 130, GET_COLOR_ALPHA255(255, 255, 255), GET_COLOR_ALPHA255(0, 0, 0));
		sprintf_s(str, "D = 0x%02X", gbx_register.D);
		debug_info_font_1->draw_process_base_leftup_with_edge(_T(str), 20, 155, GET_COLOR_ALPHA255(255, 255, 255), GET_COLOR_ALPHA255(0, 0, 0));
		sprintf_s(str, "E = 0x%02X", gbx_register.E);
		debug_info_font_1->draw_process_base_leftup_with_edge(_T(str), 20, 180, GET_COLOR_ALPHA255(255, 255, 255), GET_COLOR_ALPHA255(0, 0, 0));
		sprintf_s(str, "H = 0x%02X", gbx_register.H);
		debug_info_font_1->draw_process_base_leftup_with_edge(_T(str), 20, 205, GET_COLOR_ALPHA255(255, 255, 255), GET_COLOR_ALPHA255(0, 0, 0));
		sprintf_s(str, "L = 0x%02X", gbx_register.A);
		debug_info_font_1->draw_process_base_leftup_with_edge(_T(str), 20, 230, GET_COLOR_ALPHA255(255, 255, 255), GET_COLOR_ALPHA255(0, 0, 0));
		sprintf_s(str, "AF = 0x%04X", gbx_register.AF);
		debug_info_font_1->draw_process_base_leftup_with_edge(_T(str), 20, 260, GET_COLOR_ALPHA255(255, 255, 255), GET_COLOR_ALPHA255(0, 0, 0));
		sprintf_s(str, "BC = 0x%04X", gbx_register.BC);
		debug_info_font_1->draw_process_base_leftup_with_edge(_T(str), 20, 285, GET_COLOR_ALPHA255(255, 255, 255), GET_COLOR_ALPHA255(0, 0, 0));
		sprintf_s(str, "DE = 0x%04X", gbx_register.DE);
		debug_info_font_1->draw_process_base_leftup_with_edge(_T(str), 20, 310, GET_COLOR_ALPHA255(255, 255, 255), GET_COLOR_ALPHA255(0, 0, 0));
		sprintf_s(str, "HL = 0x%04X", gbx_register.HL);
		debug_info_font_1->draw_process_base_leftup_with_edge(_T(str), 20, 335, GET_COLOR_ALPHA255(255, 255, 255), GET_COLOR_ALPHA255(0, 0, 0));
		sprintf_s(str, "FLAGS = 0x%02X", gbx_register.Flags);
		debug_info_font_1->draw_process_base_leftup_with_edge(_T(str), 20, 365, GET_COLOR_ALPHA255(255, 255, 255), GET_COLOR_ALPHA255(0, 0, 0));
		sprintf_s(str, "Z[%s]  N[%s]  H[%s]  C[%s]",
			((gbx_register.Flags & 0b10000000) != 0) ? "1" : "0",
			((gbx_register.Flags & 0b01000000) != 0) ? "1" : "0",
			((gbx_register.Flags & 0b00100000) != 0) ? "1" : "0",
			((gbx_register.Flags & 0b00010000) != 0) ? "1" : "0");
		debug_info_font_1->draw_process_base_leftup_with_edge(_T(str), 20, 390, GET_COLOR_ALPHA255(255, 255, 255), GET_COLOR_ALPHA255(0, 0, 0));
		sprintf_s(str, "SP = 0x%04X", gbx_register.SP);
		debug_info_font_1->draw_process_base_leftup_with_edge(_T(str), 20, 420, GET_COLOR_ALPHA255(255, 255, 255), GET_COLOR_ALPHA255(0, 0, 0));
		sprintf_s(str, "PC = 0x%04X", gbx_register.PC);
		debug_info_font_1->draw_process_base_leftup_with_edge(_T(str), 20, 445, GET_COLOR_ALPHA255(255, 255, 255), GET_COLOR_ALPHA255(0, 0, 0));

		//==============================================================================

		debug_info_font_1->draw_process_base_leftup_with_edge(_T("パレット"), WINDOW_WIDTH / 2, 100, GET_COLOR_ALPHA255(255, 255, 255), GET_COLOR_ALPHA255(0, 0, 0));

		if (hardware_type == Main::GAME_HARDWARE_TYPE::GAMEBOY) {
			for (int j = 0; j < 4; j++) {
				DWORD color = get_bg_window_palette(j);

				MyDirectXDraw::draw_box_leftup(myDirectXSystem, (WINDOW_WIDTH / 2) + (j * 30), 140, (WINDOW_WIDTH / 2) + (j * 30) + 30, 140 + 30, GET_COLOR_ALPHA255(0, 0, 0));
				MyDirectXDraw::draw_box_leftup(myDirectXSystem, (WINDOW_WIDTH / 2) + (j * 30) + 1, 140 + 1, (WINDOW_WIDTH / 2) + (j * 30) + 30 - 1, 140 + 30 - 1, GET_COLOR_ALPHA255(255, 255, 255));
				MyDirectXDraw::draw_box_leftup(myDirectXSystem, (WINDOW_WIDTH / 2) + (j * 30) + 2, 140 + 2, (WINDOW_WIDTH / 2) + (j * 30) + 30 - 2, 140 + 30 - 2, color);
			}

			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < 4; j++) {
					DWORD color = get_sprite_palette(j, (i == 0) ? false : true);

					MyDirectXDraw::draw_box_leftup(myDirectXSystem, (WINDOW_WIDTH / 2) + 130 + (j * 30), 140 + (i * 30), (WINDOW_WIDTH / 2) + 130 + (j * 30) + 30, 140 + (i * 30) + 30, GET_COLOR_ALPHA255(0, 0, 0));
					MyDirectXDraw::draw_box_leftup(myDirectXSystem, (WINDOW_WIDTH / 2) + 130 + (j * 30) + 1, 140 + (i * 30) + 1, (WINDOW_WIDTH / 2) + 130 + (j * 30) + 30 - 1, 140 + (i * 30) + 30 - 1, GET_COLOR_ALPHA255(255, 255, 255));
					MyDirectXDraw::draw_box_leftup(myDirectXSystem, (WINDOW_WIDTH / 2) + 130 + (j * 30) + 2, 140 + (i * 30) + 2, (WINDOW_WIDTH / 2) + 130 + (j * 30) + 30 - 2, 140 + (i * 30) + 30 - 2, color);
				}
			}
		}
		else {
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 4; j++) {
					uint16_t color_2byte = get_bg_window_palette__cgb(i, j);

					uint8_t r_5bit = color_2byte & 0b11111;
					uint8_t g_5bit = (color_2byte >> 5) & 0b11111;
					uint8_t b_5bit = (color_2byte >> 10) & 0b11111;

					DWORD color;
					color = GET_5BIT_COLOR_ALPHA255(r_5bit, g_5bit, b_5bit);

					//M_debug_printf("[%d][%d] r = 0x%02x, g = 0x%02x, b = 0x%02x, COLOR = 0x%08x\n", j, i, r_5bit, g_5bit, b_5bit, color);

					MyDirectXDraw::draw_box_leftup(myDirectXSystem, (WINDOW_WIDTH / 2) + (j * 30), 140 + (i * 30), (WINDOW_WIDTH / 2) + (j * 30) + 30, 140 + (i * 30) + 30, GET_COLOR_ALPHA255(0, 0, 0));
					MyDirectXDraw::draw_box_leftup(myDirectXSystem, (WINDOW_WIDTH / 2) + (j * 30) + 1, 140 + (i * 30) + 1, (WINDOW_WIDTH / 2) + (j * 30) + 30 - 1, 140 + (i * 30) + 30 - 1, GET_COLOR_ALPHA255(255, 255, 255));
					MyDirectXDraw::draw_box_leftup(myDirectXSystem, (WINDOW_WIDTH / 2) + (j * 30) + 2, 140 + (i * 30) + 2, (WINDOW_WIDTH / 2) + (j * 30) + 30 - 2, 140 + (i * 30) + 30 - 2, color);
				}
			}

			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 4; j++) {
					uint16_t color_2byte = get_sprite_palette__cgb(i, j);

					uint8_t r_5bit = color_2byte & 0b11111;
					uint8_t g_5bit = (color_2byte >> 5) & 0b11111;
					uint8_t b_5bit = (color_2byte >> 10) & 0b11111;

					DWORD color;
					color = GET_5BIT_COLOR_ALPHA255(r_5bit, g_5bit, b_5bit);

					//M_debug_printf("[%d][%d] r = 0x%02x, g = 0x%02x, b = 0x%02x, COLOR = 0x%08x\n", j, i, r_5bit, g_5bit, b_5bit, color);

					MyDirectXDraw::draw_box_leftup(myDirectXSystem, (WINDOW_WIDTH / 2) + 130 + (j * 30), 140 + (i * 30), (WINDOW_WIDTH / 2) + 130 + (j * 30) + 30, 140 + (i * 30) + 30, GET_COLOR_ALPHA255(0, 0, 0));
					MyDirectXDraw::draw_box_leftup(myDirectXSystem, (WINDOW_WIDTH / 2) + 130 + (j * 30) + 1, 140 + (i * 30) + 1, (WINDOW_WIDTH / 2) + 130 + (j * 30) + 30 - 1, 140 + (i * 30) + 30 - 1, GET_COLOR_ALPHA255(255, 255, 255));
					MyDirectXDraw::draw_box_leftup(myDirectXSystem, (WINDOW_WIDTH / 2) + 130 + (j * 30) + 2, 140 + (i * 30) + 2, (WINDOW_WIDTH / 2) + 130 + (j * 30) + 30 - 2, 140 + (i * 30) + 30 - 2, color);
				}
			}
		}

		debug_info_font_1->draw_process_base_leftup_with_edge(_T("割り込み"), (WINDOW_WIDTH / 2) - 30, 430, GET_COLOR_ALPHA255(255, 255, 255), GET_COLOR_ALPHA255(0, 0, 0));
		uint8_t IE_value = gbx_ram.RAM[0xFFFF];
		uint8_t IF_value = gbx_ram.RAM[0xFF0F];
		sprintf_s(str, "IME = [%s]", (IME_Flag == true) ? "1" : "0");
		debug_info_font_1->draw_process_base_leftup_with_edge(_T(str), (WINDOW_WIDTH / 2) - 30, 460, GET_COLOR_ALPHA255(255, 255, 255), GET_COLOR_ALPHA255(0, 0, 0));
		sprintf_s(str, "IE = 0x%02X", IE_value);
		debug_info_font_1->draw_process_base_leftup_with_edge(_T(str), (WINDOW_WIDTH / 2) - 30, 485, GET_COLOR_ALPHA255(255, 255, 255), GET_COLOR_ALPHA255(0, 0, 0));
		sprintf_s(str, "IF = 0x%02X", IF_value);
		debug_info_font_1->draw_process_base_leftup_with_edge(_T(str), (WINDOW_WIDTH / 2) - 30, 510, GET_COLOR_ALPHA255(255, 255, 255), GET_COLOR_ALPHA255(0, 0, 0));
	}

public:
	GameBoyColor(MyDirectXSystem* myDirectXSystem, const char* rom_filename, Key* key) : myDirectXSystem(myDirectXSystem), key(key) {
		LOGFONT lf;

		memset(gbx_ram.RAM, 0, RAM_SIZE);

		if (calc_rom_file_crc32(rom_filename, &rom_crc32) != 0) {
			goto gbx_init_error;
		}
		M_debug_printf("ROMのcrc32の計算結果 = 0x%08X\n", rom_crc32);

		if (read_rom_file(rom_filename) != 0) {
			goto gbx_init_error;
		}

		patch_read_rom_data(rom_crc32);//ソフトによっては動くようにパッチをあてる

		cpu_init();
		ppu_init();
		init_util();

		size_t savedata_filename_size = strlen(rom_filename) + strlen(SAVEDATA_FILE_EXT_NAME) + 1;
		char* savedata_filename = (char*)malloc(savedata_filename_size);
		if (savedata_filename == NULL) {
			goto gbx_init_error;
		}
		memset(savedata_filename, 0x00, savedata_filename_size);
		memcpy(savedata_filename, rom_filename, strlen(rom_filename));
		memcpy((char*)(savedata_filename + strlen(rom_filename)), SAVEDATA_FILE_EXT_NAME, strlen(SAVEDATA_FILE_EXT_NAME));
		create_savedata_file(savedata_filename);
		free(savedata_filename);
		load_gamedata();


		Main::ROM_loaded_flag = true;

		apu = new APU(Main::Sound_Channel1_Mute_Flag, Main::Sound_Channel2_Mute_Flag, Main::Sound_Channel3_Mute_Flag, Main::Sound_Channel4_Mute_Flag);



		int font_size = 22;
		lf = { font_size, 0, 0, 0, 0, 0, 0, 0, SHIFTJIS_CHARSET, OUT_TT_ONLY_PRECIS,
		CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, _T("ＭＳ Ｐゴシック") };
		debug_info_font_1 = new MyDirectXString(myDirectXSystem, &lf);


		M_debug_printf("################################\n");
		M_debug_printf("GameBoyColor::GameBoyColor() Succeed!\n");
		M_debug_printf("################################\n");

		return;

	gbx_init_error:
		M_debug_printf("################################\n");
		M_debug_printf("GameBoyColor::GameBoyColor() Failed......\n");
		M_debug_printf("################################\n");

		MessageBox(NULL, _T("ROMの初期化の際にエラーが発生しました"), _T("ERROR"), MB_OK | MB_ICONERROR);

		FATAL_ERROR_FLAG = true;
	}

	~GameBoyColor() {
		save_gamedata();
		close_savedata_file();

		free(ROM_bank_data_ptr);
		free(SRAM_bank_data_ptr);

		delete apu;

		delete debug_info_font_1;
	}

	bool get_FATAL_ERROR_FLAG() {
		return FATAL_ERROR_FLAG;
	}

	APU* get_apu_ptr() {
		return apu;
	}

	void apply_cheat_code_list(vector<uint32_t> &cheat_code_list) {
		for (int i = 0; i < cheat_code_list.size(); i++) {
			uint32_t cheat_code = cheat_code_list[i];

			uint16_t write_address = ((cheat_code & 0b11111111) << 8) | ((cheat_code >> 8) & 0b11111111);
			uint16_t value = ((cheat_code >> 16) & 0b1111111111111111);

			write_RAM_16bit(write_address, value);

			//M_debug_printf("[i = %d] <CHEAT_WRITE> addr = 0x%04X, value = 0x%04X\n", i, write_address, value);
			//M_debug_printf("[i = %d] <CHEAT_READ> addr = 0x%04X, value = 0x%04X\n", i, write_address, read_RAM_16bit(write_address));
		}
	}

	void update_resident_cheat_code_list(vector<resident_cheat_info>& resident_cheat_info_array) {
		//resident_cheat_code_list.clear();
		//resident_cheat_code_list.shrink_to_fit();

		vector<uint32_t> resident_cheat_code_array;
		for (int m = 0; m < resident_cheat_info_array.size(); m++) {
			for (int n = 0; n < resident_cheat_info_array[m].code_total_number; n++) {
				uint32_t r_cheat_code = resident_cheat_info_array[m].resident_cheat_code_list_ptr[n];
				resident_cheat_code_array.push_back(r_cheat_code);
			}
		}
		resident_cheat_code_list = resident_cheat_code_array;
	}


	void first_search_memory(uint16_t search_value, vector<found_info>& found_address_info_list, bool search_16bit_flag) {
		if (search_16bit_flag == false) {
			for (uint16_t i = 0; i < 0xFFFF; i++) {
				uint8_t read_value = read_RAM_8bit(i);
				if (read_value == (uint8_t)search_value) {
					found_info fi;
					fi.address = i;
					fi.value = search_value;
					fi.prev_value = 0x00;
					found_address_info_list.push_back(fi);
				}
			}
		}
		else {
			for (uint16_t i = 0; i < 0xFFFE; i++) {
				uint16_t read_value = read_RAM_16bit(i);
				if (read_value == search_value) {
					found_info fi;
					fi.address = i;
					fi.value = search_value;
					fi.prev_value = 0x0000;
					found_address_info_list.push_back(fi);
				}
			}
		}
	}

	void search_memory_cmp_equal(uint16_t search_value, vector<found_info>& found_address_info_list, bool search_16bit_flag) {
		vector<found_info> new_found_addr_i_l;

		if (search_16bit_flag == false) {
			for (int i = 0; i < found_address_info_list.size(); i++) {
				uint8_t read_value = read_RAM_8bit(found_address_info_list[i].address);
				if (search_value == read_value) {
					found_info fi;
					fi.address = found_address_info_list[i].address;
					fi.prev_value = found_address_info_list[i].value;
					fi.value = search_value;

					new_found_addr_i_l.push_back(fi);
				}
			}
		}
		else {
			for (int i = 0; i < found_address_info_list.size(); i++) {
				uint16_t read_value = read_RAM_16bit(found_address_info_list[i].address);
				if (search_value == read_value) {
					found_info fi;
					fi.address = found_address_info_list[i].address;
					fi.prev_value = found_address_info_list[i].value;
					fi.value = search_value;

					new_found_addr_i_l.push_back(fi);
				}
			}
		}

		found_address_info_list = new_found_addr_i_l;
	}

	void search_memory_cmp_not_equal(uint16_t search_value, vector<found_info>& found_address_info_list, bool search_16bit_flag) {
		vector<found_info> new_found_addr_i_l;

		if (search_16bit_flag == false) {
			for (int i = 0; i < found_address_info_list.size(); i++) {
				uint8_t read_value = read_RAM_8bit(found_address_info_list[i].address);
				if (search_value != read_value) {
					found_info fi;
					fi.address = found_address_info_list[i].address;
					fi.prev_value = found_address_info_list[i].value;
					fi.value = search_value;

					new_found_addr_i_l.push_back(fi);
				}
			}
		}
		else {
			for (int i = 0; i < found_address_info_list.size(); i++) {
				uint16_t read_value = read_RAM_16bit(found_address_info_list[i].address);
				if (search_value != read_value) {
					found_info fi;
					fi.address = found_address_info_list[i].address;
					fi.prev_value = found_address_info_list[i].value;
					fi.value = search_value;

					new_found_addr_i_l.push_back(fi);
				}
			}
		}

		found_address_info_list = new_found_addr_i_l;
	}

	void search_memory_cmp_biggar(uint16_t cmp_value, vector<found_info>& found_address_info_list, bool search_16bit_flag) {
		vector<found_info> new_found_addr_i_l;

		if (search_16bit_flag == false) {
			for (int i = 0; i < found_address_info_list.size(); i++) {
				uint8_t read_value = read_RAM_8bit(found_address_info_list[i].address);
				if (cmp_value < read_value) {
					found_info fi;
					fi.address = found_address_info_list[i].address;
					fi.prev_value = found_address_info_list[i].value;
					fi.value = read_value;

					new_found_addr_i_l.push_back(fi);
				}
			}
		}
		else {
			for (int i = 0; i < found_address_info_list.size(); i++) {
				uint16_t read_value = read_RAM_16bit(found_address_info_list[i].address);
				if (cmp_value < read_value) {
					found_info fi;
					fi.address = found_address_info_list[i].address;
					fi.prev_value = found_address_info_list[i].value;
					fi.value = read_value;

					new_found_addr_i_l.push_back(fi);
				}
			}
		}

		found_address_info_list = new_found_addr_i_l;
	}

	void search_memory_cmp_smaller(uint16_t cmp_value, vector<found_info>& found_address_info_list, bool search_16bit_flag) {
		vector<found_info> new_found_addr_i_l;

		if (search_16bit_flag == false) {
			for (int i = 0; i < found_address_info_list.size(); i++) {
				uint8_t read_value = read_RAM_8bit(found_address_info_list[i].address);
				if (cmp_value > read_value) {
					found_info fi;
					fi.address = found_address_info_list[i].address;
					fi.prev_value = found_address_info_list[i].value;
					fi.value = read_value;

					new_found_addr_i_l.push_back(fi);
				}
			}
		}
		else {
			for (int i = 0; i < found_address_info_list.size(); i++) {
				uint16_t read_value = read_RAM_16bit(found_address_info_list[i].address);
				if (cmp_value > read_value) {
					found_info fi;
					fi.address = found_address_info_list[i].address;
					fi.prev_value = found_address_info_list[i].value;
					fi.value = read_value;

					new_found_addr_i_l.push_back(fi);
				}
			}
		}

		found_address_info_list = new_found_addr_i_l;
	}

	void search_memory_unknown_value(vector<found_info>& found_address_info_list, bool search_16bit_flag) {
		if (search_16bit_flag == false) {
			for (uint16_t i = 0; i < 0xFFFF; i++) {
				uint8_t read_value = read_RAM_8bit(i);

				found_info fi;
				fi.address = i;
				fi.value = read_value;
				fi.prev_value = 0x00;
				found_address_info_list.push_back(fi);
			}
		}
		else {
			for (uint16_t i = 0; i < 0xFFFE; i++) {
				uint16_t read_value = read_RAM_16bit(i);

				found_info fi;
				fi.address = i;
				fi.value = read_value;
				fi.prev_value = 0x0000;
				found_address_info_list.push_back(fi);
			}
		}
	}

	void search_memory_cmp_prevvalue_equal(vector<found_info>& found_address_info_list, bool search_16bit_flag) {
		vector<found_info> new_found_addr_i_l;

		if (search_16bit_flag == false) {
			for (int i = 0; i < found_address_info_list.size(); i++) {
				uint8_t read_value = read_RAM_8bit(found_address_info_list[i].address);
				if (found_address_info_list[i].value == read_value) {
					found_info fi;
					fi.address = found_address_info_list[i].address;
					fi.prev_value = found_address_info_list[i].value;
					fi.value = read_value;

					new_found_addr_i_l.push_back(fi);
				}
			}
		}
		else {
			for (int i = 0; i < found_address_info_list.size(); i++) {
				uint16_t read_value = read_RAM_16bit(found_address_info_list[i].address);
				if (found_address_info_list[i].value == read_value) {
					found_info fi;
					fi.address = found_address_info_list[i].address;
					fi.prev_value = found_address_info_list[i].value;
					fi.value = read_value;

					new_found_addr_i_l.push_back(fi);
				}
			}
		}

		found_address_info_list = new_found_addr_i_l;
	}

	void search_memory_cmp_prevvalue_not_equal(vector<found_info>& found_address_info_list, bool search_16bit_flag) {
		vector<found_info> new_found_addr_i_l;

		if (search_16bit_flag == false) {
			for (int i = 0; i < found_address_info_list.size(); i++) {
				uint8_t read_value = read_RAM_8bit(found_address_info_list[i].address);
				if (found_address_info_list[i].value != read_value) {
					found_info fi;
					fi.address = found_address_info_list[i].address;
					fi.prev_value = found_address_info_list[i].value;
					fi.value = read_value;

					new_found_addr_i_l.push_back(fi);
				}
			}
		}
		else {
			for (int i = 0; i < found_address_info_list.size(); i++) {
				uint16_t read_value = read_RAM_16bit(found_address_info_list[i].address);
				if (found_address_info_list[i].value != read_value) {
					found_info fi;
					fi.address = found_address_info_list[i].address;
					fi.prev_value = found_address_info_list[i].value;
					fi.value = read_value;

					new_found_addr_i_l.push_back(fi);
				}
			}
		}

		found_address_info_list = new_found_addr_i_l;
	}

	//uint64_t __debug_counter_1 = 0;
	void execute_all() {
		memset(backbuffer_isnobackgroundcolor_mask, false, sizeof(bool) * GBX_WIDTH * GBX_HEIGHT);
		if (hardware_type == Main::GAME_HARDWARE_TYPE::GAMEBOY_COLOR) {
			memset(LCDC_0bit_master_flag__cgb, false, sizeof(bool) * GBX_WIDTH * GBX_HEIGHT);
			memset(BG_0bit_attribute_flag__cgb, false, sizeof(bool) * GBX_WIDTH * GBX_HEIGHT);
		}

		if (hardware_type == Main::GAME_HARDWARE_TYPE::GAMEBOY) {
			init_sprite_info_list();
		}
		else {
			init_sprite_info_list__cgb();
		}
		memset(_8bit_sprite_screen_data_160x144, 0xFF, GBX_WIDTH * GBX_HEIGHT);//スプライトのバックバッファを全部透明にする

		memset(_8bit_window_screen_data_160x144, 0xFF, GBX_WIDTH * GBX_HEIGHT);//ウインドウのバックバッファを全部透明にする
		window_backbuffer_draw_internal_flag_x = false;
		window_backbuffer_draw_internal_counter_y = 0;

		cpu_machine_cycle = 0;
		ppu_line_x = 0;
		ppu_line_y = 0;
		write_RAM_8bit(0xFF44, ppu_line_y);

		for (;;) {
			watch_key_interrupt();

			uint8_t instruction_code;
			instruction_code = read_RAM_8bit(gbx_register.PC);

			//=========================================================
			
			//M_debug_printf("=========================================================\n");
			//M_debug_printf("PC:0x%04x [命令:0x%02x] A:0x%02x, BC:0x%04x, DE:0x%04x, HL:0x%04x, Flags:0x%02x, SP:0x%04x\n",
			//	gbx_register.PC, instruction_code, gbx_register.A, gbx_register.BC, gbx_register.DE, gbx_register.HL, gbx_register.Flags, gbx_register.SP);
			////M_debug_printf("IME_Flag = %s, IE = 0x%02x, IF = 0x%02x\n", (IME_Flag == true) ? "true" : "false", gbx_ram.RAM[0xFFFF], gbx_ram.RAM[0xFF0F]);

			//if (key->get_input_state__normal__(INPUT_MY_ID_SELECT) != 0) {
			//	M_debug_printf("=========================================================\n");
			//	M_debug_printf("PC:0x%04x [命令:0x%02x] A:0x%02x, BC:0x%04x, DE:0x%04x, HL:0x%04x, Flags:0x%02x, SP:0x%04x\n",
			//		gbx_register.PC, instruction_code, gbx_register.A, gbx_register.BC, gbx_register.DE, gbx_register.HL, gbx_register.Flags, gbx_register.SP);
			//	//M_debug_printf("IME_Flag = %s, IE = 0x%02x, IF = 0x%02x\n", (IME_Flag == true) ? "true" : "false", gbx_ram.RAM[0xFFFF], gbx_ram.RAM[0xFF0F]);
			//}

			//=========================================================

			gbx_register.PC++;

			(this->*(cpu_instruction_table[instruction_code]))();

			uint32_t instruction_machine_cycle = instruction_machine_cycle_table[instruction_code];
			if (instruction_machine_cycle == 0xDEADBEEF) {
				M_debug_printf("ERROR 未定義の命令を実行しました[PC:0x%04x, コード:0x%02x]\n", gbx_register.PC, instruction_code);
				MessageBox(NULL, _T("未定義の命令を実行しました"), _T("情報"), MB_ICONERROR);

				FATAL_ERROR_FLAG = true;
			}

			cpu_machine_cycle += instruction_machine_cycle;//割り込みした場合はそのサイクル数も加算してある

			total_cpu_machine_cycle__div += cpu_machine_cycle;
			while (total_cpu_machine_cycle__div >= ((CPU_FREQ / 16384) / 4)) {
				((uint8_t)(gbx_ram.RAM[0xFF04]))++;//DIVレジスタを加算する

				total_cpu_machine_cycle__div -= ((CPU_FREQ / 16384) / 4);
			}

			if (get_timer_enable_flag() == true) {
				const uint64_t timer_count_freq = get_timer_count_freq();
				total_cpu_machine_cycle__tima += cpu_machine_cycle;
				while (total_cpu_machine_cycle__tima >= (timer_count_freq / 4)) {
					((uint8_t)(gbx_ram.RAM[0xFF05]))++;//Timerカウンタレジスタを加算する
					if (gbx_ram.RAM[0xFF05] == 0x00) {//Timerカウンタレジスタがオーバーフローしたとき
						gbx_ram.RAM[0xFF05] = gbx_ram.RAM[0xFF06];

						gbx_ram.RAM[0xFF0F] |= 0b00000100;
					}

					total_cpu_machine_cycle__tima -= (timer_count_freq / 4);
				}
			}

			if (hardware_type == Main::GAME_HARDWARE_TYPE::GAMEBOY) {
				execute_ppu_process(cpu_machine_cycle * 4 + c_cycle_mod);
				apu->update_all_channel(cpu_machine_cycle * 4 + c_cycle_mod);
			}
			else {
				if (CURRENT_CPU_Clock_2x_Flag__CGB == true) {
					execute_ppu_process(((cpu_machine_cycle * 4) / 2) + c_cycle_mod);//倍速モードのとき
					apu->update_all_channel(((cpu_machine_cycle * 4) / 2) + c_cycle_mod);
				}
				else {
					execute_ppu_process(cpu_machine_cycle * 4 + c_cycle_mod);
					apu->update_all_channel(cpu_machine_cycle * 4 + c_cycle_mod);
				}
			}

			update_LCD_STAT();

			//apu->update_all_channel(cpu_machine_cycle * 4 + c_cycle_mod);

			cpu_machine_cycle = 0;

			if (IME_Flag == true) {
				if (interrupt_process() == true) {
					cpu_machine_cycle += 5;//割り込みに5Mサイクルかかる
				}
			}

			if (ppu_line_y >= 154) {//Vblank終了
				create_all_256x256_backbuffer();
				if (hardware_type == Main::GAME_HARDWARE_TYPE::GAMEBOY) {
					draw_screenbuffer_sprite_data();
				}
				else {
					draw_screenbuffer_sprite_data__cgb();
				}

				break;
			}
		}

		if ((gbx_ram.RAM[0xFF40] & 0b10000000) != 0) {//LCD有効フラグが有効のとき
			if (hardware_type == Main::GAME_HARDWARE_TYPE::GAMEBOY) {
				draw_screen_bg();
				draw_screen_window();
				draw_screen_sprite();
			}
			else {
				draw_screen_bg__cgb();
				draw_screen_window__cgb();
				draw_screen_sprite__cgb();
			}
		}
		else {//LCD有効フラグが無効のとき
			draw_screen_LCD_off();
		}

		if (Main::Show_DEBUG_INFO_Flag == true) {
			draw_debugmode_info();
		}

#ifdef GAMEBOYCOLOR_EMULATOR_DEBUG
		//_debug_draw_screen_256x256_backbuffer(0);
		//_debug_draw_screen_256x256_backbuffer(1);
		//_debug_draw_screen_256x256_backbuffer(2);
		//_debug_draw_screen_256x256_backbuffer(3);

		//_debug_draw_screen_256x256_backbuffer__cgb(0);
		//_debug_draw_screen_256x256_backbuffer__cgb(1);
		//_debug_draw_screen_256x256_backbuffer__cgb(2);
		//_debug_draw_screen_256x256_backbuffer__cgb(3);

		//__debug_draw_all_palette__cgb();
#endif

		//M_debug_printf("End 1 frame...\n");

		apu->execute_all_channel();//音声のキューを更新する

		apply_cheat_code_list(resident_cheat_code_list);

		frame_counter++;
		if (frame_counter >= 60) {
			frame_counter = 0;

			RTC_time_lapse__1sec();

			//M_debug_printf("1秒カウント\n");
		}
	}
};

