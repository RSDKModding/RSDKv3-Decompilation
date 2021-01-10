#include "RetroEngine.hpp"
#include <cmath>

int globalSFXCount = 0;
int stageSFXCount  = 0;

int masterVolume  = MAX_VOLUME;
int trackID     = -1;
int sfxVolume   = MAX_VOLUME;
int bgmVolume     = MAX_VOLUME;
bool audioEnabled = false;

int nextChannelPos;
bool musicEnabled;
int musicStatus;
TrackInfo musicTracks[TRACK_COUNT];
SFXInfo sfxList[SFX_COUNT];

ChannelInfo sfxChannels[CHANNEL_COUNT];

MusicPlaybackInfo musInfo;

#if RETRO_USING_SDL
SDL_AudioSpec audioDeviceFormat;
SDL_AudioStream *ogv_stream;

#define AUDIO_FREQUENCY (44100)
#define AUDIO_FORMAT    (AUDIO_S16SYS) /**< Signed 16-bit samples */
#define AUDIO_SAMPLES   (0x800)
#define AUDIO_CHANNELS  (2)

#define ADJUST_VOLUME(s, v) (s = (s * v) / MAX_VOLUME)

#endif

#define AUDIO_BUFFERSIZE (0x4000)

#define MIX_BUFFER_SAMPLES (256)

int InitAudioPlayback()
{
    StopAllSfx(); //"init"
#if RETRO_USING_SDL
    SDL_AudioSpec want;
    audioDeviceFormat.freq     = AUDIO_FREQUENCY;
    audioDeviceFormat.format   = AUDIO_FORMAT;
    audioDeviceFormat.samples  = AUDIO_SAMPLES;
    audioDeviceFormat.channels = AUDIO_CHANNELS;
    audioDeviceFormat.callback = ProcessAudioPlayback;

    if (SDL_OpenAudio(&audioDeviceFormat, NULL) >= 0) {
        audioEnabled = true;
        SDL_PauseAudio(0);
    }
    else {
        printLog("Unable to open audio device: %s", SDL_GetError());
        return false;
    }

    // Init video sound stuff
    // TODO: Unfortunately, we're assuming that video sound is stereo at 48000Hz.
    // This is true of every .ogv file in the game (the Steam version, at least),
    // but it would be nice to make this dynamic. Unfortunately, THEORAPLAY's API
    // makes this awkward.
    ogv_stream = SDL_NewAudioStream(AUDIO_F32, 2, 48000, audioDeviceFormat.format,
                                  audioDeviceFormat.channels, audioDeviceFormat.freq);
    if (!ogv_stream) {
        printLog("Failed to create stream: %s", SDL_GetError());
        SDL_CloseAudio();
        return false;
    }

    #endif

    FileInfo info;
    FileInfo infoStore;
    char strBuffer[0x100];
    int fileBuffer  = 0;
    int fileBuffer2 = 0;

    if (LoadFile("Data/Game/Gameconfig.bin", &info)) {
        infoStore = info;

        FileRead(&fileBuffer, 1);
        FileRead(strBuffer, fileBuffer);
        strBuffer[fileBuffer] = 0;

        FileRead(&fileBuffer, 1);
        FileRead(&strBuffer, fileBuffer); // Load 'Data'
        strBuffer[fileBuffer] = 0;

        FileRead(&fileBuffer, 1);
        FileRead(strBuffer, fileBuffer);
        strBuffer[fileBuffer] = 0;

        // Read Obect Names
        int objectCount = 0;
        FileRead(&objectCount, 1);
        for (int o = 0; o < objectCount; ++o) {
            FileRead(&fileBuffer, 1);
            FileRead(strBuffer, fileBuffer);
            strBuffer[fileBuffer] = 0;
        }

        // Read Script Paths
        for (int s = 0; s < objectCount; ++s) {
            FileRead(&fileBuffer, 1);
            FileRead(strBuffer, fileBuffer);
            strBuffer[fileBuffer] = 0;
        }

        int varCnt = 0;
        FileRead(&varCnt, 1);
        for (int v = 0; v < varCnt; ++v) {
            FileRead(&fileBuffer, 1);
            FileRead(strBuffer, fileBuffer);
            strBuffer[fileBuffer] = 0;

            // Read Variable Value
            FileRead(&fileBuffer2, 4);
        }

        // Read SFX
        globalSFXCount = 0;
        FileRead(&globalSFXCount, 1);
        for (int s = 0; s < globalSFXCount; ++s) {
            FileRead(&fileBuffer, 1);
            FileRead(strBuffer, fileBuffer);
            strBuffer[fileBuffer] = 0;

            GetFileInfo(&infoStore);
            LoadSfx(strBuffer, s);
            SetFileInfo(&infoStore);
        }

        CloseFile();
    }

    // sfxDataPosStage = sfxDataPos;
    nextChannelPos = 0;
    for (int i = 0; i < CHANNEL_COUNT; ++i) sfxChannels[i].sfxID = -1;

    return true;
}

#if RETRO_USING_SDL
int readVorbisStream(void *dst, uint size)
{
    int tot = 0;
    int read;
    int to_read = size;
    char *buf   = (char *)dst;
    unsigned long long left   = musInfo.audioLen;
    while (to_read && (read = (int)ov_read(&musInfo.vorbisFile, buf, to_read, 0, 2, 1, &musInfo.vorbBitstream))) {
        if (read < 0) {
            return 0;
        }
        to_read -= read;
        buf += read;
        tot += read;
        left -= read;
        if (left <= 0)
            break;
    }
    return tot;
}


int trackRequestMoreData(uint samples, uint amount)
{
    int out   = amount / 2;
    int avail = SDL_AudioStreamAvailable(musInfo.stream);

    if (avail < out * 2) {

        int numSamples = 0;
        numSamples = readVorbisStream(musInfo.extraBuffer, (musInfo.spec.format & 0xFF) / 8 * samples * musInfo.spec.channels);

        if (numSamples == 0)
            return 0;

        int rc = SDL_AudioStreamPut(musInfo.stream, musInfo.extraBuffer, numSamples);
        if (rc == -1)
            return -1;
    }

    int get = SDL_AudioStreamGet(musInfo.stream, musInfo.buffer, out);
    if (get == -1) {
        return -1;
    }
    if (get == 0)
        get = -2;

    return get;
}
#endif


void ProcessMusicStream(Sint32 *stream, size_t len)
{
    if (!musInfo.loaded)
        return;
    switch (musicStatus) {
        case MUSIC_READY:
        case MUSIC_PLAYING: {
#if RETRO_USING_SDL
            int bytes        = trackRequestMoreData(AUDIO_SAMPLES, len * sizeof(Sint16) * 2);
            if (bytes > 0) {
                int vol = (bgmVolume * masterVolume) / MAX_VOLUME;
                ProcessAudioMixing(stream, musInfo.buffer, len, vol, 0);
            }

            switch (bytes) {
                case -2:
                case -1: break;
                case 0:
                    if (musInfo.trackLoop)
                        ov_pcm_seek(&musInfo.vorbisFile, musInfo.loopPoint);
                    else
                        musicStatus = MUSIC_STOPPED;
                    break;
            }
#endif
        } break;
        case MUSIC_STOPPED:
        case MUSIC_PAUSED:
        case MUSIC_LOADING:
            // dont play
            break;
    }
}

void ProcessAudioPlayback(void *userdata, Uint8 *stream, int len)
{
    (void)userdata; // Unused

    if (!audioEnabled)
        return;

    Sint16 *output_buffer = (Sint16*)stream;

    size_t samples_remaining = (size_t)len / sizeof(Sint16);
    while (samples_remaining != 0) {
        Sint32 mix_buffer[MIX_BUFFER_SAMPLES];
        memset(mix_buffer, 0, sizeof(mix_buffer));

        const size_t samples_to_do = (samples_remaining < MIX_BUFFER_SAMPLES) ? samples_remaining : MIX_BUFFER_SAMPLES;

        ProcessMusicStream(mix_buffer, samples_to_do);

        // Process music being played by a video
        if (videoPlaying) {
            // Fetch THEORAPLAY audio packets, and shove them into the SDL Audio Stream
            const size_t bytes_to_do = samples_to_do * sizeof(Sint16);

            const THEORAPLAY_AudioPacket *packet;

            while ((packet = THEORAPLAY_getAudio(videoDecoder)) != NULL) {
                SDL_AudioStreamPut(ogv_stream, packet->samples, packet->frames * sizeof(float) * 2); // 2 for stereo
                THEORAPLAY_freeAudio(packet);
            }

            Sint16 buffer[MIX_BUFFER_SAMPLES];

            // If we need more samples, assume we've reached the end of the file,
            // and flush the audio stream so we can get more. If we were wrong, and
            // there's still more file left, then there will be a gap in the audio. Sorry.
            if (SDL_AudioStreamAvailable(ogv_stream) < bytes_to_do)
                SDL_AudioStreamFlush(ogv_stream);

            // Fetch the converted audio data, which is ready for mixing.
            // TODO: This code doesn't account for `len` being larger than the buffer.
            // ...But neither does `trackRequestMoreData`, so I guess it's not my problem.
            int get = SDL_AudioStreamGet(ogv_stream, buffer, bytes_to_do);

            // Mix the converted audio data into the final output
            if (get != -1)
                ProcessAudioMixing(mix_buffer, buffer, get / sizeof(Sint16), (bgmVolume * masterVolume) / MAX_VOLUME, 0); // TODO - Should we be using the music volume?
        }
        else {
            SDL_AudioStreamClear(ogv_stream);   // Prevent leftover audio from playing at the start of the next video
        }

        for (byte i = 0; i < CHANNEL_COUNT; ++i) {
            ChannelInfo *sfx = &sfxChannels[i];
            if (sfx == NULL)
                continue;

            if (sfx->sfxID < 0)
                continue;

            if (sfx->samplePtr) {
                if (sfx->sampleLength > 0) {
                    int sampleLen = ((sfx->sampleLength/sizeof(Sint16)) < samples_to_do) ? (sfx->sampleLength/sizeof(Sint16)) : samples_to_do;
    #if RETRO_USING_SDL
                    ProcessAudioMixing(mix_buffer, sfx->samplePtr, sampleLen, sfxVolume, sfx->pan);
    #endif

                    sfx->samplePtr += sampleLen;
                    sfx->sampleLength -= sampleLen*sizeof(Sint16);
                }

                if (sfx->sampleLength <= 0) {
                    if (sfx->loopSFX) {
                        sfx->samplePtr    = sfxList[sfx->sfxID].buffer;
                        sfx->sampleLength = sfxList[sfx->sfxID].length;
                    }
                    else {
                        StopSfx(sfx->sfxID);
                    }
                }
            }
        }

        for (size_t i = 0; i < sizeof(mix_buffer) / sizeof(*mix_buffer); ++i)
        {
            const Sint16 max_audioval = ((1 << (16 - 1)) - 1);
            const Sint16 min_audioval = -(1 << (16 - 1));

            const Sint32 sample = mix_buffer[i];

            if (sample > max_audioval)
                *output_buffer++ = max_audioval;
            else if (sample < min_audioval)
                *output_buffer++ = min_audioval;
            else
                *output_buffer++ = sample;
        }

        samples_remaining -= samples_to_do;
    }
}

#if RETRO_USING_SDL
void ProcessAudioMixing(Sint32 *dst, const Sint16 *src, int len, int volume, signed char pan)
{
    if (volume == 0)
        return;

    if (volume > MAX_VOLUME)
        volume = MAX_VOLUME;

    float panL     = 0;
    float panR     = 0;
    int i          = 0;

    if (pan < 0) {
        panR = 1.0f - abs(pan / 100.0f);
        panL = 1.0f;
    }
    else if (pan > 0) {
        panL = 1.0f - abs(pan / 100.0f);
        panR = 1.0f;
    }

    while (len--) {
        Sint32 sample = *src++;
        ADJUST_VOLUME(sample, volume);

        if (pan != 0) {
            if ((i % 2) != 0) {
                sample *= panR;
            }
            else {
                sample *= panL;
            }
        }

        *dst++ += sample;

        i++;
    }
}
#endif

#if RETRO_USING_SDL
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
int closeVorbis(void *ptr)
{
    return CloseFile2();
}
#endif

void SetMusicTrack(char *filePath, byte trackID, bool loop, uint loopPoint)
{
    TrackInfo *track = &musicTracks[trackID];
    StrCopy(track->fileName, "Data/Music/");
    StrAdd(track->fileName, filePath);
    track->trackLoop = loop;
    track->loopPoint = loopPoint;
}
bool PlayMusic(int track)
{
    if (track < 0 || track >= TRACK_COUNT) {
        StopMusic();
        return false;
    }

    TrackInfo *trackPtr = &musicTracks[track];

    if (!trackPtr->fileName[0]) {
        StopMusic();
        return false;
    }

    if (LoadFile(trackPtr->fileName, &musInfo.fileInfo)) {
        if (musInfo.loaded)
            StopMusic();

        cFileHandleStream = cFileHandle;
        cFileHandle       = nullptr;

        musInfo.trackLoop = trackPtr->trackLoop;
        musInfo.loopPoint = trackPtr->loopPoint;
        musInfo.loaded       = true;

#if RETRO_USING_SDL
        ov_callbacks callbacks;

        callbacks.read_func  = readVorbis;
        callbacks.seek_func  = seekVorbis;
        callbacks.tell_func  = tellVorbis;
        callbacks.close_func = closeVorbis;

        int error = ov_open_callbacks(&musInfo, &musInfo.vorbisFile, NULL, 0, callbacks);
        if (error != 0) {
            return false;
        }

        musInfo.vorbBitstream = -1;
        musInfo.vorbisFile.vi   = ov_info(&musInfo.vorbisFile, -1);

        memset(&musInfo.spec, 0, sizeof(SDL_AudioSpec));

        musInfo.spec.format   = AUDIO_S16;
        musInfo.spec.channels = musInfo.vorbisFile.vi->channels;
        musInfo.spec.freq     = musInfo.vorbisFile.vi->rate;
        musInfo.spec.samples  = 4096;

        unsigned long long samples = (unsigned long long)ov_pcm_total(&musInfo.vorbisFile, -1);

        musInfo.audioLen = samples * musInfo.spec.channels * 2;
        musInfo.spec.size = AUDIO_BUFFERSIZE;

        musInfo.stream = SDL_NewAudioStream(musInfo.spec.format, musInfo.spec.channels, musInfo.spec.freq, audioDeviceFormat.format,
                                      audioDeviceFormat.channels, audioDeviceFormat.freq);
        if (!musInfo.stream) {
            printLog("Failed to create stream: %s", SDL_GetError());
        }

        musInfo.buffer      = new Sint16[AUDIO_BUFFERSIZE];
        musInfo.extraBuffer = new Sint16[AUDIO_BUFFERSIZE];
#endif

        musicStatus = MUSIC_PLAYING;
        masterVolume = MAX_VOLUME;
        trackID      = track;
        return true;
    }
    return false;
}

void LoadSfx(char *filePath, byte sfxID) {
    FileInfo info;
    char fullPath[0x80];

    StrCopy(fullPath, "Data/SoundFX/");
    StrAdd(fullPath, filePath);

    if (LoadFile(fullPath, &info)) {
        byte* sfx = new byte[info.fileSize];
        FileRead(sfx, info.fileSize);
        CloseFile();

#if RETRO_USING_SDL
        SDL_LockAudio();
        SDL_RWops *src = SDL_RWFromMem(sfx, info.fileSize);
        if (src == NULL) {
            printLog("Unable to open sfx: %s", info.fileName);
        }
        else {
            SDL_AudioSpec wav_spec;
            uint wav_length;
            byte *wav_buffer;
            SDL_AudioSpec *wav = SDL_LoadWAV_RW(src, 0, &wav_spec, &wav_buffer, &wav_length);

            SDL_RWclose(src);
            delete[] sfx;
            if (wav == NULL) {
                printLog("Unable to read sfx: %s", info.fileName);
            }
            else {
                SDL_AudioCVT convert;
                if (SDL_BuildAudioCVT(&convert, wav->format, wav->channels, wav->freq, audioDeviceFormat.format, audioDeviceFormat.channels,
                                      audioDeviceFormat.freq)
                    > 0) {
                    convert.buf = (byte *)malloc(wav_length * convert.len_mult);
                    convert.len = wav_length;
                    memcpy(convert.buf, wav_buffer, wav_length);
                    SDL_ConvertAudio(&convert);

                    StrCopy(sfxList[sfxID].name, filePath);
                    sfxList[sfxID].buffer = (Sint16*)convert.buf;
                    sfxList[sfxID].length = convert.len_cvt;
                    sfxList[sfxID].loaded = true;
                    SDL_FreeWAV(wav_buffer);
                }
                else {
                    StrCopy(sfxList[sfxID].name, filePath);
                    sfxList[sfxID].buffer = (Sint16*)wav_buffer;
                    sfxList[sfxID].length = wav_length;
                    sfxList[sfxID].loaded = true;
                }
            }
        }
        SDL_UnlockAudio();
#endif
    }
}
void PlaySfx(int sfx, bool loop)
{
    int sfxChannelID = nextChannelPos++;
    for (int c = 0; c < CHANNEL_COUNT; ++c) {
        if (sfxChannels[c].sfxID == sfx) {
            sfxChannelID = c;
            break;
        }
    }

    ChannelInfo *sfxInfo      = &sfxChannels[sfxChannelID];
    sfxInfo->sfxID        = sfx;
    sfxInfo->samplePtr        = sfxList[sfx].buffer;
    sfxInfo->sampleLength     = sfxList[sfx].length;
    sfxInfo->loopSFX      = loop;
    sfxInfo->pan          = 0;
    if (nextChannelPos == CHANNEL_COUNT)
        nextChannelPos = 0;
}
void SetSfxAttributes(int sfx, int loopCount, sbyte pan)
{
    int sfxChannel = -1;
    for (int i = 0; i < CHANNEL_COUNT; ++i) {
        if (sfxChannels[i].sfxID == sfx || sfxChannels[i].sfxID == -1) {
            sfxChannel = i;
            break;
        }
    }
    if (sfxChannel == -1)
        return; // wasn't found

    //TODO: is this right? should it play an sfx here? without this rings dont play any sfx so I assume it must be?
    ChannelInfo *sfxInfo  = &sfxChannels[sfxChannel];
    sfxInfo->samplePtr    = sfxList[sfx].buffer;
    sfxInfo->sampleLength = sfxList[sfx].length;
    sfxInfo->loopSFX     = loopCount == -1 ? sfxInfo->loopSFX : loopCount;
    sfxInfo->pan         = pan;
    sfxInfo->sfxID       = sfx;
}
