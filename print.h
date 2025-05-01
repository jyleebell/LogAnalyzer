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

void printOperand(FILE *fp, int kind, OPERAND *opP, int isPrint);
void printOperation(FILE *fp, OPERATION *operP);
void printLogLine(FILE *fp, LogLine *lineP, int printOperation);

void printOpDListNode(FILE *fp, OpDListNode *opNode);
void printDepNodes(FILE *fp, OpDListNode *n, int kind);

void printDepNodesWithTabs(FILE *fp, OpDListNode *n, unsigned long tabs);
#endif

