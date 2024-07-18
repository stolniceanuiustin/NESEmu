#ifndef HEADERGUARD_OPERATIONS
#define HEADERGUARD_OPERATIONS
#include "cpu_types.h"


//GROUP 1 OPERATIONS
void ORA(CPU_t* cpu, byte* address, bool page_cross);
void AND(CPU_t* cpu, byte* address, bool page_cross);
void EOR(CPU_t* cpu, byte* address, bool page_cross);
void ADC(CPU_t* cpu, byte* address, bool page_cross);
void STA(CPU_t* cpu, byte* address);
void LDA(CPU_t* cpu, byte* address, bool page_cross);
void CMP(CPU_t* cpu, byte* address, bool page_cross);
void SBC(CPU_t* cpu, byte* address, bool page_cross);
#endif