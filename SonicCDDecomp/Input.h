#ifndef INPUT_H
#define INPUT_H

namespace Input {

	class InputData {
	public:
		bool Up;
		bool Down;
		bool Left;
		bool Right;
		bool A;
		bool B;
		bool C;
		bool Start;
		int TouchDown[8];
		int TouchX[8];
		int TouchY[8];
		int TouchID[8];
		int Touches;
	};

	static InputData KeyPress;
	static InputData KeyDown;
	static InputData KeyRelease;

	void CheckKeyPress(InputData* input, byte Flags);
	void CheckKeyDown(InputData* input, byte Flags);

}

#endif // !INPUT_H
