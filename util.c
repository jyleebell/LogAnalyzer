#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "codeGen.h"

unsigned long lineNo = 0;

int
readLine(FILE *fp, char *lineStr, size_t size) {
  if (fgets(lineStr, (int) size, fp) == NULL) {
    if (feof(fp)) {
      return EOF;
    }
    else if(ferror(fp)) {
      fprintf(stderr, "Read/Write error\n");
      clearerr(fp);
      return RD_WR_ERR;
    }
  }

  lineNo++;
  return (int) strlen(lineStr);
}

char *
skipWhiteSpaces(char *lineStr) {
  while (isspace(*lineStr)) 
	lineStr++;

  return lineStr;
}

