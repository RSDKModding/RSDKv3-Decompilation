#include "RetroEngine.hpp"
#include <cmath>

ObjectScript objectScriptList[OBJECT_COUNT];

ScriptFunction scriptFunctionList[FUNCTION_COUNT];
int scriptFunctionCount = 0;

int scriptCode[SCRIPTDATA_COUNT];
int jumpTable[JUMPTABLE_COUNT];
int jumpTableStack[JUMPSTACK_COUNT];
int functionStack[FUNCSTACK_COUNT];

int scriptCodePos     = 0;
int scriptCodeOffset  = 0;
int jumpTablePos      = 0;
int jumpTableOffset   = 0;
int jumpTableStackPos = 0;
int functionStackPos  = 0;

ScriptEngine scriptEng = ScriptEngine();
char scriptText[0x100];


#define COMMONALIAS_COUNT (0x20)
#define ALIAS_COUNT       (COMMONALIAS_COUNT + 0x60)
int aliasCount = 0;
int lineID     = 0;

#if RETRO_USE_COMPILER
struct AliasInfo {
    AliasInfo()
    {
        StrCopy(name, "");
        StrCopy(value, "");
    }
    AliasInfo(const char *aliasName, const char *aliasVal)
    {
        StrCopy(name, aliasName);
        StrCopy(value, aliasVal);
    }

    char name[0x20];
    char value[0x20];
};
#endif

struct FunctionInfo {
    FunctionInfo()
    {
        StrCopy(name, "");
        opcodeSize = 0;
    }
    FunctionInfo(const char *functionName, int opSize)
    {
        StrCopy(name, functionName);
        opcodeSize = opSize;
    }

    char name[0x20];
    int opcodeSize;
};

#if RETRO_USE_COMPILER
const char variableNames[][0x20] = {
    "TempValue0",
    "TempValue1",
    "TempValue2",
    "TempValue3",
    "TempValue4",
    "TempValue5",
    "TempValue6",
    "TempValue7",
    "CheckResult",
    "ArrayPos0",
    "ArrayPos1",
    "Global",
    "Object.EntityNo",
    "Object.Type",
    "Object.PropertyValue",
    "Object.XPos",
    "Object.YPos",
    "Object.iXPos",
    "Object.iYPos",
    "Object.State",
    "Object.Rotation",
    "Object.Scale",
    "Object.Priority",
    "Object.DrawOrder",
    "Object.Direction",
    "Object.InkEffect",
    "Object.Alpha",
    "Object.Frame",
    "Object.Animation",
    "Object.PrevAnimation",
    "Object.AnimationSpeed",
    "Object.AnimationTimer",
    "Object.Value0",
    "Object.Value1",
    "Object.Value2",
    "Object.Value3",
    "Object.Value4",
    "Object.Value5",
    "Object.Value6",
    "Object.Value7",
    "Object.OutOfBounds",
    "Player.State",
    "Player.ControlMode",
    "Player.ControlLock",
    "Player.CollisionMode",
    "Player.CollisionPlane",
    "Player.XPos",
    "Player.YPos",
    "Player.iXPos",
    "Player.iYPos",
    "Player.ScreenXPos",
    "Player.ScreenYPos",
    "Player.Speed",
    "Player.XVelocity",
    "Player.YVelocity",
    "Player.Gravity",
    "Player.Angle",
    "Player.Skidding",
    "Player.Pushing",
    "Player.TrackScroll",
    "Player.Up",
    "Player.Down",
    "Player.Left",
    "Player.Right",
    "Player.JumpPress",
    "Player.JumpHold",
    "Player.FollowPlayer1",
    "Player.LookPos",
    "Player.Water",
    "Player.TopSpeed",
    "Player.Acceleration",
    "Player.Deceleration",
    "Player.AirAcceleration",
    "Player.AirDeceleration",
    "Player.GravityStrength",
    "Player.JumpStrength",
    "Player.JumpCap",
    "Player.RollingAcceleration",
    "Player.RollingDeceleration",
    "Player.EntityNo",
    "Player.CollisionLeft",
    "Player.CollisionTop",
    "Player.CollisionRight",
    "Player.CollisionBottom",
    "Player.Flailing",
    "Player.Timer",
    "Player.TileCollisions",
    "Player.ObjectInteraction",
    "Player.Visible",
    "Player.Rotation",
    "Player.Scale",
    "Player.Priority",
    "Player.DrawOrder",
    "Player.Direction",
    "Player.InkEffect",
    "Player.Alpha",
    "Player.Frame",
    "Player.Animation",
    "Player.PrevAnimation",
    "Player.AnimationSpeed",
    "Player.AnimationTimer",
    "Player.Value0",
    "Player.Value1",
    "Player.Value2",
    "Player.Value3",
    "Player.Value4",
    "Player.Value5",
    "Player.Value6",
    "Player.Value7",
    "Player.Value8",
    "Player.Value9",
    "Player.Value10",
    "Player.Value11",
    "Player.Value12",
    "Player.Value13",
    "Player.Value14",
    "Player.Value15",
    "Player.OutOfBounds",
    "Stage.State",
    "Stage.ActiveList",
    "Stage.ListPos",
    "Stage.TimeEnabled",
    "Stage.MilliSeconds",
    "Stage.Seconds",
    "Stage.Minutes",
    "Stage.ActNo",
    "Stage.PauseEnabled",
    "Stage.ListSize",
    "Stage.NewXBoundary1",
    "Stage.NewXBoundary2",
    "Stage.NewYBoundary1",
    "Stage.NewYBoundary2",
    "Stage.XBoundary1",
    "Stage.XBoundary2",
    "Stage.YBoundary1",
    "Stage.YBoundary2",
    "Stage.DeformationData0",
    "Stage.DeformationData1",
    "Stage.DeformationData2",
    "Stage.DeformationData3",
    "Stage.WaterLevel",
    "Stage.ActiveLayer",
    "Stage.MidPoint",
    "Stage.PlayerListPos",
    "Stage.ActivePlayer",
    "Screen.CameraEnabled",
    "Screen.CameraTarget",
    "Screen.CameraStyle",
    "Screen.DrawListSize",
    "Screen.CenterX",
    "Screen.CenterY",
    "Screen.XSize",
    "Screen.YSize",
    "Screen.XOffset",
    "Screen.YOffset",
    "Screen.ShakeX",
    "Screen.ShakeY",
    "Screen.AdjustCameraY",
    "TouchScreen.Down",
    "TouchScreen.XPos",
    "TouchScreen.YPos",
    "Music.Volume",
    "Music.CurrentTrack",
    "KeyDown.Up",
    "KeyDown.Down",
    "KeyDown.Left",
    "KeyDown.Right",
    "KeyDown.ButtonA",
    "KeyDown.ButtonB",
    "KeyDown.ButtonC",
    "KeyDown.Start",
    "KeyPress.Up",
    "KeyPress.Down",
    "KeyPress.Left",
    "KeyPress.Right",
    "KeyPress.ButtonA",
    "KeyPress.ButtonB",
    "KeyPress.ButtonC",
    "KeyPress.Start",
    "Menu1.Selection",
    "Menu2.Selection",
    "TileLayer.XSize",
    "TileLayer.YSize",
    "TileLayer.Type",
    "TileLayer.Angle",
    "TileLayer.XPos",
    "TileLayer.YPos",
    "TileLayer.ZPos",
    "TileLayer.ParallaxFactor",
    "TileLayer.ScrollSpeed",
    "TileLayer.ScrollPos",
    "TileLayer.DeformationOffset",
    "TileLayer.DeformationOffsetW",
    "HParallax.ParallaxFactor",
    "HParallax.ScrollSpeed",
    "HParallax.ScrollPos",
    "VParallax.ParallaxFactor",
    "VParallax.ScrollSpeed",
    "VParallax.ScrollPos",
    "3DScene.NoVertices",
    "3DScene.NoFaces",
    "VertexBuffer.x",
    "VertexBuffer.y",
    "VertexBuffer.z",
    "VertexBuffer.u",
    "VertexBuffer.v",
    "FaceBuffer.a",
    "FaceBuffer.b",
    "FaceBuffer.c",
    "FaceBuffer.d",
    "FaceBuffer.Flag",
    "FaceBuffer.Color",
    "3DScene.ProjectionX",
    "3DScene.ProjectionY",
    "Engine.State",
    "Stage.DebugMode",
    "Engine.Message",
    "SaveRAM",
    "Engine.Language",
    "Object.SpriteSheet",
    "Engine.OnlineActive",
    "Engine.FrameSkipTimer",
    "Engine.FrameSkipSetting",
    "Engine.SFXVolume",
    "Engine.BGMVolume",
    "Engine.PlatformID",
    "Engine.TrialMode",
    "KeyPress.AnyStart",
#if RETRO_USE_HAPTICS
    "Engine.HapticsEnabled",
#endif
};
#endif

const FunctionInfo functions[] = {
    FunctionInfo("End", 0),
    FunctionInfo("Equal", 2),
    FunctionInfo("Add", 2),
    FunctionInfo("Sub", 2),
    FunctionInfo("Inc", 1),
    FunctionInfo("Dec", 1),
    FunctionInfo("Mul", 2),
    FunctionInfo("Div", 2),
    FunctionInfo("ShR", 2),
    FunctionInfo("ShL", 2),
    FunctionInfo("And", 2),
    FunctionInfo("Or", 2),
    FunctionInfo("Xor", 2),
    FunctionInfo("Mod", 2),
    FunctionInfo("FlipSign", 1),
    FunctionInfo("CheckEqual", 2),
    FunctionInfo("CheckGreater", 2),
    FunctionInfo("CheckLower", 2),
    FunctionInfo("CheckNotEqual", 2),
    FunctionInfo("IfEqual", 3),
    FunctionInfo("IfGreater", 3),
    FunctionInfo("IfGreaterOrEqual", 3),
    FunctionInfo("IfLower", 3),
    FunctionInfo("IfLowerOrEqual", 3),
    FunctionInfo("IfNotEqual", 3),
    FunctionInfo("else", 0),
    FunctionInfo("endif", 0),
    FunctionInfo("WEqual", 3),
    FunctionInfo("WGreater", 3),
    FunctionInfo("WGreaterOrEqual", 3),
    FunctionInfo("WLower", 3),
    FunctionInfo("WLowerOrEqual", 3),
    FunctionInfo("WNotEqual", 3),
    FunctionInfo("loop", 0),
    FunctionInfo("switch", 2),
    FunctionInfo("break", 0),
    FunctionInfo("endswitch", 0),
    FunctionInfo("Rand", 2),
    FunctionInfo("Sin", 2),
    FunctionInfo("Cos", 2),
    FunctionInfo("Sin256", 2),
    FunctionInfo("Cos256", 2),
    FunctionInfo("SinChange", 5),
    FunctionInfo("CosChange", 5),
    FunctionInfo("ATan2", 3),
    FunctionInfo("Interpolate", 4),
    FunctionInfo("InterpolateXY", 7),
    FunctionInfo("LoadSpriteSheet", 1),
    FunctionInfo("RemoveSpriteSheet", 1),
    FunctionInfo("DrawSprite", 1),
    FunctionInfo("DrawSpriteXY", 3),
    FunctionInfo("DrawSpriteScreenXY", 3),
    FunctionInfo("DrawTintRect", 4),
    FunctionInfo("DrawNumbers", 7),
    FunctionInfo("DrawActName", 7),
    FunctionInfo("DrawMenu", 3),
    FunctionInfo("SpriteFrame", 6),
    FunctionInfo("EditFrame", 7),
    FunctionInfo("LoadPalette", 5),
    FunctionInfo("RotatePalette", 3),
    FunctionInfo("SetScreenFade", 4),
    FunctionInfo("SetActivePalette", 3),
    FunctionInfo("SetPaletteFade", 7),
    FunctionInfo("CopyPalette", 2),
    FunctionInfo("ClearScreen", 1),
    FunctionInfo("DrawSpriteFX", 4),
    FunctionInfo("DrawSpriteScreenFX", 4),
    FunctionInfo("LoadAnimation", 1),
    FunctionInfo("SetupMenu", 4),
    FunctionInfo("AddMenuEntry", 3),
    FunctionInfo("EditMenuEntry", 4),
    FunctionInfo("LoadStage", 0),
    FunctionInfo("DrawRect", 8),
    FunctionInfo("ResetObjectEntity", 5),
    FunctionInfo("PlayerObjectCollision", 5),
    FunctionInfo("CreateTempObject", 4),
    FunctionInfo("BindPlayerToObject", 2),
    FunctionInfo("PlayerTileCollision", 0),
    FunctionInfo("ProcessPlayerControl", 0),
    FunctionInfo("ProcessAnimation", 0),
    FunctionInfo("DrawObjectAnimation", 0),
    FunctionInfo("DrawPlayerAnimation", 0),
    FunctionInfo("SetMusicTrack", 3),
    FunctionInfo("PlayMusic", 1),
    FunctionInfo("StopMusic", 0),
    FunctionInfo("PlaySfx", 2),
    FunctionInfo("StopSfx", 1),
    FunctionInfo("SetSfxAttributes", 3),
    FunctionInfo("ObjectTileCollision", 4),
    FunctionInfo("ObjectTileGrip", 4),
    FunctionInfo("LoadVideo", 1),
    FunctionInfo("NextVideoFrame", 0),
    FunctionInfo("PlayStageSfx", 2),
    FunctionInfo("StopStageSfx", 1),
    FunctionInfo("Not", 1),
    FunctionInfo("Draw3DScene", 0),
    FunctionInfo("SetIdentityMatrix", 1),
    FunctionInfo("MatrixMultiply", 2),
    FunctionInfo("MatrixTranslateXYZ", 4),
    FunctionInfo("MatrixScaleXYZ", 4),
    FunctionInfo("MatrixRotateX", 2),
    FunctionInfo("MatrixRotateY", 2),
    FunctionInfo("MatrixRotateZ", 2),
    FunctionInfo("MatrixRotateXYZ", 4),
    FunctionInfo("TransformVertices", 3),
    FunctionInfo("CallFunction", 1),
    FunctionInfo("EndFunction", 0),
    FunctionInfo("SetLayerDeformation", 6),
    FunctionInfo("CheckTouchRect", 4),
    FunctionInfo("GetTileLayerEntry", 4),
    FunctionInfo("SetTileLayerEntry", 4),
    FunctionInfo("GetBit", 3),
    FunctionInfo("SetBit", 3),
    FunctionInfo("PauseMusic", 0),
    FunctionInfo("ResumeMusic", 0),
    FunctionInfo("ClearDrawList", 1),
    FunctionInfo("AddDrawListEntityRef", 2),
    FunctionInfo("GetDrawListEntityRef", 3),
    FunctionInfo("SetDrawListEntityRef", 3),
    FunctionInfo("Get16x16TileInfo", 4),
    FunctionInfo("Copy16x16Tile", 2),
    FunctionInfo("Set16x16TileInfo", 4),
    FunctionInfo("GetAnimationByName", 2),
    FunctionInfo("ReadSaveRAM", 0),
    FunctionInfo("WriteSaveRAM", 0),
    FunctionInfo("LoadTextFont", 1),
    FunctionInfo("LoadTextFile", 3),
    FunctionInfo("DrawText", 7),
    FunctionInfo("GetTextInfo", 5),
    FunctionInfo("GetVersionNumber", 2),
    FunctionInfo("SetAchievement", 2),
    FunctionInfo("SetLeaderboard", 2),
    FunctionInfo("LoadOnlineMenu", 1),
    FunctionInfo("EngineCallback", 1),
#if RETRO_USE_HAPTICS
    FunctionInfo("HapticEffect", 4),
#endif
};

#if RETRO_USE_COMPILER
AliasInfo aliases[ALIAS_COUNT] = { AliasInfo("true", "1"),
                                   AliasInfo("false", "0"),
                                   AliasInfo("FX_SCALE", "0"),
                                   AliasInfo("FX_ROTATE", "1"),
                                   AliasInfo("FX_ROTOZOOM", "2"),
                                   AliasInfo("FX_INK", "3"),
                                   AliasInfo("PRESENTATION_STAGE", "0"),
                                   AliasInfo("REGULAR_STAGE", "1"),
                                   AliasInfo("BONUS_STAGE", "2"),
                                   AliasInfo("SPECIAL_STAGE", "3"),
                                   AliasInfo("MENU_1", "0"),
                                   AliasInfo("MENU_2", "1"),
                                   AliasInfo("C_TOUCH", "0"),
                                   AliasInfo("C_BOX", "1"),
                                   AliasInfo("C_BOX2", "2"),
                                   AliasInfo("C_PLATFORM", "3"),
                                   AliasInfo("MAT_WORLD", "0"),
                                   AliasInfo("MAT_VIEW", "1"),
                                   AliasInfo("MAT_TEMP", "2"),
                                   AliasInfo("FX_FLIP", "5"),
                                   AliasInfo("FACING_LEFT", "1"),
                                   AliasInfo("FACING_RIGHT", "0"),
                                   AliasInfo("STAGE_PAUSED", "2"),
                                   AliasInfo("STAGE_RUNNING", "1"),
                                   AliasInfo("RESET_GAME", "2"),
                                   AliasInfo("RETRO_WIN", "0"),
                                   AliasInfo("RETRO_OSX", "1"),
                                   AliasInfo("RETRO_XBOX_360", "2"),
                                   AliasInfo("RETRO_PS3", "3"),
                                   AliasInfo("RETRO_iOS", "4"),
                                   AliasInfo("RETRO_ANDROID", "5"),
                                   AliasInfo("RETRO_WP7", "6") };

const char scriptEvaluationTokens[][0x4] = { "=",  "+=", "-=", "++", "--", "*=", "/=", ">>=", "<<=", "&=",
                                             "|=", "^=", "%=", "==", ">",  ">=", "<",  "<=",  "!=" };


enum ScriptReadModes { READMODE_NORMAL = 0, READMODE_STRING = 1, READMODE_COMMENTLINE = 2, READMODE_ENDLINE = 3, READMODE_EOF = 4 };
enum ScriptParseModes {
    PARSEMODE_SCOPELESS    = 0,
    PARSEMODE_PLATFORMSKIP = 1,
    PARSEMODE_FUNCTION     = 2,
    PARSEMODE_SWITCHREAD   = 3,
    PARSEMODE_ERROR        = 0xFF
};
#endif

enum ScriptVarTypes { SCRIPTVAR_VAR = 1, SCRIPTVAR_INTCONST = 2, SCRIPTVAR_STRCONST = 3 };
enum ScriptVarArrTypes { VARARR_NONE = 0, VARARR_ARRAY = 1, VARARR_ENTNOPLUS1 = 2, VARARR_ENTNOMINUS1 = 3 };

enum ScrVariable {
    VAR_TEMPVALUE0,
    VAR_TEMPVALUE1,
    VAR_TEMPVALUE2,
    VAR_TEMPVALUE3,
    VAR_TEMPVALUE4,
    VAR_TEMPVALUE5,
    VAR_TEMPVALUE6,
    VAR_TEMPVALUE7,
    VAR_CHECKRESULT,
    VAR_ARRAYPOS0,
    VAR_ARRAYPOS1,
    VAR_GLOBAL,
    VAR_OBJECTENTITYNO,
    VAR_OBJECTTYPE,
    VAR_OBJECTPROPERTYVALUE,
    VAR_OBJECTXPOS,
    VAR_OBJECTYPOS,
    VAR_OBJECTIXPOS,
    VAR_OBJECTIYPOS,
    VAR_OBJECTSTATE,
    VAR_OBJECTROTATION,
    VAR_OBJECTSCALE,
    VAR_OBJECTPRIORITY,
    VAR_OBJECTDRAWORDER,
    VAR_OBJECTDIRECTION,
    VAR_OBJECTINKEFFECT,
    VAR_OBJECTALPHA,
    VAR_OBJECTFRAME,
    VAR_OBJECTANIMATION,
    VAR_OBJECTPREVANIMATION,
    VAR_OBJECTANIMATIONSPEED,
    VAR_OBJECTANIMATIONTIMER,
    VAR_OBJECTVALUE0,
    VAR_OBJECTVALUE1,
    VAR_OBJECTVALUE2,
    VAR_OBJECTVALUE3,
    VAR_OBJECTVALUE4,
    VAR_OBJECTVALUE5,
    VAR_OBJECTVALUE6,
    VAR_OBJECTVALUE7,
    VAR_OBJECTOUTOFBOUNDS,
    VAR_PLAYERSTATE,
    VAR_PLAYERCONTROLMODE,
    VAR_PLAYERCONTROLLOCK,
    VAR_PLAYERCOLLISIONMODE,
    VAR_PLAYERCOLLISIONPLANE,
    VAR_PLAYERXPOS,
    VAR_PLAYERYPOS,
    VAR_PLAYERIXPOS,
    VAR_PLAYERIYPOS,
    VAR_PLAYERSCREENXPOS,
    VAR_PLAYERSCREENYPOS,
    VAR_PLAYERSPEED,
    VAR_PLAYERXVELOCITY,
    VAR_PLAYERYVELOCITY,
    VAR_PLAYERGRAVITY,
    VAR_PLAYERANGLE,
    VAR_PLAYERSKIDDING,
    VAR_PLAYERPUSHING,
    VAR_PLAYERTRACKSCROLL,
    VAR_PLAYERUP,
    VAR_PLAYERDOWN,
    VAR_PLAYERLEFT,
    VAR_PLAYERRIGHT,
    VAR_PLAYERJUMPPRESS,
    VAR_PLAYERJUMPHOLD,
    VAR_PLAYERFOLLOWPLAYER1,
    VAR_PLAYERLOOKPOS,
    VAR_PLAYERWATER,
    VAR_PLAYERTOPSPEED,
    VAR_PLAYERACCELERATION,
    VAR_PLAYERDECELERATION,
    VAR_PLAYERAIRACCELERATION,
    VAR_PLAYERAIRDECELERATION,
    VAR_PLAYERGRAVITYSTRENGTH,
    VAR_PLAYERJUMPSTRENGTH,
    VAR_PLAYERJUMPCAP,
    VAR_PLAYERROLLINGACCELERATION,
    VAR_PLAYERROLLINGDECELERATION,
    VAR_PLAYERENTITYNO,
    VAR_PLAYERCOLLISIONLEFT,
    VAR_PLAYERCOLLISIONTOP,
    VAR_PLAYERCOLLISIONRIGHT,
    VAR_PLAYERCOLLISIONBOTTOM,
    VAR_PLAYERFLAILING,
    VAR_PLAYERTIMER,
    VAR_PLAYERTILECOLLISIONS,
    VAR_PLAYEROBJECTINTERACTION,
    VAR_PLAYERVISIBLE,
    VAR_PLAYERROTATION,
    VAR_PLAYERSCALE,
    VAR_PLAYERPRIORITY,
    VAR_PLAYERDRAWORDER,
    VAR_PLAYERDIRECTION,
    VAR_PLAYERINKEFFECT,
    VAR_PLAYERALPHA,
    VAR_PLAYERFRAME,
    VAR_PLAYERANIMATION,
    VAR_PLAYERPREVANIMATION,
    VAR_PLAYERANIMATIONSPEED,
    VAR_PLAYERANIMATIONTIMER,
    VAR_PLAYERVALUE0,
    VAR_PLAYERVALUE1,
    VAR_PLAYERVALUE2,
    VAR_PLAYERVALUE3,
    VAR_PLAYERVALUE4,
    VAR_PLAYERVALUE5,
    VAR_PLAYERVALUE6,
    VAR_PLAYERVALUE7,
    VAR_PLAYERVALUE8,
    VAR_PLAYERVALUE9,
    VAR_PLAYERVALUE10,
    VAR_PLAYERVALUE11,
    VAR_PLAYERVALUE12,
    VAR_PLAYERVALUE13,
    VAR_PLAYERVALUE14,
    VAR_PLAYERVALUE15,
    VAR_PLAYEROUTOFBOUNDS,
    VAR_STAGESTATE,
    VAR_STAGEACTIVELIST,
    VAR_STAGELISTPOS,
    VAR_STAGETIMEENABLED,
    VAR_STAGEMILLISECONDS,
    VAR_STAGESECONDS,
    VAR_STAGEMINUTES,
    VAR_STAGEACTNO,
    VAR_STAGEPAUSEENABLED,
    VAR_STAGELISTSIZE,
    VAR_STAGENEWXBOUNDARY1,
    VAR_STAGENEWXBOUNDARY2,
    VAR_STAGENEWYBOUNDARY1,
    VAR_STAGENEWYBOUNDARY2,
    VAR_STAGEXBOUNDARY1,
    VAR_STAGEXBOUNDARY2,
    VAR_STAGEYBOUNDARY1,
    VAR_STAGEYBOUNDARY2,
    VAR_STAGEDEFORMATIONDATA0,
    VAR_STAGEDEFORMATIONDATA1,
    VAR_STAGEDEFORMATIONDATA2,
    VAR_STAGEDEFORMATIONDATA3,
    VAR_STAGEWATERLEVEL,
    VAR_STAGEACTIVELAYER,
    VAR_STAGEMIDPOINT,
    VAR_STAGEPLAYERLISTPOS,
    VAR_STAGEACTIVEPLAYER,
    VAR_SCREENCAMERAENABLED,
    VAR_SCREENCAMERATARGET,
    VAR_SCREENCAMERASTYLE,
    VAR_SCREENDRAWLISTSIZE,
    VAR_SCREENCENTERX,
    VAR_SCREENCENTERY,
    VAR_SCREENXSIZE,
    VAR_SCREENYSIZE,
    VAR_SCREENXOFFSET,
    VAR_SCREENYOFFSET,
    VAR_SCREENSHAKEX,
    VAR_SCREENSHAKEY,
    VAR_SCREENADJUSTCAMERAY,
    VAR_TOUCHSCREENDOWN,
    VAR_TOUCHSCREENXPOS,
    VAR_TOUCHSCREENYPOS,
    VAR_MUSICVOLUME,
    VAR_MUSICCURRENTTRACK,
    VAR_KEYDOWNUP,
    VAR_KEYDOWNDOWN,
    VAR_KEYDOWNLEFT,
    VAR_KEYDOWNRIGHT,
    VAR_KEYDOWNBUTTONA,
    VAR_KEYDOWNBUTTONB,
    VAR_KEYDOWNBUTTONC,
    VAR_KEYDOWNSTART,
    VAR_KEYPRESSUP,
    VAR_KEYPRESSDOWN,
    VAR_KEYPRESSLEFT,
    VAR_KEYPRESSRIGHT,
    VAR_KEYPRESSBUTTONA,
    VAR_KEYPRESSBUTTONB,
    VAR_KEYPRESSBUTTONC,
    VAR_KEYPRESSSTART,
    VAR_MENU1SELECTION,
    VAR_MENU2SELECTION,
    VAR_TILELAYERXSIZE,
    VAR_TILELAYERYSIZE,
    VAR_TILELAYERTYPE,
    VAR_TILELAYERANGLE,
    VAR_TILELAYERXPOS,
    VAR_TILELAYERYPOS,
    VAR_TILELAYERZPOS,
    VAR_TILELAYERPARALLAXFACTOR,
    VAR_TILELAYERSCROLLSPEED,
    VAR_TILELAYERSCROLLPOS,
    VAR_TILELAYERDEFORMATIONOFFSET,
    VAR_TILELAYERDEFORMATIONOFFSETW,
    VAR_HPARALLAXPARALLAXFACTOR,
    VAR_HPARALLAXSCROLLSPEED,
    VAR_HPARALLAXSCROLLPOS,
    VAR_VPARALLAXPARALLAXFACTOR,
    VAR_VPARALLAXSCROLLSPEED,
    VAR_VPARALLAXSCROLLPOS,
    VAR_3DSCENENOVERTICES,
    VAR_3DSCENENOFACES,
    VAR_VERTEXBUFFERX,
    VAR_VERTEXBUFFERY,
    VAR_VERTEXBUFFERZ,
    VAR_VERTEXBUFFERU,
    VAR_VERTEXBUFFERV,
    VAR_FACEBUFFERA,
    VAR_FACEBUFFERB,
    VAR_FACEBUFFERC,
    VAR_FACEBUFFERD,
    VAR_FACEBUFFERFLAG,
    VAR_FACEBUFFERCOLOR,
    VAR_3DSCENEPROJECTIONX,
    VAR_3DSCENEPROJECTIONY,
    VAR_ENGINESTATE,
    VAR_STAGEDEBUGMODE,
    VAR_ENGINEMESSAGE,
    VAR_SAVERAM,
    VAR_ENGINELANGUAGE,
    VAR_OBJECTSPRITESHEET,
    VAR_ENGINEONLINEACTIVE,
    VAR_ENGINEFRAMESKIPTIMER,
    VAR_ENGINEFRAMESKIPSETTING,
    VAR_ENGINESFXVOLUME,
    VAR_ENGINEBGMVOLUME,
    VAR_ENGINEPLATFORMID,
    VAR_ENGINETRIALMODE,
    VAR_KEYPRESSANYSTART,
#if RETRO_USE_HAPTICS
    VAR_ENGINEHAPTICSENABLED,
#endif
    VAR_MAX_CNT
};

enum ScrFunction {
    FUNC_END,
    FUNC_EQUAL,
    FUNC_ADD,
    FUNC_SUB,
    FUNC_INC,
    FUNC_DEC,
    FUNC_MUL,
    FUNC_DIV,
    FUNC_SHR,
    FUNC_SHL,
    FUNC_AND,
    FUNC_OR,
    FUNC_XOR,
    FUNC_MOD,
    FUNC_FLIPSIGN,
    FUNC_CHECKEQUAL,
    FUNC_CHECKGREATER,
    FUNC_CHECKLOWER,
    FUNC_CHECKNOTEQUAL,
    FUNC_IFEQUAL,
    FUNC_IFGREATER,
    FUNC_IFGREATEROREQUAL,
    FUNC_IFLOWER,
    FUNC_IFLOWEROREQUAL,
    FUNC_IFNOTEQUAL,
    FUNC_ELSE,
    FUNC_ENDIF,
    FUNC_WEQUAL,
    FUNC_WGREATER,
    FUNC_WGREATEROREQUAL,
    FUNC_WLOWER,
    FUNC_WLOWEROREQUAL,
    FUNC_WNOTEQUAL,
    FUNC_LOOP,
    FUNC_SWITCH,
    FUNC_BREAK,
    FUNC_ENDSWITCH,
    FUNC_RAND,
    FUNC_SIN,
    FUNC_COS,
    FUNC_SIN256,
    FUNC_COS256,
    FUNC_SINCHANGE,
    FUNC_COSCHANGE,
    FUNC_ATAN2,
    FUNC_INTERPOLATE,
    FUNC_INTERPOLATEXY,
    FUNC_LOADSPRITESHEET,
    FUNC_REMOVESPRITESHEET,
    FUNC_DRAWSPRITE,
    FUNC_DRAWSPRITEXY,
    FUNC_DRAWSPRITESCREENXY,
    FUNC_DRAWTINTRECT,
    FUNC_DRAWNUMBERS,
    FUNC_DRAWACTNAME,
    FUNC_DRAWMENU,
    FUNC_SPRITEFRAME,
    FUNC_EDITFRAME,
    FUNC_LOADPALETTE,
    FUNC_ROTATEPALETTE,
    FUNC_SETSCREENFADE,
    FUNC_SETACTIVEPALETTE,
    FUNC_SETPALETTEFADE,
    FUNC_COPYPALETTE,
    FUNC_CLEARSCREEN,
    FUNC_DRAWSPRITEFX,
    FUNC_DRAWSPRITESCREENFX,
    FUNC_LOADANIMATION,
    FUNC_SETUPMENU,
    FUNC_ADDMENUENTRY,
    FUNC_EDITMENUENTRY,
    FUNC_LOADSTAGE,
    FUNC_DRAWRECT,
    FUNC_RESETOBJECTENTITY,
    FUNC_PLAYEROBJECTCOLLISION,
    FUNC_CREATETEMPOBJECT,
    FUNC_BINDPLAYERTOOBJECT,
    FUNC_PLAYERTILECOLLISION,
    FUNC_PROCESSPLAYERCONTROL,
    FUNC_PROCESSANIMATION,
    FUNC_DRAWOBJECTANIMATION,
    FUNC_DRAWPLAYERANIMATION,
    FUNC_SETMUSICTRACK,
    FUNC_PLAYMUSIC,
    FUNC_STOPMUSIC,
    FUNC_PLAYSFX,
    FUNC_STOPSFX,
    FUNC_SETSFXATTRIBUTES,
    FUNC_OBJECTTILECOLLISION,
    FUNC_OBJECTTILEGRIP,
    FUNC_LOADVIDEO,
    FUNC_NEXTVIDEOFRAME,
    FUNC_PLAYSTAGESFX,
    FUNC_STOPSTAGESFX,
    FUNC_NOT,
    FUNC_DRAW3DSCENE,
    FUNC_SETIDENTITYMATRIX,
    FUNC_MATRIXMULTIPLY,
    FUNC_MATRIXTRANSLATEXYZ,
    FUNC_MATRIXSCALEXYZ,
    FUNC_MATRIXROTATEX,
    FUNC_MATRIXROTATEY,
    FUNC_MATRIXROTATEZ,
    FUNC_MATRIXROTATEXYZ,
    FUNC_TRANSFORMVERTICES,
    FUNC_CALLFUNCTION,
    FUNC_ENDFUNCTION,
    FUNC_SETLAYERDEFORMATION,
    FUNC_CHECKTOUCHRECT,
    FUNC_GETTILELAYERENTRY,
    FUNC_SETTILELAYERENTRY,
    FUNC_GETBIT,
    FUNC_SETBIT,
    FUNC_PAUSEMUSIC,
    FUNC_RESUMEMUSIC,
    FUNC_CLEARDRAWLIST,
    FUNC_ADDDRAWLISTENTITYREF,
    FUNC_GETDRAWLISTENTITYREF,
    FUNC_SETDRAWLISTENTITYREF,
    FUNC_GET16X16TILEINFO,
    FUNC_COPY16X16TILE,
    FUNC_SET16X16TILEINFO,
    FUNC_GETANIMATIONBYNAME,
    FUNC_READSAVERAM,
    FUNC_WRITESAVERAM,
    FUNC_LOADTEXTFONT,
    FUNC_LOADTEXTFILE,
    FUNC_DRAWTEXT,
    FUNC_GETTEXTINFO,
    FUNC_GETVERSIONNUMBER,
    FUNC_SETACHIEVEMENT,
    FUNC_SETLEADERBOARD,
    FUNC_LOADONLINEMENU,
    FUNC_ENGINECALLBACK,
#if RETRO_USE_HAPTICS
    FUNC_HAPTICEFFECT,
#endif
    FUNC_MAX_CNT
};

#if RETRO_USE_COMPILER
void CheckAliasText(char *text)
{
    if (FindStringToken(text, "#alias", 1) != 0)
        return;

#if !RETRO_USE_ORIGINAL_CODE
    if (aliasCount >= ALIAS_COUNT) {
        SetupTextMenu(&gameMenu[0], 0);
        AddTextMenuEntry(&gameMenu[0], "SCRIPT PARSING FAILED");
        AddTextMenuEntry(&gameMenu[0], " ");
        AddTextMenuEntry(&gameMenu[0], "TOO MANY ALIASES");
        Engine.gameMode = ENGINE_SCRIPTERROR;
        return;
    }
#endif

    int textPos     = 6;
    int aliasStrPos = 0;
    int parseMode  = 0;

    while (parseMode < 2) {
        if (parseMode) {
            if (parseMode == 1) {
                aliases[aliasCount].name[aliasStrPos] = text[textPos];
                if (text[textPos]) {
                    aliasStrPos++;
                }
                else {
                    aliasStrPos = 0;
                    ++parseMode;
                }
            }
        }
        else if (text[textPos] == ':') {
            aliases[aliasCount].value[aliasStrPos] = 0;
            aliasStrPos                            = 0;
            parseMode                             = 1;
        }
        else {
            aliases[aliasCount].value[aliasStrPos++] = text[textPos];
        }
        ++textPos;
    }

    ++aliasCount;
}
void ConvertArithmaticSyntax(char *text)
{
    int token  = 0;
    int offset = 0;
    int findID = 0;
    char dest[260];

    for (int i = FUNC_EQUAL; i <= FUNC_MOD; ++i) {
        findID = FindStringToken(text, scriptEvaluationTokens[i - 1], 1);
        if (findID > -1) {
            offset = findID;
            token  = i;
        }
    }

    if (token > 0) {
        StrCopy(dest, functions[token].name);
        StrAdd(dest, "(");
        findID = StrLength(dest);
        for (int i = 0; i < offset; ++i) dest[findID++] = text[i];
        if (functions[token].opcodeSize > 1) {
            dest[findID] = ',';
            int len      = StrLength(scriptEvaluationTokens[token - 1]);
            offset += len;
            ++findID;
            while (text[offset]) dest[findID++] = text[offset++];
        }
        dest[findID] = 0;
        StrAdd(dest, ")");
        StrCopy(text, dest);
    }
}
void ConvertIfWhileStatement(char *text)
{
    char dest[260];
    int compareOp  = -1;
    int strPos     = 0;
    int destStrPos = 0;

    if (FindStringToken(text, "if", 1) == 0) {
        for (int i = 0; i < 6; ++i) {
            destStrPos = FindStringToken(text, scriptEvaluationTokens[i + FUNC_MOD], 1);
            if (destStrPos > -1) {
                strPos    = destStrPos;
                compareOp = i;
            }
        }

        if (compareOp > -1) {
            text[strPos] = ',';

            StrCopy(dest, functions[compareOp + FUNC_IFEQUAL].name);
            StrAdd(dest, "(");
            AppendIntegerToString(dest, jumpTablePos - jumpTableOffset);
            StrAdd(dest, ",");

            destStrPos = StrLength(dest);
            for (int i = 2; text[i]; ++i) {
                if (text[i] != '=' && text[i] != '(' && text[i] != ')')
                    dest[destStrPos++] = text[i];
            }
            dest[destStrPos] = 0;

            StrAdd(dest, ")");
            StrCopy(text, dest);

            jumpTableStack[++jumpTableStackPos] = jumpTablePos;
            jumpTable[jumpTablePos++]       = -1;
            jumpTable[jumpTablePos++]       = 0;
        }
    }
    else if (FindStringToken(text, "while", 1) == 0) {
        for (int i = 0; i < 6; ++i) {
            destStrPos = FindStringToken(text, scriptEvaluationTokens[i + FUNC_MOD], 1);
            if (destStrPos > -1) {
                strPos    = destStrPos;
                compareOp = i;
            }
        }

        if (compareOp > -1) {
            text[strPos] = ',';

            StrCopy(dest, functions[compareOp + FUNC_WEQUAL].name);
            StrAdd(dest, "(");
            AppendIntegerToString(dest, jumpTablePos - jumpTableOffset);
            StrAdd(dest, ",");

            destStrPos = StrLength(dest);
            for (int i = 5; text[i]; ++i) {
                if (text[i] != '=' && text[i] != '(' && text[i] != ')')
                    dest[destStrPos++] = text[i];
            }
            dest[destStrPos] = 0;

            StrAdd(dest, ")");
            StrCopy(text, dest);

            jumpTableStack[++jumpTableStackPos] = jumpTablePos;
            jumpTable[jumpTablePos++]       = scriptCodePos - scriptCodeOffset;
            jumpTable[jumpTablePos++]       = 0;
        }
    }
}
bool ConvertSwitchStatement(char *text)
{
    if (FindStringToken(text, "switch", 1))
        return false;

    char switchText[260];
    StrCopy(switchText, "switch");
    StrAdd(switchText, "(");
    AppendIntegerToString(switchText, jumpTablePos - jumpTableOffset);
    StrAdd(switchText, ",");
    int pos = StrLength(switchText);
    for (int i = 6; text[i]; ++i) {
        if (text[i] != '=' && text[i] != '(' && text[i] != ')')
            switchText[pos++] = text[i];
    }
    switchText[pos] = 0;
    StrAdd(switchText, ")");
    StrCopy(text, switchText);

    jumpTableStack[++jumpTableStackPos] = jumpTablePos;
    jumpTable[jumpTablePos++]   = 0x10000;
    jumpTable[jumpTablePos++]   = -0x10000;
    jumpTable[jumpTablePos++]   = -1;
    jumpTable[jumpTablePos++]   = 0;

    return true;
}
void ConvertFunctionText(char *text)
{
    char arrayStr[128];
    char funcName[132];
    int opcode     = 0;
    int opcodeSize = 0;
    int textPos    = 0;
    int namePos    = 0;
    for (namePos = 0; text[namePos] != '(' && text[namePos]; ++namePos) funcName[namePos] = text[namePos];
    funcName[namePos] = 0;
    for (int i = 0; i < FUNC_MAX_CNT; ++i) {
        if (StrComp(funcName, functions[i].name)) {
            opcode     = i;
            opcodeSize = functions[i].opcodeSize;
            textPos    = StrLength(functions[i].name);
            i          = FUNC_MAX_CNT;
        }
    }

    if (opcode <= 0) {
        SetupTextMenu(&gameMenu[0], 0);
        AddTextMenuEntry(&gameMenu[0], "SCRIPT PARSING FAILED");
        AddTextMenuEntry(&gameMenu[0], " ");
        AddTextMenuEntry(&gameMenu[0], "OPCODE NOT FOUND");
        AddTextMenuEntry(&gameMenu[0], funcName);
#if !RETRO_USE_ORIGINAL_CODE
        AddTextMenuEntry(&gameMenu[0], " ");
        AddTextMenuEntry(&gameMenu[0], "LINE NUMBER");
        char buffer[0x10];
        buffer[0] = 0;
        AppendIntegerToString(buffer, lineID);
        AddTextMenuEntry(&gameMenu[0], buffer);
#endif
        Engine.gameMode = ENGINE_SCRIPTERROR;
    }
    else {
        scriptCode[scriptCodePos++] = opcode;
        if (StrComp("else", functions[opcode].name))
            jumpTable[jumpTableStack[jumpTableStackPos]] = scriptCodePos - scriptCodeOffset;

        if (StrComp("endif", functions[opcode].name) == 1) {
            int jPos                = jumpTableStack[jumpTableStackPos];
            jumpTable[jPos + 1] = scriptCodePos - scriptCodeOffset;
            if (jumpTable[jPos] == -1)
                jumpTable[jPos] = (scriptCodePos - scriptCodeOffset) - 1;
            --jumpTableStackPos;
        }

        if (StrComp("endswitch", functions[opcode].name)) {
            int jPos                = jumpTableStack[jumpTableStackPos];
            jumpTable[jPos + 3] = scriptCodePos - scriptCodeOffset;
            if (jumpTable[jPos + 2] == -1) {
                jumpTable[jPos + 2] = (scriptCodePos - scriptCodeOffset) - 1;
                int caseCnt             = abs(jumpTable[jPos + 1] - jumpTable[jPos]) + 1;

                int jOffset = jPos + 4;
                for (int c = 0; c < caseCnt; ++c) {
                    if (jumpTable[jOffset + c] < 0)
                        jumpTable[jOffset + c] = jumpTable[jPos + 2];
                }
            }
            --jumpTableStackPos;
        }

        if (StrComp("loop", functions[opcode].name)) {
            jumpTable[jumpTableStack[jumpTableStackPos--] + 1] = scriptCodePos - scriptCodeOffset;
        }

        for (int i = 0; i < opcodeSize; ++i) {
            ++textPos;
            int varNamePos  = 0;
            int parseMode   = 0;
            int arrayStrPos = 0;

            while (text[textPos] != ',' && text[textPos] != ')' && text[textPos]) {
                if (parseMode) {
                    if (text[textPos] == ']')
                        parseMode = 0;
                    else
                        arrayStr[arrayStrPos++] = text[textPos];
                    ++textPos;
                }
                else {
                    if (text[textPos] == '[')
                        parseMode = 1;
                    else
                        funcName[varNamePos++] = text[textPos];
                    ++textPos;
                }
            }
            funcName[varNamePos]   = 0;
            arrayStr[arrayStrPos] = 0;

            // Eg: TempValue0 = FX_SCALE
            for (int a = 0; a < aliasCount; ++a) {
                if (StrComp(funcName, aliases[a].name)) {
                    CopyAliasStr(funcName, aliases[a].value, 0);
                    if (FindStringToken(aliases[a].value, "[", 1) > -1)
                        CopyAliasStr(arrayStr, aliases[a].value, 1);
                }
            }

            // Eg: TempValue0 = Game.Variable
            for (int v = 0; v < globalVariablesCount; ++v) {
                if (StrComp(funcName, globalVariableNames[v])) {
                    StrCopy(funcName, "Global");
                    arrayStr[0] = 0;
                    AppendIntegerToString(arrayStr, v);
                }
            }

            // Eg: TempValue0 = Function1
            for (int f = 0; f < scriptFunctionCount; ++f) {
                if (StrComp(funcName, scriptFunctionList[f].name)) {
                    funcName[0] = 0;
                    AppendIntegerToString(funcName, f);
                }
            }

            // Eg: TempValue0 = TypeName[PlayerObject]
            if (StrComp(funcName, "TypeName")) {
                funcName[0]  = '0';
                funcName[1] = 0;

                for (int o = 0; o < OBJECT_COUNT; ++o) {
                    if (StrComp(arrayStr, typeNames[o])) {
                        funcName[0] = 0;
                        AppendIntegerToString(funcName, o);
                    }
                }
            }

#if RETRO_USE_MOD_LOADER
            // Eg: TempValue0 = SfxName[Jump]
            if (StrComp(funcName, "SfxName")) {
                funcName[0] = '0';
                funcName[1] = 0;

                int s = 0;
                for (; s < globalSFXCount; ++s) {
                    if (StrComp(arrayStr, globalSfxNames[s])) {
                        funcName[0] = 0;
                        AppendIntegerToString(funcName, s);
                        break;
                    }
                }

                if (s == globalSFXCount) {
                    s = 0;
                    for (; s < stageSFXCount; ++s) {
                        if (StrComp(arrayStr, stageSfxNames[s])) {
                            funcName[0] = 0;
                            AppendIntegerToString(funcName, s);
                            break;
                        }
                    }

                    if (s == stageSFXCount) {
                        char buf[0x40];
                        sprintf(buf, "WARNING: Unknown SfxName \"%s\"", arrayStr);
                        PrintLog(buf);
                    }
                }
            }

            // Eg: TempValue0 = AchievementName[88 Miles Per Hour]
            if (StrComp(funcName, "AchievementName")) {
                funcName[0] = '0';
                funcName[1] = 0;

                int a = 0;
                for (; a < ACHIEVEMENT_COUNT; ++a) {
                    if (StrComp(arrayStr, achievements[a].name)) {
                        funcName[0] = 0;
                        AppendIntegerToString(funcName, a);
                        break;
                    }
                }

                if (a == ACHIEVEMENT_COUNT) {
                    char buf[0x40];
                    sprintf(buf, "WARNING: Unknown AchievementName \"%s\"", arrayStr);
                    PrintLog(buf);
                }
            }

            // Eg: TempValue0 = PlayerName[SONIC]
            if (StrComp(funcName, "PlayerName")) {
                funcName[0] = '0';
                funcName[1] = 0;

                int p = 0;
                for (; p < PLAYERNAME_COUNT; ++p) {
                    if (StrComp(arrayStr, playerNames[p])) {
                        funcName[0] = 0;
                        AppendIntegerToString(funcName, p);
                        break;
                    }
                }

                if (p == PLAYERNAME_COUNT) {
                    char buf[0x40];
                    sprintf(buf, "WARNING: Unknown PlayerName \"%s\"", arrayStr);
                    PrintLog(buf);
                }
            }

            // Eg: TempValue0 = StageName[R - PALMTREE PANIC ZONE 1 A]
            if (StrComp(funcName, "StageName")) {
                funcName[0] = '0';
                funcName[1] = 0;

                int s       = -1;
                if (StrLength(arrayStr) >= 2) {
                    char list = arrayStr[0];
                    switch (list) {
                        case 'P': list = STAGELIST_PRESENTATION; break;
                        case 'R': list = STAGELIST_REGULAR; break;
                        case 'S': list = STAGELIST_SPECIAL; break;
                        case 'B': list = STAGELIST_BONUS; break;
                    }
                    s = GetSceneID(list, &arrayStr[2]);
                }

                if (s == -1) {
                    char buf[0x40];
                    sprintf(buf, "WARNING: Unknown StageName \"%s\", on line %d", arrayStr, lineID);
                    PrintLog(buf);
                    s = 0;
                }
                funcName[0] = 0;
                AppendIntegerToString(funcName, s);
            }
#endif

            int constant = 0;
            if (ConvertStringToInteger(funcName, &constant)) {
                scriptCode[scriptCodePos++] = SCRIPTVAR_INTCONST;
                scriptCode[scriptCodePos++] = constant;
            }
            else if (funcName[0] == '"') {
                scriptCode[scriptCodePos++] = SCRIPTVAR_STRCONST;
                scriptCode[scriptCodePos++] = StrLength(funcName) - 2;

                int scriptTextPos = 1;
                arrayStrPos       = 0;
                while (scriptTextPos > -1) {
                    switch (arrayStrPos) {
                        case 0:
                            scriptCode[scriptCodePos] = funcName[scriptTextPos] << 24;
                            ++arrayStrPos;
                            break;

                        case 1:
                            scriptCode[scriptCodePos] += funcName[scriptTextPos] << 16;
                            ++arrayStrPos;
                            break;

                        case 2:
                            scriptCode[scriptCodePos] += funcName[scriptTextPos] << 8;
                            ++arrayStrPos;
                            break;

                        case 3:
                            scriptCode[scriptCodePos++] += funcName[scriptTextPos];
                            arrayStrPos = 0;
                            break;

                        default: break;
                    }

                    if (funcName[scriptTextPos] == '"') {
                        if (arrayStrPos > 0)
                            ++scriptCodePos;
                        scriptTextPos = -1;
                    }
                    else {
                        scriptTextPos++;
                    }
                }
            }
            else {
                scriptCode[scriptCodePos++] = SCRIPTVAR_VAR;
                if (arrayStr[0]) {
                    scriptCode[scriptCodePos] = VARARR_ARRAY;

                    if (arrayStr[0] == '+')
                        scriptCode[scriptCodePos] = VARARR_ENTNOPLUS1;

                    if (arrayStr[0] == '-')
                        scriptCode[scriptCodePos] = VARARR_ENTNOMINUS1;

                    ++scriptCodePos;

                    if (arrayStr[0] == '-' || arrayStr[0] == '+') {
                        for (int i = 0; i < StrLength(arrayStr); ++i) arrayStr[i] = arrayStr[i + 1];
                    }

                    if (ConvertStringToInteger(arrayStr, &constant)) {
                        scriptCode[scriptCodePos++] = 0;
                        scriptCode[scriptCodePos++] = constant;
                    }
                    else {
                        if (StrComp(arrayStr, "ArrayPos0"))
                            constant = 0;
                        if (StrComp(arrayStr, "ArrayPos1"))
                            constant = 1;
                        if (StrComp(arrayStr, "TempObjectPos"))
                            constant = 2;

                        scriptCode[scriptCodePos++] = 1;
                        scriptCode[scriptCodePos++] = constant;
                    }
                }
                else {
                    scriptCode[scriptCodePos++] = VARARR_NONE;
                }

                constant = -1;
                for (int i = 0; i < VAR_MAX_CNT; ++i) {
                    if (StrComp(funcName, variableNames[i]))
                        constant = i;
                }

                if (constant == -1 && Engine.gameMode != ENGINE_SCRIPTERROR) {
                    SetupTextMenu(&gameMenu[0], 0);
                    AddTextMenuEntry(&gameMenu[0], "SCRIPT PARSING FAILED");
                    AddTextMenuEntry(&gameMenu[0], " ");
                    AddTextMenuEntry(&gameMenu[0], "OPERAND NOT FOUND");
                    AddTextMenuEntry(&gameMenu[0], funcName);
                    AddTextMenuEntry(&gameMenu[0], " ");
                    AddTextMenuEntry(&gameMenu[0], "LINE NUMBER");
                    funcName[0] = 0;
                    AppendIntegerToString(funcName, lineID);
                    AddTextMenuEntry(&gameMenu[0], funcName);
                    Engine.gameMode = ENGINE_SCRIPTERROR;
                    constant        = 0;
                }

                scriptCode[scriptCodePos++] = constant;
            }
        }
    }
}
void CheckCaseNumber(char *text)
{
    if (FindStringToken(text, "case", 1) != 0)
        return;

    char caseString[128];
    char caseChar  = text[4];

    int textPos    = 5;
    int caseStrPos = 0;
    while (caseChar) {
        if (caseChar != ':')
            caseString[caseStrPos++] = caseChar;
        caseChar = text[textPos++];
    }
    caseString[caseStrPos] = 0;

    for (int a = 0; a < aliasCount; ++a) {
        if (StrComp(aliases[a].name, caseString)) {
            StrCopy(caseString, aliases[a].value);
            break;
        }
    }

    int caseID = 0;
    if (ConvertStringToInteger(caseString, &caseID)) {
        int stackValue = jumpTableStack[jumpTableStackPos];
        if (caseID < jumpTable[stackValue])
            jumpTable[stackValue] = caseID;
        stackValue++;
        if (caseID > jumpTable[stackValue])
            jumpTable[stackValue] = caseID;
    }
}
bool ReadSwitchCase(char *text)
{
    char caseText[0x80];
    if (FindStringToken(text, "case", 1) == 0) {
        int textPos       = 4;
        int caseStringPos = 0;
        while (text[textPos]) {
            if (text[textPos] != ':')
                caseText[caseStringPos++] = text[textPos];
            ++textPos;
        }
        caseText[caseStringPos] = 0;
        for (int a = 0; a < aliasCount; ++a) {
            if (StrComp(caseText, aliases[a].name))
                StrCopy(caseText, aliases[a].value);
        }

        int val = 0;

        int jPos    = jumpTableStack[jumpTableStackPos];
        int jOffset = jPos + 4;
        if (ConvertStringToInteger(caseText, &val))
            jumpTable[val - jumpTable[jPos] + jOffset] = scriptCodePos - scriptCodeOffset;
        return true;
    }
    else if (FindStringToken(text, "default", 1) == 0) {
        int jumpTablepos                = jumpTableStack[jumpTableStackPos];
        jumpTable[jumpTablepos + 2] = scriptCodePos - scriptCodeOffset;
        int cnt                         = abs(jumpTable[jumpTablepos + 1] - jumpTable[jumpTablepos]) + 1;

        int jOffset = jumpTablepos + 4;
        for (int i = 0; i < cnt; ++i) {
            if (jumpTable[jOffset + i] < 0)
                jumpTable[jOffset + i] = scriptCodePos - scriptCodeOffset;
        }
        return true;
    }

    return false;
}
void AppendIntegerToString(char *text, int value)
{
    int textPos = 0;
    while (true) {
        if (!text[textPos])
            break;
        ++textPos;
    }

    int cnt = 0;
    int v   = value;
    while (v != 0) {
        v /= 10;
        cnt++;
    }

    v = 0;
    for (int i = cnt - 1; i >= 0; --i) {
        v = value / pow(10, i);
        v %= 10;

        int strValue = v + '0';
        if (strValue < '0' || strValue > '9') {
            // what
        }
        text[textPos++] = strValue;
    }
    if (value == 0)
        text[textPos++] = '0';
    text[textPos] = 0;
}
#endif

bool ConvertStringToInteger(char *text, int *value)
{
    int charID    = 0;
    bool negative = false;
    int base      = 10;
    *value        = 0;
    if (*text != '+' && !(*text >= '0' && *text <= '9') && *text != '-')
        return false;
    int strLength = StrLength(text) - 1;
    int charVal   = 0;
    if (*text == '-') {
        negative = true;
        charID   = 1;
        --strLength;
    }
    else if (*text == '+') {
        charID = 1;
        --strLength;
    }

    if (text[charID] == '0') {
        if (text[charID + 1] == 'x' || text[charID + 1] == 'X') {
            charID += 2;
            strLength -= 2;
            base = 0x10;
        }
    }

    while (strLength > -1) {
        if (text[charID] < '0' || text[charID] > (base == 10 ? '9' : (base == 0x10 ? 'F' : '1'))) {
            return 0;
        }
        if (strLength <= 0) {
            if (text[charID] >= '0' && text[charID] <= '9') {
                *value = text[charID] + *value - '0';
            }
            else if (text[charID] >= 'a' && text[charID] <= 'f') {
                charVal = text[charID] - 'a';
                charVal += 10;
                *value += charVal;
            }
            else if (text[charID] >= 'A' && text[charID] <= 'F') {
                charVal = text[charID] - 'A';
                charVal += 10;
                *value += charVal;
            }
        }
        else {
            int strlen = strLength + 1;
            charVal    = 0;
            if (text[charID] >= '0' && text[charID] <= '9') {
                charVal = text[charID] - '0';
            }
            else if (text[charID] >= 'a' && text[charID] <= 'f') {
                charVal = text[charID] - 'a';
                charVal += 10;
            }
            else if (text[charID] >= 'A' && text[charID] <= 'F') {
                charVal = text[charID] - 'A';
                charVal += 10;
            }
            for (; --strlen; charVal *= base)
                ;
            *value += charVal;
        }
        --strLength;
        ++charID;
    }
    if (negative)
        *value = -*value;
    return true;
}

#if RETRO_USE_COMPILER
void CopyAliasStr(char *dest, char *text, bool arrayIndex)
{
    int textPos     = 0;
    int destPos     = 0;
    bool arrayValue = false;
    if (arrayIndex) {
        while (text[textPos]) {
            if (arrayValue) {
                if (text[textPos] == ']')
                    arrayValue = false;
                else
                    dest[destPos++] = text[textPos];
                ++textPos;
            }
            else {
                if (text[textPos] == '[')
                    arrayValue = true;
                ++textPos;
            }
        }
    }
    else {
        while (text[textPos]) {
            if (arrayValue) {
                if (text[textPos] == ']')
                    arrayValue = false;
                ++textPos;
            }
            else {
                if (text[textPos] == '[')
                    arrayValue = true;
                else
                    dest[destPos++] = text[textPos];
                ++textPos;
            }
        }
    }
    dest[destPos] = 0;
}
bool CheckOpcodeType(char *text)
{
    while (true) {
        int c = *text;
        if (!*text)
            break;
        ++text;
        if (c == '(')
            return false;
    }
    return true;
}

void ParseScriptFile(char *scriptName, int scriptID)
{

    jumpTableStackPos = 0;
    lineID            = 0;
    aliasCount        = COMMONALIAS_COUNT;
    for (int i = COMMONALIAS_COUNT; i < ALIAS_COUNT; ++i) {
        StrCopy(aliases[i].name, "");
        StrCopy(aliases[i].value, "");
    }

    char scriptPath[0x40];
    StrCopy(scriptPath, "Data/Scripts/");
    StrAdd(scriptPath, scriptName);
    FileInfo info;
    if (LoadFile(scriptPath, &info)) {
        objectScriptList[scriptID].mobile = true; // all parsed scripts will use the updated format, old format support is purely for pc bytecode
        int readMode                      = READMODE_NORMAL;
        int parseMode                     = PARSEMODE_SCOPELESS;
        char prevChar                     = 0;
        char curChar                      = 0;
        int switchDeep                    = 0;
        while (readMode < READMODE_EOF) {
            int textPos = 0;
            readMode    = READMODE_NORMAL;
            while (readMode < READMODE_ENDLINE) {
                prevChar = curChar;
                FileRead(&curChar, 1);
                if (readMode == READMODE_STRING) {
                    if (curChar == '\t' || curChar == '\r' || curChar == '\n' || curChar == ';' || readMode >= READMODE_COMMENTLINE) {
                        if ((curChar == '\n' && prevChar != '\r') || (curChar == '\n' && prevChar == '\r')) {
                            readMode            = READMODE_ENDLINE;
                            scriptText[textPos] = 0;
                        }
                    }
                    else if (curChar != '/' || textPos <= 0) {
                        scriptText[textPos++] = curChar;
                        if (curChar == '"')
                            readMode = READMODE_NORMAL;
                    }
                    else if (curChar == '/' && prevChar == '/') {
                        readMode              = READMODE_COMMENTLINE;
                        scriptText[--textPos] = 0;
                    }
                    else {
                        scriptText[textPos++] = curChar;
                    }
                }
                else if (curChar == ' ' || curChar == '\t' || curChar == '\r' || curChar == '\n' || curChar == ';'
                         || readMode >= READMODE_COMMENTLINE) {
                    if ((curChar == '\n' && prevChar != '\r') || (curChar == '\n' && prevChar == '\r')) {
                        readMode            = READMODE_ENDLINE;
                        scriptText[textPos] = 0;
                    }
                }
                else if (curChar != '/' || textPos <= 0) {
                    scriptText[textPos++] = curChar;
                    if (curChar == '"' && !readMode)
                        readMode = READMODE_STRING;
                }
                else if (curChar == '/' && prevChar == '/') {
                    readMode              = READMODE_COMMENTLINE;
                    scriptText[--textPos] = 0;
                }
                else {
                    scriptText[textPos++] = curChar;
                }
                if (ReachedEndOfFile()) {
                    scriptText[textPos] = 0;
                    readMode            = READMODE_EOF;
                }
            }

            switch (parseMode) {
                case PARSEMODE_SCOPELESS:
                    ++lineID;
                    CheckAliasText(scriptText);
                    if (StrComp(scriptText, "subObjectMain")) {
                        parseMode                                        = PARSEMODE_FUNCTION;
                        objectScriptList[scriptID].subMain.scriptCodePtr = scriptCodePos;
                        objectScriptList[scriptID].subMain.jumpTablePtr  = jumpTablePos;
                        scriptCodeOffset                                 = scriptCodePos;
                        jumpTableOffset                              = jumpTablePos;
                    }
                    if (StrComp(scriptText, "subObjectPlayerInteraction")) {
                        parseMode                                                     = PARSEMODE_FUNCTION;
                        objectScriptList[scriptID].subPlayerInteraction.scriptCodePtr = scriptCodePos;
                        objectScriptList[scriptID].subPlayerInteraction.jumpTablePtr  = jumpTablePos;
                        scriptCodeOffset                                              = scriptCodePos;
                        jumpTableOffset                                           = jumpTablePos;
                    }
                    if (StrComp(scriptText, "subObjectDraw")) {
                        parseMode                                        = PARSEMODE_FUNCTION;
                        objectScriptList[scriptID].subDraw.scriptCodePtr = scriptCodePos;
                        objectScriptList[scriptID].subDraw.jumpTablePtr  = jumpTablePos;
                        scriptCodeOffset                                 = scriptCodePos;
                        jumpTableOffset                              = jumpTablePos;
                    }
                    if (StrComp(scriptText, "subObjectStartup")) {
                        parseMode                                           = PARSEMODE_FUNCTION;
                        objectScriptList[scriptID].subStartup.scriptCodePtr = scriptCodePos;
                        objectScriptList[scriptID].subStartup.jumpTablePtr  = jumpTablePos;
                        scriptCodeOffset                                    = scriptCodePos;
                        jumpTableOffset                                 = jumpTablePos;
                    }

                    if (FindStringToken(scriptText, "function", 1) == 0) { // regular decl.
                        char funcName[0x20];
                        for (textPos = 8; scriptText[textPos]; ++textPos) funcName[textPos - 8] = scriptText[textPos];
                        funcName[textPos - 8] = 0;

                        int funcID = -1;
                        for (int f = 0; f < scriptFunctionCount; ++f) {
                            if (StrComp(funcName, scriptFunctionList[f].name))
                                funcID = f;
                        }

                        if (funcID <= -1) {
                            if (scriptFunctionCount >= FUNCTION_COUNT) {
                                parseMode = PARSEMODE_SCOPELESS;
                            }
                            else {
                                StrCopy(scriptFunctionList[scriptFunctionCount].name, funcName);
                                scriptFunctionList[scriptFunctionCount].ptr.scriptCodePtr = scriptCodePos;
                                scriptFunctionList[scriptFunctionCount].ptr.jumpTablePtr  = jumpTablePos;
                                scriptCodeOffset                                          = scriptCodePos;
                                jumpTableOffset                                           = jumpTablePos;
                                parseMode                                                 = PARSEMODE_FUNCTION;
                                ++scriptFunctionCount;
                            }
                        }
                        else {
                            StrCopy(scriptFunctionList[funcID].name, funcName);
                            scriptFunctionList[funcID].ptr.scriptCodePtr = scriptCodePos;
                            scriptFunctionList[funcID].ptr.jumpTablePtr  = jumpTablePos;
                            scriptCodeOffset                             = scriptCodePos;
                            jumpTableOffset                              = jumpTablePos;
                            parseMode                                    = PARSEMODE_FUNCTION;
                        }
                    }
                    else if (FindStringToken(scriptText, "function", 1) == 1) { // forward decl.
                        char funcName[0x20];
                        for (textPos = 9; scriptText[textPos]; ++textPos) funcName[textPos - 9] = scriptText[textPos];
                        funcName[textPos - 9] = 0;

                        int funcID = -1;
                        for (int f = 0; f < scriptFunctionCount; ++f) {
                            if (StrComp(funcName, scriptFunctionList[f].name))
                                funcID = f;
                        }

                        if (scriptFunctionCount < FUNCTION_COUNT && funcID == -1) {
                            StrCopy(scriptFunctionList[scriptFunctionCount++].name, funcName);
                        }

                        parseMode = PARSEMODE_SCOPELESS;
                    }
                    break;

                case PARSEMODE_PLATFORMSKIP:
                    ++lineID;

                    if (FindStringToken(scriptText, "#endplatform", 1) == 0)
                        parseMode = PARSEMODE_FUNCTION;
                    break;

                case PARSEMODE_FUNCTION:
                    ++lineID;

                    if (scriptText[0]) {
                        if (StrComp(scriptText, "endsub")) {
                            scriptCode[scriptCodePos++] = FUNC_END;
                            parseMode                   = PARSEMODE_SCOPELESS;
                        }
                        else if (StrComp(scriptText, "endfunction")) {
                            scriptCode[scriptCodePos++] = FUNC_ENDFUNCTION;
                            parseMode                   = PARSEMODE_SCOPELESS;
                        }
                        else if (FindStringToken(scriptText, "#platform:", 1) == 0) {
                            if (FindStringToken(scriptText, Engine.gamePlatform, 1) == -1
                                && FindStringToken(scriptText, Engine.gameRenderType, 1) == -1
#if RETRO_USE_HAPTICS
                                && FindStringToken(scriptText, Engine.gameHapticSetting, 1) == -1
#endif
#if !RETRO_USE_ORIGINAL_CODE
                                && FindStringToken(scriptText, Engine.releaseType, 1) == -1 && FindStringToken(scriptText, "Use_Decomp", 1) == -1
#endif
#if RETRO_USE_MOD_LOADER
                                && FindStringToken(scriptText, "Use_Mod_Loader", 1) == -1
#endif
                            ) { // if NONE of these checks succeeded, then we skip everything until "end platform"
                                parseMode = PARSEMODE_PLATFORMSKIP;
                            }
                        }
                        else if (FindStringToken(scriptText, "#endplatform", 1) == -1) {
                            ConvertIfWhileStatement(scriptText);

                            if (ConvertSwitchStatement(scriptText)) {
                                parseMode    = PARSEMODE_SWITCHREAD;
                                info.readPos = (int)GetFilePosition();
                                switchDeep   = 0;
                            }

                            ConvertArithmaticSyntax(scriptText);

                            if (!ReadSwitchCase(scriptText)) {
                                ConvertFunctionText(scriptText);

                                if (Engine.gameMode == ENGINE_SCRIPTERROR) {
                                    AddTextMenuEntry(&gameMenu[0], " ");
                                    AddTextMenuEntry(&gameMenu[0], "ERROR IN");
                                    AddTextMenuEntry(&gameMenu[0], scriptName);
                                    parseMode = PARSEMODE_ERROR;
                                }
                            }
                        }
                    }
                    break;

                case PARSEMODE_SWITCHREAD:
                    if (FindStringToken(scriptText, "switch", 1) == 0)
                        ++switchDeep;

                    if (switchDeep) {
                        if (FindStringToken(scriptText, "endswitch", 1) == 0)
                            --switchDeep;
                    }
                    else if (FindStringToken(scriptText, "endswitch", 1) == 0) {
                        SetFilePosition(info.readPos);
                        parseMode  = PARSEMODE_FUNCTION;
                        int jPos   = jumpTableStack[jumpTableStackPos];
                        switchDeep = abs(jumpTable[jPos + 1] - jumpTable[jPos]) + 1;
                        for (textPos = 0; textPos < switchDeep; ++textPos) jumpTable[jumpTablePos++] = -1;
                    }
                    else {
                        CheckCaseNumber(scriptText);
                    }
                    break;

                default: break;
            }
        }

        CloseFile();
    }
}

#endif

void LoadBytecode(int stageListID, int scriptID)
{
    char scriptPath[0x40];
    if (Engine.bytecodeMode == BYTECODE_MOBILE) {
        switch (stageListID) {
            case STAGELIST_PRESENTATION:
            case STAGELIST_REGULAR:
            case STAGELIST_BONUS:
            case STAGELIST_SPECIAL:
                StrCopy(scriptPath, "Data/Scripts/ByteCode/");
                StrAdd(scriptPath, stageList[stageListID][stageListPosition].folder);
                StrAdd(scriptPath, ".bin");
                break;

            case 4: StrCopy(scriptPath, "Data/Scripts/ByteCode/GlobalCode.bin"); break;

            default: break;
        }
    }
    else {
        StrCopy(scriptPath, "Data/Scripts/ByteCode/GS000.bin");
        int pos = StrLength(scriptPath) - 9;
        if (stageListID < STAGELIST_MAX) {
            char listIDs[4]     = { 'P', 'R', 'B', 'S' };
            scriptPath[pos]     = listIDs[stageListID];
            scriptPath[pos + 2] = stageListPosition / 100 + '0';
            scriptPath[pos + 3] = stageListPosition % 100 / 10 + '0';
            scriptPath[pos + 4] = stageListPosition % 10 + '0';
        }
    }

    FileInfo info;
    if (LoadFile(scriptPath, &info)) {
        byte fileBuffer = 0;
        int *scriptCodePtr = &scriptCode[scriptCodePos];
        int *jumpTablePtr       = &jumpTable[jumpTablePos];

        FileRead(&fileBuffer, 1);
        int scriptCodeSize = fileBuffer;
        FileRead(&fileBuffer, 1);
        scriptCodeSize |= fileBuffer << 8;
        FileRead(&fileBuffer, 1);
        scriptCodeSize |= fileBuffer << 16;
        FileRead(&fileBuffer, 1);
        scriptCodeSize |= fileBuffer << 24;

        while (scriptCodeSize > 0) {
            FileRead(&fileBuffer, 1);
            int blockSize = fileBuffer & 0x7F;

            if (fileBuffer >= 0x80) {
                while (blockSize > 0) {
                    FileRead(&fileBuffer, 1);
                    *scriptCodePtr = fileBuffer;
                    FileRead(&fileBuffer, 1);
                    *scriptCodePtr |= fileBuffer << 8;
                    FileRead(&fileBuffer, 1);
                    *scriptCodePtr |= fileBuffer << 16;
                    FileRead(&fileBuffer, 1);
                    *scriptCodePtr |= fileBuffer << 24;

                    ++scriptCodePtr;
                    ++scriptCodePos;
                    --scriptCodeSize;
                    --blockSize;
                }
            }
            else {
                while (blockSize > 0) {
                    FileRead(&fileBuffer, 1);
                    *scriptCodePtr = fileBuffer;

                    ++scriptCodePtr;
                    ++scriptCodePos;
                    --scriptCodeSize;
                    --blockSize;
                }
            }
        }

        FileRead(&fileBuffer, 1);
        int jumpTableSize = fileBuffer;
        FileRead(&fileBuffer, 1);
        jumpTableSize |= fileBuffer << 8;
        FileRead(&fileBuffer, 1);
        jumpTableSize |= fileBuffer << 16;
        FileRead(&fileBuffer, 1);
        jumpTableSize |= fileBuffer << 24;

        while (jumpTableSize > 0) {
            FileRead(&fileBuffer, 1);
            int blockSize = fileBuffer & 0x7F;

            if (fileBuffer >= 0x80) {
                while (blockSize > 0) {
                    FileRead(&fileBuffer, 1);
                    *jumpTablePtr = fileBuffer;
                    FileRead(&fileBuffer, 1);
                    *jumpTablePtr |= fileBuffer << 8;
                    FileRead(&fileBuffer, 1);
                    *jumpTablePtr |= fileBuffer << 16;
                    FileRead(&fileBuffer, 1);
                    *jumpTablePtr |= fileBuffer << 24;

                    ++jumpTablePtr;
                    ++jumpTablePos;
                    --jumpTableSize;
                    --blockSize;
                }
            }
            else {
                while (blockSize > 0) {
                    FileRead(&fileBuffer, 1);
                    *jumpTablePtr = fileBuffer;

                    ++jumpTablePtr;
                    ++jumpTablePos;
                    --jumpTableSize;
                    --blockSize;
                }
            }
        }

        FileRead(&fileBuffer, 1);
        int scriptCount = fileBuffer;
        FileRead(&fileBuffer, 1);
        scriptCount |= fileBuffer << 8;

        for (int s = 0; s < scriptCount; ++s) {
            ObjectScript *script = &objectScriptList[scriptID + s];

            script->mobile = Engine.bytecodeMode == BYTECODE_MOBILE;

            FileRead(&fileBuffer, 1);
            script->subMain.scriptCodePtr = fileBuffer;
            FileRead(&fileBuffer, 1);
            script->subMain.scriptCodePtr |= fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            script->subMain.scriptCodePtr |= fileBuffer << 16;
            FileRead(&fileBuffer, 1);
            script->subMain.scriptCodePtr |= fileBuffer << 24;

            FileRead(&fileBuffer, 1);
            script->subPlayerInteraction.scriptCodePtr = fileBuffer;
            FileRead(&fileBuffer, 1);
            script->subPlayerInteraction.scriptCodePtr |= fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            script->subPlayerInteraction.scriptCodePtr |= fileBuffer << 16;
            FileRead(&fileBuffer, 1);
            script->subPlayerInteraction.scriptCodePtr |= fileBuffer << 24;

            FileRead(&fileBuffer, 1);
            script->subDraw.scriptCodePtr = fileBuffer;
            FileRead(&fileBuffer, 1);
            script->subDraw.scriptCodePtr |= fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            script->subDraw.scriptCodePtr |= fileBuffer << 16;
            FileRead(&fileBuffer, 1);
            script->subDraw.scriptCodePtr |= fileBuffer << 24;

            FileRead(&fileBuffer, 1);
            script->subStartup.scriptCodePtr = fileBuffer;
            FileRead(&fileBuffer, 1);
            script->subStartup.scriptCodePtr |= fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            script->subStartup.scriptCodePtr |= fileBuffer << 16;
            FileRead(&fileBuffer, 1);
            script->subStartup.scriptCodePtr |= fileBuffer << 24;
        }

        for (int s = 0; s < scriptCount; ++s) {
            ObjectScript *script = &objectScriptList[scriptID + s];

            FileRead(&fileBuffer, 1);
            script->subMain.jumpTablePtr = fileBuffer;
            FileRead(&fileBuffer, 1);
            script->subMain.jumpTablePtr |= fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            script->subMain.jumpTablePtr |= fileBuffer << 16;
            FileRead(&fileBuffer, 1);
            script->subMain.jumpTablePtr |= fileBuffer << 24;

            FileRead(&fileBuffer, 1);
            script->subPlayerInteraction.jumpTablePtr = fileBuffer;
            FileRead(&fileBuffer, 1);
            script->subPlayerInteraction.jumpTablePtr |= fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            script->subPlayerInteraction.jumpTablePtr |= fileBuffer << 16;
            FileRead(&fileBuffer, 1);
            script->subPlayerInteraction.jumpTablePtr |= fileBuffer << 24;

            FileRead(&fileBuffer, 1);
            script->subDraw.jumpTablePtr = fileBuffer;
            FileRead(&fileBuffer, 1);
            script->subDraw.jumpTablePtr |= fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            script->subDraw.jumpTablePtr |= fileBuffer << 16;
            FileRead(&fileBuffer, 1);
            script->subDraw.jumpTablePtr |= fileBuffer << 24;

            FileRead(&fileBuffer, 1);
            script->subStartup.jumpTablePtr = fileBuffer;
            FileRead(&fileBuffer, 1);
            script->subStartup.jumpTablePtr |= fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            script->subStartup.jumpTablePtr |= fileBuffer << 16;
            FileRead(&fileBuffer, 1);
            script->subStartup.jumpTablePtr |= fileBuffer << 24;
        }

        FileRead(&fileBuffer, 1);
        int functionCount = fileBuffer;
        FileRead(&fileBuffer, 1);
        functionCount |= fileBuffer << 8;

        for (int f = 0; f < functionCount; ++f) {
            ScriptFunction *function = &scriptFunctionList[f];

            FileRead(&fileBuffer, 1);
            function->ptr.scriptCodePtr = fileBuffer;
            FileRead(&fileBuffer, 1);
            function->ptr.scriptCodePtr |= fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            function->ptr.scriptCodePtr |= fileBuffer << 16;
            FileRead(&fileBuffer, 1);
            function->ptr.scriptCodePtr |= fileBuffer << 24;
        }

        for (int f = 0; f < functionCount; ++f) {
            ScriptFunction *function = &scriptFunctionList[f];

            FileRead(&fileBuffer, 1);
            function->ptr.jumpTablePtr = fileBuffer;
            FileRead(&fileBuffer, 1);
            function->ptr.jumpTablePtr |= fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            function->ptr.jumpTablePtr |= fileBuffer << 16;
            FileRead(&fileBuffer, 1);
            function->ptr.jumpTablePtr |= fileBuffer << 24;
        }

        CloseFile();
    }
}

void ClearScriptData()
{
    memset(scriptCode, 0, SCRIPTDATA_COUNT * sizeof(int));
    memset(jumpTable, 0, JUMPTABLE_COUNT * sizeof(int));

    scriptFrameCount = 0;

    scriptCodePos     = 0;
    jumpTablePos      = 0;
    jumpTableStackPos = 0;
    functionStackPos  = 0;

    scriptCodePos    = 0;
    scriptCodeOffset = 0;
    jumpTablePos     = 0;
    jumpTableOffset  = 0;

#if RETRO_USE_COMPILER
    scriptFunctionCount = 0;
#endif

    aliasCount = COMMONALIAS_COUNT;
    lineID     = 0;

    ClearGraphicsData();
    ClearAnimationData();

    for (int p = 0; p < PLAYER_COUNT; ++p) {
        playerList[p].animationFile = GetDefaultAnimationRef();
        playerList[p].boundEntity   = &objectEntityList[p];
    }

    for (int o = 0; o < OBJECT_COUNT; ++o) {
        ObjectScript *scriptInfo                       = &objectScriptList[o];
        scriptInfo->subMain.scriptCodePtr              = SCRIPTDATA_COUNT - 1;
        scriptInfo->subMain.jumpTablePtr               = JUMPTABLE_COUNT - 1;
        scriptInfo->subPlayerInteraction.scriptCodePtr = SCRIPTDATA_COUNT - 1;
        scriptInfo->subPlayerInteraction.jumpTablePtr  = JUMPTABLE_COUNT - 1;
        scriptInfo->subDraw.scriptCodePtr              = SCRIPTDATA_COUNT - 1;
        scriptInfo->subDraw.jumpTablePtr               = JUMPTABLE_COUNT - 1;
        scriptInfo->subStartup.scriptCodePtr           = SCRIPTDATA_COUNT - 1;
        scriptInfo->subStartup.jumpTablePtr            = JUMPTABLE_COUNT - 1;
        scriptInfo->frameListOffset                    = 0;
        scriptInfo->spriteSheetID                      = 0;
        scriptInfo->animFile                           = GetDefaultAnimationRef();
        scriptInfo->mobile                             = true;
        typeNames[o][0]                                = 0;
    }

    for (int f = 0; f < FUNCTION_COUNT; ++f) {
        scriptFunctionList[f].ptr.scriptCodePtr = SCRIPTDATA_COUNT - 1;
        scriptFunctionList[f].ptr.jumpTablePtr  = JUMPTABLE_COUNT - 1;
    }

    SetObjectTypeName((char *)"Blank Object", 0);

    for (int s = 0; s < 2; s++) {
        collisionStorage[s].entityNo = -1;
        collisionStorage[s].type     = -1;
        collisionStorage[s].left     = 0;
        collisionStorage[s].top      = 0;
        collisionStorage[s].right    = 0;
        collisionStorage[s].bottom   = 0;
    }

}

void ProcessScript(int scriptCodeStart, int jumpTableStart, byte scriptSub)
{
    bool running      = true;
    int scriptCodePtr = scriptCodeStart;

    jumpTableStackPos = 0;
    functionStackPos  = 0;
    while (running) {
        int opcode           = scriptCode[scriptCodePtr++];
        int opcodeSize       = functions[opcode].opcodeSize;
        int scriptCodeOffset = scriptCodePtr;

        // Get Values
        for (int i = 0; i < opcodeSize; ++i) {
            int opcodeType = scriptCode[scriptCodePtr++];

            if (opcodeType == SCRIPTVAR_VAR) {
                int arrayVal = 0;
                switch (scriptCode[scriptCodePtr++]) {
                    case VARARR_NONE: arrayVal = objectLoop; break;
                    case VARARR_ARRAY:
                        if (scriptCode[scriptCodePtr++] == 1)
                            arrayVal = scriptEng.arrayPosition[scriptCode[scriptCodePtr++]];
                        else
                            arrayVal = scriptCode[scriptCodePtr++];
                        break;
                    case VARARR_ENTNOPLUS1:
                        if (scriptCode[scriptCodePtr++] == 1)
                            arrayVal = scriptEng.arrayPosition[scriptCode[scriptCodePtr++]] + objectLoop;
                        else
                            arrayVal = scriptCode[scriptCodePtr++] + objectLoop;
                        break;
                    case VARARR_ENTNOMINUS1:
                        if (scriptCode[scriptCodePtr++] == 1)
                            arrayVal = objectLoop - scriptEng.arrayPosition[scriptCode[scriptCodePtr++]];
                        else
                            arrayVal = objectLoop - scriptCode[scriptCodePtr++];
                        break;
                    default: break;
                }

                // Variables
                switch (scriptCode[scriptCodePtr++]) {
                    default: break;
                    case VAR_TEMPVALUE0: scriptEng.operands[i] = scriptEng.tempValue[0]; break;
                    case VAR_TEMPVALUE1: scriptEng.operands[i] = scriptEng.tempValue[1]; break;
                    case VAR_TEMPVALUE2: scriptEng.operands[i] = scriptEng.tempValue[2]; break;
                    case VAR_TEMPVALUE3: scriptEng.operands[i] = scriptEng.tempValue[3]; break;
                    case VAR_TEMPVALUE4: scriptEng.operands[i] = scriptEng.tempValue[4]; break;
                    case VAR_TEMPVALUE5: scriptEng.operands[i] = scriptEng.tempValue[5]; break;
                    case VAR_TEMPVALUE6: scriptEng.operands[i] = scriptEng.tempValue[6]; break;
                    case VAR_TEMPVALUE7: scriptEng.operands[i] = scriptEng.tempValue[7]; break;
                    case VAR_CHECKRESULT: scriptEng.operands[i] = scriptEng.checkResult; break;
                    case VAR_ARRAYPOS0: scriptEng.operands[i] = scriptEng.arrayPosition[0]; break;
                    case VAR_ARRAYPOS1: scriptEng.operands[i] = scriptEng.arrayPosition[1]; break;
                    case VAR_GLOBAL: scriptEng.operands[i] = globalVariables[arrayVal]; break;
                    case VAR_OBJECTENTITYNO: scriptEng.operands[i] = arrayVal; break;
                    case VAR_OBJECTTYPE: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].type;
                        break;
                    }
                    case VAR_OBJECTPROPERTYVALUE: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].propertyValue;
                        break;
                    }
                    case VAR_OBJECTXPOS: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].XPos;
                        break;
                    }
                    case VAR_OBJECTYPOS: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].YPos;
                        break;
                    }
                    case VAR_OBJECTIXPOS: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].XPos >> 16;
                        break;
                    }
                    case VAR_OBJECTIYPOS: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].YPos >> 16;
                        break;
                    }
                    case VAR_OBJECTSTATE: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].state;
                        break;
                    }
                    case VAR_OBJECTROTATION: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].rotation;
                        break;
                    }
                    case VAR_OBJECTSCALE: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].scale;
                        break;
                    }
                    case VAR_OBJECTPRIORITY: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].priority;
                        break;
                    }
                    case VAR_OBJECTDRAWORDER: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].drawOrder;
                        break;
                    }
                    case VAR_OBJECTDIRECTION: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].direction;
                        break;
                    }
                    case VAR_OBJECTINKEFFECT: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].inkEffect;
                        break;
                    }
                    case VAR_OBJECTALPHA: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].alpha;
                        break;
                    }
                    case VAR_OBJECTFRAME: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].frame;
                        break;
                    }
                    case VAR_OBJECTANIMATION: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].animation;
                        break;
                    }
                    case VAR_OBJECTPREVANIMATION: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].prevAnimation;
                        break;
                    }
                    case VAR_OBJECTANIMATIONSPEED: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].animationSpeed;
                        break;
                    }
                    case VAR_OBJECTANIMATIONTIMER: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].animationTimer;
                        break;
                    }
                    case VAR_OBJECTVALUE0: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[0];
                        break;
                    }
                    case VAR_OBJECTVALUE1: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[1];
                        break;
                    }
                    case VAR_OBJECTVALUE2: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[2];
                        break;
                    }
                    case VAR_OBJECTVALUE3: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[3];
                        break;
                    }
                    case VAR_OBJECTVALUE4: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[4];
                        break;
                    }
                    case VAR_OBJECTVALUE5: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[5];
                        break;
                    }
                    case VAR_OBJECTVALUE6: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[6];
                        break;
                    }
                    case VAR_OBJECTVALUE7: {
                        scriptEng.operands[i] = objectEntityList[arrayVal].values[7];
                        break;
                    }
                    case VAR_OBJECTOUTOFBOUNDS: {
                        int pos = objectEntityList[arrayVal].XPos >> 16;
                        if (pos <= xScrollOffset - OBJECT_BORDER_X1 || pos >= OBJECT_BORDER_X2 + xScrollOffset) {
                            scriptEng.operands[i] = 1;
                        }
                        else {
                            int pos               = objectEntityList[arrayVal].YPos >> 16;
                            scriptEng.operands[i] = pos <= yScrollOffset - OBJECT_BORDER_Y1 || pos >= yScrollOffset + OBJECT_BORDER_Y2;
                        }
                        break;
                    }
                    case VAR_PLAYERSTATE: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->state;
                        break;
                    }
                    case VAR_PLAYERCONTROLMODE: {
                        scriptEng.operands[i] = playerList[activePlayer].controlMode;
                        break;
                    }
                    case VAR_PLAYERCONTROLLOCK: {
                        scriptEng.operands[i] = playerList[activePlayer].controlLock;
                        break;
                    }
                    case VAR_PLAYERCOLLISIONMODE: {
                        scriptEng.operands[i] = playerList[activePlayer].collisionMode;
                        break;
                    }
                    case VAR_PLAYERCOLLISIONPLANE: {
                        scriptEng.operands[i] = playerList[activePlayer].collisionPlane;
                        break;
                    }
                    case VAR_PLAYERXPOS: {
                        scriptEng.operands[i] = playerList[activePlayer].XPos;
                        break;
                    }
                    case VAR_PLAYERYPOS: {
                        scriptEng.operands[i] = playerList[activePlayer].YPos;
                        break;
                    }
                    case VAR_PLAYERIXPOS: {
                        scriptEng.operands[i] = playerList[activePlayer].XPos >> 16;
                        break;
                    }
                    case VAR_PLAYERIYPOS: {
                        scriptEng.operands[i] = playerList[activePlayer].YPos >> 16;
                        break;
                    }
                    case VAR_PLAYERSCREENXPOS: {
                        scriptEng.operands[i] = playerList[activePlayer].screenXPos;
                        break;
                    }
                    case VAR_PLAYERSCREENYPOS: {
                        scriptEng.operands[i] = playerList[activePlayer].screenYPos;
                        break;
                    }
                    case VAR_PLAYERSPEED: {
                        scriptEng.operands[i] = playerList[activePlayer].speed;
                        break;
                    }
                    case VAR_PLAYERXVELOCITY: {
                        scriptEng.operands[i] = playerList[activePlayer].XVelocity;
                        break;
                    }
                    case VAR_PLAYERYVELOCITY: {
                        scriptEng.operands[i] = playerList[activePlayer].YVelocity;
                        break;
                    }
                    case VAR_PLAYERGRAVITY: {
                        scriptEng.operands[i] = playerList[activePlayer].gravity;
                        break;
                    }
                    case VAR_PLAYERANGLE: {
                        scriptEng.operands[i] = playerList[activePlayer].angle;
                        break;
                    }
                    case VAR_PLAYERSKIDDING: {
                        scriptEng.operands[i] = playerList[activePlayer].skidding;
                        break;
                    }
                    case VAR_PLAYERPUSHING: {
                        scriptEng.operands[i] = playerList[activePlayer].pushing;
                        break;
                    }
                    case VAR_PLAYERTRACKSCROLL: {
                        scriptEng.operands[i] = playerList[activePlayer].trackScroll;
                        break;
                    }
                    case VAR_PLAYERUP: {
                        scriptEng.operands[i] = playerList[activePlayer].up;
                        break;
                    }
                    case VAR_PLAYERDOWN: {
                        scriptEng.operands[i] = playerList[activePlayer].down;
                        break;
                    }
                    case VAR_PLAYERLEFT: {
                        scriptEng.operands[i] = playerList[activePlayer].left;
                        break;
                    }
                    case VAR_PLAYERRIGHT: {
                        scriptEng.operands[i] = playerList[activePlayer].right;
                        break;
                    }
                    case VAR_PLAYERJUMPPRESS: {
                        scriptEng.operands[i] = playerList[activePlayer].jumpPress;
                        break;
                    }
                    case VAR_PLAYERJUMPHOLD: {
                        scriptEng.operands[i] = playerList[activePlayer].jumpHold;
                        break;
                    }
                    case VAR_PLAYERFOLLOWPLAYER1: {
                        scriptEng.operands[i] = playerList[activePlayer].followPlayer1;
                        break;
                    }
                    case VAR_PLAYERLOOKPOS: {
                        scriptEng.operands[i] = playerList[activePlayer].lookPos;
                        break;
                    }
                    case VAR_PLAYERWATER: {
                        scriptEng.operands[i] = playerList[activePlayer].water;
                        break;
                    }
                    case VAR_PLAYERTOPSPEED: {
                        scriptEng.operands[i] = playerList[activePlayer].topSpeed;
                        break;
                    }
                    case VAR_PLAYERACCELERATION: {
                        scriptEng.operands[i] = playerList[activePlayer].acceleration;
                        break;
                    }
                    case VAR_PLAYERDECELERATION: {
                        scriptEng.operands[i] = playerList[activePlayer].deceleration;
                        break;
                    }
                    case VAR_PLAYERAIRACCELERATION: {
                        scriptEng.operands[i] = playerList[activePlayer].airAcceleration;
                        break;
                    }
                    case VAR_PLAYERAIRDECELERATION: {
                        scriptEng.operands[i] = playerList[activePlayer].airDeceleration;
                        break;
                    }
                    case VAR_PLAYERGRAVITYSTRENGTH: {
                        scriptEng.operands[i] = playerList[activePlayer].gravityStrength;
                        break;
                    }
                    case VAR_PLAYERJUMPSTRENGTH: {
                        scriptEng.operands[i] = playerList[activePlayer].jumpStrength;
                        break;
                    }
                    case VAR_PLAYERJUMPCAP: {
                        scriptEng.operands[i] = playerList[activePlayer].jumpCap;
                        break;
                    }
                    case VAR_PLAYERROLLINGACCELERATION: {
                        scriptEng.operands[i] = playerList[activePlayer].rollingAcceleration;
                        break;
                    }
                    case VAR_PLAYERROLLINGDECELERATION: {
                        scriptEng.operands[i] = playerList[activePlayer].rollingDeceleration;
                        break;
                    }
                    case VAR_PLAYERENTITYNO: {
                        scriptEng.operands[i] = playerList[activePlayer].entityNo;
                        break;
                    }
                    case VAR_PLAYERCOLLISIONLEFT: {
                        AnimationFile *animFile = playerList[activePlayer].animationFile;
                        Player *plr             = &playerList[activePlayer];
                        if (animFile) {
                            int h = animFrames[animationList[animFile->aniListOffset + plr->boundEntity->animation].frameListOffset
                                               + plr->boundEntity->frame]
                                        .hitboxID;

                            scriptEng.operands[i] = hitboxList[animFile->hitboxListOffset + h].left[0];
                        }
                        else {
                            scriptEng.operands[i] = 0;
                        }
                        break;
                    }
                    case VAR_PLAYERCOLLISIONTOP: {
                        AnimationFile *animFile = playerList[activePlayer].animationFile;
                        Player *plr             = &playerList[activePlayer];
                        if (animFile) {
                            int h = animFrames[animationList[animFile->aniListOffset + plr->boundEntity->animation].frameListOffset
                                               + plr->boundEntity->frame]
                                        .hitboxID;

                            scriptEng.operands[i] = hitboxList[animFile->hitboxListOffset + h].top[0];
                        }
                        else {
                            scriptEng.operands[i] = 0;
                        }
                        break;
                    }
                    case VAR_PLAYERCOLLISIONRIGHT: {
                        AnimationFile *animFile = playerList[activePlayer].animationFile;
                        Player *plr             = &playerList[activePlayer];
                        if (animFile) {
                            int h = animFrames[animationList[animFile->aniListOffset + plr->boundEntity->animation].frameListOffset
                                               + plr->boundEntity->frame]
                                        .hitboxID;

                            scriptEng.operands[i] = hitboxList[animFile->hitboxListOffset + h].right[0];
                        }
                        else {
                            scriptEng.operands[i] = 0;
                        }
                        break;
                    }
                    case VAR_PLAYERCOLLISIONBOTTOM: {
                        AnimationFile *animFile = playerList[activePlayer].animationFile;
                        Player *plr             = &playerList[activePlayer];
                        if (animFile) {
                            int h = animFrames[animationList[animFile->aniListOffset + plr->boundEntity->animation].frameListOffset
                                               + plr->boundEntity->frame]
                                        .hitboxID;

                            scriptEng.operands[i] = hitboxList[animFile->hitboxListOffset + h].bottom[0];
                        }
                        else {
                            scriptEng.operands[i] = 0;
                        }
                        break;
                    }
                    case VAR_PLAYERFLAILING: {
                        scriptEng.operands[i] = playerList[activePlayer].flailing[arrayVal];
                        break;
                    }
                    case VAR_PLAYERTIMER: {
                        scriptEng.operands[i] = playerList[activePlayer].timer;
                        break;
                    }
                    case VAR_PLAYERTILECOLLISIONS: {
                        scriptEng.operands[i] = playerList[activePlayer].tileCollisions;
                        break;
                    }
                    case VAR_PLAYEROBJECTINTERACTION: {
                        scriptEng.operands[i] = playerList[activePlayer].objectInteractions;
                        break;
                    }
                    case VAR_PLAYERVISIBLE: {
                        scriptEng.operands[i] = playerList[activePlayer].visible;
                        break;
                    }
                    case VAR_PLAYERROTATION: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->rotation;
                        break;
                    }
                    case VAR_PLAYERSCALE: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->scale;
                        break;
                    }
                    case VAR_PLAYERPRIORITY: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->priority;
                        break;
                    }
                    case VAR_PLAYERDRAWORDER: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->drawOrder;
                        break;
                    }
                    case VAR_PLAYERDIRECTION: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->direction;
                        break;
                    }
                    case VAR_PLAYERINKEFFECT: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->inkEffect;
                        break;
                    }
                    case VAR_PLAYERALPHA: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->alpha;
                        break;
                    }
                    case VAR_PLAYERFRAME: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->frame;
                        break;
                    }
                    case VAR_PLAYERANIMATION: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->animation;
                        break;
                    }
                    case VAR_PLAYERPREVANIMATION: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->prevAnimation;
                        break;
                    }
                    case VAR_PLAYERANIMATIONSPEED: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->animationSpeed;
                        break;
                    }
                    case VAR_PLAYERANIMATIONTIMER: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->animationTimer;
                        break;
                    }
                    case VAR_PLAYERVALUE0: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->values[0];
                        break;
                    }
                    case VAR_PLAYERVALUE1: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->values[1];
                        break;
                    }
                    case VAR_PLAYERVALUE2: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->values[2];
                        break;
                    }
                    case VAR_PLAYERVALUE3: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->values[3];
                        break;
                    }
                    case VAR_PLAYERVALUE4: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->values[4];
                        break;
                    }
                    case VAR_PLAYERVALUE5: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->values[5];
                        break;
                    }
                    case VAR_PLAYERVALUE6: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->values[6];
                        break;
                    }
                    case VAR_PLAYERVALUE7: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->values[7];
                        break;
                    }
                    case VAR_PLAYERVALUE8: {
                        scriptEng.operands[i] = playerList[activePlayer].values[0];
                        break;
                    }
                    case VAR_PLAYERVALUE9: {
                        scriptEng.operands[i] = playerList[activePlayer].values[1];
                        break;
                    }
                    case VAR_PLAYERVALUE10: {
                        scriptEng.operands[i] = playerList[activePlayer].values[2];
                        break;
                    }
                    case VAR_PLAYERVALUE11: {
                        scriptEng.operands[i] = playerList[activePlayer].values[3];
                        break;
                    }
                    case VAR_PLAYERVALUE12: {
                        scriptEng.operands[i] = playerList[activePlayer].values[4];
                        break;
                    }
                    case VAR_PLAYERVALUE13: {
                        scriptEng.operands[i] = playerList[activePlayer].values[5];
                        break;
                    }
                    case VAR_PLAYERVALUE14: {
                        scriptEng.operands[i] = playerList[activePlayer].values[6];
                        break;
                    }
                    case VAR_PLAYERVALUE15: {
                        scriptEng.operands[i] = playerList[activePlayer].values[7];
                        break;
                    }
                    case VAR_PLAYEROUTOFBOUNDS: {
                        int pos = playerList[activePlayer].XPos >> 16;
                        if (pos <= xScrollOffset - OBJECT_BORDER_X1 || pos >= OBJECT_BORDER_X2 + xScrollOffset) {
                            scriptEng.operands[i] = 1;
                        }
                        else {
                            int pos               = playerList[activePlayer].YPos >> 16;
                            scriptEng.operands[i] = pos <= yScrollOffset - OBJECT_BORDER_Y1 || pos >= yScrollOffset + OBJECT_BORDER_Y2;
                        }
                        break;
                    }
                    case VAR_STAGESTATE: scriptEng.operands[i] = stageMode; break;
                    case VAR_STAGEACTIVELIST: scriptEng.operands[i] = activeStageList; break;
                    case VAR_STAGELISTPOS: scriptEng.operands[i] = stageListPosition; break;
                    case VAR_STAGETIMEENABLED: scriptEng.operands[i] = timeEnabled; break;
                    case VAR_STAGEMILLISECONDS: scriptEng.operands[i] = stageMilliseconds; break;
                    case VAR_STAGESECONDS: scriptEng.operands[i] = stageSeconds; break;
                    case VAR_STAGEMINUTES: scriptEng.operands[i] = stageMinutes; break;
                    case VAR_STAGEACTNO: scriptEng.operands[i] = actID; break;
                    case VAR_STAGEPAUSEENABLED: scriptEng.operands[i] = pauseEnabled; break;
                    case VAR_STAGELISTSIZE: scriptEng.operands[i] = stageListCount[activeStageList]; break;
                    case VAR_STAGENEWXBOUNDARY1: scriptEng.operands[i] = newXBoundary1; break;
                    case VAR_STAGENEWXBOUNDARY2: scriptEng.operands[i] = newXBoundary2; break;
                    case VAR_STAGENEWYBOUNDARY1: scriptEng.operands[i] = newYBoundary1; break;
                    case VAR_STAGENEWYBOUNDARY2: scriptEng.operands[i] = newYBoundary2; break;
                    case VAR_STAGEXBOUNDARY1: scriptEng.operands[i] = xBoundary1; break;
                    case VAR_STAGEXBOUNDARY2: scriptEng.operands[i] = xBoundary2; break;
                    case VAR_STAGEYBOUNDARY1: scriptEng.operands[i] = yBoundary1; break;
                    case VAR_STAGEYBOUNDARY2: scriptEng.operands[i] = yBoundary2; break;
                    case VAR_STAGEDEFORMATIONDATA0: scriptEng.operands[i] = bgDeformationData0[arrayVal]; break;
                    case VAR_STAGEDEFORMATIONDATA1: scriptEng.operands[i] = bgDeformationData1[arrayVal]; break;
                    case VAR_STAGEDEFORMATIONDATA2: scriptEng.operands[i] = bgDeformationData2[arrayVal]; break;
                    case VAR_STAGEDEFORMATIONDATA3: scriptEng.operands[i] = bgDeformationData3[arrayVal]; break;
                    case VAR_STAGEWATERLEVEL: scriptEng.operands[i] = waterLevel; break;
                    case VAR_STAGEACTIVELAYER: scriptEng.operands[i] = activeTileLayers[arrayVal]; break;
                    case VAR_STAGEMIDPOINT: scriptEng.operands[i] = tLayerMidPoint; break;
                    case VAR_STAGEPLAYERLISTPOS: scriptEng.operands[i] = playerListPos; break;
                    case VAR_STAGEACTIVEPLAYER: scriptEng.operands[i] = activePlayer; break;
                    case VAR_SCREENCAMERAENABLED: scriptEng.operands[i] = cameraEnabled; break;
                    case VAR_SCREENCAMERATARGET: scriptEng.operands[i] = cameraTarget; break;
                    case VAR_SCREENCAMERASTYLE: scriptEng.operands[i] = cameraStyle; break;
                    case VAR_SCREENDRAWLISTSIZE: scriptEng.operands[i] = drawListEntries[arrayVal].listSize; break;
                    case VAR_SCREENCENTERX: scriptEng.operands[i] = SCREEN_CENTERX; break;
                    case VAR_SCREENCENTERY: scriptEng.operands[i] = SCREEN_CENTERY; break;
                    case VAR_SCREENXSIZE: scriptEng.operands[i] = SCREEN_XSIZE; break;
                    case VAR_SCREENYSIZE: scriptEng.operands[i] = SCREEN_YSIZE; break;
                    case VAR_SCREENXOFFSET: scriptEng.operands[i] = xScrollOffset; break;
                    case VAR_SCREENYOFFSET: scriptEng.operands[i] = yScrollOffset; break;
                    case VAR_SCREENSHAKEX: scriptEng.operands[i] = cameraShakeX; break;
                    case VAR_SCREENSHAKEY: scriptEng.operands[i] = cameraShakeY; break;
                    case VAR_SCREENADJUSTCAMERAY: scriptEng.operands[i] = cameraAdjustY; break;
                    case VAR_TOUCHSCREENDOWN: scriptEng.operands[i] = touchDown[arrayVal]; break;
                    case VAR_TOUCHSCREENXPOS: scriptEng.operands[i] = touchX[arrayVal]; break;
                    case VAR_TOUCHSCREENYPOS: scriptEng.operands[i] = touchY[arrayVal]; break;
                    case VAR_MUSICVOLUME: scriptEng.operands[i] = masterVolume; break;
                    case VAR_MUSICCURRENTTRACK: scriptEng.operands[i] = trackID; break;
                    case VAR_KEYDOWNUP: scriptEng.operands[i] = keyDown.up; break;
                    case VAR_KEYDOWNDOWN: scriptEng.operands[i] = keyDown.down; break;
                    case VAR_KEYDOWNLEFT: scriptEng.operands[i] = keyDown.left; break;
                    case VAR_KEYDOWNRIGHT: scriptEng.operands[i] = keyDown.right; break;
                    case VAR_KEYDOWNBUTTONA: scriptEng.operands[i] = keyDown.A; break;
                    case VAR_KEYDOWNBUTTONB: scriptEng.operands[i] = keyDown.B; break;
                    case VAR_KEYDOWNBUTTONC: scriptEng.operands[i] = keyDown.C; break;
                    case VAR_KEYDOWNSTART: scriptEng.operands[i] = keyDown.start; break;
                    case VAR_KEYPRESSUP: scriptEng.operands[i] = keyPress.up; break;
                    case VAR_KEYPRESSDOWN: scriptEng.operands[i] = keyPress.down; break;
                    case VAR_KEYPRESSLEFT: scriptEng.operands[i] = keyPress.left; break;
                    case VAR_KEYPRESSRIGHT: scriptEng.operands[i] = keyPress.right; break;
                    case VAR_KEYPRESSBUTTONA: scriptEng.operands[i] = keyPress.A; break;
                    case VAR_KEYPRESSBUTTONB: scriptEng.operands[i] = keyPress.B; break;
                    case VAR_KEYPRESSBUTTONC: scriptEng.operands[i] = keyPress.C; break;
                    case VAR_KEYPRESSSTART: scriptEng.operands[i] = keyPress.start; break;
                    case VAR_MENU1SELECTION: scriptEng.operands[i] = gameMenu[0].selection1; break;
                    case VAR_MENU2SELECTION: scriptEng.operands[i] = gameMenu[1].selection1; break;
                    case VAR_TILELAYERXSIZE: scriptEng.operands[i] = stageLayouts[arrayVal].xsize; break;
                    case VAR_TILELAYERYSIZE: scriptEng.operands[i] = stageLayouts[arrayVal].ysize; break;
                    case VAR_TILELAYERTYPE: scriptEng.operands[i] = stageLayouts[arrayVal].type; break;
                    case VAR_TILELAYERANGLE: scriptEng.operands[i] = stageLayouts[arrayVal].angle; break;
                    case VAR_TILELAYERXPOS: scriptEng.operands[i] = stageLayouts[arrayVal].XPos; break;
                    case VAR_TILELAYERYPOS: scriptEng.operands[i] = stageLayouts[arrayVal].YPos; break;
                    case VAR_TILELAYERZPOS: scriptEng.operands[i] = stageLayouts[arrayVal].ZPos; break;
                    case VAR_TILELAYERPARALLAXFACTOR: scriptEng.operands[i] = stageLayouts[arrayVal].parallaxFactor; break;
                    case VAR_TILELAYERSCROLLSPEED: scriptEng.operands[i] = stageLayouts[arrayVal].scrollSpeed; break;
                    case VAR_TILELAYERSCROLLPOS: scriptEng.operands[i] = stageLayouts[arrayVal].scrollPos; break;
                    case VAR_TILELAYERDEFORMATIONOFFSET: scriptEng.operands[i] = stageLayouts[arrayVal].deformationOffset; break;
                    case VAR_TILELAYERDEFORMATIONOFFSETW: scriptEng.operands[i] = stageLayouts[arrayVal].deformationOffsetW; break;
                    case VAR_HPARALLAXPARALLAXFACTOR: scriptEng.operands[i] = hParallax.parallaxFactor[arrayVal]; break;
                    case VAR_HPARALLAXSCROLLSPEED: scriptEng.operands[i] = hParallax.scrollSpeed[arrayVal]; break;
                    case VAR_HPARALLAXSCROLLPOS: scriptEng.operands[i] = hParallax.scrollPos[arrayVal]; break;
                    case VAR_VPARALLAXPARALLAXFACTOR: scriptEng.operands[i] = vParallax.parallaxFactor[arrayVal]; break;
                    case VAR_VPARALLAXSCROLLSPEED: scriptEng.operands[i] = vParallax.scrollSpeed[arrayVal]; break;
                    case VAR_VPARALLAXSCROLLPOS: scriptEng.operands[i] = vParallax.scrollPos[arrayVal]; break;
                    case VAR_3DSCENENOVERTICES: scriptEng.operands[i] = vertexCount; break;
                    case VAR_3DSCENENOFACES: scriptEng.operands[i] = faceCount; break;
                    case VAR_VERTEXBUFFERX: scriptEng.operands[i] = vertexBuffer[arrayVal].x; break;
                    case VAR_VERTEXBUFFERY: scriptEng.operands[i] = vertexBuffer[arrayVal].y; break;
                    case VAR_VERTEXBUFFERZ: scriptEng.operands[i] = vertexBuffer[arrayVal].z; break;
                    case VAR_VERTEXBUFFERU: scriptEng.operands[i] = vertexBuffer[arrayVal].u; break;
                    case VAR_VERTEXBUFFERV: scriptEng.operands[i] = vertexBuffer[arrayVal].v; break;
                    case VAR_FACEBUFFERA: scriptEng.operands[i] = faceBuffer[arrayVal].a; break;
                    case VAR_FACEBUFFERB: scriptEng.operands[i] = faceBuffer[arrayVal].b; break;
                    case VAR_FACEBUFFERC: scriptEng.operands[i] = faceBuffer[arrayVal].c; break;
                    case VAR_FACEBUFFERD: scriptEng.operands[i] = faceBuffer[arrayVal].d; break;
                    case VAR_FACEBUFFERFLAG: scriptEng.operands[i] = faceBuffer[arrayVal].flags; break;
                    case VAR_FACEBUFFERCOLOR: scriptEng.operands[i] = faceBuffer[arrayVal].colour; break;
                    case VAR_3DSCENEPROJECTIONX: scriptEng.operands[i] = projectionX; break;
                    case VAR_3DSCENEPROJECTIONY: scriptEng.operands[i] = projectionY; break;
                    case VAR_ENGINESTATE: scriptEng.operands[i] = Engine.gameMode; break;
                    case VAR_STAGEDEBUGMODE: scriptEng.operands[i] = debugMode; break;
                    case VAR_ENGINEMESSAGE: scriptEng.operands[i] = Engine.message; break;
                    case VAR_SAVERAM: scriptEng.operands[i] = saveRAM[arrayVal]; break;
                    case VAR_ENGINELANGUAGE: scriptEng.operands[i] = Engine.language; break;
                    case VAR_OBJECTSPRITESHEET: {
                        scriptEng.operands[i] = objectScriptList[objectEntityList[arrayVal].type].spriteSheetID;
                        break;
                    }
                    case VAR_ENGINEONLINEACTIVE: scriptEng.operands[i] = Engine.onlineActive; break;
                    case VAR_ENGINEFRAMESKIPTIMER: scriptEng.operands[i] = Engine.frameSkipTimer; break;
                    case VAR_ENGINEFRAMESKIPSETTING: scriptEng.operands[i] = Engine.frameSkipSetting; break;
                    case VAR_ENGINESFXVOLUME: scriptEng.operands[i] = sfxVolume; break;
                    case VAR_ENGINEBGMVOLUME: scriptEng.operands[i] = bgmVolume; break;
                    case VAR_ENGINEPLATFORMID: scriptEng.operands[i] = RETRO_GAMEPLATFORMID; break;
                    case VAR_ENGINETRIALMODE: scriptEng.operands[i] = Engine.trialMode; break;
                    case VAR_KEYPRESSANYSTART: scriptEng.operands[i] = anyPress; break;
#if RETRO_USE_HAPTICS
                    case VAR_ENGINEHAPTICSENABLED: scriptEng.operands[i] = Engine.hapticsEnabled; break;
#endif
                }
            }
            else if (opcodeType == SCRIPTVAR_INTCONST) { // int constant
                scriptEng.operands[i] = scriptCode[scriptCodePtr++];
            }
            else if (opcodeType == SCRIPTVAR_STRCONST) { // string constant
                int strLen         = scriptCode[scriptCodePtr++];
                scriptText[strLen] = 0;
                for (int c = 0; c < strLen; ++c) {
                    switch (c % 4) {
                        case 0: scriptText[c] = scriptCode[scriptCodePtr] >> 24; break;

                        case 1: scriptText[c] = (0xFFFFFF & scriptCode[scriptCodePtr]) >> 16; break;

                        case 2: scriptText[c] = (0xFFFF & scriptCode[scriptCodePtr]) >> 8; break;

                        case 3: scriptText[c] = scriptCode[scriptCodePtr++]; break;

                        default: break;
                    }
                }
                scriptCodePtr++;
            }
        }

        ObjectScript *scriptInfo = &objectScriptList[objectEntityList[objectLoop].type];
        Entity *entity           = &objectEntityList[objectLoop];
        Player *player           = &playerList[activePlayer];
        SpriteFrame *spriteFrame = nullptr;

        // Functions
        switch (opcode) {
            default: break;
            case FUNC_END: running = false; break;
            case FUNC_EQUAL: scriptEng.operands[0] = scriptEng.operands[1]; break;
            case FUNC_ADD: scriptEng.operands[0] += scriptEng.operands[1]; break;
            case FUNC_SUB: scriptEng.operands[0] -= scriptEng.operands[1]; break;
            case FUNC_INC: ++scriptEng.operands[0]; break;
            case FUNC_DEC: --scriptEng.operands[0]; break;
            case FUNC_MUL: scriptEng.operands[0] *= scriptEng.operands[1]; break;
            case FUNC_DIV: scriptEng.operands[0] /= scriptEng.operands[1]; break;
            case FUNC_SHR: scriptEng.operands[0] >>= scriptEng.operands[1]; break;
            case FUNC_SHL: scriptEng.operands[0] <<= scriptEng.operands[1]; break;
            case FUNC_AND: scriptEng.operands[0] &= scriptEng.operands[1]; break;
            case FUNC_OR: scriptEng.operands[0] |= scriptEng.operands[1]; break;
            case FUNC_XOR: scriptEng.operands[0] ^= scriptEng.operands[1]; break;
            case FUNC_MOD: scriptEng.operands[0] %= scriptEng.operands[1]; break;
            case FUNC_FLIPSIGN: scriptEng.operands[0] = -scriptEng.operands[0]; break;
            case FUNC_CHECKEQUAL:
                scriptEng.checkResult = scriptEng.operands[0] == scriptEng.operands[1];
                opcodeSize            = 0;
                break;
            case FUNC_CHECKGREATER:
                scriptEng.checkResult = scriptEng.operands[0] > scriptEng.operands[1];
                opcodeSize            = 0;
                break;
            case FUNC_CHECKLOWER:
                scriptEng.checkResult = scriptEng.operands[0] < scriptEng.operands[1];
                opcodeSize            = 0;
                break;
            case FUNC_CHECKNOTEQUAL:
                scriptEng.checkResult = scriptEng.operands[0] != scriptEng.operands[1];
                opcodeSize            = 0;
                break;
            case FUNC_IFEQUAL:
                if (scriptEng.operands[1] != scriptEng.operands[2])
                    scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + scriptEng.operands[0]];
                jumpTableStack[++jumpTableStackPos] = scriptEng.operands[0];
                opcodeSize                          = 0;
                break;
            case FUNC_IFGREATER:
                if (scriptEng.operands[1] <= scriptEng.operands[2])
                    scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + scriptEng.operands[0]];
                jumpTableStack[++jumpTableStackPos] = scriptEng.operands[0];
                opcodeSize                          = 0;
                break;
            case FUNC_IFGREATEROREQUAL:
                if (scriptEng.operands[1] < scriptEng.operands[2])
                    scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + scriptEng.operands[0]];
                jumpTableStack[++jumpTableStackPos] = scriptEng.operands[0];
                opcodeSize                          = 0;
                break;
            case FUNC_IFLOWER:
                if (scriptEng.operands[1] >= scriptEng.operands[2])
                    scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + scriptEng.operands[0]];
                jumpTableStack[++jumpTableStackPos] = scriptEng.operands[0];
                opcodeSize                          = 0;
                break;
            case FUNC_IFLOWEROREQUAL:
                if (scriptEng.operands[1] > scriptEng.operands[2])
                    scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + scriptEng.operands[0]];
                jumpTableStack[++jumpTableStackPos] = scriptEng.operands[0];
                opcodeSize                          = 0;
                break;
            case FUNC_IFNOTEQUAL:
                if (scriptEng.operands[1] == scriptEng.operands[2])
                    scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + scriptEng.operands[0]];
                jumpTableStack[++jumpTableStackPos] = scriptEng.operands[0];
                opcodeSize                          = 0;
                break;
            case FUNC_ELSE:
                opcodeSize    = 0;
                scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + jumpTableStack[jumpTableStackPos--] + 1];
                break;
            case FUNC_ENDIF:
                opcodeSize = 0;
                --jumpTableStackPos;
                break;
            case FUNC_WEQUAL:
                if (scriptEng.operands[1] != scriptEng.operands[2])
                    scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + scriptEng.operands[0] + 1];
                else
                    jumpTableStack[++jumpTableStackPos] = scriptEng.operands[0];
                opcodeSize = 0;
                break;
            case FUNC_WGREATER:
                if (scriptEng.operands[1] <= scriptEng.operands[2])
                    scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + scriptEng.operands[0] + 1];
                else
                    jumpTableStack[++jumpTableStackPos] = scriptEng.operands[0];
                opcodeSize = 0;
                break;
            case FUNC_WGREATEROREQUAL:
                if (scriptEng.operands[1] < scriptEng.operands[2])
                    scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + scriptEng.operands[0] + 1];
                else
                    jumpTableStack[++jumpTableStackPos] = scriptEng.operands[0];
                opcodeSize = 0;
                break;
            case FUNC_WLOWER:
                if (scriptEng.operands[1] >= scriptEng.operands[2])
                    scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + scriptEng.operands[0] + 1];
                else
                    jumpTableStack[++jumpTableStackPos] = scriptEng.operands[0];
                opcodeSize = 0;
                break;
            case FUNC_WLOWEROREQUAL:
                if (scriptEng.operands[1] > scriptEng.operands[2])
                    scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + scriptEng.operands[0] + 1];
                else
                    jumpTableStack[++jumpTableStackPos] = scriptEng.operands[0];
                opcodeSize = 0;
                break;
            case FUNC_WNOTEQUAL:
                if (scriptEng.operands[1] == scriptEng.operands[2])
                    scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + scriptEng.operands[0] + 1];
                else
                    jumpTableStack[++jumpTableStackPos] = scriptEng.operands[0];
                opcodeSize = 0;
                break;
            case FUNC_LOOP:
                opcodeSize    = 0;
                scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + jumpTableStack[jumpTableStackPos--]];
                break;
            case FUNC_SWITCH:
                jumpTableStack[++jumpTableStackPos] = scriptEng.operands[0];
                if (scriptEng.operands[1] < jumpTable[jumpTableStart + scriptEng.operands[0]]
                    || scriptEng.operands[1] > jumpTable[jumpTableStart + scriptEng.operands[0] + 1])
                    scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + scriptEng.operands[0] + 2];
                else
                    scriptCodePtr = scriptCodeStart
                                    + jumpTable[jumpTableStart + scriptEng.operands[0] + 4
                                                    + (scriptEng.operands[1] - jumpTable[jumpTableStart + scriptEng.operands[0]])];
                opcodeSize = 0;
                break;
            case FUNC_BREAK:
                opcodeSize    = 0;
                scriptCodePtr = scriptCodeStart + jumpTable[jumpTableStart + jumpTableStack[jumpTableStackPos--] + 3];
                break;
            case FUNC_ENDSWITCH:
                opcodeSize = 0;
                --jumpTableStackPos;
                break;
            case FUNC_RAND: scriptEng.operands[0] = rand() % scriptEng.operands[1]; break;
            case FUNC_SIN: {
                scriptEng.operands[0] = Sin512(scriptEng.operands[1]);
                break;
            }
            case FUNC_COS: {
                scriptEng.operands[0] = Cos512(scriptEng.operands[1]);
                break;
            }
            case FUNC_SIN256: {
                scriptEng.operands[0] = Sin256(scriptEng.operands[1]);
                break;
            }
            case FUNC_COS256: {
                scriptEng.operands[0] = Cos256(scriptEng.operands[1]);
                break;
            }
            case FUNC_SINCHANGE: {
                scriptEng.operands[0] = scriptEng.operands[3] + (Sin512(scriptEng.operands[1]) >> scriptEng.operands[2]) - scriptEng.operands[4];
                break;
            }
            case FUNC_COSCHANGE: {
                scriptEng.operands[0] = scriptEng.operands[3] + (Cos512(scriptEng.operands[1]) >> scriptEng.operands[2]) - scriptEng.operands[4];
                break;
            }
            case FUNC_ATAN2: {
                scriptEng.operands[0] = ArcTanLookup(scriptEng.operands[1], scriptEng.operands[2]);
                break;
            }
            case FUNC_INTERPOLATE:
                scriptEng.operands[0] =
                    (scriptEng.operands[2] * (0x100 - scriptEng.operands[3]) + scriptEng.operands[3] * scriptEng.operands[1]) >> 8;
                break;
            case FUNC_INTERPOLATEXY:
                scriptEng.operands[0] =
                    (scriptEng.operands[3] * (0x100 - scriptEng.operands[6]) >> 8) + ((scriptEng.operands[6] * scriptEng.operands[2]) >> 8);
                scriptEng.operands[1] =
                    (scriptEng.operands[5] * (0x100 - scriptEng.operands[6]) >> 8) + (scriptEng.operands[6] * scriptEng.operands[4] >> 8);
                break;
            case FUNC_LOADSPRITESHEET:
                opcodeSize                = 0;
                scriptInfo->spriteSheetID = AddGraphicsFile(scriptText);
                break;
            case FUNC_REMOVESPRITESHEET:
                opcodeSize = 0;
                RemoveGraphicsFile(scriptText, -1);
                break;
            case FUNC_DRAWSPRITE:
                opcodeSize  = 0;
                spriteFrame = &scriptFrames[scriptInfo->frameListOffset + scriptEng.operands[0]];
                DrawSprite((entity->XPos >> 16) - xScrollOffset + spriteFrame->pivotX, (entity->YPos >> 16) - yScrollOffset + spriteFrame->pivotY,
                           spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, scriptInfo->spriteSheetID);
                break;
            case FUNC_DRAWSPRITEXY:
                opcodeSize  = 0;
                spriteFrame = &scriptFrames[scriptInfo->frameListOffset + scriptEng.operands[0]];
                DrawSprite((scriptEng.operands[1] >> 16) - xScrollOffset + spriteFrame->pivotX,
                           (scriptEng.operands[2] >> 16) - yScrollOffset + spriteFrame->pivotY, spriteFrame->width, spriteFrame->height,
                           spriteFrame->sprX, spriteFrame->sprY, scriptInfo->spriteSheetID);
                break;
            case FUNC_DRAWSPRITESCREENXY:
                opcodeSize  = 0;
                spriteFrame = &scriptFrames[scriptInfo->frameListOffset + scriptEng.operands[0]];
                DrawSprite(scriptEng.operands[1] + spriteFrame->pivotX, scriptEng.operands[2] + spriteFrame->pivotY, spriteFrame->width,
                           spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, scriptInfo->spriteSheetID);
                break;
            case FUNC_DRAWTINTRECT:
                opcodeSize = 0;
                DrawTintRectangle(scriptEng.operands[0], scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]);
                break;
            case FUNC_DRAWNUMBERS: {
                opcodeSize = 0;
                int i      = 10;
                if (scriptEng.operands[6]) {
                    while (scriptEng.operands[4] > 0) {
                        int frameID = scriptEng.operands[3] % i / (i / 10) + scriptEng.operands[0];
                        spriteFrame = &scriptFrames[scriptInfo->frameListOffset + frameID];
                        DrawSprite(spriteFrame->pivotX + scriptEng.operands[1], spriteFrame->pivotY + scriptEng.operands[2], spriteFrame->width,
                                   spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, scriptInfo->spriteSheetID);
                        scriptEng.operands[1] -= scriptEng.operands[5];
                        i *= 10;
                        --scriptEng.operands[4];
                    }
                }
                else {
                    int extra = 10;
                    if (scriptEng.operands[3])
                        extra = 10 * scriptEng.operands[3];
                    while (scriptEng.operands[4] > 0) {
                        if (extra >= i) {
                            int frameID = scriptEng.operands[3] % i / (i / 10) + scriptEng.operands[0];
                            spriteFrame = &scriptFrames[scriptInfo->frameListOffset + frameID];
                            DrawSprite(spriteFrame->pivotX + scriptEng.operands[1], spriteFrame->pivotY + scriptEng.operands[2], spriteFrame->width,
                                       spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, scriptInfo->spriteSheetID);
                        }
                        scriptEng.operands[1] -= scriptEng.operands[5];
                        i *= 10;
                        --scriptEng.operands[4];
                    }
                }
                break;
            }
            case FUNC_DRAWACTNAME: {
                opcodeSize = 0;
                int charID = 0;

                switch (scriptEng.operands[3]) {
                    default: break;

                    case 1: // Draw Word 1
                        charID = 0;

                        // Draw the first letter as a capital letter, the rest are lowercase (if scriptEng.operands[4] is true, otherwise they're all
                        // uppercase)
                        if (scriptEng.operands[4] == 1 && titleCardText[charID] != 0) {
                            int character = titleCardText[charID];
                            if (character == ' ')
                                character = 0;
                            if (character == '-')
                                character = 0;
                            if (character >= '0' && character <= '9')
                                character -= 22;
                            if (character > '9' && character < 'f')
                                character -= 'A';

                            if (character <= -1) {
                                scriptEng.operands[1] += scriptEng.operands[5] + scriptEng.operands[6]; // spaceWidth + spacing
                            }
                            else {
                                character += scriptEng.operands[0];
                                spriteFrame = &scriptFrames[scriptInfo->frameListOffset + character];
                                DrawSprite(scriptEng.operands[1] + spriteFrame->pivotX, scriptEng.operands[2] + spriteFrame->pivotY,
                                           spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, scriptInfo->spriteSheetID);
                                scriptEng.operands[1] += spriteFrame->width + scriptEng.operands[6];
                            }

                            scriptEng.operands[0] += 26;
                            charID++;
                        }

                        while (titleCardText[charID] != 0 && titleCardText[charID] != '-') {
                            int character = titleCardText[charID];
                            if (character == ' ')
                                character = 0;
                            if (character == '-')
                                character = 0;
                            if (character > '/' && character < ':')
                                character -= 22;
                            if (character > '9' && character < 'f')
                                character -= 'A';

                            if (character <= -1) {
                                scriptEng.operands[1] += scriptEng.operands[5] + scriptEng.operands[6]; // spaceWidth + spacing
                            }
                            else {
                                character += scriptEng.operands[0];
                                spriteFrame = &scriptFrames[scriptInfo->frameListOffset + character];
                                DrawSprite(scriptEng.operands[1] + spriteFrame->pivotX, scriptEng.operands[2] + spriteFrame->pivotY,
                                           spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, scriptInfo->spriteSheetID);
                                scriptEng.operands[1] += spriteFrame->width + scriptEng.operands[6];
                            }
                            charID++;
                        }
                        break;

                    case 2: // Draw Word 2
                        charID = titleCardWord2;

                        // Draw the first letter as a capital letter, the rest are lowercase (if scriptEng.operands[4] is true, otherwise they're all
                        // uppercase)
                        if (scriptEng.operands[4] == 1 && titleCardText[charID] != 0) {
                            int character = titleCardText[charID];
                            if (character == ' ')
                                character = 0;
                            if (character == '-')
                                character = 0;
                            if (character >= '0' && character <= '9')
                                character -= 22;
                            if (character > '9' && character < 'f')
                                character -= 'A';

                            if (character <= -1) {
                                scriptEng.operands[1] += scriptEng.operands[5] + scriptEng.operands[6]; // spaceWidth + spacing
                            }
                            else {
                                character += scriptEng.operands[0];
                                spriteFrame = &scriptFrames[scriptInfo->frameListOffset + character];
                                DrawSprite(scriptEng.operands[1] + spriteFrame->pivotX, scriptEng.operands[2] + spriteFrame->pivotY,
                                           spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, scriptInfo->spriteSheetID);
                                scriptEng.operands[1] += spriteFrame->width + scriptEng.operands[6];
                            }
                            scriptEng.operands[0] += 26;
                            charID++;
                        }

                        while (titleCardText[charID] != 0) {
                            int character = titleCardText[charID];
                            if (character == ' ')
                                character = -1;
                            if (character == '-')
                                character = 0;
                            if (character >= '0' && character <= '9')
                                character -= 22;
                            if (character > '9' && character < 'f')
                                character -= 'A';

                            if (character <= -1) {
                                scriptEng.operands[1] += scriptEng.operands[5] + scriptEng.operands[6]; // spaceWidth + spacing
                            }
                            else {
                                character += scriptEng.operands[0];
                                spriteFrame = &scriptFrames[scriptInfo->frameListOffset + character];
                                DrawSprite(scriptEng.operands[1] + spriteFrame->pivotX, scriptEng.operands[2] + spriteFrame->pivotY,
                                           spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, scriptInfo->spriteSheetID);
                                scriptEng.operands[1] += spriteFrame->width + scriptEng.operands[6];
                            }
                            charID++;
                        }
                        break;
                }
                break;
            }
            case FUNC_DRAWMENU:
                opcodeSize        = 0;
                textMenuSurfaceNo = scriptInfo->spriteSheetID;
                DrawTextMenu(&gameMenu[scriptEng.operands[0]], scriptEng.operands[1], scriptEng.operands[2]);
                break;
            case FUNC_SPRITEFRAME:
                opcodeSize = 0;
                if (scriptSub == SUB_SETUP && scriptFrameCount < SPRITEFRAME_COUNT) {
                    scriptFrames[scriptFrameCount].pivotX = scriptEng.operands[0];
                    scriptFrames[scriptFrameCount].pivotY = scriptEng.operands[1];
                    scriptFrames[scriptFrameCount].width  = scriptEng.operands[2];
                    scriptFrames[scriptFrameCount].height = scriptEng.operands[3];
                    scriptFrames[scriptFrameCount].sprX   = scriptEng.operands[4];
                    scriptFrames[scriptFrameCount].sprY   = scriptEng.operands[5];
                    ++scriptFrameCount;
                }
                break;
            case FUNC_EDITFRAME: {
                if (scriptInfo->mobile) {
                    opcodeSize  = 0;
                    spriteFrame = &scriptFrames[scriptInfo->frameListOffset + scriptEng.operands[0]];

                    spriteFrame->pivotX = scriptEng.operands[1];
                    spriteFrame->pivotY = scriptEng.operands[2];
                    spriteFrame->width  = scriptEng.operands[3];
                    spriteFrame->height = scriptEng.operands[4];
                    spriteFrame->sprX   = scriptEng.operands[5];
                    spriteFrame->sprY   = scriptEng.operands[6];
                }
                else {
                    //"SetEditorIcon"
                }
            } break;
            case FUNC_LOADPALETTE:
                opcodeSize = 0;
                LoadPalette(scriptText, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3], scriptEng.operands[4]);
                break;
            case FUNC_ROTATEPALETTE:
                opcodeSize = 0;
                RotatePalette(scriptEng.operands[0], scriptEng.operands[1], scriptEng.operands[2]);
                break;
            case FUNC_SETSCREENFADE:
                opcodeSize = 0;
                SetFade(scriptEng.operands[0], scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]);
                break;
            case FUNC_SETACTIVEPALETTE:
                opcodeSize = 0;
                SetActivePalette(scriptEng.operands[0], scriptEng.operands[1], scriptEng.operands[2]);
                break;
            case FUNC_SETPALETTEFADE:
                opcodeSize = 0;
                SetLimitedFade(scriptEng.operands[0], scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3], scriptEng.operands[4],
                               scriptEng.operands[5], scriptEng.operands[6]);
                break;
            case FUNC_COPYPALETTE:
                opcodeSize = 0;
                CopyPalette(scriptEng.operands[0], scriptEng.operands[1]);
                break;
            case FUNC_CLEARSCREEN:
                opcodeSize = 0;
                ClearScreen(scriptEng.operands[0]);
                break;
            case FUNC_DRAWSPRITEFX:
                opcodeSize  = 0;
                spriteFrame = &scriptFrames[scriptInfo->frameListOffset + scriptEng.operands[0]];
                switch (scriptEng.operands[1]) {
                    default: break;
                    case FX_SCALE:
                        DrawSpriteScaled(entity->direction, (scriptEng.operands[2] >> 16) - xScrollOffset,
                                         (scriptEng.operands[3] >> 16) - yScrollOffset, -spriteFrame->pivotX, -spriteFrame->pivotY, entity->scale,
                                         entity->scale, spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY,
                                         scriptInfo->spriteSheetID);
                        break;
                    case FX_ROTATE:
                        DrawSpriteRotated(entity->direction, (scriptEng.operands[2] >> 16) - xScrollOffset,
                                          (scriptEng.operands[3] >> 16) - yScrollOffset, -spriteFrame->pivotX, -spriteFrame->pivotY,
                                          spriteFrame->sprX, spriteFrame->sprY, spriteFrame->width, spriteFrame->height, entity->rotation,
                                          scriptInfo->spriteSheetID);
                        break;
                    case FX_ROTOZOOM:
                        DrawSpriteRotozoom(entity->direction, (scriptEng.operands[2] >> 16) - xScrollOffset,
                                           (scriptEng.operands[3] >> 16) - yScrollOffset, -spriteFrame->pivotX, -spriteFrame->pivotY,
                                           spriteFrame->sprX, spriteFrame->sprY, spriteFrame->width, spriteFrame->height, entity->rotation,
                                           entity->scale, scriptInfo->spriteSheetID);
                        break;
                    case FX_INK:
                        switch (entity->inkEffect) {
                            case INK_NONE:
                                DrawSprite((scriptEng.operands[2] >> 16) - xScrollOffset + spriteFrame->pivotX,
                                           (scriptEng.operands[3] >> 16) - yScrollOffset + spriteFrame->pivotY, spriteFrame->width,
                                           spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, scriptInfo->spriteSheetID);
                                break;
                            case INK_BLEND:
                                DrawBlendedSprite((scriptEng.operands[2] >> 16) - xScrollOffset + spriteFrame->pivotX,
                                                  (scriptEng.operands[3] >> 16) - yScrollOffset + spriteFrame->pivotY, spriteFrame->width,
                                                  spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, scriptInfo->spriteSheetID);
                                break;
                            case INK_ALPHA:
                                DrawAlphaBlendedSprite((scriptEng.operands[2] >> 16) - xScrollOffset + spriteFrame->pivotX,
                                                       (scriptEng.operands[3] >> 16) - yScrollOffset + spriteFrame->pivotY, spriteFrame->width,
                                                       spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, entity->alpha,
                                                       scriptInfo->spriteSheetID);
                                break;
                            case INK_ADD:
                                DrawAdditiveBlendedSprite((scriptEng.operands[2] >> 16) - xScrollOffset + spriteFrame->pivotX,
                                                          (scriptEng.operands[3] >> 16) - yScrollOffset + spriteFrame->pivotY, spriteFrame->width,
                                                          spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, entity->alpha,
                                                          scriptInfo->spriteSheetID);
                                break;
                            case INK_SUB:
                                DrawSubtractiveBlendedSprite((scriptEng.operands[2] >> 16) - xScrollOffset + spriteFrame->pivotX,
                                                             (scriptEng.operands[3] >> 16) - yScrollOffset + spriteFrame->pivotY, spriteFrame->width,
                                                             spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, entity->alpha,
                                                             scriptInfo->spriteSheetID);
                                break;
                        }
                        break;
                    case FX_TINT:
                        if (entity->inkEffect == INK_ALPHA) {
                            DrawScaledTintMask(entity->direction, (scriptEng.operands[2] >> 16) - xScrollOffset,
                                               (scriptEng.operands[3] >> 16) - yScrollOffset, -spriteFrame->pivotX, -spriteFrame->pivotY,
                                               entity->scale, entity->scale, spriteFrame->width, spriteFrame->height, spriteFrame->sprX,
                                               spriteFrame->sprY, scriptInfo->spriteSheetID);
                        }
                        else {
                            DrawSpriteScaled(entity->direction, (scriptEng.operands[2] >> 16) - xScrollOffset,
                                             (scriptEng.operands[3] >> 16) - yScrollOffset, -spriteFrame->pivotX, -spriteFrame->pivotY, entity->scale,
                                             entity->scale, spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY,
                                             scriptInfo->spriteSheetID);
                        }
                        break;
                    case FX_FLIP:
                        switch (entity->direction) {
                            default:
                            case FLIP_NONE:
                                DrawSpriteFlipped((scriptEng.operands[2] >> 16) - xScrollOffset + spriteFrame->pivotX,
                                                  (scriptEng.operands[3] >> 16) - yScrollOffset + spriteFrame->pivotY, spriteFrame->width,
                                                  spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, FLIP_NONE, scriptInfo->spriteSheetID);
                                break;
                            case FLIP_X:
                                DrawSpriteFlipped((scriptEng.operands[2] >> 16) - xScrollOffset - spriteFrame->width - spriteFrame->pivotX,
                                                  (scriptEng.operands[3] >> 16) - yScrollOffset + spriteFrame->pivotY, spriteFrame->width,
                                                  spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, FLIP_X, scriptInfo->spriteSheetID);
                                break;
                            case FLIP_Y:
                                DrawSpriteFlipped((scriptEng.operands[2] >> 16) - xScrollOffset + spriteFrame->pivotX,
                                                  (scriptEng.operands[3] >> 16) - yScrollOffset - spriteFrame->height - spriteFrame->pivotY,
                                                  spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, FLIP_Y,
                                                  scriptInfo->spriteSheetID);
                                break;
                            case FLIP_XY:
                                DrawSpriteFlipped((scriptEng.operands[2] >> 16) - xScrollOffset - spriteFrame->width - spriteFrame->pivotX,
                                                  (scriptEng.operands[3] >> 16) - yScrollOffset - spriteFrame->height - spriteFrame->pivotY,
                                                  spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, FLIP_XY,
                                                  scriptInfo->spriteSheetID);
                                break;
                        }
                        break;
                }
                break;
            case FUNC_DRAWSPRITESCREENFX:
                opcodeSize  = 0;
                spriteFrame = &scriptFrames[scriptInfo->frameListOffset + scriptEng.operands[0]];
                switch (scriptEng.operands[1]) {
                    default: break;
                    case FX_SCALE:
                        DrawSpriteScaled(entity->direction, scriptEng.operands[2], scriptEng.operands[3], -spriteFrame->pivotX, -spriteFrame->pivotY,
                                         entity->scale, entity->scale, spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY,
                                         scriptInfo->spriteSheetID);
                        break;
                    case FX_ROTATE:
                        DrawSpriteRotated(entity->direction, scriptEng.operands[2], scriptEng.operands[3], -spriteFrame->pivotX, -spriteFrame->pivotY,
                                          spriteFrame->sprX, spriteFrame->sprY, spriteFrame->width, spriteFrame->height, entity->rotation,
                                          scriptInfo->spriteSheetID);
                        break;
                    case FX_ROTOZOOM:
                        DrawSpriteRotozoom(entity->direction, scriptEng.operands[2], scriptEng.operands[3], -spriteFrame->pivotX,
                                           -spriteFrame->pivotY, spriteFrame->sprX, spriteFrame->sprY, spriteFrame->width, spriteFrame->height,
                                           entity->rotation, entity->scale, scriptInfo->spriteSheetID);
                        break;
                    case FX_INK:
                        switch (entity->inkEffect) {
                            case INK_NONE:
                                DrawSprite(scriptEng.operands[2] + spriteFrame->pivotX, scriptEng.operands[3] + spriteFrame->pivotY,
                                           spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, scriptInfo->spriteSheetID);
                                break;
                            case INK_BLEND:
                                DrawBlendedSprite(scriptEng.operands[2] + spriteFrame->pivotX, scriptEng.operands[3] + spriteFrame->pivotY,
                                                  spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY,
                                                  scriptInfo->spriteSheetID);
                                break;
                            case INK_ALPHA:
                                DrawAlphaBlendedSprite(scriptEng.operands[2] + spriteFrame->pivotX, scriptEng.operands[3] + spriteFrame->pivotY,
                                                       spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, entity->alpha,
                                                       scriptInfo->spriteSheetID);
                                break;
                            case INK_ADD:
                                DrawAdditiveBlendedSprite(scriptEng.operands[2] + spriteFrame->pivotX, scriptEng.operands[3] + spriteFrame->pivotY,
                                                          spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY,
                                                          entity->alpha, scriptInfo->spriteSheetID);
                                break;
                            case INK_SUB:
                                DrawSubtractiveBlendedSprite(scriptEng.operands[2] + spriteFrame->pivotX, scriptEng.operands[3] + spriteFrame->pivotY,
                                                             spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY,
                                                             entity->alpha, scriptInfo->spriteSheetID);
                                break;
                        }
                        break;
                    case FX_TINT:
                        if (entity->inkEffect == INK_ALPHA) {
                            DrawScaledTintMask(entity->direction, scriptEng.operands[2], scriptEng.operands[3], -spriteFrame->pivotX,
                                               -spriteFrame->pivotY, entity->scale, entity->scale, spriteFrame->width, spriteFrame->height,
                                               spriteFrame->sprX, spriteFrame->sprY, scriptInfo->spriteSheetID);
                        }
                        else {
                            DrawSpriteScaled(entity->direction, scriptEng.operands[2], scriptEng.operands[3], -spriteFrame->pivotX,
                                             -spriteFrame->pivotY, entity->scale, entity->scale, spriteFrame->width, spriteFrame->height,
                                             spriteFrame->sprX, spriteFrame->sprY, scriptInfo->spriteSheetID);
                        }
                        break;
                    case FX_FLIP:
                        switch (entity->direction) {
                            default:
                            case FLIP_NONE:
                                DrawSpriteFlipped(scriptEng.operands[2] + spriteFrame->pivotX, scriptEng.operands[3] + spriteFrame->pivotY,
                                                  spriteFrame->width, spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, FLIP_NONE,
                                                  scriptInfo->spriteSheetID);
                                break;
                            case FLIP_X:
                                DrawSpriteFlipped(scriptEng.operands[2] - spriteFrame->width - spriteFrame->pivotX,
                                                  scriptEng.operands[3] + spriteFrame->pivotY, spriteFrame->width, spriteFrame->height,
                                                  spriteFrame->sprX, spriteFrame->sprY, FLIP_X, scriptInfo->spriteSheetID);
                                break;
                            case FLIP_Y:
                                DrawSpriteFlipped(scriptEng.operands[2] + spriteFrame->pivotX,
                                                  scriptEng.operands[3] - spriteFrame->height - spriteFrame->pivotY, spriteFrame->width,
                                                  spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, FLIP_Y, scriptInfo->spriteSheetID);
                                break;
                            case FLIP_XY:
                                DrawSpriteFlipped(scriptEng.operands[2] - spriteFrame->width - spriteFrame->pivotX,
                                                  scriptEng.operands[3] - spriteFrame->height - spriteFrame->pivotY, spriteFrame->width,
                                                  spriteFrame->height, spriteFrame->sprX, spriteFrame->sprY, FLIP_XY, scriptInfo->spriteSheetID);
                                break;
                        }
                        break;
                }
                break;
            case FUNC_LOADANIMATION:
                opcodeSize           = 0;
                scriptInfo->animFile = AddAnimationFile(scriptText);
                break;
            case FUNC_SETUPMENU: {
                opcodeSize     = 0;
                TextMenu *menu = &gameMenu[scriptEng.operands[0]];
                SetupTextMenu(menu, scriptEng.operands[1]);
                menu->selectionCount = scriptEng.operands[2];
                menu->alignment      = scriptEng.operands[3];
                break;
            }
            case FUNC_ADDMENUENTRY: {
                opcodeSize                           = 0;
                TextMenu *menu                       = &gameMenu[scriptEng.operands[0]];
                menu->entryHighlight[menu->rowCount] = scriptEng.operands[2];
                AddTextMenuEntry(menu, scriptText);
                break;
            }
            case FUNC_EDITMENUENTRY: {
                opcodeSize     = 0;
                TextMenu *menu = &gameMenu[scriptEng.operands[0]];
                EditTextMenuEntry(menu, scriptText, scriptEng.operands[2]);
                menu->entryHighlight[scriptEng.operands[2]] = scriptEng.operands[3];
                break;
            }
            case FUNC_LOADSTAGE:
                opcodeSize = 0;
                stageMode  = STAGEMODE_LOAD;
                break;
            case FUNC_DRAWRECT:
                opcodeSize = 0;
                DrawRectangle(scriptEng.operands[0], scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3], scriptEng.operands[4],
                              scriptEng.operands[5], scriptEng.operands[6], scriptEng.operands[7]);
                break;
            case FUNC_RESETOBJECTENTITY: {
                opcodeSize            = 0;
                Entity *newEnt        = &objectEntityList[scriptEng.operands[0]];
                newEnt->type          = scriptEng.operands[1];
                newEnt->propertyValue = scriptEng.operands[2];
                newEnt->XPos          = scriptEng.operands[3];
                newEnt->YPos          = scriptEng.operands[4];
                newEnt->direction     = FLIP_NONE;
                newEnt->frame         = 0;
                newEnt->priority      = PRIORITY_BOUNDS;
                newEnt->rotation      = 0;
                newEnt->state         = 0;
                newEnt->drawOrder     = 3;
                newEnt->scale         = 0x200;
                newEnt->inkEffect     = INK_NONE;
                newEnt->values[0]     = 0;
                newEnt->values[1]     = 0;
                newEnt->values[2]     = 0;
                newEnt->values[3]     = 0;
                newEnt->values[4]     = 0;
                newEnt->values[5]     = 0;
                newEnt->values[6]     = 0;
                newEnt->values[7]     = 0;
                break;
            }
            case FUNC_PLAYEROBJECTCOLLISION:
                opcodeSize              = 0;
                switch (scriptEng.operands[0]) {
                    default: break;
                    case C_TOUCH:
                        scriptEng.operands[5] = entity->XPos >> 16;
                        scriptEng.operands[6] = entity->YPos >> 16;
                        TouchCollision(scriptEng.operands[5] + scriptEng.operands[1], scriptEng.operands[6] + scriptEng.operands[2],
                                       scriptEng.operands[5] + scriptEng.operands[3], scriptEng.operands[6] + scriptEng.operands[4]);
                        break;
                    case C_BOX:
                        BoxCollision(entity->XPos + (scriptEng.operands[1] << 16), entity->YPos + (scriptEng.operands[2] << 16),
                                     entity->XPos + (scriptEng.operands[3] << 16), entity->YPos + (scriptEng.operands[4] << 16));
                        break;
                    case C_BOX2:
                        // C_PLATFORM if pc ver, as there's no box2 there
                        if (!scriptInfo->mobile) {
                            PlatformCollision(entity->XPos + (scriptEng.operands[1] << 16), entity->YPos + (scriptEng.operands[2] << 16),
                                              entity->XPos + (scriptEng.operands[3] << 16), entity->YPos + (scriptEng.operands[4] << 16));
                        }
                        else {
                            BoxCollision2(entity->XPos + (scriptEng.operands[1] << 16), entity->YPos + (scriptEng.operands[2] << 16),
                                          entity->XPos + (scriptEng.operands[3] << 16), entity->YPos + (scriptEng.operands[4] << 16));
                        }
                        break;
                    case C_PLATFORM:
                        if (scriptInfo->mobile) {
                            PlatformCollision(entity->XPos + (scriptEng.operands[1] << 16), entity->YPos + (scriptEng.operands[2] << 16),
                                              entity->XPos + (scriptEng.operands[3] << 16), entity->YPos + (scriptEng.operands[4] << 16));
                        }
                        break;
                    case C_BOX3:
                        BoxCollision3(entity->XPos + (scriptEng.operands[1] << 16), entity->YPos + (scriptEng.operands[2] << 16),
                                      entity->XPos + (scriptEng.operands[3] << 16), entity->YPos + (scriptEng.operands[4] << 16));
                        break;
                    case C_ENEMY:
                        scriptEng.operands[5] = entity->XPos >> 16;
                        scriptEng.operands[6] = entity->YPos >> 16;
                        EnemyCollision(scriptEng.operands[5] + scriptEng.operands[1], scriptEng.operands[6] + scriptEng.operands[2],
                                      scriptEng.operands[5] + scriptEng.operands[3], scriptEng.operands[6] + scriptEng.operands[4]);
                        break;
                }
                break;
            case FUNC_CREATETEMPOBJECT: {
                opcodeSize = 0;
                if (objectEntityList[scriptEng.arrayPosition[2]].type > 0 && ++scriptEng.arrayPosition[2] == ENTITY_COUNT)
                    scriptEng.arrayPosition[2] = TEMPENTITY_START;
                Entity *temp         = &objectEntityList[scriptEng.arrayPosition[2]];
                temp->type           = scriptEng.operands[0];
                temp->propertyValue  = scriptEng.operands[1];
                temp->XPos           = scriptEng.operands[2];
                temp->YPos           = scriptEng.operands[3];
                temp->direction      = FLIP_NONE;
                temp->frame          = 0;
                temp->priority       = PRIORITY_ACTIVE;
                temp->rotation       = 0;
                temp->state          = 0;
                temp->drawOrder      = 3;
                temp->scale          = 512;
                temp->inkEffect      = INK_NONE;
                temp->alpha          = 0;
                temp->animation      = 0;
                temp->prevAnimation  = 0;
                temp->animationSpeed = 0;
                temp->animationTimer = 0;
                temp->values[0]      = 0;
                temp->values[1]      = 0;
                temp->values[2]      = 0;
                temp->values[3]      = 0;
                temp->values[4]      = 0;
                temp->values[5]      = 0;
                temp->values[6]      = 0;
                temp->values[7]      = 0;
                break;
            }
            case FUNC_BINDPLAYERTOOBJECT: {
                opcodeSize   = 0;
                Entity *pEnt = &objectEntityList[scriptEng.operands[1]];

                playerList[scriptEng.operands[0]].animationFile = scriptInfo->animFile;
                playerList[scriptEng.operands[0]].boundEntity   = pEnt;
                playerList[scriptEng.operands[0]].entityNo      = scriptEng.operands[1];
                break;
            }
            case FUNC_PLAYERTILECOLLISION:
                opcodeSize = 0;
                if (player->tileCollisions) {
                    ProcessPlayerTileCollisions(player);
                }
                else {
                    player->XPos += player->XVelocity;
                    player->YPos += player->YVelocity;
                }
                break;
            case FUNC_PROCESSPLAYERCONTROL:
                opcodeSize = 0;
                ProcessPlayerControl(player);
                break;
            case FUNC_PROCESSANIMATION:
                ProcessObjectAnimation(scriptInfo, entity);
                opcodeSize = 0;
                break;
            case FUNC_DRAWOBJECTANIMATION:
                opcodeSize = 0;
                DrawObjectAnimation(scriptInfo, entity, (entity->XPos >> 16) - xScrollOffset, (entity->YPos >> 16) - yScrollOffset);
                break;
            case FUNC_DRAWPLAYERANIMATION:
                opcodeSize = 0;
                if (player->visible) {
                    if (cameraEnabled == activePlayer)
                        DrawObjectAnimation(scriptInfo, entity, player->screenXPos, player->screenYPos);
                    else
                        DrawObjectAnimation(scriptInfo, entity, (player->XPos >> 16) - xScrollOffset, (player->YPos >> 16) - yScrollOffset);
                }
                break;
            case FUNC_SETMUSICTRACK:
                opcodeSize = 0;
                if (scriptEng.operands[2] <= 1)
                    SetMusicTrack(scriptText, scriptEng.operands[1], scriptEng.operands[2], 0);
                else
                    SetMusicTrack(scriptText, scriptEng.operands[1], true, scriptEng.operands[2]);
                break;
            case FUNC_PLAYMUSIC:
                opcodeSize = 0;
                PlayMusic(scriptEng.operands[0]);
                break;
            case FUNC_STOPMUSIC:
                opcodeSize = 0;
                StopMusic();
                break;
            case FUNC_PLAYSFX:
                opcodeSize = 0;
                PlaySfx(scriptEng.operands[0], scriptEng.operands[1]);
                break;
            case FUNC_STOPSFX:
                opcodeSize = 0;
                StopSfx(scriptEng.operands[0]);
                break;
            case FUNC_SETSFXATTRIBUTES:
                opcodeSize = 0;
                SetSfxAttributes(scriptEng.operands[0], scriptEng.operands[1], scriptEng.operands[2]);
                break;
            case FUNC_OBJECTTILECOLLISION:
                opcodeSize = 0;
                switch (scriptEng.operands[0]) {
                    default: break;
                    case CSIDE_FLOOR: ObjectFloorCollision(scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                    case CSIDE_LWALL: ObjectLWallCollision(scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                    case CSIDE_RWALL: ObjectRWallCollision(scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                    case CSIDE_ROOF: ObjectRoofCollision(scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                }
                break;
            case FUNC_OBJECTTILEGRIP:
                opcodeSize = 0;
                switch (scriptEng.operands[0]) {
                    default: break;
                    case CSIDE_FLOOR: ObjectFloorGrip(scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                    case CSIDE_LWALL: ObjectLWallGrip(scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                    case CSIDE_RWALL: ObjectRWallGrip(scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                    case CSIDE_ROOF: ObjectRoofGrip(scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                    case CSIDE_ENTITY: ObjectEntityGrip(scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                }
                break;
            case FUNC_LOADVIDEO:
                opcodeSize = 0;
                PauseSound();
                if (FindStringToken(scriptText, ".rsv", 1) <= -1)
                    PlayVideoFile(scriptText); // not an rsv
                else
                    scriptInfo->spriteSheetID = AddGraphicsFile(scriptText);
                ResumeSound();
                break;
            case FUNC_NEXTVIDEOFRAME:
                opcodeSize = 0;
                UpdateVideoFrame();
                break;
            case FUNC_PLAYSTAGESFX:
                opcodeSize = 0;
                PlaySfx(globalSFXCount + scriptEng.operands[0], scriptEng.operands[1]);
                break;
            case FUNC_STOPSTAGESFX:
                opcodeSize = 0;
                StopSfx(globalSFXCount + scriptEng.operands[0]);
                break;
            case FUNC_NOT: scriptEng.operands[0] = ~scriptEng.operands[0]; break;
            case FUNC_DRAW3DSCENE:
                opcodeSize = 0;
                TransformVertexBuffer();
                Sort3DDrawList();
                Draw3DScene(scriptInfo->spriteSheetID);
                break;
            case FUNC_SETIDENTITYMATRIX:
                opcodeSize = 0;
                switch (scriptEng.operands[0]) {
                    case MAT_WORLD: SetIdentityMatrix(&matWorld); break;
                    case MAT_VIEW: SetIdentityMatrix(&matView); break;
                    case MAT_TEMP: SetIdentityMatrix(&matTemp); break;
                }
                break;
            case FUNC_MATRIXMULTIPLY:
                opcodeSize = 0;
                switch (scriptEng.operands[0]) {
                    case MAT_WORLD:
                        switch (scriptEng.operands[1]) {
                            case MAT_WORLD: MatrixMultiply(&matWorld, &matWorld); break;
                            case MAT_VIEW: MatrixMultiply(&matWorld, &matView); break;
                            case MAT_TEMP: MatrixMultiply(&matWorld, &matTemp); break;
                        }
                        break;
                    case MAT_VIEW:
                        switch (scriptEng.operands[1]) {
                            case MAT_WORLD: MatrixMultiply(&matView, &matWorld); break;
                            case MAT_VIEW: MatrixMultiply(&matView, &matView); break;
                            case MAT_TEMP: MatrixMultiply(&matView, &matTemp); break;
                        }
                        break;
                    case MAT_TEMP:
                        switch (scriptEng.operands[1]) {
                            case MAT_WORLD: MatrixMultiply(&matTemp, &matWorld); break;
                            case MAT_VIEW: MatrixMultiply(&matTemp, &matView); break;
                            case MAT_TEMP: MatrixMultiply(&matTemp, &matTemp); break;
                        }
                        break;
                }
                break;
            case FUNC_MATRIXTRANSLATEXYZ:
                opcodeSize = 0;
                switch (scriptEng.operands[0]) {
                    case MAT_WORLD: MatrixTranslateXYZ(&matWorld, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                    case MAT_VIEW: MatrixTranslateXYZ(&matView, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                    case MAT_TEMP: MatrixTranslateXYZ(&matTemp, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                }
                break;
            case FUNC_MATRIXSCALEXYZ:
                opcodeSize = 0;
                switch (scriptEng.operands[0]) {
                    case MAT_WORLD: MatrixScaleXYZ(&matWorld, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                    case MAT_VIEW: MatrixScaleXYZ(&matView, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                    case MAT_TEMP: MatrixScaleXYZ(&matTemp, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                }
                break;
            case FUNC_MATRIXROTATEX:
                opcodeSize = 0;
                switch (scriptEng.operands[0]) {
                    case MAT_WORLD: MatrixRotateX(&matWorld, scriptEng.operands[1]); break;
                    case MAT_VIEW: MatrixRotateX(&matView, scriptEng.operands[1]); break;
                    case MAT_TEMP: MatrixRotateX(&matTemp, scriptEng.operands[1]); break;
                }
                break;
            case FUNC_MATRIXROTATEY:
                opcodeSize = 0;
                switch (scriptEng.operands[0]) {
                    case MAT_WORLD: MatrixRotateY(&matWorld, scriptEng.operands[1]); break;
                    case MAT_VIEW: MatrixRotateY(&matView, scriptEng.operands[1]); break;
                    case MAT_TEMP: MatrixRotateY(&matTemp, scriptEng.operands[1]); break;
                }
                break;
            case FUNC_MATRIXROTATEZ:
                opcodeSize = 0;
                switch (scriptEng.operands[0]) {
                    case MAT_WORLD: MatrixRotateZ(&matWorld, scriptEng.operands[1]); break;
                    case MAT_VIEW: MatrixRotateZ(&matView, scriptEng.operands[1]); break;
                    case MAT_TEMP: MatrixRotateZ(&matTemp, scriptEng.operands[1]); break;
                }
                break;
            case FUNC_MATRIXROTATEXYZ:
                opcodeSize = 0;
                switch (scriptEng.operands[0]) {
                    case MAT_WORLD: MatrixRotateXYZ(&matWorld, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                    case MAT_VIEW: MatrixRotateXYZ(&matView, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                    case MAT_TEMP: MatrixRotateXYZ(&matTemp, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]); break;
                }
                break;
            case FUNC_TRANSFORMVERTICES:
                opcodeSize = 0;
                switch (scriptEng.operands[0]) {
                    case MAT_WORLD: TransformVerticies(&matWorld, scriptEng.operands[1], scriptEng.operands[2]); break;
                    case MAT_VIEW: TransformVerticies(&matView, scriptEng.operands[1], scriptEng.operands[2]); break;
                    case MAT_TEMP: TransformVerticies(&matTemp, scriptEng.operands[1], scriptEng.operands[2]); break;
                }
                break;
            case FUNC_CALLFUNCTION: {
                opcodeSize                        = 0;
                functionStack[functionStackPos++] = scriptCodePtr;
                functionStack[functionStackPos++] = jumpTableStart;
                functionStack[functionStackPos++] = scriptCodeStart;
                scriptCodeStart                   = scriptFunctionList[scriptEng.operands[0]].ptr.scriptCodePtr;
                jumpTableStart                    = scriptFunctionList[scriptEng.operands[0]].ptr.jumpTablePtr;
                scriptCodePtr                     = scriptCodeStart;
            } break;
            case FUNC_ENDFUNCTION:
                opcodeSize      = 0;
                scriptCodeStart = functionStack[--functionStackPos];
                jumpTableStart  = functionStack[--functionStackPos];
                scriptCodePtr   = functionStack[--functionStackPos];
                break;
            case FUNC_SETLAYERDEFORMATION:
                opcodeSize = 0;
                SetLayerDeformation(scriptEng.operands[0], scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3], scriptEng.operands[4],
                                    scriptEng.operands[5]);
                break;
            case FUNC_CHECKTOUCHRECT: opcodeSize = 0; scriptEng.checkResult = -1;
#if !RETRO_USE_ORIGINAL_CODE
                AddDebugHitbox(H_TYPE_FINGER, NULL, scriptEng.operands[0], scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]);
#endif
                for (int f = 0; f < touches; ++f) {
                    if (touchDown[f] && touchX[f] > scriptEng.operands[0] && touchX[f] < scriptEng.operands[2] && touchY[f] > scriptEng.operands[1]
                        && touchY[f] < scriptEng.operands[3]) {
                        scriptEng.checkResult = f;
                    }
                }
                break;
            case FUNC_GETTILELAYERENTRY:
                scriptEng.operands[0] = stageLayouts[scriptEng.operands[1]].tiles[scriptEng.operands[2] + 0x100 * scriptEng.operands[3]];
                break;
            case FUNC_SETTILELAYERENTRY:
                stageLayouts[scriptEng.operands[1]].tiles[scriptEng.operands[2] + 0x100 * scriptEng.operands[3]] = scriptEng.operands[0];
                break;
            case FUNC_GETBIT: scriptEng.operands[0] = (scriptEng.operands[1] & (1 << scriptEng.operands[2])) >> scriptEng.operands[2]; break;
            case FUNC_SETBIT:
                if (scriptEng.operands[2] <= 0)
                    scriptEng.operands[0] &= ~(1 << scriptEng.operands[1]);
                else
                    scriptEng.operands[0] |= 1 << scriptEng.operands[1];
                break;
            case FUNC_PAUSEMUSIC:
                opcodeSize = 0;
                PauseSound();
                break;
            case FUNC_RESUMEMUSIC:
                opcodeSize = 0;
                ResumeSound();
                break;
            case FUNC_CLEARDRAWLIST:
                opcodeSize                                      = 0;
                drawListEntries[scriptEng.operands[0]].listSize = 0;
                break;
            case FUNC_ADDDRAWLISTENTITYREF: {
                opcodeSize                                                                                           = 0;
                drawListEntries[scriptEng.operands[0]].entityRefs[drawListEntries[scriptEng.operands[0]].listSize++] = scriptEng.operands[1];
                break;
            }
            case FUNC_GETDRAWLISTENTITYREF: scriptEng.operands[0] = drawListEntries[scriptEng.operands[1]].entityRefs[scriptEng.operands[2]]; break;
            case FUNC_SETDRAWLISTENTITYREF:
                opcodeSize                                                               = 0;
                drawListEntries[scriptEng.operands[1]].entityRefs[scriptEng.operands[2]] = scriptEng.operands[0];
                break;
            case FUNC_GET16X16TILEINFO: {
                scriptEng.operands[4] = scriptEng.operands[1] >> 7;
                scriptEng.operands[5] = scriptEng.operands[2] >> 7;
                scriptEng.operands[6] = stageLayouts[0].tiles[scriptEng.operands[4] + (scriptEng.operands[5] << 8)] << 6;
                scriptEng.operands[6] += ((scriptEng.operands[1] & 0x7F) >> 4) + 8 * ((scriptEng.operands[2] & 0x7F) >> 4);
                int index = tiles128x128.tileIndex[scriptEng.operands[6]];
                switch (scriptEng.operands[3]) {
                    case TILEINFO_INDEX: scriptEng.operands[0] = tiles128x128.tileIndex[scriptEng.operands[6]]; break;
                    case TILEINFO_DIRECTION: scriptEng.operands[0] = tiles128x128.direction[scriptEng.operands[6]]; break;
                    case TILEINFO_VISUALPLANE: scriptEng.operands[0] = tiles128x128.visualPlane[scriptEng.operands[6]]; break;
                    case TILEINFO_SOLIDITYA: scriptEng.operands[0] = tiles128x128.collisionFlags[0][scriptEng.operands[6]]; break;
                    case TILEINFO_SOLIDITYB: scriptEng.operands[0] = tiles128x128.collisionFlags[1][scriptEng.operands[6]]; break;
                    case TILEINFO_FLAGSA: scriptEng.operands[0] = collisionMasks[0].flags[index]; break;
                    case TILEINFO_ANGLEA: scriptEng.operands[0] = collisionMasks[0].angles[index]; break;
                    case TILEINFO_FLAGSB: scriptEng.operands[0] = collisionMasks[1].flags[index]; break;
                    case TILEINFO_ANGLEB: scriptEng.operands[0] = collisionMasks[1].angles[index]; break;
                    default: break;
                }
                break;
            }
            case FUNC_COPY16X16TILE:
                opcodeSize = 0;
                Copy16x16Tile(scriptEng.operands[0], scriptEng.operands[1]);
                break;
            case FUNC_SET16X16TILEINFO: {
                scriptEng.operands[4] = scriptEng.operands[1] >> 7;
                scriptEng.operands[5] = scriptEng.operands[2] >> 7;
                scriptEng.operands[6] = stageLayouts[0].tiles[scriptEng.operands[4] + (scriptEng.operands[5] << 8)] << 6;
                scriptEng.operands[6] += ((scriptEng.operands[1] & 0x7F) >> 4) + 8 * ((scriptEng.operands[2] & 0x7F) >> 4);
                switch (scriptEng.operands[3]) {
                    case TILEINFO_INDEX:
                        tiles128x128.tileIndex[scriptEng.operands[6]]  = scriptEng.operands[0];
                        if (renderType == RENDER_SW)
                            tiles128x128.gfxDataPos[scriptEng.operands[6]] = scriptEng.operands[0] << 8;
                        else if (renderType == RENDER_HW)
                            tiles128x128.gfxDataPos[scriptEng.operands[6]] = (scriptEng.operands[0] << 2) & 0x3FFFC;
                        break;
                    case TILEINFO_DIRECTION: tiles128x128.direction[scriptEng.operands[6]] = scriptEng.operands[0]; break;
                    case TILEINFO_VISUALPLANE: tiles128x128.visualPlane[scriptEng.operands[6]] = scriptEng.operands[0]; break;
                    case TILEINFO_SOLIDITYA: tiles128x128.collisionFlags[0][scriptEng.operands[6]] = scriptEng.operands[0]; break;
                    case TILEINFO_SOLIDITYB: tiles128x128.collisionFlags[1][scriptEng.operands[6]] = scriptEng.operands[0]; break;
                    case TILEINFO_FLAGSA: collisionMasks[1].flags[tiles128x128.tileIndex[scriptEng.operands[6]]] = scriptEng.operands[0]; break;
                    case TILEINFO_ANGLEA: collisionMasks[1].angles[tiles128x128.tileIndex[scriptEng.operands[6]]] = scriptEng.operands[0]; break;
                    default: break;
                }
                break;
            }
            case FUNC_GETANIMATIONBYNAME: {
                AnimationFile *animFile = scriptInfo->animFile;
                scriptEng.operands[0]   = -1;
                int id                  = 0;
                while (scriptEng.operands[0] == -1) {
                    SpriteAnimation *anim = &animationList[animFile->aniListOffset + id];
                    if (StrComp(scriptText, anim->name))
                        scriptEng.operands[0] = id;
                    else if (++id == animFile->animCount)
                        scriptEng.operands[0] = 0;
                }
                break;
            }
            case FUNC_READSAVERAM:
                opcodeSize            = 0;
                scriptEng.checkResult = ReadSaveRAMData();
                break;
            case FUNC_WRITESAVERAM:
                opcodeSize            = 0;
                scriptEng.checkResult = WriteSaveRAMData();
                break;
            case FUNC_LOADTEXTFONT:
                opcodeSize = 0;
                LoadFontFile(scriptText);
                break;
            case FUNC_LOADTEXTFILE: {
                opcodeSize     = 0;
                TextMenu *menu = &gameMenu[scriptEng.operands[0]];
                LoadTextFile(menu, scriptText, scriptEng.operands[2] != 0);
                break;
            }
            case FUNC_DRAWTEXT: {
                opcodeSize        = 0;
                textMenuSurfaceNo = scriptInfo->spriteSheetID;
                TextMenu *menu    = &gameMenu[scriptEng.operands[0]];
                DrawBitmapText(menu, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3], scriptEng.operands[4],
                               scriptEng.operands[5], scriptEng.operands[6]);
                break;
            }
            case FUNC_GETTEXTINFO: {
                TextMenu *menu = &gameMenu[scriptEng.operands[1]];
                switch (scriptEng.operands[2]) {
                    case TEXTINFO_TEXTDATA:
                        scriptEng.operands[0] = menu->textData[menu->entryStart[scriptEng.operands[3]] + scriptEng.operands[4]];
                        break;
                    case TEXTINFO_TEXTSIZE: scriptEng.operands[0] = menu->entrySize[scriptEng.operands[3]]; break;
                    case TEXTINFO_ROWCOUNT: scriptEng.operands[0] = menu->rowCount; break;
                }
                break;
            }
            case FUNC_GETVERSIONNUMBER: {
                opcodeSize                           = 0;
                TextMenu *menu                       = &gameMenu[scriptEng.operands[0]];
                menu->entryHighlight[menu->rowCount] = scriptEng.operands[1];
                AddTextMenuEntry(menu, Engine.gameVersion);
                break;
            }
            case FUNC_SETACHIEVEMENT:
                opcodeSize = 0;
                SetAchievement(scriptEng.operands[0], scriptEng.operands[1]);
                break;
            case FUNC_SETLEADERBOARD:
                opcodeSize = 0;
                SetLeaderboard(scriptEng.operands[0], scriptEng.operands[1]);
                break;
            case FUNC_LOADONLINEMENU:
                opcodeSize = 0;
                switch (scriptEng.operands[0]) {
                    default: break;
                    case ONLINEMENU_ACHIEVEMENTS: LoadAchievementsMenu(); break;
                    case ONLINEMENU_LEADERBOARDS: LoadLeaderboardsMenu(); break;
                }
                break;
            case FUNC_ENGINECALLBACK:
                opcodeSize = 0;
                Engine.Callback(scriptEng.operands[0]);
                break;
#if RETRO_USE_HAPTICS
            case FUNC_HAPTICEFFECT:
                opcodeSize = 0;
                // params: scriptEng.operands[0], scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]
                if (scriptEng.operands[0] != -1)
                    QueueHapticEffect(scriptEng.operands[0]);
                else
                    PlayHaptics(scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]);
                break;
#endif
        }

        // Set Values
        if (opcodeSize > 0)
            scriptCodePtr -= scriptCodePtr - scriptCodeOffset;
        for (int i = 0; i < opcodeSize; ++i) {
            int opcodeType = scriptCode[scriptCodePtr++];
            if (opcodeType == SCRIPTVAR_VAR) {
                int arrayVal = 0;
                switch (scriptCode[scriptCodePtr++]) { // variable
                    case VARARR_NONE: arrayVal = objectLoop; break;
                    case VARARR_ARRAY:
                        if (scriptCode[scriptCodePtr++] == 1)
                            arrayVal = scriptEng.arrayPosition[scriptCode[scriptCodePtr++]];
                        else
                            arrayVal = scriptCode[scriptCodePtr++];
                        break;
                    case VARARR_ENTNOPLUS1:
                        if (scriptCode[scriptCodePtr++] == 1)
                            arrayVal = scriptEng.arrayPosition[scriptCode[scriptCodePtr++]] + objectLoop;
                        else
                            arrayVal = scriptCode[scriptCodePtr++] + objectLoop;
                        break;
                    case VARARR_ENTNOMINUS1:
                        if (scriptCode[scriptCodePtr++] == 1)
                            arrayVal = objectLoop - scriptEng.arrayPosition[scriptCode[scriptCodePtr++]];
                        else
                            arrayVal = objectLoop - scriptCode[scriptCodePtr++];
                        break;
                    default: break;
                }

                // Variables
                switch (scriptCode[scriptCodePtr++]) {
                    default: break;
                    case VAR_TEMPVALUE0: scriptEng.tempValue[0] = scriptEng.operands[i]; break;
                    case VAR_TEMPVALUE1: scriptEng.tempValue[1] = scriptEng.operands[i]; break;
                    case VAR_TEMPVALUE2: scriptEng.tempValue[2] = scriptEng.operands[i]; break;
                    case VAR_TEMPVALUE3: scriptEng.tempValue[3] = scriptEng.operands[i]; break;
                    case VAR_TEMPVALUE4: scriptEng.tempValue[4] = scriptEng.operands[i]; break;
                    case VAR_TEMPVALUE5: scriptEng.tempValue[5] = scriptEng.operands[i]; break;
                    case VAR_TEMPVALUE6: scriptEng.tempValue[6] = scriptEng.operands[i]; break;
                    case VAR_TEMPVALUE7: scriptEng.tempValue[7] = scriptEng.operands[i]; break;
                    case VAR_CHECKRESULT: scriptEng.checkResult = scriptEng.operands[i]; break;
                    case VAR_ARRAYPOS0: scriptEng.arrayPosition[0] = scriptEng.operands[i]; break;
                    case VAR_ARRAYPOS1: scriptEng.arrayPosition[1] = scriptEng.operands[i]; break;
                    case VAR_GLOBAL: globalVariables[arrayVal] = scriptEng.operands[i]; break;
                    case VAR_OBJECTENTITYNO: break;
                    case VAR_OBJECTTYPE: {
                        objectEntityList[arrayVal].type = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTPROPERTYVALUE: {
                        objectEntityList[arrayVal].propertyValue = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTXPOS: {
                        objectEntityList[arrayVal].XPos = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTYPOS: {
                        objectEntityList[arrayVal].YPos = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTIXPOS: {
                        objectEntityList[arrayVal].XPos = scriptEng.operands[i] << 16;
                        break;
                    }
                    case VAR_OBJECTIYPOS: {
                        objectEntityList[arrayVal].YPos = scriptEng.operands[i] << 16;
                        break;
                    }
                    case VAR_OBJECTSTATE: {
                        objectEntityList[arrayVal].state = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTROTATION: {
                        objectEntityList[arrayVal].rotation = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTSCALE: {
                        objectEntityList[arrayVal].scale = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTPRIORITY: {
                        objectEntityList[arrayVal].priority = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTDRAWORDER: {
                        objectEntityList[arrayVal].drawOrder = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTDIRECTION: {
                        objectEntityList[arrayVal].direction = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTINKEFFECT: {
                        objectEntityList[arrayVal].inkEffect = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTALPHA: {
                        objectEntityList[arrayVal].alpha = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTFRAME: {
                        objectEntityList[arrayVal].frame = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTANIMATION: {
                        objectEntityList[arrayVal].animation = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTPREVANIMATION: {
                        objectEntityList[arrayVal].prevAnimation = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTANIMATIONSPEED: {
                        objectEntityList[arrayVal].animationSpeed = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTANIMATIONTIMER: {
                        objectEntityList[arrayVal].animationTimer = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE0: {
                        objectEntityList[arrayVal].values[0] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE1: {
                        objectEntityList[arrayVal].values[1] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE2: {
                        objectEntityList[arrayVal].values[2] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE3: {
                        objectEntityList[arrayVal].values[3] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE4: {
                        objectEntityList[arrayVal].values[4] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE5: {
                        objectEntityList[arrayVal].values[5] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE6: {
                        objectEntityList[arrayVal].values[6] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTVALUE7: {
                        objectEntityList[arrayVal].values[7] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_OBJECTOUTOFBOUNDS: break;
                    case VAR_PLAYERSTATE: {
                        playerList[activePlayer].boundEntity->state = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERCONTROLMODE: {
                        playerList[activePlayer].controlMode = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERCONTROLLOCK: {
                        playerList[activePlayer].controlLock = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERCOLLISIONMODE: {
                        playerList[activePlayer].collisionMode = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERCOLLISIONPLANE: {
                        playerList[activePlayer].collisionPlane = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERXPOS: {
                        playerList[activePlayer].XPos = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERYPOS: {
                        playerList[activePlayer].YPos = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERIXPOS: {
                        playerList[activePlayer].XPos = scriptEng.operands[i] << 16;
                        break;
                    }
                    case VAR_PLAYERIYPOS: {
                        playerList[activePlayer].YPos = scriptEng.operands[i] << 16;
                        break;
                    }
                    case VAR_PLAYERSCREENXPOS: {
                        playerList[activePlayer].screenXPos = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERSCREENYPOS: {
                        playerList[activePlayer].screenYPos = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERSPEED: {
                        playerList[activePlayer].speed = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERXVELOCITY: {
                        playerList[activePlayer].XVelocity = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERYVELOCITY: {
                        playerList[activePlayer].YVelocity = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERGRAVITY: {
                        playerList[activePlayer].gravity = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERANGLE: {
                        playerList[activePlayer].angle = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERSKIDDING: {
                        playerList[activePlayer].skidding = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERPUSHING: {
                        playerList[activePlayer].pushing = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERTRACKSCROLL: {
                        playerList[activePlayer].trackScroll = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERUP: {
                        playerList[activePlayer].up = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERDOWN: {
                        playerList[activePlayer].down = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERLEFT: {
                        playerList[activePlayer].left = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERRIGHT: {
                        playerList[activePlayer].right = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERJUMPPRESS: {
                        playerList[activePlayer].jumpPress = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERJUMPHOLD: {
                        playerList[activePlayer].jumpHold = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERFOLLOWPLAYER1: {
                        playerList[activePlayer].followPlayer1 = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERLOOKPOS: {
                        playerList[activePlayer].lookPos = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERWATER: {
                        playerList[activePlayer].water = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERTOPSPEED: {
                        playerList[activePlayer].topSpeed = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERACCELERATION: {
                        playerList[activePlayer].acceleration = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERDECELERATION: {
                        playerList[activePlayer].deceleration = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERAIRACCELERATION: {
                        playerList[activePlayer].airAcceleration = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERAIRDECELERATION: {
                        playerList[activePlayer].airDeceleration = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERGRAVITYSTRENGTH: {
                        playerList[activePlayer].gravityStrength = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERJUMPSTRENGTH: {
                        playerList[activePlayer].jumpStrength = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERJUMPCAP: {
                        playerList[activePlayer].jumpCap = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERROLLINGACCELERATION: {
                        scriptEng.operands[i] = playerList[activePlayer].rollingAcceleration = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERROLLINGDECELERATION: {
                        scriptEng.operands[i] = playerList[activePlayer].rollingDeceleration = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERENTITYNO: break;
                    case VAR_PLAYERCOLLISIONLEFT: break;
                    case VAR_PLAYERCOLLISIONTOP: break;
                    case VAR_PLAYERCOLLISIONRIGHT: break;
                    case VAR_PLAYERCOLLISIONBOTTOM: break;
                    case VAR_PLAYERFLAILING: {
                        scriptEng.operands[i] = playerList[activePlayer].flailing[arrayVal] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERTIMER: {
                        playerList[activePlayer].timer = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERTILECOLLISIONS: {
                        playerList[activePlayer].tileCollisions = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYEROBJECTINTERACTION: {
                        scriptEng.operands[i] = playerList[activePlayer].objectInteractions = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERVISIBLE: {
                        playerList[activePlayer].visible = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERROTATION: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->rotation = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERSCALE: {
                        playerList[activePlayer].boundEntity->scale = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERPRIORITY: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->priority = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERDRAWORDER: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->drawOrder = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERDIRECTION: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->direction = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERINKEFFECT: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->inkEffect = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERALPHA: {
                        playerList[activePlayer].boundEntity->alpha = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERFRAME: {
                        playerList[activePlayer].boundEntity->frame = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERANIMATION: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->animation = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERPREVANIMATION: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->prevAnimation = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERANIMATIONSPEED: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->animationSpeed = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERANIMATIONTIMER: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->animationTimer = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERVALUE0: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->values[0] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERVALUE1: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->values[1] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERVALUE2: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->values[2] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERVALUE3: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->values[3] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERVALUE4: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->values[4] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERVALUE5: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->values[5] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERVALUE6: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->values[6] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERVALUE7: {
                        scriptEng.operands[i] = playerList[activePlayer].boundEntity->values[7] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERVALUE8: {
                        playerList[activePlayer].values[0] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERVALUE9: {
                        playerList[activePlayer].values[1] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERVALUE10: {
                        playerList[activePlayer].values[2] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERVALUE11: {
                        playerList[activePlayer].values[3] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERVALUE12: {
                        playerList[activePlayer].values[4] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERVALUE13: {
                        playerList[activePlayer].values[5] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERVALUE14: {
                        playerList[activePlayer].values[6] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYERVALUE15: {
                        playerList[activePlayer].values[7] = scriptEng.operands[i];
                        break;
                    }
                    case VAR_PLAYEROUTOFBOUNDS: break;
                    case VAR_STAGESTATE: stageMode = scriptEng.operands[i]; break;
                    case VAR_STAGEACTIVELIST: activeStageList = scriptEng.operands[i]; break;
                    case VAR_STAGELISTPOS: stageListPosition = scriptEng.operands[i]; break;
                    case VAR_STAGETIMEENABLED: timeEnabled = scriptEng.operands[i]; break;
                    case VAR_STAGEMILLISECONDS: stageMilliseconds = scriptEng.operands[i]; break;
                    case VAR_STAGESECONDS: stageSeconds = scriptEng.operands[i]; break;
                    case VAR_STAGEMINUTES: stageMinutes = scriptEng.operands[i]; break;
                    case VAR_STAGEACTNO: actID = scriptEng.operands[i]; break;
                    case VAR_STAGEPAUSEENABLED: pauseEnabled = scriptEng.operands[i]; break;
                    case VAR_STAGELISTSIZE: break;
                    case VAR_STAGENEWXBOUNDARY1: newXBoundary1 = scriptEng.operands[i]; break;
                    case VAR_STAGENEWXBOUNDARY2: newXBoundary2 = scriptEng.operands[i]; break;
                    case VAR_STAGENEWYBOUNDARY1: newYBoundary1 = scriptEng.operands[i]; break;
                    case VAR_STAGENEWYBOUNDARY2: newYBoundary2 = scriptEng.operands[i]; break;
                    case VAR_STAGEXBOUNDARY1:
                        if (xBoundary1 != scriptEng.operands[i]) {
                            xBoundary1    = scriptEng.operands[i];
                            newXBoundary1 = scriptEng.operands[i];
                        }
                        break;
                    case VAR_STAGEXBOUNDARY2:
                        if (xBoundary2 != scriptEng.operands[i]) {
                            xBoundary2    = scriptEng.operands[i];
                            newXBoundary2 = scriptEng.operands[i];
                        }
                        break;
                    case VAR_STAGEYBOUNDARY1:
                        if (yBoundary1 != scriptEng.operands[i]) {
                            yBoundary1    = scriptEng.operands[i];
                            newYBoundary1 = scriptEng.operands[i];
                        }
                        break;
                    case VAR_STAGEYBOUNDARY2:
                        if (yBoundary2 != scriptEng.operands[i]) {
                            yBoundary2    = scriptEng.operands[i];
                            newYBoundary2 = scriptEng.operands[i];
                        }
                        break;
                    case VAR_STAGEDEFORMATIONDATA0: bgDeformationData0[arrayVal] = scriptEng.operands[i]; break;
                    case VAR_STAGEDEFORMATIONDATA1: bgDeformationData1[arrayVal] = scriptEng.operands[i]; break;
                    case VAR_STAGEDEFORMATIONDATA2: bgDeformationData2[arrayVal] = scriptEng.operands[i]; break;
                    case VAR_STAGEDEFORMATIONDATA3: bgDeformationData3[arrayVal] = scriptEng.operands[i]; break;
                    case VAR_STAGEWATERLEVEL: waterLevel = scriptEng.operands[i]; break;
                    case VAR_STAGEACTIVELAYER: activeTileLayers[arrayVal] = scriptEng.operands[i]; break;
                    case VAR_STAGEMIDPOINT: tLayerMidPoint = scriptEng.operands[i]; break;
                    case VAR_STAGEPLAYERLISTPOS: playerListPos = scriptEng.operands[i]; break;
                    case VAR_STAGEACTIVEPLAYER:
                        activePlayer = scriptEng.operands[i];
                        if (activePlayer > activePlayerCount)
                            activePlayer = 0;
                        break;
                    case VAR_SCREENCAMERAENABLED: cameraEnabled = scriptEng.operands[i]; break;
                    case VAR_SCREENCAMERATARGET: cameraTarget = scriptEng.operands[i]; break;
                    case VAR_SCREENCAMERASTYLE: cameraStyle = scriptEng.operands[i]; break;
                    case VAR_SCREENDRAWLISTSIZE: drawListEntries[arrayVal].listSize = scriptEng.operands[i]; break;
                    case VAR_SCREENCENTERX: break;
                    case VAR_SCREENCENTERY: break;
                    case VAR_SCREENXSIZE: break;
                    case VAR_SCREENYSIZE: break;
                    case VAR_SCREENXOFFSET:
                        xScrollOffset = scriptEng.operands[i];
                        xScrollA      = xScrollOffset;
                        xScrollB      = SCREEN_XSIZE + xScrollOffset;
                        break;
                    case VAR_SCREENYOFFSET:
                        yScrollOffset = scriptEng.operands[i];
                        yScrollA      = yScrollOffset;
                        yScrollB      = SCREEN_YSIZE + yScrollOffset;
                        break;
                    case VAR_SCREENSHAKEX: cameraShakeX = scriptEng.operands[i]; break;
                    case VAR_SCREENSHAKEY: cameraShakeY = scriptEng.operands[i]; break;
                    case VAR_SCREENADJUSTCAMERAY: cameraAdjustY = scriptEng.operands[i]; break;
                    case VAR_TOUCHSCREENDOWN: break;
                    case VAR_TOUCHSCREENXPOS: break;
                    case VAR_TOUCHSCREENYPOS: break;
                    case VAR_MUSICVOLUME: SetMusicVolume(scriptEng.operands[i]); break;
                    case VAR_MUSICCURRENTTRACK: break;
                    case VAR_KEYDOWNUP: keyDown.up = scriptEng.operands[i]; break;
                    case VAR_KEYDOWNDOWN: keyDown.down = scriptEng.operands[i]; break;
                    case VAR_KEYDOWNLEFT: keyDown.left = scriptEng.operands[i]; break;
                    case VAR_KEYDOWNRIGHT: keyDown.right = scriptEng.operands[i]; break;
                    case VAR_KEYDOWNBUTTONA: keyDown.A = scriptEng.operands[i]; break;
                    case VAR_KEYDOWNBUTTONB: keyDown.B = scriptEng.operands[i]; break;
                    case VAR_KEYDOWNBUTTONC: keyDown.C = scriptEng.operands[i]; break;
                    case VAR_KEYDOWNSTART: keyDown.start = scriptEng.operands[i]; break;
                    case VAR_KEYPRESSUP: keyPress.up = scriptEng.operands[i]; break;
                    case VAR_KEYPRESSDOWN: keyPress.down = scriptEng.operands[i]; break;
                    case VAR_KEYPRESSLEFT: keyPress.left = scriptEng.operands[i]; break;
                    case VAR_KEYPRESSRIGHT: keyPress.right = scriptEng.operands[i]; break;
                    case VAR_KEYPRESSBUTTONA: keyPress.A = scriptEng.operands[i]; break;
                    case VAR_KEYPRESSBUTTONB: keyPress.B = scriptEng.operands[i]; break;
                    case VAR_KEYPRESSBUTTONC: keyPress.C = scriptEng.operands[i]; break;
                    case VAR_KEYPRESSSTART: keyPress.start = scriptEng.operands[i]; break;
                    case VAR_MENU1SELECTION: gameMenu[0].selection1 = scriptEng.operands[i]; break;
                    case VAR_MENU2SELECTION: gameMenu[1].selection1 = scriptEng.operands[i]; break;
                    case VAR_TILELAYERXSIZE: stageLayouts[arrayVal].xsize = scriptEng.operands[i]; break;
                    case VAR_TILELAYERYSIZE: stageLayouts[arrayVal].ysize = scriptEng.operands[i]; break;
                    case VAR_TILELAYERTYPE: stageLayouts[arrayVal].type = scriptEng.operands[i]; break;
                    case VAR_TILELAYERANGLE:
                        stageLayouts[arrayVal].angle = scriptEng.operands[i];
                        if (stageLayouts[arrayVal].angle < 0)
                            stageLayouts[arrayVal].angle += 0x200;
                        stageLayouts[arrayVal].angle &= 0x1FFu;
                        break;
                    case VAR_TILELAYERXPOS: stageLayouts[arrayVal].XPos = scriptEng.operands[i]; break;
                    case VAR_TILELAYERYPOS: stageLayouts[arrayVal].YPos = scriptEng.operands[i]; break;
                    case VAR_TILELAYERZPOS: stageLayouts[arrayVal].ZPos = scriptEng.operands[i]; break;
                    case VAR_TILELAYERPARALLAXFACTOR: stageLayouts[arrayVal].parallaxFactor = scriptEng.operands[i]; break;
                    case VAR_TILELAYERSCROLLSPEED: stageLayouts[arrayVal].scrollSpeed = scriptEng.operands[i]; break;
                    case VAR_TILELAYERSCROLLPOS: stageLayouts[arrayVal].scrollPos = scriptEng.operands[i]; break;
                    case VAR_TILELAYERDEFORMATIONOFFSET:
                        stageLayouts[arrayVal].deformationOffset = scriptEng.operands[i];
                        stageLayouts[arrayVal].deformationOffset &= 0xFFu;
                        break;
                    case VAR_TILELAYERDEFORMATIONOFFSETW:
                        stageLayouts[arrayVal].deformationOffsetW = scriptEng.operands[i];
                        stageLayouts[arrayVal].deformationOffsetW &= 0xFFu;
                        break;
                    case VAR_HPARALLAXPARALLAXFACTOR: hParallax.parallaxFactor[arrayVal] = scriptEng.operands[i]; break;
                    case VAR_HPARALLAXSCROLLSPEED: hParallax.scrollSpeed[arrayVal] = scriptEng.operands[i]; break;
                    case VAR_HPARALLAXSCROLLPOS: hParallax.scrollPos[arrayVal] = scriptEng.operands[i]; break;
                    case VAR_VPARALLAXPARALLAXFACTOR: vParallax.parallaxFactor[arrayVal] = scriptEng.operands[i]; break;
                    case VAR_VPARALLAXSCROLLSPEED: vParallax.scrollSpeed[arrayVal] = scriptEng.operands[i]; break;
                    case VAR_VPARALLAXSCROLLPOS: vParallax.scrollPos[arrayVal] = scriptEng.operands[i]; break;
                    case VAR_3DSCENENOVERTICES: vertexCount = scriptEng.operands[i]; break;
                    case VAR_3DSCENENOFACES: faceCount = scriptEng.operands[i]; break;
                    case VAR_VERTEXBUFFERX: vertexBuffer[arrayVal].x = scriptEng.operands[i]; break;
                    case VAR_VERTEXBUFFERY: vertexBuffer[arrayVal].y = scriptEng.operands[i]; break;
                    case VAR_VERTEXBUFFERZ: vertexBuffer[arrayVal].z = scriptEng.operands[i]; break;
                    case VAR_VERTEXBUFFERU: vertexBuffer[arrayVal].u = scriptEng.operands[i]; break;
                    case VAR_VERTEXBUFFERV: vertexBuffer[arrayVal].v = scriptEng.operands[i]; break;
                    case VAR_FACEBUFFERA: faceBuffer[arrayVal].a = scriptEng.operands[i]; break;
                    case VAR_FACEBUFFERB: faceBuffer[arrayVal].b = scriptEng.operands[i]; break;
                    case VAR_FACEBUFFERC: faceBuffer[arrayVal].c = scriptEng.operands[i]; break;
                    case VAR_FACEBUFFERD: faceBuffer[arrayVal].d = scriptEng.operands[i]; break;
                    case VAR_FACEBUFFERFLAG: faceBuffer[arrayVal].flags = scriptEng.operands[i]; break;
                    case VAR_FACEBUFFERCOLOR: faceBuffer[arrayVal].colour = scriptEng.operands[i]; break;
                    case VAR_3DSCENEPROJECTIONX: projectionX = scriptEng.operands[i]; break;
                    case VAR_3DSCENEPROJECTIONY: projectionY = scriptEng.operands[i]; break;
                    case VAR_ENGINESTATE: Engine.gameMode = scriptEng.operands[i]; break;
                    case VAR_STAGEDEBUGMODE: debugMode = scriptEng.operands[i]; break;
                    case VAR_ENGINEMESSAGE: break;
                    case VAR_SAVERAM: saveRAM[arrayVal] = scriptEng.operands[i]; break;
                    case VAR_ENGINELANGUAGE: Engine.language = scriptEng.operands[i]; break;
                    case VAR_OBJECTSPRITESHEET: {
                        objectScriptList[objectEntityList[arrayVal].type].spriteSheetID = scriptEng.operands[i];
                        break;
                    }
                    case VAR_ENGINEONLINEACTIVE: break;
                    case VAR_ENGINEFRAMESKIPTIMER: Engine.frameSkipTimer = scriptEng.operands[i]; break;
                    case VAR_ENGINEFRAMESKIPSETTING: Engine.frameSkipSetting = scriptEng.operands[i]; break;
                    case VAR_ENGINESFXVOLUME:
                        sfxVolume = scriptEng.operands[i];
                        if (sfxVolume < 0)
                            sfxVolume = 0;
                        if (sfxVolume > MAX_VOLUME)
                            sfxVolume = MAX_VOLUME;
                        break;
                    case VAR_ENGINEBGMVOLUME:
                        bgmVolume = scriptEng.operands[i];
                        if (bgmVolume < 0)
                            bgmVolume = 0;
                        if (bgmVolume > MAX_VOLUME)
                            bgmVolume = MAX_VOLUME;
                        break;
                    case VAR_ENGINEPLATFORMID: break;
                    case VAR_ENGINETRIALMODE: break;
                    case VAR_KEYPRESSANYSTART: break;
#if RETRO_USE_HAPTICS
                    case VAR_ENGINEHAPTICSENABLED: Engine.hapticsEnabled = scriptEng.operands[i]; break;
#endif
                }
            }
            else if (opcodeType == SCRIPTVAR_INTCONST) { // int constant
                scriptCodePtr++;
            }
            else if (opcodeType == SCRIPTVAR_STRCONST) { // string constant
                int strLen = scriptCode[scriptCodePtr++];
                for (int c = 0; c < strLen; ++c) {
                    switch (c % 4) {
                        case 0: break;
                        case 1: break;
                        case 2: break;
                        case 3: ++scriptCodePtr; break;
                        default: break;
                    }
                }
                scriptCodePtr++;
            }
        }
    }
}
