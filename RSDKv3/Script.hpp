#ifndef SCRIPT_H
#define SCRIPT_H

#define SCRIPTDATA_COUNT (0x40000)
#define JUMPTABLE_COUNT  (0x4000)
#define FUNCTION_COUNT   (0x200)

#define JUMPSTACK_COUNT (0x400)
#define FUNCSTACK_COUNT (0x400)

#define RETRO_USE_COMPILER (1)

struct ScriptPtr {
    int scriptCodePtr;
    int jumpTablePtr;
};

struct ScriptFunction {
#if RETRO_USE_COMPILER
    char name[0x20];
#endif
    ScriptPtr ptr;
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
#if !RETRO_USE_ORIGINAL_CODE
    bool mobile; // flag for detecting mobile/updated bytecode
#endif
};

struct ScriptEngine {
    int operands[10];
    int tempValue[8];
    int arrayPosition[3];
    int checkResult;
};

enum ScriptSubs { SUB_MAIN = 0, SUB_PLAYERINTERACTION = 1, SUB_DRAW = 2, SUB_SETUP = 3 };

extern ObjectScript objectScriptList[OBJECT_COUNT];

extern ScriptFunction scriptFunctionList[FUNCTION_COUNT];
extern int scriptFunctionCount;

extern int scriptCode[SCRIPTDATA_COUNT];
extern int jumpTableData[JUMPTABLE_COUNT];

extern int jumpTableStack[JUMPSTACK_COUNT];
extern int functionStack[FUNCSTACK_COUNT];

extern int scriptCodePos;
extern int scriptCodeOffset;
extern int jumpTablePos;
extern int jumpTableOffset;

extern int jumpTableStackPos;
extern int functionStackPos;

extern ScriptEngine scriptEng;
extern char scriptText[0x100];


extern int aliasCount;
extern int lineID;

bool ConvertStringToInteger(char *text, int *value);

#if RETRO_USE_COMPILER

void CheckAliasText(char *text);
void ConvertArithmaticSyntax(char *text);
void ConvertIfWhileStatement(char *text);
bool ConvertSwitchStatement(char *text);
void ConvertFunctionText(char *text);
void CheckCaseNumber(char *text);
bool ReadSwitchCase(char *text);
void AppendIntegerToString(char *text, int value);
void CopyAliasStr(char *dest, char *text, bool arrayIndex);
bool CheckOpcodeType(char *text); // Never actually used

void ParseScriptFile(char *scriptName, int scriptID);
#endif
void LoadBytecode(int stageListID, int scriptID);

void ProcessScript(int scriptCodeStart, int jumpTableStart, byte scriptSub);

void ClearScriptData();

#endif // !SCRIPT_H
