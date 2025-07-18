#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "codeGen.h"
#include "opList.h"
#include "find_dep.h"
#include "util.h"

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



static unsigned long
calc_fun(PATTERN_OP_TYPE type, unsigned long v0, unsigned long v1) {
  switch (type) {
	case ADD_2B:
	  return (v0 + v1)&0xFFFF;

	case ADD_4B:
	  return (v0 + v1) & 0xFFFFFFFF;

	case SUB_2B:
	  return (v0 - v1) & 0xFFFF;

	case SUB_4B:
	  return (v0 - v1) & 0xFFFFFFFF;

	case RSHIFT_2B:
	  return (v0 >> v1) & 0xFFFF;

	case RSHIFT_4B:
	  return (v0 >> v1) & 0xFFFFFFFF;

	case LSHIFT_2B:
	  return (v0 << v1) & 0xFFFF;

	case LSHIFT_4B:
	  return (v0 << v1) & 0xFFFFFFFF;

	case MUL_2B:
	  return (v0 * v1) & 0xFFFF;

	case MUL_4B:
	  return (v0 * v1) & 0xFFFFFFFF;

	case DIV_2B:
	  return (v0 / v1) & 0xFFFFF;

	case DIV_4B:
	  return (v0 / v1) & 0xFFFFFFFF;

	case SQRT:
	  return ((unsigned long) sqrt((double) v0)) & 0xFFFFFFFF;

	case EXP:
	  return ((unsigned long) exp((double) v0)) & 0xFFFFFFFF;
  }
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
  static unsigned long call_count = 0;
  OpDListNode *summaryNode;
  unsigned long opnd0 = 0x8, opnd1 = 0x9, opnd0_clk = 0xA, opnd1_clk = 0xB, result = 0xAA;
  char oper[5] = "OPER";
  OpDListNode *n;

  char summaryPCClk[100];
  char summaryValue0[100];
  char summaryClock0[100];
  char summaryClock1[100];
  char temp_str[4][50];
  char *summaryStr;

  unsigned long startLineNo = GET_LINE_NO(startDLNode);
  unsigned long endLineNo = GET_LINE_NO(endDLNode);
  unsigned long clk0, clk1;
  unsigned long arg0_value, arg1_value, result_value;

  DEP *dep_list0[MAX_ARGS * 2], *dep_list1[MAX_ARGS * 2], *dep;

  DBG_PRINT(DEBUG, stdout, "^^^^^^^^^^^^^ call count = %ld\n", call_count++);

  summaryNode = (OpDListNode *) malloc(sizeof(OpDListNode));
  if (!summaryNode) {
	fprintf(stderr, "Memory allocation error: summaryNode");
	exit(100);
  }
  summaryNode->lineP = (LogLine *) malloc(sizeof(LogLine));
  if (!summaryNode->lineP) {
	fprintf(stderr, "Memory allocation error: summaryNode->lineP");
	exit(100);
  }

  for (int i = 0; i < MAX_ARGS*2; i++) {
	dep_list0[i] = NULL;
  }

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
	  dep_list0[i] = dep;

	  if (arg1 && GET_OPERAND_TYPE(arg1) != CONST) {
		clk1 = findDepDListNodesReverse(startDLNode, arg1);

		dep = (DEP *) malloc(sizeof(DEP));
		dep->clock = clk1;
		dep->op = arg1;
		dep_list0[i + MAX_ARGS] = dep;
	  }
  }
  DBG_PRINT(DEBUG, stdout, ">>>>>>>> DEP_LIST0\n");
  print_dep_list(stdout, dep_list0, MAX_ARGS*2, DEBUG);

  for (int i = 0; i < MAX_ARGS*2; i++) {
	dep_list1[i] = NULL;
  }

  for (int i = 0; i < MAX_ARGS; i++) {
	  int offset = GET_PATTERN_OFFSET1(pn, i);
	  OPERAND *arg0, *arg1;
	  n = startDLNode;
	  for (int j = 0; j < offset; j++) {
		n = n->right;
	  }

	  arg0 = GET_SRC0_OPERAND(n);
	  arg1 = GET_SRC1_OPERAND(n);


	  clk1 = findDepDListNodesReverse(startDLNode, arg0);

	  dep = (DEP *) malloc(sizeof(DEP));
	  dep->clock = clk1;
	  dep->op = arg0;
	  dep_list1[i] = dep;

	  if (arg1 && GET_OPERAND_TYPE(arg1) != CONST) {
		clk1 = findDepDListNodesReverse(startDLNode, arg1);

		dep = (DEP *) malloc(sizeof(DEP));
		dep->clock = clk1;
		dep->op = arg1;
		dep_list0[i + MAX_ARGS] = dep;
	  }
  }
  DBG_PRINT(DEBUG, stdout, ">>>>>>>> DEP_LIST1\n");
  print_dep_list(stdout, dep_list1, MAX_ARGS*2, DEBUG);

  SET_NODE_TYPE(summaryNode, 1);

  /*
   dep_list0 has the first arg and dep_list1 has the second arg.
   dep_list0 and dep_list1 have 4 elements and each element is one byte.
   Arg value is "dep_list0[3]:dep_list0[2]:dep_list0[1]:dep_list0[0]" ==> This is not incorrent.
   */

  sprintf(summaryPCClk, "Function: clk[%08lx] pc[%08lx] ", GET_LINE_CLOCK(startDLNode), GET_LINE_PC(startDLNode));

  arg0_value = 0;
  for (int i = MAX_ARGS-1; i >= 0; i--) {
	if (dep_list0[i] && dep_list0[i]->op) {
	  arg0_value = arg0_value | (GET_OPERAND_VALUE(dep_list0[i]->op) & 0xFF);
	  arg0_value = arg0_value << 8;
	} else {
	  arg0_value = arg0_value << 8;
	}
  }

  arg1_value = 0;
  for (int i = MAX_ARGS-1; i >= 0; i--) {
	if (dep_list1[i] && dep_list1[i]->op) {
	  arg1_value = arg1_value | (GET_OPERAND_VALUE(dep_list1[i]->op) & 0xFF);
	  arg1_value = arg1_value << 8;
	} else {
	  arg1_value = arg1_value << 8;
	}
  }
  
  result_value = calc_fun(GET_PATTERN_TYPE(pn), arg0_value, arg1_value);
  sprintf(summaryValue0, "%08lx = %08lx %s %08lx ", result_value, arg0_value, GET_PATTERN_NAME(pn), arg1_value);

  for (int i = 0; i < MAX_ARGS; i++) {
	if (dep_list0[i]) {
	  sprintf(temp_str[i], "%08lx", dep_list0[i]->clock);
	} else {
	  sprintf(temp_str[i], " ");
	}
  }
  sprintf(summaryClock0, "([%s, %s, %s, %s], ", temp_str[0], temp_str[1], temp_str[2], temp_str[3]);

  for (int i = 0; i < MAX_ARGS; i++) {
	if (dep_list1[i]) {
	  sprintf(temp_str[i], "%08lx", dep_list1[i]->clock);
	} else {
	  sprintf(temp_str[i], " ");
	}
  }
  sprintf(summaryClock1, "[%s, %s, %s, %s])", temp_str[0], temp_str[1], temp_str[2], temp_str[3]);


  DBG_PRINT(DEBUG, stdout, "summaryPCClk = %ld, summaryValue0 = %ld, summaryClock0 = %ld, summaryClock1 = %ld\n",
	  strlen(summaryPCClk), 
	  strlen(summaryValue0), 
	  strlen(summaryClock0),
	  strlen(summaryClock1));

  summaryStr = (char *) malloc(
	  strlen(summaryPCClk) 
	  + strlen(summaryValue0) 
	  + strlen(summaryClock0)
	  + strlen(summaryClock1) + 10);

  sprintf(summaryStr, "%s %s : %s %s\n", summaryPCClk, summaryValue0, summaryClock0, summaryClock1);

  SET_STR_LINE(summaryNode, summaryStr);
  return summaryNode;
}


