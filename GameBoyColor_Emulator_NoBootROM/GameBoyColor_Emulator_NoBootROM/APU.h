#pragma once

#include <xaudio2.h>
#include <mmsystem.h>

#include "DebugUtility.h"
#include "Channel.h"

class APU
{
private:
	IXAudio2* xaudio = nullptr;
	IXAudio2MasteringVoice* mastering_voice = nullptr;

	Channel* ch1;
	Channel* ch2;
	Channel* ch3;
	Channel* ch4;

public:
	bool all_channel_enable_flag = false;

	APU(bool ch1_mute_flag, bool ch2_mute_flag, bool ch3_mute_flag, bool ch4_mute_flag) {
		HRESULT hr;
		if (FAILED(hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED))) {
			M_debug_printf("ERROR CoInitializeEx\n");
		}
		if (FAILED(hr = XAudio2Create(&xaudio))) {
			M_debug_printf("ERROR XAudio2Create\n");
		}

		if (FAILED(hr = xaudio->CreateMasteringVoice(&mastering_voice))) {
			M_debug_printf("ERROR CreateMasteringVoice\n");
		}

		mastering_voice->SetVolume(0.03);

		ch1 = new Channel(Channel::CH_TYPE::CH1, xaudio);
		if (ch1_mute_flag == true) {
			ch1->get_source_voice_ptr()->SetVolume(0.0f);
		}
		else {
			ch1->get_source_voice_ptr()->SetVolume(1.0f);
		}
		ch2 = new Channel(Channel::CH_TYPE::CH2, xaudio);
		if (ch2_mute_flag == true) {
			ch2->get_source_voice_ptr()->SetVolume(0.0f);
		}
		else {
			ch2->get_source_voice_ptr()->SetVolume(1.0f);
		}
		ch3 = new Channel(Channel::CH_TYPE::CH3, xaudio);
		if (ch3_mute_flag == true) {
			ch3->get_source_voice_ptr()->SetVolume(0.0f);
		}
		else {
			ch3->get_source_voice_ptr()->SetVolume(1.0f);
		}
		ch4 = new Channel(Channel::CH_TYPE::CH4, xaudio);
		if (ch4_mute_flag == true) {
			ch4->get_source_voice_ptr()->SetVolume(0.0f);
		}
		else {
			ch4->get_source_voice_ptr()->SetVolume(1.0f);
		}
	}

	~APU() {
		delete ch1;
		delete ch2;
		delete ch3;
		delete ch4;

		if (mastering_voice != nullptr) {
			mastering_voice->DestroyVoice();
		}
		if (xaudio != nullptr) {
			xaudio->Release();
		}
		CoUninitialize();
	}

	Channel* get_channel_1() {
		return ch1;
	}

	Channel* get_channel_2() {
		return ch2;
	}

	Channel* get_channel_3() {
		return ch3;
	}

	Channel* get_channel_4() {
		return ch4;
	}

	void update_all_channel(uint64_t c_cycle) {
		ch1->update(c_cycle);
		ch2->update(c_cycle);
		ch3->update(c_cycle);
		ch4->update(c_cycle);
	}

	void execute_all_channel() {
		if (all_channel_enable_flag == true) {
			ch1->execute();
			ch2->execute();
			ch3->execute();
			ch4->execute();
		}
		else {
			ch1->execute_sleep();
			ch2->execute_sleep();
			ch3->execute_sleep();
			ch4->execute_sleep();
		}
	}

	void set_all_Channel_Left_Right_volume(float left_volume, float right_volume) {
		float tmp_buf[8] = { 0.0f };
		tmp_buf[0] = (get_channel_1()->left_sound_ON_flag == true) ? left_volume : 0.0f;
		tmp_buf[1] = (get_channel_1()->right_sound_ON_flag == true) ? right_volume : 0.0f;
		get_channel_1()->get_source_voice_ptr()->SetOutputMatrix(NULL, 1, 1, tmp_buf);
		tmp_buf[0] = (get_channel_2()->left_sound_ON_flag == true) ? left_volume : 0.0f;
		tmp_buf[1] = (get_channel_2()->right_sound_ON_flag == true) ? right_volume : 0.0f;
		get_channel_2()->get_source_voice_ptr()->SetOutputMatrix(NULL, 1, 1, tmp_buf);
		tmp_buf[0] = (get_channel_3()->left_sound_ON_flag == true) ? left_volume : 0.0f;
		tmp_buf[1] = (get_channel_3()->right_sound_ON_flag == true) ? right_volume : 0.0f;
		get_channel_3()->get_source_voice_ptr()->SetOutputMatrix(NULL, 1, 1, tmp_buf);
		tmp_buf[0] = (get_channel_4()->left_sound_ON_flag == true) ? left_volume : 0.0f;
		tmp_buf[1] = (get_channel_4()->right_sound_ON_flag == true) ? right_volume : 0.0f;
		get_channel_4()->get_source_voice_ptr()->SetOutputMatrix(NULL, 1, 1, tmp_buf);
	}

	void set_Channel1_Left_volume(float left_volume) {
		float tmp_buf[8] = { 0.0f };
		get_channel_1()->get_source_voice_ptr()->GetOutputMatrix(NULL, 1, 1, tmp_buf);
		tmp_buf[0] = left_volume;
		get_channel_1()->get_source_voice_ptr()->SetOutputMatrix(NULL, 1, 1, tmp_buf);
	}
	void set_Channel1_Right_volume(float right_volume) {
		float tmp_buf[8] = { 0.0f };
		get_channel_1()->get_source_voice_ptr()->GetOutputMatrix(NULL, 1, 1, tmp_buf);
		tmp_buf[1] = right_volume;
		get_channel_1()->get_source_voice_ptr()->SetOutputMatrix(NULL, 1, 1, tmp_buf);
	}

	void set_Channel2_Left_volume(float left_volume) {
		float tmp_buf[8] = { 0.0f };
		get_channel_2()->get_source_voice_ptr()->GetOutputMatrix(NULL, 1, 1, tmp_buf);
		tmp_buf[0] = left_volume;
		get_channel_2()->get_source_voice_ptr()->SetOutputMatrix(NULL, 1, 1, tmp_buf);
	}
	void set_Channel2_Right_volume(float right_volume) {
		float tmp_buf[8] = { 0.0f };
		get_channel_2()->get_source_voice_ptr()->GetOutputMatrix(NULL, 1, 1, tmp_buf);
		tmp_buf[1] = right_volume;
		get_channel_2()->get_source_voice_ptr()->SetOutputMatrix(NULL, 1, 1, tmp_buf);
	}

	void set_Channel3_Left_volume(float left_volume) {
		float tmp_buf[8] = { 0.0f };
		get_channel_3()->get_source_voice_ptr()->GetOutputMatrix(NULL, 1, 1, tmp_buf);
		tmp_buf[0] = left_volume;
		get_channel_3()->get_source_voice_ptr()->SetOutputMatrix(NULL, 1, 1, tmp_buf);
	}
	void set_Channel3_Right_volume(float right_volume) {
		float tmp_buf[8] = { 0.0f };
		get_channel_3()->get_source_voice_ptr()->GetOutputMatrix(NULL, 1, 1, tmp_buf);
		tmp_buf[1] = right_volume;
		get_channel_3()->get_source_voice_ptr()->SetOutputMatrix(NULL, 1, 1, tmp_buf);
	}

	void set_Channel4_Left_volume(float left_volume) {
		float tmp_buf[8] = { 0.0f };
		get_channel_4()->get_source_voice_ptr()->GetOutputMatrix(NULL, 1, 1, tmp_buf);
		tmp_buf[0] = left_volume;
		get_channel_4()->get_source_voice_ptr()->SetOutputMatrix(NULL, 1, 1, tmp_buf);
	}
	void set_Channel4_Right_volume(float right_volume) {
		float tmp_buf[8] = { 0.0f };
		get_channel_4()->get_source_voice_ptr()->GetOutputMatrix(NULL, 1, 1, tmp_buf);
		tmp_buf[1] = right_volume;
		get_channel_4()->get_source_voice_ptr()->SetOutputMatrix(NULL, 1, 1, tmp_buf);
	}
};
