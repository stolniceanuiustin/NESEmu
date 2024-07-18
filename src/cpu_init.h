#ifndef HEADERGUARD_CPU_INIT
#define HEADERGUARD_CPU_INIT
#include "cpu_core.h"

bool init_cpu(CPU_t* cpu, CONFIG_t config);
bool reset_cpu(CPU_t* cpu);
void init_config(CONFIG_t* config, char* rom_name);
void hex_dump(CPU_t cpu);

#endif