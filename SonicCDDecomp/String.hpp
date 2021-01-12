#ifndef STRING_H
#define STRING_H

inline void StrCopy(char *dest, const char *src)
{
    int i = 0;

    for (; src[i]; ++i) dest[i] = src[i];

    dest[i] = 0;
}

inline void StrAdd(char *dest, const char *src)
{
    int destStrPos = 0;
    int srcStrPos  = 0;
    while (dest[destStrPos]) ++destStrPos;
    while (true) {
        if (!src[srcStrPos]) {
            break;
        }
        dest[destStrPos++] = src[srcStrPos++];
    }
    dest[destStrPos] = 0;
}

inline bool StrComp(const char *stringA, const char *stringB)
{
    bool match    = true;
    bool finished = false;
    while (!finished) {
        if (*stringA == *stringB || *stringA == *stringB + ' ' || *stringA == *stringB - ' ') {
            if (*stringA) {
                ++stringA;
                ++stringB;
            }
            else {
                finished = true;
            }
        }
        else {
            match    = false;
            finished = true;
        }
    }
    return match;
}

inline int StrLength(const char *string)
{
    int len = 0;
    for (len = 0; string[len]; len++)
        ;
    return len;
}
int FindStringToken(const char *string, const char *token, sbyte stopID);

#endif // !STRING_H
