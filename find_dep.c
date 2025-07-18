#include "find_dep.h"
#include "opList.h"
#include "codeGen.h"
#include "print.h"

void 
print_dep_list(FILE *fp, DEP *l[], int size, int debug) {
  if (!debug) return;

  for (int i = 0; i < size; i++) {
	if (!l[i]) continue;
	fprintf(fp, ">>>>>>>: %lx\n", (l[i])->clock);
	printOperand(fp, SRC0_OPERAND, (l[i])->op, debug);
  }
}


int
is_dep_end(OPERAND_TYPE t) {
  if (t == SFR
	|| t == ALU0_L
	|| t == SFR_S0
	|| t == SFR_S1
	|| t == SFR_S2
	|| t == SFR_S3
	|| t == SFR_S4
	|| t == ALU0_H
	|| t == ALU1_L
	|| t == ALU1_H
	|| t == ALU2_L
	|| t == ALU2_H
	|| t == EEP1
	|| t == EEP1
	|| t == EEP2_I
	|| t == EEP2_M
	|| t == IR_BUF_H
	|| t == IR_BUF_L
	|| t == IR_BUF
	|| t == CONST
	)
	return 1;
  else
	return 0;
}

/*
 node1
 node0 : start here (= nodeP)

 src0 = GET_SRC0_OPERAND(node0)
 src1 = GET_SRC1_OPERAND(node0)

 dest = GET_DEST_OPERAND(node1)

 opNode = node0
 if (dest == src0) 
   opNode = opNode->left
   findDepDListNodeReverse(opNode)

  if (dest == src1) 
   opNode = opNode->left
   findDepDListNodeReverse(opNode)

*/

unsigned long
findDepDListNodesReverse(OpDListNode *nodeP, OPERAND *arg) {
  static unsigned long history = 0;
  OpDListNode *opNode;
  OPERAND *src0;
  OPERAND *src1;
  OPERAND *dest;
  OPERAND_TYPE t;

  history++;

  if (!nodeP) return 0;
  for (opNode = nodeP->left; opNode; opNode = opNode->left) {
	dest = GET_DEST_OPERAND(opNode);
    if(isEqualOperand(dest, arg)) {
	  src0 = GET_SRC0_OPERAND(opNode);
	  t = GET_OPERAND_TYPE(src0);

	  if(is_dep_end(t)) {
		return GET_LINE_CLOCK(opNode);
	  } else {
		return findDepDListNodesReverse(opNode, src0);
	  }
	}
  }
  
  history--;

  return 0;
}
