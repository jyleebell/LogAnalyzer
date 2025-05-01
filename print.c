#include <stdio.h>
#include <stdlib.h>
#include "codeGen.h"
#include "print.h"

void 
printOperand(FILE *fp, int kind, OPERAND *opP, int isPrint) {
  if (!isPrint) return;

  if (kind == SRC0_OPERAND)
      fprintf(fp, "#\t SOURCE OPERAND0\n");
  else if (kind == SRC1_OPERAND)
      fprintf(fp, "#\t SOURCE OPERAND1\n");
  else
      fprintf(fp, "#\t DESTINATION OPERAND\n");
    fprintf(fp, "#\t\tOPERAND_TYPE: %s\n", operandNameStrs[(int) opP->type]);
  if (opP->type == SFR || opP->type == SRAM) {
      fprintf(fp, "#\t\tADDRESS: %lx\n", opP->addr);
  }
  else if (opP->type == PCIB_H && opP->bitPos != 0xFF) {
      fprintf(fp, "#\t\tBIT POSITION: %x\n", opP->bitPos);
  }
    fprintf(fp, "#\t\tVALUE: %lx\n", opP->value);
  if (opP->mask != 0xFF)
      fprintf(fp, "#\t\tMASK: %x\n", opP->mask);
  
  if (opP->ext != 0xFF)
      fprintf(fp, "#\t\tEXTB: %x\n", opP->ext);

  if (opP->isMSB != 0xF)
      fprintf(fp, "#\t\tMSB/LSB: %s\n", opP->isMSB ? "MSB" : "LSB");

  if (opP->carry != 0xFF)
      fprintf(fp, "#\t\tCARRY: %x\n", opP->carry);

  if (opP->bitWiseNot)
      fprintf(fp, "#\t\tBIT_WISE_NOT: %x\n", opP->bitWiseNot);
}

void
printOperation(FILE* fp, OPERATION *operP) {
  fprintf(fp, "\n\n");
  fprintf(fp, "################## OPERATION ################\n");
  fprintf(fp, "#\tOPERATOR: %s\n", operatorNameStrs[(int)operP->op]);
  printOperand(fp, DEST_OPERAND, operP->dest, 1);
  printOperand(fp, SRC0_OPERAND, operP->src[0], 1);
  if (operP->src[1]) {
	printOperand(fp, SRC1_OPERAND, operP->src[1], 1);
  }
  fprintf(fp, "#############################################\n");
  fprintf(fp, "\n");
}

void
printLogLine(FILE *fp, LogLine *lineP, int isPrintOperation) {
  fprintf(fp, "#\t@@@@@@@@@@@@@@@@@ LOG LINE @@@@@@@@@@@@@@@@@\n");
  fprintf(fp, "#\t@\tLine Number: %ld\n", lineP->lineNo);
  fprintf(fp, "#\t@\tclock: %lx\n", lineP->clock);
  fprintf(fp, "#\t@\tPC: %lx\n", lineP->pc);
  fprintf(fp, "#\t@\tFIELDA: %x\n", lineP->fieldA);
  fprintf(fp, "#\t@\tFIELDB: %x\n", lineP->fieldB);
  fprintf(fp, "#\t@\tOP_NAME: %s\n", lineP->opName);
  fprintf(fp, "#\t@\tSTR_LINE: %s", lineP->strLine);
  fprintf(fp, "#\t@\tIS VISITED: %d", lineP->isVisited);
  if (isPrintOperation)
	printOperation(fp, lineP->opP);
  fprintf(fp, "#\t@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
}

void
printOpDListNode(FILE *fp, OpDListNode *node) {
  printLogLine(fp, node->lineP, 0);
}


void
printDepNodes(FILE *fp, OpDListNode *n, int kind) {
  fprintf(fp, "%d:%8ld: %s", kind, GET_LINE_NO(n), GET_LINE_STR(n));
}

void
printDepNodesWithTabs(FILE *fp, OpDListNode *n, unsigned long tabs) {
  int i;
  for (i = 0; i < tabs; i++)
    fprintf(fp, " ");
  fprintf(fp, "%ld: %s", GET_LINE_NO(n), GET_LINE_STR(n));
}
