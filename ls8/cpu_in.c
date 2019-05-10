#include "cpu.h"
#include <string.h>
#include <stdlib.h>

/**
 * Initialize a CPU struct
 */
void cpu_init(struct cpu *cpu)
{
	memset(cpu->reg, 0, 7);
	cpu->reg[7] = 0xF4; // stack pointer
	cpu->PC = 0;
	cpu->FL = 0;
	memset(cpu->ram, 0, 256);
}