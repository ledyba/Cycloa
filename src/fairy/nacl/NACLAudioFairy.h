#ifndef NACL_AUDIO
#define NACL_AUDIO

#include <ppapi/cpp/audio.h>
#include "../../emulator/fairy/AudioFairy.h"
class CycloaInstance;

class NACLAudioFairy : public AudioFairy{
private:
	enum{
		BUFFER_SIZE=4410
	};
private:
	NACLAudioFairy(const NACLAudioFairy& other);
	const NACLAudioFairy& operator=(const NACLAudioFairy& other);
private:
	CycloaInstance* cycloa;
	pp::AudioConfig config;
	pp::Audio audio;
	void callback(void* sample_buffer, uint32_t buffer_size_in_bytes);
public:
	NACLAudioFairy(CycloaInstance* cycloa);
	void start();
	void stop();
public:
	static void callback(void* sample_buffer, uint32_t buffer_size_in_bytes, void* user_data);
};

#endif


