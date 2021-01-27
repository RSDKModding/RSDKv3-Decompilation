// code here is heavily based on devkitPro's opus-streaming example for the 3DS
// see here:  https://github.com/devkitPro/3ds-examples/blob/master/audio/opus-decoding/source/main.c

#include "../RetroEngine.hpp"

ndspWaveBuf s_waveBufs[4];
ndspWaveBuf s_sfxWaveBufs[CHANNEL_COUNT];
int16_t* s_audioBuffer = nullptr;

LightEvent s_event;
volatile bool s_quit = false;

Thread audioThreadID;

// taken from Audio.cpp
#if !RETRO_USING_SDL1_AUDIO
size_t readVorbis(void *mem, size_t size, size_t nmemb, void *ptr)
{
    MusicPlaybackInfo *info = (MusicPlaybackInfo *)ptr;
    return FileRead2(&info->fileInfo, mem, (int)(size * nmemb));
}
int seekVorbis(void *ptr, ogg_int64_t offset, int whence)
{
    MusicPlaybackInfo *info = (MusicPlaybackInfo *)ptr;
    switch (whence) {
        case SEEK_SET: whence = 0; break;
        case SEEK_CUR: whence = (int)GetFilePosition2(&info->fileInfo); break;
        case SEEK_END: whence = info->fileInfo.fileSize; break;
        default: break;
    }
    SetFilePosition2(&info->fileInfo, (int)(whence + offset));
    return GetFilePosition2(&info->fileInfo) <= info->fileInfo.fileSize;
}
long tellVorbis(void *ptr)
{
    MusicPlaybackInfo *info = (MusicPlaybackInfo *)ptr;
    return GetFilePosition2(&info->fileInfo);
}

int closeVorbis(void *ptr) { return CloseFile2(); }
// 3ds-specific audio initialisation code
bool _3ds_audioInit() {
	ndspInit();

	ndspSetOutputMode(NDSP_OUTPUT_STEREO);

	ndspChnReset(0);
	ndspChnSetInterp(0, NDSP_INTERP_POLYPHASE);
	ndspChnSetRate(0, SAMPLE_RATE);
	ndspChnSetFormat(0, NDSP_FORMAT_STEREO_PCM16);

	// set up all NDSP channels
	for (int i = 1; i < _3DS_MAX_CHANNELS; i++) {
		ndspChnReset(i);
		ndspChnSetInterp(i, NDSP_INTERP_LINEAR);
		ndspChnSetRate(i, SAMPLE_RATE);
		ndspChnSetFormat(i, NDSP_FORMAT_STEREO_PCM8);
	}

	const size_t bufferSize = (WAVEBUF_SIZE * ARRAY_SIZE(s_waveBufs))
					+ (SFX_BUFFER_SZ * ARRAY_SIZE(s_sfxWaveBufs));
	s_audioBuffer = (int16_t*) linearAlloc(bufferSize);
	if (!s_audioBuffer) {
		printf("Failed to allocate audio buffer. Audio playback disabled.\n");
		audioEnabled = false;
		return false;
	}

	// music buffers
	memset(&s_waveBufs, 0, sizeof(s_waveBufs));
	int16_t* buffer = s_audioBuffer;

	for (size_t i = 0; i < ARRAY_SIZE(s_waveBufs); i++) {
		s_waveBufs[i].data_vaddr = buffer;
		s_waveBufs[i].status     = NDSP_WBUF_DONE;

		buffer += WAVEBUF_SIZE / sizeof(buffer[0]);
	}

	// sfx buffers
	for (size_t i = 0; i < ARRAY_SIZE(s_sfxWaveBufs); i++) {
		s_sfxWaveBufs[i].data_vaddr = buffer;
		s_sfxWaveBufs[i].status     = NDSP_WBUF_DONE;

		buffer += SFX_BUFFER_SZ;
	}

	// set up callback function for NDSP decoding
	ndspSetCallback(_3ds_audioCallback, NULL);

	// thread setup code
	int32_t prio = 0x30;
	prio = prio < 0x18 ? 0x18 : prio;
	prio = prio > 0x3f ? 0x3f : prio;
	audioThreadID = threadCreate(_3ds_audioThread, NULL, THREAD_STACK_SZ, prio, THREAD_AFFINITY, false);

	audioEnabled = true;
	return true;
}

void _3ds_audioExit() {
	s_quit = true;
	threadJoin(audioThreadID, UINT64_MAX);
	threadFree(audioThreadID);

	ndspChnReset(0);
	linearFree(s_audioBuffer);
}

void _3ds_audioCallback(void* const nul) {
	(void)nul;

	if (s_quit)
		return;

	LightEvent_Signal(&s_event);
}

void _3ds_musicLogic() {
	//if (trackBuffer != -1) {
	//	printf("Current track: %d\n", trackBuffer);
	//}

	// loading new music track
	if (musicStatus == MUSIC_LOADING) {
		//printf("Music Load\n");
		if (trackBuffer < 0 || trackBuffer >= TRACK_COUNT) {
			StopMusic();
			return;
		}

		TrackInfo *trackPtr = &musicTracks[trackBuffer];

		if (!trackPtr->fileName[0]) {
			printf("Could not load track.\n");
			StopMusic();
			return;
		}

		if (musInfo.loaded)
			StopMusic();

		if (LoadFile(trackPtr->fileName, &musInfo.fileInfo)) {
			cFileHandleStream = cFileHandle;
			cFileHandle                  = nullptr;

			musInfo.trackLoop = trackPtr->trackLoop;
			musInfo.loopPoint = trackPtr->loopPoint;
			musInfo.loaded    = true;

			unsigned long long samples = 0;

			ov_callbacks callbacks;

			callbacks.read_func  = readVorbis;
			callbacks.seek_func  = seekVorbis;
			callbacks.tell_func  = tellVorbis;
			callbacks.close_func = closeVorbis;

			int error = ov_open_callbacks(&musInfo, &musInfo.vorbisFile, 
					NULL, 0, callbacks);
			if (error != 0) {
				printf("Error reading ogg file.\n");
			}

			musInfo.vorbBitstream = -1;
			musInfo.vorbisFile.vi = ov_info(&musInfo.vorbisFile, -1);
			musInfo.buffer = new Sint16[MIX_BUFFER_SAMPLES];

			musicStatus  = MUSIC_PLAYING;
			masterVolume = MAX_VOLUME;
			trackID      = trackBuffer;
			trackBuffer  = -1;
		}
	}

	if (musicStatus == MUSIC_PLAYING) {
		for (size_t i = 0; i < ARRAY_SIZE(s_waveBufs); i++) {

			if (s_waveBufs[i].status != NDSP_WBUF_DONE) {
				//printf("WaveBuf %d Status: %d\n", i, s_waveBufs[i].status);
				continue;
			}

			//printf("Found open wbuf: %d\n", i);

			_3ds_musicDecode(&musInfo, &s_waveBufs[i]);
		}
	}


}

void _3ds_musicDecode(MusicPlaybackInfo* m, ndspWaveBuf* wbuf) {
	int totalSamples = 0;
	int currentSection;
	long ret = -1;
	while (totalSamples < SAMPLES_PER_BUF) {
		if (musicStatus != MUSIC_PLAYING) {
			ndspChnWaveBufClear(0);
			return;
		}

		s8* buffer = wbuf->data_pcm8 + (totalSamples * CHANNELS_PER_SAMPLE);
		const size_t bufferSize = (SAMPLES_PER_BUF - totalSamples) * CHANNELS_PER_SAMPLE;

		long ret = ov_read(&musInfo.vorbisFile, (char*)buffer, bufferSize,
					&musInfo.vorbBitstream);
		if (ret < -1) {
			printf("error in stream, cannot flush audio\n");
			return;
		}

                if (ret == 0) {
                    // We've reached the end of the file
                    if (m->trackLoop) {
                        ov_pcm_seek(&m->vorbisFile, m->loopPoint);
                        continue;
                    }
                    else {
                        musicStatus = MUSIC_STOPPED;
                        break;
                    }
                }

		totalSamples += ret / CHANNELS_PER_SAMPLE;
	}

	wbuf->nsamples = totalSamples / 2;
	ndspChnWaveBufAdd(0, wbuf);
	DSP_FlushDataCache(wbuf->data_pcm8, totalSamples * CHANNELS_PER_SAMPLE * sizeof(s8));
}

void _3ds_sfxLogic() {
	for (byte i = 0; i < CHANNEL_COUNT; i++) {
		ChannelInfo* sfx = &sfxChannels[i];
		if (sfx == NULL)
			continue;

		if (sfx->sfxID < 0)
			continue;

		if (sfx->samplePtr == nullptr) {
			//printf("null pointer\n");
			continue;
		}

		for (byte j = 0; j < ARRAY_SIZE(s_sfxWaveBufs); j++) {
			if (s_sfxWaveBufs[j].status == NDSP_WBUF_DONE) {
				_3ds_sfxDecode(sfx, &s_sfxWaveBufs[j]);
				sfx->samplePtr = nullptr;
				break;
			}
		}
	}
}

void _3ds_sfxDecode(ChannelInfo* sfx, ndspWaveBuf* wbuf) {
	int totalSamples = 0;
	int channelToUse = -1;
	/*
	while (totalSamples < SFX_SAMPLE_SZ) {
		u8* buffer = (u8*) wbuf->data_pcm8 + (totalSamples * CHANNELS_PER_SAMPLE);

		size_t sampleLen = (sfx->sampleLength < SFX_SAMPLE_SZ - totalSamples) ? sfx->sampleLength : SFX_SAMPLE_SZ - totalSamples;
		memcpy(&buffer[totalSamples], sfx->samplePtr, sampleLen * sizeof(u8));

 		totalSamples += sampleLen;
 		sfx->samplePtr += sampleLen;
 		sfx->sampleLength -= sampleLen;

              	if (sfx->sampleLength == 0) {
                	if (sfx->loopSFX) {
                        	sfx->samplePtr    = sfxList[sfx->sfxID].buffer;
                            	sfx->sampleLength = sfxList[sfx->sfxID].length;
                        }
                        else {
                            	StopSfx(sfx->sfxID);
                            	return;
                        }
		}

	}
	*/
	
	// find an open NDSP channel to play on
	for (int i = 0; i < _3DS_MAX_CHANNELS; i++) {
		if (!ndspChnIsPlaying(i)) {
			channelToUse = i;
			break;
		}
	}

	// it's honestly incredibly unlikely this will ever happen but just in case
	if (channelToUse == -1) {
		printf("All NDSP channels busy. Ignoring SFX.\n");
		return;
	}

	memcpy(wbuf->data_pcm8, sfx->samplePtr, SFX_BUFFER_SZ);
	wbuf->nsamples = sfx->sampleLength;
	ndspChnWaveBufAdd(channelToUse, wbuf);
	DSP_FlushDataCache(wbuf->data_pcm8, totalSamples * CHANNELS_PER_SAMPLE * sizeof(s8));
}

void _3ds_audioThread(void* const nul) {
	printf("Audio thread running\n");

	while (!s_quit) {
		_3ds_musicLogic();
		_3ds_sfxLogic();

		LightEvent_Wait(&s_event);
	}

	printf("exiting audio thread...\n");
}

#endif
