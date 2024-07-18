#include "cpu_operations.h"

void set_ZN(CPU_t* cpu, byte value);
void set_overflow(CPU_t* cpu);
void clear_overflow(CPU_t* cpu);
void set_carry(CPU_t* cpu);
void clear_carry(CPU_t* cpu);
void set_ZN_no_clear(CPU_t* cpu, byte value);
//Here will be all the helper functions for emulating the instructions 
void ORA(CPU_t* cpu, byte* address, bool page_cross)
{   
    //OR between accumulator and the contents at the given address
    const int lookup[] = {6, 3, 2, 4, 5, 4, 4, 4};
    cpu->A = cpu->A | (*address);
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

void EOR(CPU_t* cpu, byte* address, bool page_cross)
{
    const int lookup[] = {6, 3, 2, 4, 5, 4, 4, 4};
    cpu->A = cpu->A ^ (*address);
    set_ZN(cpu, cpu->A);

    cpu->cycles += lookup[cpu->inst.bbb] + (size_t)page_cross;
}

void ADC(CPU_t* cpu, byte* address, bool page_cross)
{
    //TODO: TEST THIS 
    const int lookup[] = {6, 3, 2, 4, 5, 4, 4, 4};
    uint16_t result = (uint16_t)cpu->A + (uint16_t)(*address) + (uint16_t)((cpu->SR & CARRY) != 0);
    bool overflow_check = (~(cpu->A ^ (*address)) & (cpu->A ^ result) & 0x80) != 0;
    cpu->SR &= ~(CARRY | OVERFLOW | NEGATIVE | ZERO);
    cpu->SR |= (result & 0xFF00 ? CARRY : 0);

    if(overflow_check)
    {
        set_overflow(cpu);
        set_carry(cpu);
    }
    else clear_overflow(cpu), clear_carry(cpu);
    
    cpu->A = (byte)result;
    set_ZN(cpu, cpu->A);

    cpu->cycles += lookup[cpu->inst.bbb] + (size_t)page_cross;
}

void STA(CPU_t* cpu, byte* address)
{
    //Can't use IMEDIATE ADDRESSING
    //Stores the contects of the accumulator in memory
    //Doesnt change flags
    const int lookup[] = {6, 3, -1, 4, 6, 4, 5, 5};
    (*address) = cpu->A;
    cpu->cycles += lookup[cpu->inst.bbb];
}

void LDA(CPU_t* cpu, byte* address, bool page_cross)
{
    const int lookup[] = {6, 3, 2, 4, 5, 4, 4, 4};
    cpu->A = (*address);
    set_ZN(cpu, cpu->A);
    cpu->cycles += lookup[cpu->inst.bbb] + (size_t)page_cross;
}

void CMP(CPU_t* cpu, byte* address, bool page_cross)
{
    //not sure if the flags are set correctly but i think they are
    const int lookup[] = {6, 3, 2, 4, 5, 4, 4, 4};

    uint16_t result = cpu->A - (*address);
    cpu->SR &= ~(CARRY | NEGATIVE | ZERO);
    if(!(result & 0xFF00))
        set_carry(cpu);
    set_ZN(cpu, result);
    cpu->cycles += lookup[cpu->inst.bbb] + (size_t)page_cross;
}

void SBC(CPU_t* cpu, byte* address, bool page_cross)
{
    const int lookup[] = {6, 3, 2, 4, 5, 4, 4, 4};
    
    uint16_t result = cpu->A - (*address) - !(cpu->SR & CARRY);
    cpu->SR &= ~(CARRY | ZERO | OVERFLOW | NEGATIVE);
    set_ZN_no_clear(cpu, result);
    if(!(result & 0xFF00))
        set_carry(cpu);

    if ((cpu->A ^ result) & (~(*address) ^ result) & (1 << 7))
        set_overflow(cpu);
 
    cpu->A = (byte)result;
    cpu->cycles += lookup[cpu->inst.bbb] + (size_t)page_cross;
}

void set_ZN(CPU_t* cpu, byte value)
{
    cpu->SR &= ~(ZERO | NEGATIVE); //SETS FLAG to 0 THEN ORS 
    cpu->SR |= ((!value) ? ZERO: 0);
    cpu->SR |= (value & NEGATIVE);
}

void set_ZN_no_clear(CPU_t* cpu, byte value)
{
    cpu->SR |= ((!value) ? ZERO: 0);
    cpu->SR |= (value & NEGATIVE);
}

void set_overflow(CPU_t* cpu)
{
    cpu->SR &= ~(OVERFLOW);
    cpu->SR |= OVERFLOW;
}

void clear_overflow(CPU_t* cpu)
{
    cpu->SR &= ~(OVERFLOW);
}

void set_carry(CPU_t* cpu)
{
    cpu->SR |= CARRY;
}

void clear_carry(CPU_t* cpu)
{
    cpu->SR &= ~(CARRY);
}