#include <stdio.h>

/* Rough draft of the cpu.
 */

#define MEMORYSIZE 1024
#define NUMREGISTERS 8
#define MAXLINE 100

int memory[MEMORYSIZE];
int registers[NUMREGISTERS];

enum registernames {A, B, C, D, E, F, G, H};

int op;
int operand1;
int operand2;

void parseline(char *line, int *op, int *operand1, int *operand2);
int getaline(char *line, int max);

int main() {
  char line[MAXLINE];
  int len;

  int *oploc = &op;
  int *operand1loc = &operand1;
  int *operand2loc = &operand2;

  while ((len = getaline(line, MAXLINE)) > 0) {
    parseline(line, oploc, operand1loc, operand2loc);
    printf("%d, %d, %d\n", op, operand1, operand2);
  }
  return registers[A];
}

void parseline(char *line, int *op, int *operand1, int *operand2) {
}

int getaline(char *line, int max) {
  int c, i = 0;
  while ((c = getchar()) != EOF && c != '\n' && i < max) {
    line[i++] = c;
  }
  if (c == '\n') {
    line[i++] = c;
  }
  line[i] = '\0';
  return i;
}
