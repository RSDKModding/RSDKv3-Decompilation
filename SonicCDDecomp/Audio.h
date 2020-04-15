#ifndef AUDIO_H
#define AUDIO_H

namespace Audio {

	class TrackInfo {
	public:
		char Name[0x40];
		int TrackLoop;
		int LoopPoint;
	};

	class SFXInfo {
	public:
		int SampleStartPtr;
		int SamplePtr1;
		int SamplePtr2;
		int SFXID;
		byte LoopSFX;
		byte SFXPan;
		byte field_12;
		byte field_13;
	};

	static int numGlobalSFX = 0;

	int InitAudioPlayback(void);

	void PauseSound(void);
	void ResumeSound(void);
}

#endif // !AUDIO_H
