#ifndef TEXTSYSTEM_H
#define TEXTSYSTEM_H

namespace TextSystem {

	class TextMenu {
	public:
		ushort TextData[10240];
		int EntryStart[512];
		int EntrySize[512];
		byte EntryHighlight[512];
		int TextDataPos;
		int Selection1;
		int Selection2;
		ushort numRows;
		ushort numVisibleRows;
		ushort VisibleRowOffset;
		byte Alignment;
		byte numSelections;
		byte timer;
		byte field_6215;
		byte field_6216;
		byte field_6217;
	};

	class FontCharacter {
	public:
		int ID;
		short SrcX;
		short SrcY;
		short Width;
		short Height;
		short PivotX;
		short PivotY;
		short XAdvance;
	};

	static TextMenu GameMenu;
	static int TextMenuSurfaceNo;
	static int Menu1Selection;
	static int Menu2Selection;

}

#endif // !TEXTSYSTEM_H
