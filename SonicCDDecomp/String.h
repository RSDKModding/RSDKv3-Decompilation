#ifndef STRING_H
#define STRING_H

namespace StringUtils {

	void StrCopy(char* Dest, char* Src);
	void StrAdd(char* Dest, char* Src);
	bool StrComp(char* StringA, char* StringB);
	int StrLength(char* String);
	int FindStringToken(char* String, char* Token, char StopID);
	bool ConvertStringToInteger(char* String, int* Intager);

}

#endif // !STRING_H
