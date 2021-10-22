#ifndef SCRIPT_H
#define SCRIPT_H

#define SCRIPTDATA_COUNT (0x40000)
#define JUMPTABLE_COUNT  (0x4000)
#define FUNCTION_COUNT   (0x200)

#define JUMPSTACK_COUNT (0x400)
#define FUNCSTACK_COUNT (0x400)

struct ScriptPtr {
    int scriptCodePtr;
    int jumpTablePtr;
};

struct ObjectScript {
    int frameCount;
    int spriteSheetID;
    ScriptPtr subMain;
    ScriptPtr subPlayerInteraction;
    ScriptPtr subDraw;
    ScriptPtr subStartup;
    int frameListOffset;
    AnimationFile *animFile;
    bool mobile; // flag for detecting mobile/updated bytecode
};

struct ScriptEngine {
    int operands[10];
    int tempValue[8];
    int arrayPosition[3];
    int checkResult;
};

enum ScriptSubs { SUB_MAIN = 0, SUB_PLAYERINTERACTION = 1, SUB_DRAW = 2, SUB_SETUP = 3 };

extern ObjectScript objectScriptList[OBJECT_COUNT];
extern ScriptPtr functionScriptList[FUNCTION_COUNT];

extern int scriptData[SCRIPTDATA_COUNT];
extern int jumpTableData[JUMPTABLE_COUNT];

extern int jumpTableStack[JUMPSTACK_COUNT];
extern int functionStack[FUNCSTACK_COUNT];

extern int scriptCodePos; // Bytecode file readpos
extern int jumpTablePos;  // Bytecode file readpos

extern int jumpTableStackPos;
extern int functionStackPos;

extern ScriptEngine scriptEng;
extern char scriptText[0x100];

extern int scriptDataPos;
extern int scriptDataOffset;
extern int jumpTableDataPos;
extern int jumpTableDataOffset;

extern int scriptFunctionCount;
extern char scriptFunctionNames[FUNCTION_COUNT][0x20];

extern int aliasCount;
extern int lineID;

void CheckAliasText(char *text);
void ConvertArithmaticSyntax(char *text);
void ConvertIfWhileStatement(char *text);
bool ConvertSwitchStatement(char *text);
void ConvertFunctionText(char *text);
void CheckCaseNumber(char *text);
bool ReadSwitchCase(char *text);
void AppendIntegerToString(char *text, int value);
bool ConvertStringToInteger(char *text, int *value);
void CopyAliasStr(char *dest, char *text, bool arrayIndex);
bool CheckOpcodeType(char *text); // Never actually used

void ParseScriptFile(char *scriptName, int scriptID);
void LoadBytecode(int stageListID, int scriptID);

void ProcessScript(int scriptCodePtr, int jumpTablePtr, byte scriptSub);

void ClearScriptData();

#endif // !SCRIPT_H
