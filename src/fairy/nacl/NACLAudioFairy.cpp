#include "NACLAudioFairy.h"
#include "CycloaInstance.h"

NACLAudioFairy::NACLAudioFairy(CycloaInstance* cycloa) :
AudioFairy(),
cycloa(cycloa),
config(cycloa, PP_AUDIOSAMPLERATE_44100, NACLAudioFairy::BUFFER_SIZE),
audio(cycloa, this->config, NACLAudioFairy::callback, this)
{
}

void NACLAudioFairy::callback(void* sample_buffer, uint32_t buffer_size_in_bytes)
{
	int16_t* const buffer = reinterpret_cast<int16_t*>(sample_buffer);
	const int maxLen = buffer_size_in_bytes/sizeof(int16_t)/2;

	int16_t monoBuffer[maxLen];
	const int copiedLength = this->popAudio(monoBuffer, maxLen);
	for(int i=0;i<copiedLength;++i){
		buffer[(i<<1)+0]=monoBuffer[i];
		buffer[(i<<1)+1]=monoBuffer[i];
	}
	const int16_t fill = copiedLength > 0 ? monoBuffer[copiedLength-1] : 0;
	for(int i=copiedLength;i<maxLen;i++){
		buffer[(i<<1)+0]=fill;
		buffer[(i<<1)+1]=fill;
	}
}

void NACLAudioFairy::start()
{
	this->audio.StartPlayback();
}
void NACLAudioFairy::stop()
{
	this->audio.StopPlayback();
}

void NACLAudioFairy::callback(void* sample_buffer, uint32_t buffer_size_in_bytes, void* user_data)
{
	reinterpret_cast<NACLAudioFairy*>(user_data)->callback(sample_buffer, buffer_size_in_bytes);
}

