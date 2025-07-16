//
//  opList.h
//  LogAnalyzerXcode
//
//  Created by Jong-Yeol LEE on 12/20/23.
//

#ifndef opList_h
#define opList_h
#include <stdio.h>
#include "codeGen.h"

typedef struct OpDListNode {
    struct OpDListNode *left;
    struct OpDListNode *right;
    LogLine *lineP;
} OpDListNode;

extern OpDListNode *OpDListHead;

OpDListNode *createNode(LogLine *lineP);
void initOpDList(void);

void insertFirst(LogLine *lineP);
void insertLast(LogLine *lineP);
void instertBefore(OpDListNode *at, LogLine *lineP);
void instertAfter(OpDListNode *at, LogLine *lineP);

OpDListNode *deleteFirst(void);
OpDListNode *deleteNode(OpDListNode *node);
OpDListNode *deleteBefore(OpDListNode *at);
OpDListNode *deleteAfter(OpDListNode *at);

extern OpDListNode *startNode, *endNode;


typedef struct GrpHeadNode {
  unsigned long lineNo;
  struct GrpHeadNode *next;
} GrpHeadNode;

int isLineNoInList(unsigned long lineNo, GrpHeadNode *h);
int insertLineNo(unsigned long lineNo, GrpHeadNode *h, GrpHeadNode **t);

#endif /* opList_h */
