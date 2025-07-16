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

#define DBG_PRINT(debug, fp, fmt, args...) do { if (debug) fprintf(fp, fmt, ##args); } while (0)

int readLine(FILE *fp, char *lineStr, size_t size);
char * skipWhiteSpaces(char *lineStr);
extern unsigned long lineNo;

#endif /* util_h */
