#include "cpu_core.h"

void set_ZN(CPU_t* cpu, byte value);

//Here will be all the helper functions for emulating the instructions 
void ORA(CPU_t* cpu, byte* address, bool page_cross)
{   
    //XOR between accumulator and the contents at the given address
    const int lookup[] = {6, 3, 2, 4, 5, 4, 4, 4};
    cpu->A = cpu->A ^ (*address);
    set_ZN(cpu, cpu->A);

    cpu->cycles += lookup[cpu->inst.bbb] + (size_t)page_cross; 
}

void AND(CPU_t* cpu, byte* address, bool page_cross)
{
    const int lookup[] = {6, 3, 2, 4, 5, 4, 4, 4};
    cpu->A = cpu->A & (*address);
    set_ZN(cpu, cpu->A);

    cpu->cycles += lookup[cpu->inst.bbb] + (size_t)page_cross;
}

void set_ZN(CPU_t* cpu, byte value)
{
    //TODO: Verify this, taken from github 
    cpu->SR &= ~(ZERO | NEGATIVE); //SETS FLAG to 0 THEN ORS 
    cpu->SR |= ((!value) ? ZERO: 0);
    cpu->SR |= (value & NEGATIVE);
}