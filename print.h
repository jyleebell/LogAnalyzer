#ifndef print_h
#define print_h

#ifndef util_h
#include "util.h"
#endif

#ifndef codeGen_h
#include "codeGen.h"
#endif

#ifndef opList_h
#include "opList.h"
#endif

extern char *operandNameStrs[];
extern char *operatorNameStrs[];
extern char *operators[];


void printOperand(FILE *fp, int kind, OPERAND *opP, int isPrint);
void printOperation(FILE *fp, OPERATION *operP);
void printLogLine(FILE *fp, LogLine *lineP, int printOperation);

void printOpDListNode(FILE *fp, OpDListNode *opNode, int debug);
void printDepNodes(FILE *fp, OpDListNode *n, int kind);
void printMatchLogLine(FILE *fp, LogLine *lineP, int isPrintOperation);


void printDepNodesWithTabs(FILE *fp, OpDListNode *n, unsigned long tabs);
#endif

