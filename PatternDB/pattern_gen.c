#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "pattern.h"
#include "codeGen.h"
#include "util.h"

PATTERN_NODE *pattern_head = NULL;
PATTERN_NODE *pattern_tail = NULL;

char *pattern_op_name[] = {
  "ADD_2B",
  "ADD_4B",
  "SUB_2B",
  "SUB_4B",
  "RSHIFT_2B",
  "RSHIFT_4B",
  "LSHIFT_2B",
  "LSHIFT_4B",
  "MUL_2B",
  "MUL_4B",
  "DIV_2B",
  "DIV_4B",
  "SQRT",
  "EXP"
};


static PATTERN_OP_TYPE 
find_pattern_type(char *s) {
  if (!strcmp(s, "ADD_2B")) {
	return ADD_2B;
  } else if (!strcmp(s, "ADD_4B")) {
	return ADD_4B;
  } else if (!strcmp(s, "SUB_2B")) {
	return SUB_2B;
  } else if (!strcmp(s, "SUB_4B")) {
	return SUB_4B;
  } else if (!strcmp(s, "RSHIFT_2B")) {
	return RSHIFT_2B;
  } else if (!strcmp(s, "RSHIFT_4B")) {
	return RSHIFT_4B;
  } else if (!strcmp(s, "LSHIFT_2B")) {
	return LSHIFT_2B;
  } else if (!strcmp(s, "LSHIFT_4B")) {
	return LSHIFT_4B;
  } else if (!strcmp(s, "MUL_2B")) {
	return MUL_2B;
  } else if (!strcmp(s, "MUL_4B")) {
	return MUL_4B;
  } else if (!strcmp(s, "DIV_2B")) {
	return DIV_2B;
  } else if (!strcmp(s, "DIV_4B")) {
	return DIV_4B;
  } else if (!strcmp(s, "SQRT")) {
	return SQRT;
  } else if (!strcmp(s, "EXP")) {
	return EXP;
  } else {
	fprintf(stderr, "Pattern operation type error\n");
	exit(20);
  } 
}

void
print_pattern_list(int debug) {
  PATTERN_NODE *pn;
  if (!debug) return;
  for (pn = pattern_head; pn ; pn = pn->link) {
	printf("\n#########################\n");
	printf("\tfun: %s\n", pn->pattern_data->fun);
	printf("\top: %s\n", pattern_op_name[(int) pn->pattern_data->type]);
	printf("\tstart_pc: %lx\n", pn->pattern_data->start_pc);
	printf("\tend_pc: %lx\n", pn->pattern_data->end_pc);
	printf("\tOperand0 offset: ");
	for (int j = 0; j < MAX_ARGS; j++) {
	  printf("%d ", pn->pattern_data->op0_offset[j]);
	}
	printf("\n");

	printf("\tOperand1 offset: ");
	for (int j = 0; j < MAX_ARGS; j++) {
	  printf("%d ", pn->pattern_data->op1_offset[j]);
	}
	printf("\n");
  }
}

void 
parse_pattern_file(char *fname) {
  FILE *fp;
  int readLineResult;
  char *p;
  char lineStr[MAX_LINE_LEN];
  PATTERN *pattern_data;
  PATTERN_NODE *pn;


  if ((fp = fopen(fname, "r")) == NULL) {
	fprintf(stderr, "Pattern file open error\n");
	exit(300);
  }

  p = lineStr;
  readLineResult = readLine(fp, p, MAX_LINE_LEN);
  while (readLineResult != EOF && readLineResult != RD_WR_ERR) {
	if (strstr(p, "fun:")) {
	  char fun[10];
	  int i = 0;
	  p += 4;

	  while (*p == ' ' || *p == '\r' || *p == '\t') p++;

	  while (*p != ' ' && *p != '\r' && *p != '\n' && *p != '\n')  {
		fun[i++] = *p;
		p++;
	  }
	  fun[i] = '\0';
	  pattern_data = (PATTERN *) malloc(sizeof(PATTERN));
	  strncpy(pattern_data->fun, fun, 10);
	} else if(strstr(p, "op:")) {
	  char s_type[10];
	  int i = 0;

	  p += strlen("op:");

	  while (*p == ' ' || *p == '\r' || *p == '\t') p++;

	  while ((*p >= '0' && *p <= '9') 
	  	|| (*p >= 'a' && *p <= 'z')
		|| (*p >= 'A' && *p <= 'Z')
		|| (*p == '_')) {
		s_type[i++] = toupper(*p);
		p++;
	  }
	  s_type[i] = '\0';

	  pattern_data->type = find_pattern_type(s_type);
	} else if(strstr(p, "start_pc:")) {
	  char s_pc[10];
	  int i = 0;
	  p += 9;

	  while (*p == ' ' || *p == '\r' || *p == '\t') p++;

	  while ((*p >= '0' && *p <= '9') 
	  	|| (*p >= 'a' && *p <= 'f')
		|| (*p >= 'A' && *p <= 'F')) {
		s_pc[i++] = *p;
		p++;
	  }
	  s_pc[i] = '\0';
	  pattern_data->start_pc = strtol(s_pc, NULL, 16);
	} else if (strstr(p, "end_pc:")) {
	  char e_pc[10];
	  int i = 0;
	  p += 7;

	  while (*p == ' ' || *p == '\r' || *p == '\t') p++;

	  while ((*p >= '0' && *p <= '9') 
	  	|| (*p >= 'a' && *p <= 'f')
		|| (*p >= 'A' && *p <= 'F')) {
		e_pc[i++] = *p;
		p++;
	  }
	  e_pc[i] = '\0';
	  pattern_data->end_pc = strtol(e_pc, NULL, 16);
	} else if (strstr(p, "op0_offset:")) {
	  char *comma;
	  int i = 0;
	  p += strlen("op0_offset:");
	  while (*p == ' ' || *p == '\t') p++;

	  while (*p != '\n') {
		comma = strchr(p, ',');
		if (!comma) {
		  char *c;
		  c = strchr(p, '\n');
		  *c = 0;
		  pattern_data->op0_offset[i++] = atoi(p);
		  break;
		}
		*comma = 0;
		pattern_data->op0_offset[i++] = atoi(p);
		p = comma + 1;

		if (i == MAX_ARGS) {
		  fprintf(stderr, "In pattern.in, MAX number of op0_offset is %d\n", MAX_ARGS);
		  exit(10);
		}
	  }
	} else if (strstr(p, "op1_offset:")) {
	  char *comma;
	  int i = 0;
	  p += strlen("op1_offset:");

	  while (*p == ' ' || *p == '\t') p++;

	  while (*p != '\n') {
		comma = strchr(p, ',');
		if (!comma) {
		  char *c;
		  c = strchr(p, '\n');
		  *c = 0;
		  pattern_data->op1_offset[i++] = atoi(p);
		  break;
		}
		*comma = 0;
		pattern_data->op1_offset[i++] = atoi(p);
		p = comma + 1;

		if (i == MAX_ARGS) {
		  fprintf(stderr, "In pattern.in, MAX number of op1_offset is %d\n", MAX_ARGS);
		  exit(10);
		}
	  }


	  if (pattern_head == NULL) {
		pattern_head = (PATTERN_NODE *) malloc(sizeof(PATTERN_NODE));
		pattern_tail = pattern_head;
		pattern_head->pattern_data = pattern_data;
	  } else {
		pn = (PATTERN_NODE *) malloc(sizeof(PATTERN_NODE));
		pn->pattern_data = pattern_data;
		pn->link = NULL;
		pattern_tail->link = pn;
		pattern_tail = pn;
	  }
	}

    /* read next line in log file */
    readLineResult = readLine(fp, lineStr, MAX_LINE_LEN);
    p = lineStr;
  }
}

#ifdef UNIT_TEST_PATTERN_DB
int
main() {
  FILE *inFp;

  parse_pattern_file("pattern");

  print_pattern_list();

  return 0;
}
#endif
