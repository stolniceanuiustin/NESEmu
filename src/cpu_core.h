#ifndef HEADERGUARD_CORE
#define HEADERGUARD_CORE
#include "cpu_types.h"
byte read_byte(byte* address);
void write_byte(byte* address, byte value);
uint16_t read_address(byte* ram, byte offset);
void execute_cpu(CPU_t* cpu);
uint16_t read_abs_address(byte *ram, uint16_t offset);
#endif