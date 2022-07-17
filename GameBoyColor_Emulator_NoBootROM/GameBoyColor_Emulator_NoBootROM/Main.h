#pragma once

#include <stdint.h>
#include <windows.h>

#include "GameBoyColor.h"

class Main
{
private:
	Main() {}
	~Main() {}
public:
	enum class GAME_HARDWARE_TYPE {
		GAMEBOY,
		GAMEBOY_COLOR,
	};

	static bool ready_read_ROM_flag;
	static TCHAR ROMFilePath[MAX_PATH];

	static bool ROM_loaded_flag;
	static uint8_t Cartridge_Type;
	static uint32_t PGM_size;
	static uint32_t SRAM_size;
	static GAME_HARDWARE_TYPE game_hardware_type;



	static bool Sound_Channel1_Mute_Flag;
	static bool Sound_Channel2_Mute_Flag;
	static bool Sound_Channel3_Mute_Flag;
	static bool Sound_Channel4_Mute_Flag;


	static bool Show_FPS_Flag;


	static bool Show_DEBUG_INFO_Flag;


	
	//static bool Cheat_Window_Exist_Flag;
};
