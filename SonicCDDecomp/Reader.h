#ifndef READER_H
#define READER_H

namespace Reader {
	struct FileInfo {
	public:
		char FileName[0x100];
		int FileSize;
		int ReadPos;
		int BufferPosition;
		int VirtualFileOffset;
		byte eStringPosA;
		byte eStringPosB;
		byte eStringNo;
		byte eNybbleSwap;
	};

	static char RSDKName[1024];

	static byte fileBuffer[0x2000];
	static int FileSize;
	static int vFileSize;
	static int ReadPos;
	static int ReadSize;
	static int BufferPosition;
	static int VirtualFileOffset;
	static byte eStringPosA;
	static byte eStringPosB;
	static byte eStringNo;
	static byte eNybbleSwap;
	static char encryptionStringA[] = { "4RaS9D7KaEbxcp2o5r6t" };
	static char encryptionStringB[] = { "3tRaUxLmEaSn" };

	static FILE* cFileHandle;

	void CopyFilePath(char* Dest, const char* Src);
	bool CheckRSDKFile(const char* filename);

	int LoadFile(const char* Filename, FileInfo* FileInfo);
	int CloseFile(void);

	void FileRead(void* Dest, int Size);

	int ParseVirtualFileSystem(FileInfo* File);
	size_t FillFileBuffer(void);

	void GetFileInfo(FileInfo* FileInfo);
	void SetFileInfo(FileInfo* FileInfo);
	size_t GetFilePosition(void);
	void SetFilePosition(int NewPos);

	bool ReachedEndOfFile(void);
}

#endif // !READER_H
