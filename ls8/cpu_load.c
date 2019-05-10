#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */
void cpu_load(struct cpu *cpu, char *file)
{
    FILE *fp;
    char line[256];
    int address = 0;
    char *endptr;
	unsigned int value;

    fp = fopen(file, "r");
    if (fp == NULL) 
	{
        fprintf(stderr,"comp: error opening file\n");
        exit(2);
    }

    while (fgets(line, 256, fp) != NULL) 
	{
        value = strtoul(line, &endptr, 2);
        if (endptr == line) 
		{ 
            continue; // printf("Found no digits on this line\n");
        }
		ram_w(cpu, address++, value); // printf("%u\n", value);
    }

    fclose(fp);
}