#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>

#define DATA_LEN 6

/**
 * ALU
 */
void alu(struct cpu *cpu, enum alu_op op, unsigned char regA, unsigned char regB)
{
	unsigned char *A = &cpu->reg[regA];
	unsigned char *B = &cpu->reg[regB];
	int *FL = &cpu->FL;
	
  	switch (op) {
		case ALU_ADD:	*A += *B;			break; // basic math
		case ALU_SUB:	*A -= *B;			break;
		case ALU_MUL:	*A *= *B;			break;  
		case ALU_DIV:	*A /= *B;			break;
		case ALU_MOD:	*A %= *B;			break;
		case ALU_NOT: 	*A = !*A;			break; // bitwise ops
		case ALU_AND: 	*A &= *B;			break; 
		case ALU_OR :	*A |= *B;			break; 
		case ALU_XOR:	*A ^= *B;			break;
		case ALU_DEC: 	*A =*A-1;			break; // Other
		case ALU_INC: 	*A =*A+1;			break;
		case ALU_SHL:	*A <<=*B;			break; 
		case ALU_SHR:	*A >>=*B;			break; 
		case ALU_CMP:	*FL = *A == *B	? 
							1 : *A > *B	?
								2 : 4;		break;
		default: printf("Get ALU rekt\n"); 	exit(1);
  	}
}

/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
  	int run = 1; 
 	int IR, ab, opA, opB;

  	// main loop
  	while (run) 
	{
		// if interrupts enabled:
			// see if a  timer interrupt occurred
			// 		if so, set bit 0 of IS

			// see if a  keyboard interrupt occurred
			// 		if so, set bit 1 of IS
			
			// maskedInterupts = IS & IM

			// is maskedInterupts nonzero?
			// if so:
			//		Scan maskedInterupts looking for lowest interrupt
			//		Set that bit to 0 in IS
			// 		Push everything on the stack 
			//		Disable interupts
			//		set the PC to the interrupt handler address

		IR = ram_r(cpu, 0, -1, -1);
		ab = ((IR >> DATA_LEN) & 0b11) + 1; // advance by
		opA = ram_r(cpu, 1, -1, -1);
		opB = ram_r(cpu, 2, -1, -1);

		// helpers: because the switch statement is crazy otherwise
		unsigned char *r_opA = &cpu->reg[opA];			// Register value @ opA
		unsigned char *r_opB = &cpu->reg[opB];			// Register value @ opB
		int *PC = &cpu->PC; 							// Process Counter
		int *FL = &cpu->FL; 							// Flags
		int rjv = *r_opA-ab; 							// register jump value

		// main switch
		switch (IR) 
		{	// ALU operations 
			case ADD :	alu(cpu, ALU_ADD, opA, opB); 			break; // Basic Math
			case SUB :	alu(cpu, ALU_SUB, opA, opB); 			break;
			case MUL :	alu(cpu, ALU_MUL, opA, opB); 			break;
			case DIV :	if(*r_opB){alu(cpu,ALU_DIV,opA,opB);}	// div by 0 ERR	   
			    else	{run = 0;printf("ERR: div by 0.\n");}	break;
			case MOD :	if(*r_opB){alu(cpu,ALU_MOD,opA,opB);}	// div by 0 ERR
			    else	{run = 0;printf("ERR: div by 0.\n");}	break;
			case NOT :	alu(cpu, ALU_NOT, opA, opB);			break; // Bitwise
			case AND : 	alu(cpu, ALU_AND, opA, opB); 			break;
			case OR  :	alu(cpu, ALU_OR,  opA, opB);			break;
			case XOR :	alu(cpu, ALU_XOR, opA, opB);			break;
			case CMP : 	alu(cpu, ALU_CMP, opA, opB);			break; // Other
			case DEC :	alu(cpu, ALU_DEC, opA, opB);			break;
			case INC :	alu(cpu, ALU_INC, opA, opB);			break;
			case SHL : 	alu(cpu, ALU_SHL, opA, opB);			break;
			case SHR : 	alu(cpu, ALU_SHR, opA, opB);			break;
			// CPU processes
			case PUSH:	s_push(cpu, *r_opA);					break; // Stack
			case POP :	*r_opA = s_pop(cpu); 					break;
			case CALL: 	s_push(cpu, *PC+ab); *PC = rjv;			break; // Subroutines 
			case RET :	*PC = s_pop(cpu)-ab; 					break;
			case INT :											break; // Interupts
			case IRET:											break;
			case JMP : 	*PC = rjv;								break; // Jumps
			case JNE :	*PC = *FL!=1 			? 	rjv : *PC;	break;	// !=
			case JEQ :	*PC = *FL==1 			? 	rjv : *PC; 	break; 	// ==
			case JGE :	*PC = *FL==2 || *FL==1	? 	rjv : *PC;	break;	// >= 
			case JGT :	*PC = *FL==2 			? 	rjv : *PC;	break;	// >
			case JLE :	*PC = *FL==4 || *FL==1	? 	rjv : *PC;	break;	// <=
			case JLT :	*PC = *FL==4 			? 	rjv : *PC;	break;	// <
			case LD  : 	*r_opA = ram_r(cpu, 0, -1, *r_opB);		break; // Load
			case LDI : 	*r_opA = opB; 							break;
			case ST  :	ram_w(cpu, *r_opA, *r_opB);				break;
			case PRN : 	printf("%d\n", *r_opA);					break; // Printing
			case PRA : 	printf("%c", *r_opA);					break;
			case NOP :	/* do nothing LOL */					break; // Admin
			case HLT : 	run = 0; 								break;
			default  :	printf("lol, Get rekt. %d\n", IR);		exit(1);
		}
		*PC += ab;
  	}
} 

/**
 * helper functions
 */
int ram_r(struct cpu *cpu, int offset, int r_addr, int m_addr)
{
	return r_addr==-1 && m_addr==-1 ? // lol, this formatting? Regular C ain't any better
		cpu->ram[cpu->PC + offset]	/* reading PC directions from a program	*/	: m_addr==-1 ? 
		cpu->ram[cpu->reg[r_addr]] 	/* reading memory by a register address	*/  : 
		cpu->ram[m_addr];			/* reading memory by a memory address 	*/
}

void ram_w(struct cpu *cpu, int address, unsigned int value)
{
	cpu->ram[address] = value;
}

void s_push(struct cpu *cpu, int value)
{
	cpu->reg[7]--;
	ram_w(cpu, cpu->reg[7], value);
}

int s_pop(struct cpu *cpu)
{
	int temp = ram_r(cpu, 0, 7, -1);
	cpu->reg[7]++; 
	return temp;
}
