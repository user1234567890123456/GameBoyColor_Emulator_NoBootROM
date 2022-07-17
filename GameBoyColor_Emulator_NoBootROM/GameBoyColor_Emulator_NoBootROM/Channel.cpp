#include "Channel.h"

unsigned int __stdcall Channel::play_thread_func(PVOID pv) {
	//M_debug_printf("Channel::play_thread_func() start\n");

	Channel* channel_ptr = (Channel*)pv;

	channel_ptr->mtx.lock();
	//channel_ptr->isPlaying = true;
	//channel_ptr->buffer_no_latch = false;
	channel_ptr->current_buffer_no = 0;
	channel_ptr->source_voice->Start();
	channel_ptr->mtx.unlock();

	for (;;) {
		if (channel_ptr->stop_flag == true) {
			channel_ptr->mtx.lock();
			channel_ptr->stop_flag = false;
			//channel_ptr->isPlaying = false;
			channel_ptr->source_voice->Stop();
			channel_ptr->source_voice->FlushSourceBuffers();
			channel_ptr->mtx.unlock();
			break;
		}

		XAUDIO2_VOICE_STATE state;
		channel_ptr->source_voice->GetState(&state);
		if (state.BuffersQueued < 3) {//キューは3個まで
			//M_debug_printf("queue = %d\n", state.BuffersQueued);

			if (channel_ptr->current_data_ready_flag == false) {//供給データがないときは供給されるのを待つ
				//M_debug_printf("queue wait...\n");
				WaitForSingleObject(channel_ptr->notify_event, INFINITE);
			}

			channel_ptr->mtx.lock();

			uint8_t* new_buffer_ptr = channel_ptr->consume_stream_buffer__th();

			XAUDIO2_BUFFER submit = { 0 };
			submit.AudioBytes = channel_ptr->wave_data_size;
			submit.pAudioData = new_buffer_ptr;
			submit.LoopCount = 0;

			channel_ptr->source_voice->SubmitSourceBuffer(&submit);

			channel_ptr->mtx.unlock();
		}
		else {
			//M_debug_printf("waiting...\n");
			WaitForSingleObject(channel_ptr->callback->get_event_handle(), INFINITE);
		}
	}

	XAUDIO2_VOICE_STATE state_2;
	channel_ptr->source_voice->GetState(&state_2);
	while (state_2.BuffersQueued != 0) {
		//M_debug_printf("waiting2...\n");
		WaitForSingleObject(channel_ptr->callback->get_event_handle(), INFINITE);
		channel_ptr->source_voice->GetState(&state_2);
	}

	channel_ptr->mtx.lock();
	//channel_ptr->isPlaying = false;
	channel_ptr->mtx.unlock();

	//M_debug_printf("Channel::play_thread_func() end\n");

	_endthreadex(0);

	return 0;
}
