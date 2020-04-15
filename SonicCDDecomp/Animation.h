#ifndef ANIMATION_H
#define ANIMATION_H

namespace Animation {

	class AnimationFile {
	public:
		char FileName[0x20];
		int AnimCount;
		int AniListOffset;
		int HitboxListOffset;
	};

}

#endif // !ANIMATION_H
