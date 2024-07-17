#ifndef HEADERGUARD_CORE
#define HEADERGUARD_CORE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include "cpu_operations.h"
#include "tracer.h"
#include "cpu_init.h"


byte read_byte(byte* address);
void write_byte(byte* address, byte value);
uint16_t read_address(byte* ram, byte offset);
void execute_cpu(CPU_t* cpu);

#endif