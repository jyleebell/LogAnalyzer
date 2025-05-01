#include <stdio.h>
#include <stdlib.h>

#include "codeGen.h"
#include "opList.h"

OpDListNode *OpDListHead;
OpDListNode *OpDListLast;

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
