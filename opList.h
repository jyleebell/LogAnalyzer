//
//  opList.h
//  LogAnalyzerXcode
//
//  Created by Jong-Yeol LEE on 12/20/23.
//

#ifndef opList_h
#define opList_h
#include <stdio.h>


#ifndef codeGen_h
#include "codeGen.h"
#endif

#ifndef pattern_h
#include "pattern.h"
#endif

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

typedef struct SummaryListNode {
  OpDListNode *startDLNode;
  OpDListNode *endDLNode;
  OpDListNode *summaryNode;
  struct SummaryListNode *next;
} SummaryListNode;

extern SummaryListNode *SummaryHead, *SummaryTail;


typedef struct GrpHeadNode {
  unsigned long lineNo;
  struct GrpHeadNode *next;
} GrpHeadNode;

int isLineNoInList(unsigned long lineNo, GrpHeadNode *h);
int insertLineNo(unsigned long lineNo, GrpHeadNode *h, GrpHeadNode **t);

int deleteAndReplaceSummaryNode(OpDListNode *startNode, OpDListNode *endNode, OpDListNode *summaryNode);
OpDListNode * make_summary_node(OpDListNode *startDLNode, OpDListNode *endDLNode, PATTERN_NODE *pn);
int registerSummaryNode(OpDListNode *startNode, OpDListNode *endNode, OpDListNode *summaryNode);

#endif /* opList_h */
