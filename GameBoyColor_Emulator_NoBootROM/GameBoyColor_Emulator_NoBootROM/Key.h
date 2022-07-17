#pragma once

#include "My_Input.h"

#include "SystemDefine.h"

class Key
{
private:

	unsigned char mix_input_state[ALL_MIX_INPUT_NUMBER];

	//unsigned char input_state[KEY_INPUT_NUMBER];

	//bool tmp_input_state[KEY_INPUT_NUMBER];
	bool tmp_input_state[ALL_MIX_INPUT_NUMBER];
public:
	Key();
	~Key();

	void key_state_update()
	{
		memset(tmp_input_state, 0, sizeof(tmp_input_state));

		unsigned char tmp_key[ALL_KEY_NUMBER];
		My_Input::get_instance_ptr()->get_raw_kay_data(tmp_key);

		/*
		A
		B
		SELECT
		START
		UP
		DOWN
		LEFT
		RIGHT

		#define INPUT_MY_ID_A 0
		#define INPUT_MY_ID_B 1
		#define INPUT_MY_ID_SELECT 2
		#define INPUT_MY_ID_START 3
		#define INPUT_MY_ID_UP 4
		#define INPUT_MY_ID_DOWN 5
		#define INPUT_MY_ID_LEFT 6
		#define INPUT_MY_ID_RIGHT 7

		*/

		if (tmp_key[DIK_Q] != 0) {
			tmp_input_state[INPUT_MY_ID_A] = true;
		}
		if (tmp_key[DIK_W] != 0) {
			tmp_input_state[INPUT_MY_ID_B] = true;
		}
		if (tmp_key[DIK_M] != 0) {
			tmp_input_state[INPUT_MY_ID_SELECT] = true;
		}
		if (tmp_key[DIK_N] != 0) {
			tmp_input_state[INPUT_MY_ID_START] = true;
		}
		if (tmp_key[DIK_UP] != 0) {
			tmp_input_state[INPUT_MY_ID_UP] = true;
		}
		if (tmp_key[DIK_DOWN] != 0) {
			tmp_input_state[INPUT_MY_ID_DOWN] = true;
		}
		if (tmp_key[DIK_LEFT] != 0) {
			tmp_input_state[INPUT_MY_ID_LEFT] = true;
		}
		if (tmp_key[DIK_RIGHT] != 0) {
			tmp_input_state[INPUT_MY_ID_RIGHT] = true;
		}

		//########################################################

		unsigned char tmp_pad[PAD_DATA_NO];
		My_Input::get_instance_ptr()->get_raw_pad_data(tmp_pad);

		if (tmp_pad[PAD_A_NO] != 0) {
			tmp_input_state[INPUT_MY_ID_A] = true;
		}
		if (tmp_pad[PAD_B_NO] != 0) {
			tmp_input_state[INPUT_MY_ID_B] = true;
		}
		if (tmp_pad[PAD_START_NO] != 0) {
			tmp_input_state[INPUT_MY_ID_START] = true;
		}
		if (tmp_pad[PAD_SELECT_NO] != 0) {
			tmp_input_state[INPUT_MY_ID_SELECT] = true;
		}
		if (tmp_pad[PAD_DATA_INDEX_UP] != 0) {
			tmp_input_state[INPUT_MY_ID_UP] = true;
		}
		if (tmp_pad[PAD_DATA_INDEX_DOWN] != 0) {
			tmp_input_state[INPUT_MY_ID_DOWN] = true;
		}
		if (tmp_pad[PAD_DATA_INDEX_LEFT] != 0) {
			tmp_input_state[INPUT_MY_ID_LEFT] = true;
		}
		if (tmp_pad[PAD_DATA_INDEX_RIGHT] != 0) {
			tmp_input_state[INPUT_MY_ID_RIGHT] = true;
		}

		//########################################################


		if (tmp_input_state[INPUT_MY_ID_A] == true) {
			mix_input_state[INPUT_MY_ID_A]++;
			if (mix_input_state[INPUT_MY_ID_A] > 2) {
				mix_input_state[INPUT_MY_ID_A] = 2;
			}
		}
		else {
			mix_input_state[INPUT_MY_ID_A] = 0;
		}

		if (tmp_input_state[INPUT_MY_ID_B] == true) {
			mix_input_state[INPUT_MY_ID_B]++;
			if (mix_input_state[INPUT_MY_ID_B] > 2) {
				mix_input_state[INPUT_MY_ID_B] = 2;
			}
		}
		else {
			mix_input_state[INPUT_MY_ID_B] = 0;
		}

		if (tmp_input_state[INPUT_MY_ID_SELECT] == true) {
			mix_input_state[INPUT_MY_ID_SELECT]++;
			if (mix_input_state[INPUT_MY_ID_SELECT] > 2) {
				mix_input_state[INPUT_MY_ID_SELECT] = 2;
			}
		}
		else {
			mix_input_state[INPUT_MY_ID_SELECT] = 0;
		}

		if (tmp_input_state[INPUT_MY_ID_START] == true) {
			mix_input_state[INPUT_MY_ID_START]++;
			if (mix_input_state[INPUT_MY_ID_START] > 2) {
				mix_input_state[INPUT_MY_ID_START] = 2;
			}
		}
		else {
			mix_input_state[INPUT_MY_ID_START] = 0;
		}

		if (tmp_input_state[INPUT_MY_ID_UP] == true) {
			mix_input_state[INPUT_MY_ID_UP]++;
			if (mix_input_state[INPUT_MY_ID_UP] > 2) {
				mix_input_state[INPUT_MY_ID_UP] = 2;
			}
		}
		else {
			mix_input_state[INPUT_MY_ID_UP] = 0;
		}

		if (tmp_input_state[INPUT_MY_ID_DOWN] == true) {
			mix_input_state[INPUT_MY_ID_DOWN]++;
			if (mix_input_state[INPUT_MY_ID_DOWN] > 2) {
				mix_input_state[INPUT_MY_ID_DOWN] = 2;
			}
		}
		else {
			mix_input_state[INPUT_MY_ID_DOWN] = 0;
		}

		if (tmp_input_state[INPUT_MY_ID_LEFT] == true) {
			mix_input_state[INPUT_MY_ID_LEFT]++;
			if (mix_input_state[INPUT_MY_ID_LEFT] > 2) {
				mix_input_state[INPUT_MY_ID_LEFT] = 2;
			}
		}
		else {
			mix_input_state[INPUT_MY_ID_LEFT] = 0;
		}

		if (tmp_input_state[INPUT_MY_ID_RIGHT] == true) {
			mix_input_state[INPUT_MY_ID_RIGHT]++;
			if (mix_input_state[INPUT_MY_ID_RIGHT] > 2) {
				mix_input_state[INPUT_MY_ID_RIGHT] = 2;
			}
		}
		else {
			mix_input_state[INPUT_MY_ID_RIGHT] = 0;
		}

	}

	unsigned char get_input_state__normal__(int INPUT_TYPE) {
		return mix_input_state[INPUT_TYPE];
	}

	unsigned char get_input_state__GBX__(int INPUT_TYPE) {
		//ã‚Æ‰º‚Ì“¯‰Ÿ‚µ‚Í‚Å‚«‚È‚¢‚æ‚¤‚É‚·‚é
		if (INPUT_TYPE == INPUT_MY_ID_UP || INPUT_TYPE == INPUT_MY_ID_DOWN) {
			if (mix_input_state[INPUT_MY_ID_UP] != 0 && mix_input_state[INPUT_MY_ID_DOWN] != 0) {
				return 0;
			}
		}
		//‰E‚Æ‚Æ¶‚Ì“¯‰Ÿ‚µ‚Í‚Å‚«‚È‚¢‚æ‚¤‚É‚·‚é
		else if (INPUT_TYPE == INPUT_MY_ID_LEFT || INPUT_TYPE == INPUT_MY_ID_RIGHT) {
			if (mix_input_state[INPUT_MY_ID_LEFT] != 0 && mix_input_state[INPUT_MY_ID_RIGHT] != 0) {
				return 0;
			}
		}

		return mix_input_state[INPUT_TYPE];
	}

};
