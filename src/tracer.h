#ifndef HEADERGUARD_TRACER
#define HEADERGUARD_TRACER
#include "cpu_types.h"
void tracer(CPU_t* cpu, uint16_t addr_tracer, bool page_cross, uint16_t PC);


#endif