#pragma once

#include <xaudio2.h>

class VoiceCallBack : public IXAudio2VoiceCallback
{
public:
	VoiceCallBack() : event_handle(CreateEvent(NULL, FALSE, FALSE, NULL)) {
	}

	~VoiceCallBack() {
		CloseHandle(event_handle);
	}

	HANDLE get_event_handle() {
		return event_handle;
	}

	void STDMETHODCALLTYPE OnBufferEnd(void*) override {
		SetEvent(event_handle);
	}

	//##########################################################################
	//–¢Žg—p
	//##########################################################################
	void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32) override {}
	void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override {}
	void STDMETHODCALLTYPE OnStreamEnd() override {}
	void STDMETHODCALLTYPE OnBufferStart(void*) override {}
	void STDMETHODCALLTYPE OnLoopEnd(void*) override {}
	void STDMETHODCALLTYPE OnVoiceError(void*, HRESULT) override {}
	//##########################################################################
private:
	HANDLE event_handle;
};
