#ifndef find_dep_h
#define find_dep_h
#include "opList.h"
#include "codeGen.h"

typedef struct DEP {
  unsigned long clock;
  OPERAND *op;
} DEP;


unsigned long findDepDListNodesReverse(OpDListNode *nodeP, OPERAND *arg);
int isEqualOperand(OPERAND *opnd0, OPERAND *opnd1);
void print_dep_list(FILE *fp, DEP *l[], int size, int debug);
 
#endif
