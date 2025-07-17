#include <stdio.h>
#include <stdlib.h>

#include "codeGen.h"
#include "opList.h"
#include "find_dep.h"

OpDListNode *OpDListHead = NULL;
OpDListNode *OpDListLast = NULL;

SummaryListNode *SummaryHead = NULL;
SummaryListNode *SummaryTail = NULL;


OpDListNode *
createNode(LogLine *lineP) {
  OpDListNode *new_node = (OpDListNode *) malloc(sizeof(OpDListNode));
  new_node->left = new_node->right = NULL;
  new_node->lineP = lineP;
  return new_node;
}

void 
initOpDList (void)
{
  OpDListHead = NULL;
  OpDListLast = NULL;
}


void
insertFirst(LogLine *lineP) {
  OpDListNode *new_node = createNode(lineP);

  if (OpDListHead) { 
	new_node->right = OpDListHead;
	OpDListHead->left = new_node;
	OpDListHead = new_node;
  } else {
	OpDListHead = new_node;
	OpDListLast = new_node;
  }
}

void
insertLast(LogLine *lineP) {
  OpDListNode *new_node = createNode(lineP);

  if (OpDListLast) {
	new_node->left = OpDListLast;
	OpDListLast->right = new_node;
	OpDListLast = new_node;
  } else {
	if (OpDListHead == NULL) {
	  OpDListLast = OpDListHead = new_node;
	} else {
	  fprintf(stderr, "insertLast Error: OpDListLast == NULL and OpDListHead != NULL\n");
	  exit(INSERT_LAST_ERROR);
	}
  }
}

void
instertBefore(OpDListNode *at, LogLine *lineP) {
  OpDListNode *new_node = createNode(lineP);

  if (at->left) {
	new_node->right = at;
	new_node->left = at->left;
	at->left->right = new_node;
	at->left = new_node;
  } else {
	/* at is the first node */
	new_node->right = OpDListHead;
	OpDListHead->left = new_node;
	OpDListHead = new_node;
  }
}

void 
instertAfter(OpDListNode *at, LogLine *lineP) {
  OpDListNode *new_node = createNode(lineP);
  if (at->right) {
	new_node->left = at;
	new_node->right = at->right;
	at->right->left = new_node;
	at->right = new_node;
  } else {
	/* at is the last node */
	at->right = new_node;
	new_node->left = at;
	OpDListLast = new_node;
  }
}

OpDListNode * deleteFirst(void) {
  OpDListNode *deleted;

  if (OpDListHead) {
	deleted = OpDListHead;
	OpDListHead = deleted->right;
	OpDListHead->left = NULL;
	if (OpDListHead->right == NULL) 
	  OpDListLast = OpDListHead;
	return deleted;
  }
  else 
	return NULL;
}

OpDListNode * 
deleteNode(OpDListNode *node) {
  OpDListNode *deleted = node;
  if (deleted->left ==  NULL) {
	/* node is the first node */
	OpDListHead = deleted->right;
	OpDListHead->left = NULL;
	if (OpDListHead->right == NULL) 
	  OpDListLast = OpDListHead;
	return deleted;
  }

  if (deleted->right == NULL) {
	/* node is the last node */
	deleted->left->right = NULL;
	OpDListLast = deleted->left;
	return deleted;
  }

  deleted->left->right = deleted->right;
  deleted->right->left = deleted->left;

  return deleted;
}

OpDListNode * 
deleteBefore(OpDListNode *at) {
  OpDListNode *deleted = at->left;

  if (deleted ==  NULL) {
	/* at is the first node */
	fprintf(stderr, "DeleteBefore Error: no before node\n");
	exit(DELETE_BEFORE_ERROR);
  }

  return deleteNode(deleted);
}

OpDListNode * 
deleteAfter(OpDListNode *at) {
  OpDListNode *deleted = at->right;

  if (deleted == NULL) {
	/* at is the last node */
	fprintf(stderr, "DeleteAfter Error: no after node\n");
	exit(DELETE_AFTER_ERROR);
  }

  return deleteNode(deleted);
}


int
registerSummaryNode(OpDListNode *startNode, OpDListNode *endNode, OpDListNode *summaryNode) {
  SummaryListNode *newSummary;
  newSummary = (SummaryListNode *) malloc(sizeof(SummaryListNode));

  newSummary->startDLNode = startNode;
  newSummary->endDLNode = endNode;
  newSummary->summaryNode = summaryNode;;
  newSummary->next = NULL;

  if (SummaryHead == NULL) {
	SummaryHead = newSummary;
	SummaryTail = newSummary;
  } else {
	SummaryTail->next = newSummary;
	SummaryTail = newSummary;
  }

  return 0;
}


int
deleteAndReplaceSummaryNode(OpDListNode *startNode, OpDListNode *endNode, OpDListNode *summaryNode) {
  OpDListNode *n0, *n1, *tn, *t2;

  n0 = startNode->left;
  n1 = endNode->right;

  n0->right = summaryNode;
  summaryNode->left = n0;
  summaryNode->right = n1;
  n1->left = summaryNode;

  for (tn = startNode; tn != endNode; ) {
	t2 = tn;
	tn = t2->right;
	free(t2->lineP);
	free(t2);
  }

  return 1;
}

OpDListNode *
make_summary_node(OpDListNode *startDLNode, OpDListNode *endDLNode, PATTERN_NODE *pn) {
  OpDListNode *summaryNode;
  char *summaryStr;
  unsigned long opnd0 = 0x8, opnd1 = 0x9, opnd0_clk = 0xA, opnd1_clk = 0xB, result = 0xAA;
  char oper[5] = "OPER";
  OpDListNode *n;

  unsigned long startLineNo = GET_LINE_NO(startDLNode);
  unsigned long endLineNo = GET_LINE_NO(endDLNode);
  unsigned long clk0, clk1;

  DEP *dep_list[MAX_ARGS], *dep;


  summaryNode = (OpDListNode *) malloc(sizeof(OpDListNode));
  summaryNode->lineP = (LogLine *) malloc(sizeof(LogLine));
  summaryStr = (char *) malloc(256);


  for (int i = 0; i < MAX_ARGS; i++) {
	  int offset = GET_PATTERN_OFFSET0(pn, i);
	  OPERAND *arg0, *arg1;
	  n = startDLNode;
	  for (int j = 0; j < offset; j++) {
		n = n->right;
	  }

	  arg0 = GET_SRC0_OPERAND(n);
	  arg1 = GET_SRC1_OPERAND(n);


	  clk0 = findDepDListNodesReverse(startDLNode, arg0);

	  dep = (DEP *) malloc(sizeof(DEP));
	  dep->clock = clk0;
	  dep->op = arg0;
	  dep_list[i] = dep;
  }

  print_dep_list(stdout, dep_list, MAX_ARGS, 1);
 
  SET_NODE_TYPE(summaryNode, 1);

  sprintf(summaryStr, "clk[%08lx]pc[%08lx] %s 0x%08lx = 0x%08lx %s 0x%08lx : (0x%08lx, 0x%08lx)\n", 
		GET_LINE_CLOCK(startDLNode),
		GET_LINE_PC(startDLNode),
		GET_PATTERN_NAME(pn),
		result,
		opnd0,
		oper,
		opnd1,
		opnd0_clk,
		opnd1_clk
	  );

  SET_STR_LINE(summaryNode, summaryStr);
  return summaryNode;
}


