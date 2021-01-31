#define SAMPLE_RATE         (44100)
#define SAMPLES_PER_BUF     (SAMPLE_RATE * 120 / 1000)
#define CHANNELS_PER_SAMPLE (2)

#define THREAD_AFFINITY     (-1)
#define THREAD_STACK_SZ     (32 * 1024)

// came to this size after looking at some of the game's audio files
#define SFX_BUFFER_SZ       (300 * 1024)
#define SFX_SAMPLE_SZ       (SFX_BUFFER_SZ / SAMPLES_PER_BUF)

#define WAVEBUF_SIZE        (SAMPLES_PER_BUF * CHANNELS_PER_SAMPLE * sizeof(int16_t))
#define MIX_BUFFER_SAMPLES  (256)
#define BYTES_PER_SAMPLE    (4)
#define _3DS_MAX_CHANNELS   (24)

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define ADJUST_VOLUME(s, v) (s = (s * v) / MAX_VOLUME)

#if !RETRO_USING_SDL1_AUDIO
bool _3ds_audioInit();
void _3ds_audioExit();
void _3ds_musicLogic();
void _3ds_musicDecode(MusicPlaybackInfo* m, ndspWaveBuf* wbuf);
void _3ds_sfxLogic();
void _3ds_sfxDecode(ChannelInfo* sfx, ndspWaveBuf* wbuf);
void _3ds_audioCallback(void* const nul);
void _3ds_audioThread(void* const nul);

extern int trackBuffer;
extern LightEvent s_event;
#endif
