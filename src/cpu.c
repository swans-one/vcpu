#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Rough draft of the cpu.
 */


/* Here we define the memory layout that the cpu can access

There is 16k of memory (16 * 1024 bytes)

The CPU starts by loading the program into Read Only Memory which is
located from offset 8 * 1024 to offset 12 * 1024.

The CPU also has 8 registers. These are named A-H.

There are 14 usefull instructions and nop.

Operands are stored in machine integers. They either represent a
literal 16-bit integer, or a reference to a register, which will be
dereferenced at run time.

*/

/* NEXT:

   Figure out a scheme for loading the program into memory and seting
   PC

   How am I going to do this? I'm not sure. I'm probably going to load
   them into some reserved space. Let's do some math.

   Aiming for 32 (or possibly 48-bit) words.

   12 bits -> integers up to 4096.

   Bit budgeting
     +----------------+-----------+---------------+
     | Requirement    | Bits used | Running total |
     +----------------+-----------+---------------+
     | instructions   | 4 bits    | 4             |
     | operand1 flags | 2 bits    | 6             |
     | operand2 flags | 2 bits    | 8             |
     | operand1 data  | 12 bits   | 20            |
     | operand2 data  | 12 bits   | 32            |
     +----------------+-----------+---------------+

 */

#define MEMORYSIZE (16 * 1024)
#define ROMSTART (8 * 1024)
#define NUMREGISTERS 9
#define MAXLINE 100

#define NUMBERMASK 0xFFFF
#define ISINTEGERLITERALMASK 0x10000
#define ISREGISTERMASK 0x20000

int memory[MEMORYSIZE];
int registers[NUMREGISTERS];
static int *rom = memory + ROMSTART;

enum registernames {A, B, C, D, E, F, G, H, PC};

enum instructions {ADD = 0, SUB, BLTZ, BGEZ,
		   BEQZ, BNEZ, SHR, SHL,
		   AND, OR, XOR, LW,
		   SW, CP, NOP, NOP2};

int op;
int operand1;
int operand2;

void parseline(char *line, int *op, int *operand1, int *operand2);
int getop(char *operator);
int getoperand(char *operand);

int decodeoperand(int operand);

int getaline(char *line, int max);

int main() {
  char line[MAXLINE];
  int len;

  int *oploc = &op;
  int *operand1loc = &operand1;
  int *operand2loc = &operand2;

  /*
    printf("Add is opcode: %d\n", ADD);
    printf("Subtract is opcode: %d\n", SUB);
    printf("Branch EQ Zero is opcode: %d\n", BEQZ);
  */

  registers[A] = 100;

  while ((len = getaline(line, MAXLINE)) > 0) {
    parseline(line, oploc, operand1loc, operand2loc);
    printf("%d, %d, %d\n", op, decodeoperand(operand1), decodeoperand(operand2));
  }
  return registers[A];
}


/* Available instructions

   INSTRUCTION	| ACTION
   -------------+--------------------
   ADD		| D = A + B
   SUB		| D = A - B
   BLTZ		| if A  < 0 go S
   BGEZ		| if A >= 0 go S
   BEQZ		| if A == 0 go S
   BNEZ		| if A != 0 go S
   SHR		| D = A >> B
   SHL		| D = A << B
   AND		| D = A & B
   OR		| D = A | B
   XOR		| D = A ^ B
   LW		| reg gets address val
   SW		| address val gets reg
   CP           | reg copies reg
   NOP		| nothing
 */
void parseline(char *line, int *oploc, int *operand1loc, int *operand2loc) {
  int i,j;
  char c, operator[5], operand1[6], operand2[6];

  /* put operator section into string */
  for (i = 0; (c = line[i]) == ' '; i++) { ; } /* Munch whitespace */
  for (j = 0; (c = line[i]) != ' ' && c != '\n' && c != EOF && j < 4; j++) {
    operator[j] = c;
    i++;
  }
  operator[j] = '\0';
  while ((c = line[i]) != ' ') { i++; } /* Munch extra characters */

  /* put operand sections into string */
  /* operator 1 */
  for ( ; (c = line[i]) == ' '; i++) { ; } /* Munch whitespace */
  for (j = 0; (c = line[i]) != ' ' && c != '\n' && c != EOF && j < 10; j++) {
    operand1[j] = c;
    i++;
  }
  operand1[j] = '\0';
  while ((c = line[i]) != ' ') { i++; } /* Munch extra characters */
  /* operator 2 */
  for ( ; (c = line[i]) == ' '; i++) { ; } /* Munch whitespace */
  for (j = 0; (c = line[i++]) != ' ' && c != '\n' && c != EOF && j < 10; j++) {
    printf("line[%d]: %c\n", i, c);
    operand2[j] = c;
  }
  operand2[j] = '\0';

  printf("%s %s %s\n", operator, operand1, operand2);

  *oploc = getop(operator);
  *operand1loc = getoperand(operand1);
  *operand2loc = getoperand(operand2);
}

/* getop: returns an opcode for the given operator.

   If no known operator is matched, NOP is returned.
 */
int getop(char *op) {
  int opcode;
  if      (strcmp(op, "ADD")  == 0) { opcode = ADD;	}
  else if (strcmp(op, "SUB")  == 0) { opcode = SUB;	}
  else if (strcmp(op, "BLTZ") == 0) { opcode = BLTZ;	}
  else if (strcmp(op, "BGEZ") == 0) { opcode = BGEZ;	}
  else if (strcmp(op, "BEQZ") == 0) { opcode = BEQZ;	}
  else if (strcmp(op, "BNEZ") == 0) { opcode = BNEZ;	}
  else if (strcmp(op, "SHR")  == 0) { opcode = SHR;	}
  else if (strcmp(op, "SHL")  == 0) { opcode = SHL;	}
  else if (strcmp(op, "AND")  == 0) { opcode = AND;	}
  else if (strcmp(op, "OR")   == 0) { opcode = OR;	}
  else if (strcmp(op, "XOR")  == 0) { opcode = XOR;	}
  else if (strcmp(op, "LW")   == 0) { opcode = LW;	}
  else if (strcmp(op, "SW")   == 0) { opcode = SW;	}
  else if (strcmp(op, "CP")   == 0) { opcode = CP;	}
  else if (strcmp(op, "NOP")  == 0) { opcode = NOP;	}
  else                              { opcode = NOP;	}
  return opcode;
}

/* getoperand: Returns either the integer either that is directly
   encoded, or stored in the register.

   Literal numbers must be prefixed with a '#' character.

   Anything not prefixed with a '#' character will be interpreted as a
   register name.

   Anything that is not a integer literal or a valid register will
   result in a 0 value.

   The results are returned in a machine integer. However, since we
   only want 16 bit integers for our machine, we will mask the integer
   results down to that size.

   The extra bits are used to store whether an integer literal or a
   register location is stored.
 */
int getoperand(char *operand) {
  int reg, operandnum = 0;

  if (operand[0] == '#') {
    operandnum = strtol(operand + 1, (char**)NULL, 0) & NUMBERMASK;
    operandnum = operandnum | ISINTEGERLITERALMASK;
  }
  else if ('A' <= operand[0] && operand[0] <= 'H') {
    reg = operand[0] - 'A';
    operandnum = reg | ISREGISTERMASK;
  }
  else if (operand[0] == 'P') {
    operandnum = PC | ISREGISTERMASK;
  }
  else {
    operandnum = 0 | ISINTEGERLITERALMASK;
  }
  return operandnum;
}

int decodeoperand(int operand) {
  int reg, result;
  if ((operand & ISINTEGERLITERALMASK) > 0) {
    result = operand & NUMBERMASK;
  }
  else if ((operand & ISREGISTERMASK) > 0) {
    reg = operand & NUMBERMASK;
    result = registers[reg];
  }
  else {
    result = 0;
  }
  return result;
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

/* exec: executes the actual program.

   In the following steps:

    1) Instruction Fetch    <<<<<
    2) Instruction Decode	^
    3) Operand Fetch		^
    4) Execute			^
    5) Result Store		^
    6) Next Instruction     >>>>^
 */
void exec(void) {
}
