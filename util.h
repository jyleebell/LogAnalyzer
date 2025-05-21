//
//  util.h
//  LogAnalyzerXcode
//
//  Created by Jong-Yeol LEE on 12/20/23.
//

#ifndef util_h
#define util_h

#ifndef codeGen_h
#include "codeGen.h"
#endif

#define MYDEBUG 0

#if MYDEBUG
// #define DBG_PRINT(fmt, args...) fprintf(stderr, "[%s:%d:%s()]: " fmt, __FILE__, __LINE__, __func__, ##args)
#define DBG1_PRINT(fmt, args...) fprintf(stderr, fmt, ##args)
#define DBG2_PRINT(fmt, args...) fprintf(stderr, fmt, ##args)
#else
#define DBG1_PRINT(fmt, args...)
#define DBG2_PRINT(fmt, args...)
#endif

int readLine(FILE *fp, char *lineStr, size_t size);
char * skipWhiteSpaces(char *lineStr);
extern unsigned long lineNo;

#endif /* util_h */
