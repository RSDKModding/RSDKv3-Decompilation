#include "RetroEngine.h"

using namespace StringUtils;

void StringUtils::StrCopy(char* Dest, char* Src) {
    int i = 0;

    for (; Src[i]; ++i) {
        Dest[i] = Src[i];
    }

    Dest[i] = 0;
}

void StringUtils::StrAdd(char* Dest, char* Src) {
    int SrcStrPos;
    int DestStrPos;

    DestStrPos = 0;
    SrcStrPos = 0;
    while (Dest[DestStrPos])
        ++DestStrPos;
    while (true)
    {
        if (!Src[SrcStrPos]) {
            break;
        }
        Dest[DestStrPos++] = Src[SrcStrPos++];
    }
    Dest[DestStrPos] = 0;
}

bool StringUtils::StrComp(char* StringA, char* StringB) {
    bool Match;
    bool Finished;

    Match = true;
    Finished = false;
    while (!Finished) {
        if (*StringA == *StringB || *StringA == *StringB + ' ' || *StringA == *StringB - ' ') {
            if (*StringA) {
                ++StringA;
                ++StringB;
            }
            else {
                Finished = true;
            }
        }
        else {
            Match = false;
            Finished = true;
        }
    }
    return Match;
}

int StringUtils::StrLength(char* String) {
    int len = 0;

    for (len = 0; String[len]; len++);

    return len;
}

int StringUtils::FindStringToken(char* String, char* Token, char StopID) {
    int TokenCharID = 0; 
    char TokenMatch = true;
    int StringCharID = 0;
    int FoundTokenID = 0;

    while (String[StringCharID]) {
        TokenCharID = 0;
        TokenMatch = true;
        while (Token[TokenCharID]) {
            if (!String[TokenCharID + StringCharID]) {
                return -1;
            }

            if (String[TokenCharID + StringCharID] != Token[TokenCharID]) {
                TokenMatch = false;
            }
            ++TokenCharID;
        }
        if (TokenMatch && ++FoundTokenID == StopID) {
            return StringCharID;
        }
        ++StringCharID;
    }
    return -1;
}

bool StringUtils::ConvertStringToInteger(char* String, int* Intager) {
    bool intChar = 0; 
    int StringLength = 0; 
    int CharID = 0; 
    int CharValue = 0; 
    bool Negative = 0; 

    *Intager = 0;
    intChar = *String > '/' && *String < ':';
    if (*String != '+' && !intChar && *String != '-')
        return false;
    StringLength = StrLength(String) - 1;

    if (*String == '-') {
        Negative = true;
        CharID = 1;
        --StringLength;
    }
    else if (*String == '+') {
        CharID = 1;
        --StringLength;
    }

    while (StringLength > -1) {
        if (String[CharID] <= '/' || String[CharID] >= ':')
            return 0;
        if (StringLength <= 0) {
            *Intager = String[CharID] + *Intager - 48;
        }
        else {
            for (CharValue = String[CharID] - '0'; --StringLength; CharValue *= 10);
            *Intager += CharValue;
        }
        --StringLength;
        ++CharID;
    }

    if (Negative == 1) {
        *Intager = -*Intager;
    }
    return true;
}