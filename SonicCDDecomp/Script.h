#ifndef SCRIPT_H
#define SCRIPT_H

namespace Script {

	class ObjectScript {
	public:
		int FrameCount;
		int SpriteSheetID;
		int ScriptCodePtr_Main;
		int ScriptCodePtr_PlayerInteraction;
		int ScriptCodePtr_Draw;
		int ScriptCodePtr_Startup;
		int JumpTablePtr_Main;
		int JumpTablePtr_PlayerInteraction;
		int JumpTablePtr_Draw;
		int JumpTablePtr_Startup;
		int FrameListOffset;
		int AnimationFil;
	};

	void ClearScriptData(void);
}

#endif // !SCRIPT_H
