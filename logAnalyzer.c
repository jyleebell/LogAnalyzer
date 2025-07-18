/*
Log Analyzer 
  Author: Soc Design Lab. Jbnu
  Function: Analyze the log files from a battery gauge and find instruction patterns
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<ctype.h>
#include<setjmp.h>

#include "codeGen.h"
#include "util.h"
#include "print.h"
#include "opList.h"
#include "pattern.h"
#include "find_dep.h"

char *operandNameStrs[] = {
  "GR0_L", 
  "GR0_H", 
  "GR1_L", 
  "GR1_H", 
  "GR2_L", 
  "GR2_H", 
  "GR3_L", 
  "GR3_H", 
  "GR4_L", 
  "GR4_H", 
  "GR5_L", 
  "GR5_H", 
  "GR6_L", 
  "GR6_H", 
  "GR7_L", 
  "GR7_H", 
  "ST1_L",
  "ST1_H",
  "ST2_L",
  "ST2_H",
  "IN_X",
  "PCIB_L", 
  "PCIB_H", 
  "PCI_A_L",
  "PCI_A_H",
  "IR_BUF_H",
  "IR_BUF_L",
  "IR_BUF",		// immediate area
  "SFR_S0",		// may be ACC
  "SFR_S1",
  "SFR_S2",
  "SFR_S3",
  "SFR_S4",
  "SFR",		// SFR[xx]: xx is an address.
  "SRAM",		// SRAM[xxx]: xxx is an address.
  "ALU0_L",
  "ALU0_H",
  "ALU1_L",
  "ALU1_H",
  "ALU2_L",
  "ALU2_H",
  "EEP1",
  "EEP2_I",
  "EEP2_M",
  "LSB",
  "MSB",
  "CONST"
};

char *operatorNameStrs[] = {
  "ADD",
  "SUB",
  "MULT",
  "DIV",
  "RSHIFT",
  "LSHIFT",
  "OR",
  "AND",
  "XOR",
  "NOT",
  "MOVE",
  "MOVEC",
  "ALUMOVE",
  "RESET"
};

char *operators[] = {
  "+",
  "-",
  "*",
  "/",
  ">>",
  "<<",
  "|",
  "&",
  "^",
  "~",
  "MOVE",
  "MOVEC",
  "ALUMOVE",
  "RESET"
};

/*
 * Line number begins at 1 but ID of a node begins with 0;
 * If the line number range (to be analyzed) is from 1001 to 2000,
 * the ID of nodes ranges from 0 to 999.
 * The ID of the node correspoding to line number is calucated by using equation "NodeID = LineNo - nodeIDOffset".
 * When the range spans from line number 1001 to line number 2000 (offset is 1001),
 * the ID of the node containing Line 1500 (line number is 1500) is 1500 - 1001 = 499. 
 * nodeIDOffset is set per analysis.
 */
unsigned long nodeIDOffset = 0;
unsigned long noOfGraphNodes = 0;
OpDListNode *startNode, *endNode;
int isFirstInGroup = 0;
FILE *resultFP;
line_no_t startLine = 0, endLine = 0;

extern PATTERN_NODE *pattern_head;
extern PATTERN_NODE *pattern_tail;


/*
 typedef struct operand_t {
   OPERAND_TYPE type;
   value_t value;
   byte_t mask;
   addr_t addr;
   byte_t bitPos;
   byte_t isMSB;  // for MULT LSB or MSB
   byte_t ext;
   byte_t carry;
   byte_t bitWiseNot;
 } OPERAND;
 */

int
isEqualOperand(OPERAND *opnd0, OPERAND *opnd1) {
  if (opnd0->type == opnd1->type
      && opnd0->value == opnd1->value
      // && opnd0->mask == opnd1->mask
      && opnd0->addr == opnd1->addr
      // && opnd0->bitPos == opnd1->bitPos
      // && opnd0->isMSB == opnd1->isMSB
      // && opnd0->ext == opnd1->ext
      && opnd0->carry == opnd1->carry
      && opnd0->bitWiseNot == opnd1->bitWiseNot
    )
    return 1;
  else
    return 0;
}

/*
 typedef struct operation_t {
   OPERATOR op;
   OPERAND *dest;
   OPERAND *src[2];
 } OPERATION;
 */
int
isEqualOperation(OPERATION *op0, OPERATION *op1) {
  if (op0->op == op1->op) {
    if (isEqualOperand(op0->dest, op1->dest)
        && op0->src[0] && op1->src[0] && isEqualOperand(op0->src[0], op1->src[0])
        && op0->src[1] && op1->src[1] && isEqualOperand(op0->src[1], op1->src[1])
        )
      return 1;
    else
      return 0;
  } else
    return 0;
}

char *
getClk(char *lineStr, clk_t *clkP) {
  char *end, *start;
  lineStr = skipWhiteSpaces(lineStr);

  /* CLK[XXXXXXXX] */
  if (toupper(lineStr[0]) != 'C' ||
	  toupper(lineStr[1]) != 'L' ||
	  toupper(lineStr[2]) != 'K' ||
	  toupper(lineStr[3]) != '[') {
		fprintf(stderr, "Clock error: %s\n", lineStr);
		exit(CLK_ERR);
	  }
  lineStr += 4;
  start = lineStr;
  end = strchr(start, ']');
  lineStr = end + 1;
  *end = 0;

  *clkP = (clk_t) strtol(start, NULL, 16);

  return lineStr;
}

char *
getPC(char *lineStr, pc_t *pcP) {
  char *end, *start;
  lineStr = skipWhiteSpaces(lineStr);

  /* PC[XXXXXXXX] */
  if (toupper(lineStr[0]) != 'P' ||
	  toupper(lineStr[1]) != 'C' ||
	  toupper(lineStr[2]) != '[') {
		fprintf(stderr, "PC error: %s\n", lineStr);
		exit(PC_ERR);
	  }
  lineStr += 3;
  start = lineStr;
  end = strchr(start, ']');
  lineStr = end + 1;
  *end = 0;

  *pcP = (pc_t) strtol(start, NULL, 16);

  return lineStr;
}

char *
getFieldA(char *lineStr, byte_t *fieldP) {
  char *end, *start;
  lineStr = skipWhiteSpaces(lineStr);

  /* [XX] */
  if (toupper(lineStr[0]) != '[') {
		fprintf(stderr, "FieldA error: %s\n", lineStr);
		exit(FIELDA_ERR);
	  }
  lineStr += 1;
  start = lineStr;
  end = strchr(start, ']');
  lineStr = end + 1;
  *end = 0;

  *fieldP = (byte_t) strtol(start, NULL, 16);

  return lineStr;
}

char *
getFieldB(char *lineStr, byte_t *fieldP) {
  char *end, *start;
  lineStr = skipWhiteSpaces(lineStr);

  /* [XX] */
  if (toupper(lineStr[0]) != '[') {
		fprintf(stderr, "FieldB error: %s\n", lineStr);
		exit(FIELDB_ERR);
	  }
  lineStr += 1;
  start = lineStr;
  end = strchr(start, ']');
  lineStr = end + 1;
  *end = 0;

  *fieldP = (byte_t) strtol(start, NULL, 16);

  return lineStr;
}

char *
getOpName(char *lineStr, char *opName) {
  char *end, *start;
  lineStr = skipWhiteSpaces(lineStr);
  start = lineStr;

  /* opName // */
  end = strstr(start, "//");
  if (end == NULL) {
		fprintf(stderr, "OP NAME error: %s\n", lineStr);
		exit(OPNAME_ERR);
	  }
  *end = 0;
  lineStr = end + 2;
  strcpy(opName, start);

  return lineStr;
}

int
getOperandsStr (char *lineStr, char **srcOperandsStr, char **destOperandStr) {
  char *start;
  start = skipWhiteSpaces(lineStr);

  *destOperandStr = strstr(start, "->");
  *destOperandStr += 3;
  *destOperandStr = skipWhiteSpaces(*destOperandStr);
  *srcOperandsStr = start;
  *(*destOperandStr-4) = 0;

  return 0;
}

char * 
parseOperand(char *lineStr, OPERAND *opP) {
  int noOperandTypes = (int) (sizeof(operandNameStrs)/sizeof(char *));
  int i;
  char *start = skipWhiteSpaces(lineStr);
  char *end;
  OPERAND_TYPE opndType = 0;
  value_t opndValue = -1;
  byte_t opndMask = 0xFF;
  addr_t address = -1;
  byte_t bitPos = 0xFF;
  byte_t isMSB = 0xF;
  byte_t ext = 0xFF;
  byte_t carry = 0xFF;
  byte_t bitWiseNot = 0;


  if(start[0] == '~') {
	bitWiseNot = 1;
	start++;
  }
  opP->bitWiseNot = bitWiseNot;

  if (start[0] == '(') {
	start++;
	end = strchr(start, ')');
	if (end == NULL) {
	  fprintf(stderr, "Constant operand syntax error: ) is missing\n");
	  exit(CONST_OP_ERR);
	}
	*end = 0;
	opndValue = (unsigned int) strtol(start, NULL, 16);
	start = end + 1;

	start = skipWhiteSpaces(start);
	if (strstr(start, "LSB") != NULL) isMSB = 0;
	else if (strstr(start, "MSB") != NULL) isMSB = 1;

	opP->type = CONST;
	opP->value = opndValue;
	opP->mask = opndMask;
	opP->addr = address;
	opP->bitPos = bitPos;
	opP->isMSB = isMSB;
	opP->ext = ext;
	opP->carry = carry;

	return start;
  }
  else {
	for (i = 0; i < noOperandTypes; i++) {
	  if (strstr(start, operandNameStrs[i])) {
		/* found a matching operand */
		opndType = (OPERAND_TYPE) i;
		start += strlen(operandNameStrs[i]);

		if (opndType == PCIB_H && start[0] == '.') {
		  if (start[0] == '.' && start[2] == ',') {
			bitPos = start[1] - '0';
			if (bitPos < 0 || bitPos >= 8) {
			  fprintf(stderr, "Bit value error in PCIB_H.<bit postion>. bit position must be >= 0 and <= 7\n");
			  exit(BITPOS_VALUE_ERR);
			}
		  } else {
			fprintf(stderr, "Bit position syntax error in PCIB_H.<bit postion>,(value)\n");
			exit(BITPOS_SYNTAX_ERR);
		  }
		  start += 3;
		} else if (opndType == SFR || opndType == SRAM || opndType == EEP2_I || opndType == EEP2_M || opndType == EEP1) {
		  if (start[0] == '[') {
			start++;
			end = strchr(start, ']');
			if (end == NULL) {
			  fprintf(stderr, "%s address error. ] is missing.\n", opndType == SFR ? "SFR" : "SRAM");
			  exit(SFR_SRAM_ADDR_ERR);
			}
			*end = 0;
			address = (unsigned int) strtol(start, NULL, 16);
			start = end + 1;
		  } else {
			fprintf(stderr, "%s address error. [ is missing.\n", opndType == SFR ? "SFR" : "SRAM");
			exit(SFR_SRAM_ADDR_ERR);
		  }
		} 

		/* At this location of the input, (value) is seen. 
		   After (value), (m) can be placed optionally. 
		   */
		start = skipWhiteSpaces(start);
		if (start[0] == '(') {
		  end = strchr(start+1, ')');
		  if (end == NULL) {
			fprintf(stderr, "value error: ) is missing.\n");
			exit(VALUE_ERR);
		  }
		  *end = 0;
		  start++;
		  opndValue = (value_t) strtol(start, NULL, 16);
		  start = end + 1;
		} else {
		  fprintf(stderr, "value error: ( is missing.\n");
		  exit(VALUE_ERR);
		}

		start = skipWhiteSpaces(start);
		if (start[0] == '(') {
		  /* (m) is present */
		  char temp;
		  end = strchr(start+1, ')');
		  if (end == NULL) {
			fprintf(stderr, "mask error: ) is missing.\n");
			exit(MASK_ERR);
		  }
		  temp = *end;
		  *end = 0;
		  start++;
		  opndMask = (byte_t) strtol(start, NULL, 16);
		  *end = temp;
		  start = end + 1;
		} 

		start = skipWhiteSpaces(start);
		if (start[0] == ',') {
		  /*
			In source operand, ",(b)" can appear after mask.
			clk[00133daf]pc[000069ee][01][07] alu // SFR_S1(00)(1),(1) >> (1) -> PCIB_L(80)
			clk[00133db5]pc[000069e8][01][07] alu // SFR_S1(00)(1),(0) >> (1) -> GR5_H(00)
		  */
		  start += 2;
		  ext = *start - '0';
		  start += 2;
		}

		start = skipWhiteSpaces(start);
		/* MSB, LSB, + n, and -n are options */
		if (start[0] == 'L' || start[0] == 'M') {
		  /* LSB or MSB is present */
		  isMSB = start[0] == 'L' ? 0 : 1;
		}
		else if (
			(start[0] == '+' || start[0] == '-')
			&& start[1] == ' '
			&& start[2] == '('
		  ) {
		  start += 3;
		  end = strchr(start, ')');
		  *end = 0;
		  carry = (byte_t) strtol(start, NULL, 16);
		  start = end + 1;
		}

		break;
	  }	/* matching operand found */
	} /* for */

	if (i == noOperandTypes) {
	  fprintf(stderr, "No matching operand: %s\n", start);
	  exit(NO_MATCHNIG_OPND);
	}

	opP->type = (OPERAND_TYPE) i;
	opP->value = opndValue;
	opP->mask = opndMask;
	opP->addr = address;
	opP->bitPos = bitPos;
	opP->isMSB = isMSB;
	opP->ext = ext;
	opP->carry = carry;

	return start;
  }
}

OPERATOR
splitSrcOpStr(char *srcOperandsStr, char *srcOP1Str, char *srcOP2Str, char *opName) {
  int noOperatorTypes = (int) (sizeof(operators)/sizeof(char *));
  int i;
  int isMove = 0, isReset = 0, isMoveCon = 0;
  char *start = srcOperandsStr, *opLoc;

  if (
	  (isMoveCon = (strstr(opName, "mov src & con, des") != NULL))
	  || (isReset = (strstr(opName, "reset") != NULL))
	  || (isMove = (strstr(opName, "mov") != NULL))
	) {
	while (*start) {
	  *srcOP1Str++ = *start++;
	}
	*srcOP1Str = 0;

	if (isReset) return RESET;
	if (isMove)  return MOVE;
	if (isMoveCon)  return MOVEC;
  } 

  for (i = 0; i < noOperatorTypes; i++) {
	if ((opLoc = strstr(start, operators[i])) != NULL) {
	  if ((OPERATOR) i == NOT) {
		start++;
		while (*start != ' ' && *start != '\0' && *start != '\t') {
		  *srcOP1Str++ = *start++;
		}
		*srcOP1Str = 0;
		return NOT;
	  } else {
		/* 
		  Example:
		  SFR_S1(02)(1) & SFR_S2(00) 
						^
						|
					  opLoc
		*/
		while (start != opLoc) {
		  *srcOP1Str++ = *start++;
		}
		*srcOP1Str = 0;

		start += strlen(operators[i]);
		start = skipWhiteSpaces(start);
		while (*start) {
		  *srcOP2Str++ = *start++;
		}
		*srcOP2Str = 0;

		return (OPERATOR) i;
	  }
	}
  }

  /* at this point, this line is move operation */
  start = srcOperandsStr;
  while (*start) {
	*srcOP1Str++ = *start++;
  }

  return ALUMOVE;
}

int 
isLineNoInList(unsigned long lineNo, GrpHeadNode *h) {

  for (; h != NULL; h = h->next) {
	if (lineNo == h->lineNo)
	  return 1;
  }
  return 0;
}

int 
insertLineNo(unsigned long lineNo, GrpHeadNode *h, GrpHeadNode **t) {
  GrpHeadNode *n;
  
  if (isLineNoInList(lineNo, h))
    return 0;
  
  n = (GrpHeadNode *) malloc(sizeof(GrpHeadNode));

  if (n == NULL) {
	fprintf(stderr, "Error: Group Head List Node allocation error\n");
	exit(GRP_HEAD_LIST_ALLOC_ERR);
  }
  n->lineNo = lineNo;
  (*t)->next = n;
  *t = n;
  return 1;
}

#if 0
int
findDepDListNodes(igraph_t *pGraph, OpDListNode *nodeP) {
  static unsigned long history = 0;
  OpDListNode *opNode;
  OPERAND *src0;
  OPERAND *src1;
  OPERAND *dest = GET_DEST_OPERAND(nodeP);

  history++;
  
  for (opNode = nodeP; opNode && opNode != endNode; opNode = opNode->right) {
    src0 = GET_SRC0_OPERAND(opNode);
    src1 = GET_SRC1_OPERAND(opNode);
    if(src0 && isEqualOperand(dest, src0)) {
	  printDepNodesWithTabs(resultFP, opNode, history);

	  if (!VAN(pGraph, "Line", GET_NODE_ID_FROM_LINENO(GET_LINE_NO(opNode), nodeIDOffset))) {
		SETVAN(pGraph, "Line", GET_NODE_ID_FROM_LINENO(GET_LINE_NO(opNode), nodeIDOffset), (unsigned long) GET_LINE(opNode));

		printf("#%ld => %ld\n", GET_LINE_NO(nodeP), GET_LINE_NO(opNode));
		printf(">%ld => %ld\n", 
		  GET_NODE_ID_FROM_LINENO(GET_LINE_NO(nodeP), nodeIDOffset), 
		  GET_NODE_ID_FROM_LINENO(GET_LINE_NO(opNode), nodeIDOffset));

		igraph_add_edge(pGraph, 
			GET_NODE_ID_FROM_LINENO(GET_LINE_NO(nodeP), nodeIDOffset), 
			GET_NODE_ID_FROM_LINENO(GET_LINE_NO(opNode), nodeIDOffset)
		);
      
		findDepDListNodes(pGraph, opNode);
	  }
	}
    
	if(src1 && isEqualOperand(src1, dest)) {
      printDepNodesWithTabs(resultFP, opNode, history);

	  if (!VAN(pGraph, "Line", GET_NODE_ID_FROM_LINENO(GET_LINE_NO(opNode), nodeIDOffset))) {
		SETVAN(pGraph, "Line", GET_NODE_ID_FROM_LINENO(GET_LINE_NO(opNode), nodeIDOffset), (unsigned long) GET_LINE(opNode));

		printf("#%ld => %ld\n", GET_LINE_NO(nodeP), GET_LINE_NO(opNode));
		printf(">%ld => %ld\n", GET_NODE_ID_FROM_LINENO(GET_LINE_NO(nodeP), nodeIDOffset), GET_NODE_ID_FROM_LINENO(GET_LINE_NO(opNode), nodeIDOffset));
		igraph_add_edge(pGraph, 
			GET_NODE_ID_FROM_LINENO(GET_LINE_NO(nodeP), nodeIDOffset), 
			GET_NODE_ID_FROM_LINENO(GET_LINE_NO(opNode), nodeIDOffset)
		);

		findDepDListNodes(pGraph, opNode);
	  }
	}
  }
  history--;
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

int
findDepDListNodesReverse(igraph_t *pGraph, OpDListNode *nodeP) {
  static unsigned long history = 0;
  OpDListNode *opNode;
  OPERAND *src0;
  OPERAND *src1;
  OPERAND *dest;

  if (!nodeP) return 0;

  SETVAN(
	pGraph, 
	"Line",
	GET_NODE_ID_FROM_LINENO(GET_LINE_NO(nodeP), nodeIDOffset), 
	(unsigned long) GET_LINE(nodeP)
  );

  SETVAN(
	pGraph, 
	"Visited",
	GET_NODE_ID_FROM_LINENO(GET_LINE_NO(nodeP), nodeIDOffset), 
    1	
  );

  src0 = GET_SRC0_OPERAND(nodeP);
  src1 = GET_SRC1_OPERAND(nodeP);

  history++;
  
  /*
	if nodeP == startNode
  */
  // for (opNode = nodeP->left; opNode && opNode != startNode; opNode = opNode->left) {
  for (opNode = nodeP->left; opNode && opNode->right != startNode; opNode = opNode->left) {
	dest = GET_DEST_OPERAND(opNode);

    if(src0 && isEqualOperand(dest, src0)) {
	  /*
	  printDepNodesWithTabs(resultFP, opNode, history);
	  printf("#%ld => %ld\n", GET_LINE_NO(nodeP), GET_LINE_NO(opNode));
	  printf(">%ld => %ld\n", 
		GET_NODE_ID_FROM_LINENO(GET_LINE_NO(nodeP), nodeIDOffset), 
		GET_NODE_ID_FROM_LINENO(GET_LINE_NO(opNode), nodeIDOffset));
	  */

	  igraph_add_edge(pGraph, 
		  GET_NODE_ID_FROM_LINENO(GET_LINE_NO(opNode), nodeIDOffset),
		  GET_NODE_ID_FROM_LINENO(GET_LINE_NO(nodeP), nodeIDOffset) 
	  );

	  if (!VAN(pGraph, "Visited", GET_NODE_ID_FROM_LINENO(GET_LINE_NO(opNode), nodeIDOffset))) {
		findDepDListNodesReverse(pGraph, opNode);
	  }
	}
    
	if(src1 && isEqualOperand(src1, dest)) {
	  /*
      printDepNodesWithTabs(resultFP, opNode, history);
	  printf("#%ld => %ld\n", GET_LINE_NO(nodeP), GET_LINE_NO(opNode));
	  printf(">%ld => %ld\n", 
		GET_NODE_ID_FROM_LINENO(GET_LINE_NO(nodeP), nodeIDOffset), 
		GET_NODE_ID_FROM_LINENO(GET_LINE_NO(opNode), nodeIDOffset));
	  */

	  igraph_add_edge(pGraph, 
		  GET_NODE_ID_FROM_LINENO(GET_LINE_NO(opNode), nodeIDOffset),
		  GET_NODE_ID_FROM_LINENO(GET_LINE_NO(nodeP), nodeIDOffset) 
	  );

	  if (!VAN(pGraph, "Visited", GET_NODE_ID_FROM_LINENO(GET_LINE_NO(opNode), nodeIDOffset))) {
		findDepDListNodesReverse(pGraph, opNode);
	  }
	}
  }
  history--;
  return 0;
}
#endif


void
printUsage(char *progName) {
  fprintf(stderr, "For interactive mode, use -i option\n");
  fprintf(stderr, "Usage: %s -i \n", progName);
  fprintf(stderr, "or\n");
  fprintf(stderr, "Usage: %s -c start_clock end_clock log_file\n", progName);
  fprintf(stderr, "or\n");
  fprintf(stderr, "Usage: %s -p start_pc end_pc log_file\n", progName);
  fprintf(stderr, "or\n");
  fprintf(stderr, "Usage: %s -l start_line_no end_line_no log_file\n", progName);
}

void
findRange(int rangeKind, unsigned long start, unsigned long end) {
  OpDListNode *opNode = NULL;
  if (rangeKind == RANGE_PC) {
    startNode = endNode = NULL;
    for (opNode = OpDListHead; opNode; opNode = opNode->right) {
      if (startNode == NULL && opNode->lineP->pc == start) {
        startNode = opNode;
		startLine = GET_LINE_NO(startNode);
      }

      if (endNode == NULL && opNode->lineP->pc == end) {
        OpDListNode *t;
        for (t = opNode; t && t->lineP->pc == end; t = t->right) {
          endNode = t;
		  endLine = GET_LINE_NO(endNode);
        }
        break;
      }
    }
  } else if (rangeKind == RANGE_LINE_NO) {
    startNode = endNode = NULL;
    for (opNode = OpDListHead; opNode; ) {
      if (startNode == NULL && GET_LINE_NO(opNode) == start) {
        startNode = opNode;
		noOfGraphNodes++;
      }

      if (endNode == NULL && GET_LINE_NO(opNode) == end) {
        endNode = opNode;
        break;
      }

	  if (noOfGraphNodes) 
		noOfGraphNodes++;

      opNode = opNode->right;
    }
  }
}



int
main(int argc, char *argv[]) {
  FILE *fpLog;
  char lineStr[MAX_LINE_LEN], *p;
  char opName[OPNAME_SIZE];
  char logFileName[LOG_FILENAME_SIZE];
  char resultFileName[LOG_FILENAME_SIZE];
  char srcOP0Str[20], srcOP1Str[20];
  char *srcOperandsStr, *destOperandStr;
  int readLineResult;
  int rangeKind = RANGE_CLOCK;
  int interactiveMode = 0;

  int answer;
  char ansStr[3];
  char pattern_lgl_fname[30], pattern_dot_fname[30];
  FILE *pattern_lgl_fp,*pattern_dot_fp;

  PATTERN_NODE *pn;	/* pattern DB */
  OpDListNode *opDNode;

  clk_t clk, startClk = 0, endClk = 0;
  pc_t pc, startPC = 0, endPC = 0;
  byte_t fieldA, fieldB;
  OPERAND *destOperandP;
  OPERAND *srcOp0P, *srcOp1P;
  OPERATOR op;
  OPERATION *operationP;
  LogLine *lineP;

  initOpDList();

  printf("Interactive mode:\n");
  printf(">>> Input log file name: ");
  
  scanf("%s", logFileName);
  if ((fpLog = fopen(logFileName, "r")) == NULL) {
	fprintf(stderr, "File open error\n");
	return FILE_OPEN_ERR;
  }

  fprintf(stderr, "Reading log file %s ...\n", logFileName);

  /*
   * Read log file and construct a double-linked list
   */
  p = lineStr;
  readLineResult = readLine(fpLog, p, MAX_LINE_LEN);
  while (readLineResult != EOF && readLineResult != RD_WR_ERR) {
	lineP = (LogLine *) malloc(sizeof(LogLine));
    lineP->lineNo = lineNo;
	strcpy(lineP->strLine, p);

	DBG_PRINT(DEBUG, stdout, "Line: %s", p);
	
    p = getClk(p, &clk);
	DBG_PRINT(DEBUG, stdout, "\tCLK: %lx\n", clk);

	p = getPC(p, &pc);
    DBG_PRINT(DEBUG, stdout, "\tPC: %lx\n", pc);

	p = getFieldA(p, &fieldA);
    DBG_PRINT(DEBUG, stdout, "\tFIELDA: %x\n", fieldA);

	p = getFieldB(p, &fieldB);
    DBG_PRINT(DEBUG, stdout, "\tFIELDB: %x\n", fieldB);

	p = getOpName(p, opName);
	DBG_PRINT(DEBUG, stdout, "\tOP NAME: %s\n", opName);
	DBG_PRINT(DEBUG, stdout, "\tDescription: %s\n", p);

	getOperandsStr (p, &srcOperandsStr, &destOperandStr);
	DBG_PRINT(DEBUG, stdout, "\tsrcOperandsStr: %s || destOperandStr: %s\n", srcOperandsStr, destOperandStr);

	srcOP0Str[0] = 0;
	srcOP1Str[0] = 0;
	op = splitSrcOpStr(srcOperandsStr, srcOP0Str, srcOP1Str, opName);
	DBG_PRINT(DEBUG, stdout, "\tOP: %s\n", operatorNameStrs[(int) op]);
	DBG_PRINT(DEBUG, stdout, "\tsrcOP0Str: %s\n", srcOP0Str);
	if(
		op != MOVE && 
		op != RESET && 
		op != MOVEC && 
		op != ALUMOVE &&
		op != NOT
	  ) 
	  DBG_PRINT(DEBUG, stdout, "\tsrcOP1Str: %s\n", srcOP1Str);

	srcOp0P = srcOp1P = destOperandP = NULL;
	srcOp0P = (OPERAND *) malloc(sizeof(OPERAND));
	p = parseOperand(srcOP0Str, srcOp0P);
	printOperand(stdout, SRC0_OPERAND, srcOp0P, DEBUG);

	if(
		op != MOVE && 
		op != RESET && 
		op != MOVEC && 
		op != ALUMOVE &&
		op != NOT
	  ) {
		if ((op == LSHIFT || op == RSHIFT) && srcOP1Str[0] != '(') {
		  int j, len = (int) strlen(srcOP1Str);
		  char tempStr[30];
		  tempStr[0] = '(';
		  for (j = 0; j < len; j++) {
			tempStr[j+1] = srcOP1Str[j];
		  }
		  tempStr[j+1] = ')';
		  tempStr[j+2] = 0;
		  strcpy(srcOP1Str, tempStr);
		}
		srcOp1P = (OPERAND *) malloc(sizeof(OPERAND));
		p = parseOperand(srcOP1Str, srcOp1P);
		printOperand(stdout, SRC1_OPERAND, srcOp1P, DEBUG);
	}

	destOperandP = (OPERAND *) malloc(sizeof(OPERAND));
	p = parseOperand(destOperandStr, destOperandP);	// actually p is not used beyond this point
	printOperand(stdout, DEST_OPERAND, destOperandP, DEBUG);

	operationP = (OPERATION *) malloc(sizeof(OPERATION));
	operationP->op = op;
	operationP->dest = destOperandP;
	operationP->src[0] = srcOp0P ;
	operationP->src[1] = srcOp1P ;

	lineP->clock = clk;
	lineP->pc = pc;
	lineP->fieldA = fieldA;
	lineP->fieldB = fieldB;
	strncpy(lineP->opName, opName, OPNAME_SIZE-2);
	lineP->opP = operationP;
	lineP->opCode = op;
	lineP->nodeType = 0;	/* = 0: log line, = 1: summary (strLine) */
	insertLast(lineP);

	/* read next line in log file */
	readLineResult = readLine(fpLog, lineStr, MAX_LINE_LEN);
	p = lineStr;
  }

  DBG_PRINT(DEBUG, stdout, "### Done reading log file...\n");
  
  DBG_PRINT(DEBUG, stdout, "### Reading and pattern.in and generating PatternDB ...\n");
  parse_pattern_file("pattern.in");
  DBG_PRINT(DEBUG, stdout, "### Following patterns are read...\n");
  print_pattern_list(USER_INFO);

  /* for each pattern, search log (OpDListHead ~ OpDListTail) */
  for (pn = pattern_head; pn; pn = pn->link) {
	unsigned long startPC;
	unsigned long endPC;
	unsigned long currentPC;
	OpDListNode *opNodeInPattern;
	OpDListNode *startDLNode = NULL, *endDLNode = NULL, *summaryNode = NULL;

	startPC = GET_PATTERN_START_PC(pn);
	endPC = GET_PATTERN_END_PC(pn);

	DBG_PRINT(DEBUG, stdout, ">>>> %s: start = %lx,  end = %lx\n", GET_PATTERN_NAME(pn), GET_PATTERN_START_PC(pn), GET_PATTERN_END_PC(pn));

	for (OpDListNode *opNode = OpDListHead; opNode; opNode = opNode->right) {
	  currentPC = GET_LINE_PC(opNode);

	  if (currentPC == startPC) {
		DBG_PRINT(DEBUG, stdout, "\n>>>>>>> %s begins: %lx(Line NO. = %ld)\n", GET_PATTERN_NAME(pn), GET_LINE_PC(opNode), GET_LINE_NO(opNode));
		startDLNode = opNode;

		for (opNodeInPattern = opNode; ; opNodeInPattern = opNodeInPattern->right) {
		  if(GET_LINE_PC(opNodeInPattern) != endPC)
			  DBG_PRINT(DEBUG, stdout, ">>>>>>> In %s: %lx(Line NO. = %ld)\n", 
				GET_PATTERN_NAME(pn), GET_LINE_PC(opNodeInPattern), GET_LINE_NO(opNodeInPattern));

		  if (GET_LINE_PC(opNodeInPattern) == endPC) {
			do {
			  DBG_PRINT(DEBUG, stdout, ">>>>>>> In %s: %lx(Line NO. = %ld)\n", 
				GET_PATTERN_NAME(pn), GET_LINE_PC(opNodeInPattern), GET_LINE_NO(opNodeInPattern));
			  opNodeInPattern = opNodeInPattern->right;
			} while (GET_LINE_PC(opNodeInPattern) == endPC);
			break;
		  }
		}

		opNode = opNodeInPattern;
		endDLNode = opNode->left;

		DBG_PRINT(DEBUG, stdout, ">>>>>>> end: %s(Line NO. = %ld)\n", GET_LINE_STR(endDLNode), GET_LINE_NO(endDLNode));


		/*
		 if a pattern is found, a summary node is made and registered in summary node list
		*/
		summaryNode = make_summary_node(startDLNode, endDLNode, pn);
		registerSummaryNode(startDLNode, endDLNode, summaryNode);
	  }

	  if (startDLNode != NULL && endDLNode == NULL) {
		fprintf(stderr, "pattern %s end PC error\n", GET_PATTERN_NAME(pn));
		exit(20);
	  }

	  startDLNode = NULL;
	  endDLNode = NULL;
	}
  }

  /* Insert summary nodes */
  for (SummaryListNode *sn = SummaryHead; sn->next; sn = sn->next) {
	/* 
	  replace found Pattern with pattern node 
	  delete nodes from start to end (inclusive) 
	  add a new node containing a summary
	*/
 	deleteAndReplaceSummaryNode(sn->startDLNode, sn->endDLNode, sn->summaryNode); 
  }

  DBG_PRINT(USER_INFO, stdout, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
  for (OpDListNode *opNode = OpDListHead; opNode; ) {
	// printOpDListNode(stdout, opNode, USER_INFO);
	if (GET_LINE_NODE_TYPE(opNode))
	  DBG_PRINT(USER_INFO, stdout, "%s", GET_LINE_STR(opNode));
	else 
	  DBG_PRINT(USER_INFO, stdout, "%s", GET_LINE_STR(opNode));
	opNode = opNode->right;
  }
  DBG_PRINT(USER_INFO, stdout, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
}
