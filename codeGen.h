#ifndef codeGen_h
#define codeGen_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 0
#define USER_INFO 1


#define NO_ARGC 5
#define MAX_LINE_LEN 100
#define OPNAME_SIZE 20
#define LOG_FILENAME_SIZE 256

#define USAGE_ERR 2
#define FILE_OPEN_ERR 3
#define RD_WR_ERR 4
#define CLK_ERR 5
#define PC_ERR 6
#define FIELDA_ERR 7
#define FIELDB_ERR 8
#define OPNAME_ERR 9
#define BITPOS_SYNTAX_ERR 10
#define BITPOS_VALUE_ERR 11
#define SFR_SRAM_ADDR_ERR 12
#define VALUE_ERR 13
#define MASK_ERR 14
#define NO_MATCHNIG_OPND 15
#define CONST_OP_ERR 16
#define DELETE_BEFORE_ERROR 17
#define DELETE_AFTER_ERROR 18
#define INSERT_LAST_ERROR 19
#define OPTION_ERROR 20
#define GRP_HEAD_LIST_ALLOC_ERR 21


#define SRC0_OPERAND 0
#define SRC1_OPERAND 1
#define DEST_OPERAND 2

#define RANGE_CLOCK 0
#define RANGE_PC 1
#define RANGE_LINE_NO 2

typedef unsigned long clk_t;
typedef unsigned long pc_t;
typedef unsigned long value_t;
typedef long addr_t;
typedef unsigned char byte_t;
typedef unsigned long line_no_t;

/* operands can be registers or SRAM */
typedef enum operand_type_t { 
  GR0_L = 0, 
  GR0_H, 
  GR1_L, 
  GR1_H, 
  GR2_L, 
  GR2_H, 
  GR3_L, 
  GR3_H, 
  GR4_L, 
  GR4_H, 
  GR5_L, 
  GR5_H, 
  GR6_L, 
  GR6_H, 
  GR7_L, 
  GR7_H, 
  ST1_L,
  ST1_H,
  ST2_L,
  ST2_H,
  IN_X,
  PCIB_L, 
  PCIB_H, 
  PCI_A_L,
  PCI_A_H,
  IR_BUF_H,
  IR_BUF_L,
  IR_BUF,	// immediate area
  SFR_S0,	// may be ACC
  SFR_S1,
  SFR_S2,
  SFR_S3,
  SFR_S4,
  SFR,	// SFR[xx]: xx is an address.
  SRAM,	// SRAM[xxx]: xxx is an address.
  ALU0_L,
  ALU0_H,
  ALU1_L,
  ALU1_H,
  ALU2_L,
  ALU2_H,
  EEP1,
  EEP2_I,
  EEP2_M,
  LSB,
  MSB,
  CONST,
  NOP
} OPERAND_TYPE;

typedef enum operator {
  ADD,
  SUB,
  MULT,
  DIV,
  RSHIFT,
  LSHIFT,
  OR,
  AND,
  XOR,
  NOT,
  MOVE,
  MOVEC,
  ALUMOVE,
  RESET
} OPERATOR;

typedef enum range {
  CLOCK,
  PC
} RANGE_VAR;


/*
 Operands are as follows:
 1) operandNameStrs[n](value)
 2) operandNameStrs[n](value)(m): m is a mask. value & {8{m}} is calculated.
 3) operandNameStrs[n](value) LSB|MSB
 4) PCIB_H.7
 5) SFR[XX](value)	: XX is an address.
 6) SRAM[XX](value) : XX is an address.
 */
typedef struct operand_t {
  OPERAND_TYPE type;
  value_t value;	
  byte_t mask;
  addr_t addr;
  byte_t bitPos;
  byte_t isMSB;	// for MULT LSB or MSB
  byte_t ext;
  byte_t carry;
  byte_t bitWiseNot;
} OPERAND;

typedef struct operation_t {
  OPERATOR op;
  OPERAND *dest;
  OPERAND *src[2];
} OPERATION;

typedef struct logLine {
  char nodeType;	/* = 0: log line, = 1: summary (in strline) */
  unsigned long clock;
  unsigned long pc;
  unsigned char fieldA;
  unsigned char fieldB;
  unsigned long lineNo;
  char opName[OPNAME_SIZE];		// mov, alu
  char strLine[MAX_LINE_LEN];	// string for each line
  OPERATION *opP;
  OPERATOR opCode;
} LogLine;

extern char *operatorNameStrs[];
extern char *operandNameStrs[];
extern unsigned long lineNo;

#define GET_DEST_OPERAND(nodeP) nodeP->lineP->opP->dest
#define GET_SRC0_OPERAND(nodeP) nodeP->lineP->opP->src[0]
#define GET_SRC1_OPERAND(nodeP) nodeP->lineP->opP->src[1]
#define GET_LINE(nodeP) nodeP->lineP
#define GET_LINE_NO(nodeP) nodeP->lineP->lineNo
#define GET_LINE_STR(nodeP) nodeP->lineP->strLine
#define GET_LINE_PC(nodeP) nodeP->lineP->pc
#define GET_LINE_CLOCK(nodeP) nodeP->lineP->clock
#define GET_LINE_NODE_TYPE(nodeP) nodeP->lineP->nodeType

#define SET_NODE_TYPE(nodeP, v) nodeP->lineP->nodeType = v
#define SET_STR_LINE(nodeP, s) strcpy(nodeP->lineP->strLine, s) 

#define GET_SRC0_OPERAND_TYPE(nodeP) (nodeP->lineP->opP->src ? nodeP->lineP->opP->src[0]->type : NOP)
#define GET_SRC1_OPERAND_TYPE(nodeP) (nodeP->lineP->opP->src ? nodeP->lineP->opP->src[1]->type : NOP)

#define GET_OPERAND_TYPE(op) (op->type)
#define GET_OPERAND_VALUE(op) (op->value)

#define IS_VISITED(nodeP) nodeP->lineP->isVisited

#define GET_NODE_ID_FROM_LINENO(lineNo, offset) (lineNo - offset)

#endif /* codeGen_h */
