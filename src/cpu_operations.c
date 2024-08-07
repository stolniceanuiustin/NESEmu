#include "cpu_operations.h"

void set_ZN(CPU_t *cpu, byte value);
void set_overflow(CPU_t *cpu);
void clear_overflow(CPU_t *cpu);
void set_carry(CPU_t *cpu);
void clear_carry(CPU_t *cpu);
void set_ZN_no_clear(CPU_t *cpu, byte value);

byte read_byte_local(byte *address)
{
    // sleep(CLOCK_TIME);
    return (*address);
}

uint16_t read_address_local(byte *address)
{
    uint16_t val = read_byte_local(address + 1); // little endian
    val <<= 8;
    val |= read_byte_local(address);
    return (val);
}

uint16_t read_abs_address_local(CPU_t* cpu, uint16_t offset)
{
    uint16_t val = read_byte_local(cpu->ram + offset + 1); // little endian
    val <<= 8;
    val |= read_byte_local(cpu->ram + offset);
    return (val);
}


// Here will be all the helper functions for emulating the instructions

// GROUP 1 INSTRUCTIOS
void ORA(CPU_t *cpu, byte *address, bool page_cross)
{
    // OR between accumulator and the contents at the given address
    const int lookup[] = {6, 3, 2, 4, 5, 4, 4, 4};
    cpu->A = cpu->A | (*address);
    set_ZN(cpu, cpu->A);

    cpu->cycles += lookup[cpu->inst.bbb] + (size_t)page_cross;
}

void AND(CPU_t *cpu, byte *address, bool page_cross)
{
    const int lookup[] = {6, 3, 2, 4, 5, 4, 4, 4};
    cpu->A = cpu->A & (*address);
    set_ZN(cpu, cpu->A);

    cpu->cycles += lookup[cpu->inst.bbb] + (size_t)page_cross;
}

void EOR(CPU_t *cpu, byte *address, bool page_cross)
{
    const int lookup[] = {6, 3, 2, 4, 5, 4, 4, 4};
    cpu->A = cpu->A ^ (*address);
    set_ZN(cpu, cpu->A);

    cpu->cycles += lookup[cpu->inst.bbb] + (size_t)page_cross;
}

void ADC(CPU_t *cpu, byte *address, bool page_cross)
{
    // TODO: TEST THIS
    const int lookup[] = {6, 3, 2, 4, 5, 4, 4, 4};
    uint16_t result = (uint16_t)cpu->A + (uint16_t)(*address) + (uint16_t)((cpu->SR & CARRY) != 0);
    bool overflow_check = (~(cpu->A ^ (*address)) & (cpu->A ^ result) & 0x80) != 0;
    cpu->SR &= ~(CARRY | OVERFLOW | NEGATIVE | ZERO);
    cpu->SR |= (result & 0xFF00 ? CARRY : 0);

    if (overflow_check)
    {
        set_overflow(cpu);
        set_carry(cpu);
    }
    else
        clear_overflow(cpu), clear_carry(cpu);

    cpu->A = (byte)result;
    set_ZN(cpu, cpu->A);

    cpu->cycles += lookup[cpu->inst.bbb] + (size_t)page_cross;
}

void STA(CPU_t *cpu, byte *address)
{
    // Can't use IMEDIATE ADDRESSING
    // Stores the contects of the accumulator in memory
    // Doesnt change flags
    const int lookup[] = {6, 3, -1, 4, 6, 4, 5, 5};
    (*address) = cpu->A;
    cpu->cycles += lookup[cpu->inst.bbb];
}

void LDA(CPU_t *cpu, byte *address, bool page_cross)
{
    const int lookup[] = {6, 3, 2, 4, 5, 4, 4, 4};
    cpu->A = (*address);
    set_ZN(cpu, cpu->A);
    cpu->cycles += lookup[cpu->inst.bbb] + (size_t)page_cross;
}

void CMP(CPU_t *cpu, byte *address, bool page_cross)
{
    // not sure if the flags are set correctly but i think they are
    const int lookup[] = {6, 3, 2, 4, 5, 4, 4, 4};

    uint16_t result = cpu->A - (*address);
    cpu->SR &= ~(CARRY | NEGATIVE | ZERO);
    if (!(result & 0xFF00))
        set_carry(cpu);
    set_ZN(cpu, result);
    cpu->cycles += lookup[cpu->inst.bbb] + (size_t)page_cross;
}
void SBC(CPU_t *cpu, byte *address, bool page_cross)
{
    const int lookup[] = {6, 3, 2, 4, 5, 4, 4, 4};

    uint16_t result = cpu->A - (*address) - !(cpu->SR & CARRY);
    cpu->SR &= ~(CARRY | ZERO | OVERFLOW | NEGATIVE);
    set_ZN_no_clear(cpu, result);
    if (!(result & 0xFF00))
        set_carry(cpu);

    if ((cpu->A ^ result) & (~(*address) ^ result) & (1 << 7))
        set_overflow(cpu);

    cpu->A = (byte)result;
    cpu->cycles += lookup[cpu->inst.bbb] + (size_t)page_cross;
}

// GROUP 2 INSTRUCTIONS
// TODO: can make this functions faster by not clearing first/last bit, clearing all flags at once and doing a set ZN with no clear
void ASL(CPU_t *cpu, byte *address, bool accumulator)
{
    // Arithmetic shift left
    // Carry is set to old bit 7
    // Bit 0 is set to 0
    // standard set_zn;
    int lookup[] = {-1, 5, 2, 6, -1, 6, -1, 7}; // imediate, zero page, accumulator, absolute, NIMIC, zeropage,x , NIMIC,  absolute,x}
    if (accumulator)
    {
        int carry_flag = cpu->A & (1 << 7);
        if (carry_flag)
        {
            set_carry(cpu);
        }
        clear_carry(cpu);
        cpu->A = cpu->A << 1;
        set_ZN(cpu, cpu->A);
    }
    else
    {
        int carry_flag = (*address) & (1 << 7);
        if (carry_flag)
        {
            set_carry(cpu);
        }
        clear_carry(cpu);
        (*address) = (*address) << 1;
        set_ZN(cpu, (*address));
    }

    cpu->cycles += lookup[cpu->inst.bbb];
}
void ROL(CPU_t *cpu, byte *address, bool accumulator)
{
    // Rotate left
    int lookup[] = {-1, 5, 2, 6, -1, 6, -1, 7};
    if (accumulator)
    {
        int carry_flag = (cpu->A) & (1 << 7);

        cpu->A = cpu->A << 1;
        cpu->A &= (~(CARRY));        // clear last bit(should be clear already);
        cpu->A |= (cpu->SR & CARRY); // set last bit to carry flag
        if (carry_flag)
        {
            set_carry(cpu);
        }
        else
            clear_carry(cpu);
        set_ZN(cpu, cpu->A);
    }
    else
    {
        int carry_flag = (*address) & (1 << 7);

        (*address) = (*address) << 1;
        (*address) &= (~CARRY); // clear last bit(should be clear already);
        (*address) |= (CARRY);  // set last bit to carry flag

        if (carry_flag)
        {
            set_carry(cpu);
        }
        else
            clear_carry(cpu);

        set_ZN(cpu, (*address));
    }

    cpu->cycles += lookup[cpu->inst.bbb];
}
void ROR(CPU_t *cpu, byte *address, bool accumulator)
{
    // Rotate right
    int lookup[] = {-1, 5, 2, 6, -1, 6, -1, 7};
    if (accumulator)
    {
        int carry_flag = (cpu->A) & 1;
        cpu->A = cpu->A >> 1;
        cpu->A &= ~(1 << 7);              // clear first bit(should be clear already);
        cpu->A |= (cpu->SR & CARRY) << 7; // set first bit to carry flag

        if (carry_flag)
            set_carry(cpu);
        else
            clear_carry(cpu);
        set_ZN(cpu, cpu->A);
    }
    else
    {
        int carry_flag = (*address) & 1;

        (*address) = (*address) >> 1;
        (*address) &= ~(1 << 7);              // clear first bit(should be clear already);
        (*address) |= (cpu->SR & CARRY) << 7; // set first bit to carry flag

        if (carry_flag)
            set_carry(cpu);
        else
            clear_carry(cpu);
        set_ZN(cpu, (*address));
    }

    cpu->cycles += lookup[cpu->inst.bbb];
}
void LSR(CPU_t *cpu, byte *address, bool accumulator)
{
    // Logical shift right
    int lookup[] = {-1, 5, 2, 6, -1, 6, -1, 7};
    if (accumulator)
    {
        int carry_flag = (cpu->A) & 1;
        cpu->A = cpu->A >> 1;
        cpu->A &= ~(1 << 7); // clear first bit(should be clear already);

        if (carry_flag)
            set_carry(cpu);
        else
            clear_carry(cpu);
        set_ZN(cpu, cpu->A);
    }
    else
    {
        int carry_flag = (*address) & 1;

        (*address) = (*address) >> 1;
        (*address) &= ~(1 << 7); // clear first bit(should be clear already);

        if (carry_flag)
            set_carry(cpu);
        else
            clear_carry(cpu);
        set_ZN(cpu, (*address));
    }

    cpu->cycles += lookup[cpu->inst.bbb];
}
void STX(CPU_t *cpu, byte *address)
{
    // Curiously, doesn't affect any flag whatsoever.
    int lookup[] = {-1, 3, -1, 4, -1, 4, -1, -1};
    (*address) = cpu->X;
    cpu->cycles += lookup[cpu->inst.bbb];
}
void LDX(CPU_t *cpu, byte *address, bool page_cross)
{
    int lookup[] = {2, 3, -1, 4, -1, 4, -1, 4};
    cpu->X = (*address);
    set_ZN(cpu, (*address));
    cpu->cycles += lookup[cpu->inst.bbb] + (int)page_cross;
}
void INC(CPU_t *cpu, byte *address)
{
    int lookup[] = {-1, 5, -1, 6, -1, 6, -1, 7};
    (*address) += 1;
    set_ZN(cpu, (*address));
    cpu->cycles += lookup[cpu->inst.bbb];
}
void DEC(CPU_t *cpu, byte *address)
{
    int lookup[] = {-1, 5, -1, 6, -1, 6, -1, 7};
    (*address) -= 1;
    set_ZN(cpu, (*address));
    cpu->cycles += lookup[cpu->inst.bbb];
}

// GROUP3 OPERATIONS
void BIT(CPU_t *cpu, byte *address)
{
    int lookup[] = {-1, 3, -1, 4, -1, -1, -1, -1};
    uint8_t result = cpu->A & (*address);
    cpu->SR &= ~(ZERO | NEGATIVE);
    if (!result)
        cpu->SR |= ZERO;

    // This sets the overflow flag
    cpu->SR |= ((*address) & OVERFLOW);

    // This sets the negative flag
    cpu->SR |= ((*address) & NEGATIVE);

    cpu->cycles += lookup[cpu->inst.bbb];
}

void JMP_indirect(CPU_t *cpu, uint16_t offset_address)
{
    cpu->PC = read_abs_address_local(cpu, offset_address);
    cpu->cycles += 5;
}

void JMP_abs(CPU_t *cpu, uint16_t offset_address)
{
    cpu->PC = offset_address;
    cpu->cycles += 3;
}

void set_ZN(CPU_t *cpu, byte value)
{
    cpu->SR &= ~(ZERO | NEGATIVE); // SETS FLAG to 0 THEN ORS
    cpu->SR |= ((!value) ? ZERO : 0);
    cpu->SR |= (value & NEGATIVE);
}

// FASTER THAN SET ZN IF FLAGS ALREADY CLEARED
void set_ZN_no_clear(CPU_t *cpu, byte value)
{
    cpu->SR |= ((!value) ? ZERO : 0);
    cpu->SR |= (value & NEGATIVE);
}

void set_overflow(CPU_t *cpu)
{
    // i dont think the first operation is necesary
    // cpu->SR &= ~(OVERFLOW);
    cpu->SR |= OVERFLOW;
}

void clear_overflow(CPU_t *cpu)
{
    cpu->SR &= ~(OVERFLOW);
}

void set_carry(CPU_t *cpu)
{
    cpu->SR |= CARRY;
}

void clear_carry(CPU_t *cpu)
{
    cpu->SR &= ~(CARRY);
}