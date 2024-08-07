#include "tracer.h"
// WILL TRACE

//TODO: CHANGE TRACER SO LOGS ARE MORE COMPACT
byte read_byte_tracer(byte *address)
{
    // sleep(CLOCK_TIME);
    return (*address);
}

uint16_t read_address_tracer(byte *ram, byte offset)
{
    uint16_t val = read_byte_tracer(ram + offset + 1); // little endian
    val <<= 8;
    val |= read_byte_tracer(ram + offset);
    return (val);
}

void print_flags_group1(byte SR)
{
    printf("C:%01d O:%01d Z:%01d N:%01d\n", (SR) & 1, (SR >> 6) & 1, (SR >> 1) & 1, (SR >> 7) & 1);
}

void trace_instruction_group1(CPU_t *cpu, uint16_t offset_address)
{
    // PCR STANDS FOR PAGE CROSS, booolean
    switch (cpu->inst.aaa)
    {
    case 0x0:
        printf("ORA - OR with accumulator. Cycles:%01lu. Flags: ", cpu->cycles);
        print_flags_group1(cpu->SR);
        break;
    case 0x1:
        printf("AND - A(%02d) and M(%02d). Cycles:%01lu. Flags: ", cpu->A, cpu->ram[offset_address], cpu->cycles);
        print_flags_group1(cpu->SR);
        break;
    case 0x2:
        printf("EOR - XOR with accumulator. Cycles:%01lu. Flags: ", cpu->cycles);
        print_flags_group1(cpu->SR);
        break;
    case 0x3:
        // ADC();
        printf("ADC - Add A + M with C. Cycles:%01lu. Flags: ", cpu->cycles);
        print_flags_group1(cpu->SR);
        break;
    case 0x4:
        // STA();
        printf("STA - Store A at M. Cycles:%01lu. Flags: ", cpu->cycles);
        print_flags_group1(cpu->SR);
        break;
    case 0x5:
        // LDA();
        printf("LDA - Load A(%02X) from M. Cycles:%01lu. Flags: ", cpu->A, cpu->cycles);
        print_flags_group1(cpu->SR);
        break;
    case 0x6:
        // CMP();
        printf("CMP - CMP A(%02X) with M(%02X). Cycles:%01lu. Flags: ", cpu->A, cpu->ram[offset_address], cpu->cycles);
        print_flags_group1(cpu->SR);
        break;
    case 0x7:
        // SBC();
        printf("SBC - Sub A with M with borrow. Cycles:%01lu. Flags: ", cpu->cycles);
        print_flags_group1(cpu->SR);
        break;
    default:
        printf("Unimplemented opcode\n");
    }
}

void trace_instruction_group2(CPU_t *cpu, uint16_t offset_address, bool onaddress_group2)
{
    // TODO: could add results but unsure if worth it. flags are more significant here i think.
    switch (cpu->inst.aaa)
    {
    case 0x0:
        if (onaddress_group2)
        {
            printf("ASL - Shift left arithmetic. Value(%02X), Result(%02X), Cycles:%01lu, Flags: ", cpu->ram[offset_address], (cpu->ram[offset_address] << 1), cpu->cycles);
            print_flags_group1(cpu->SR);
        }
        else
        {
            printf("ASL - Shift left arithmetic. A(%02X), Result(%02X), Cycles:%01lu, Flags: ", cpu->A, cpu->A << 1, cpu->cycles);
            print_flags_group1(cpu->SR);
        }
        break;
    case 0x1:
        if (onaddress_group2)
        {
            printf("ROL - Rotate left. Value(%02X), Cycles:%01lu, Flags: ", cpu->ram[offset_address], cpu->cycles);
            print_flags_group1(cpu->SR);
        }
        else
        {
            printf("ROL - Rotate left. A(%02X), Cycles:%01lu, Flags: ", cpu->A, cpu->cycles);
            print_flags_group1(cpu->SR);
        }
        break;
    case 0x2:
        if (onaddress_group2)
        {
            printf("LSR - Logic shift right. Value(%02X), Cycles:%01lu, Flags: ", cpu->ram[offset_address], cpu->cycles);
            print_flags_group1(cpu->SR);
        }
        else
        {
            printf("LSR - Logic shift right. A(%02X), Cycles:%01lu, Flags: ", cpu->A, cpu->cycles);
            print_flags_group1(cpu->SR);
        }
        break;
    case 0x3:
        if (onaddress_group2)
        {
            printf("ROR - Rotate right. Value(%02X), Cycles:%01lu, Flags: ", cpu->ram[offset_address], cpu->cycles);
            print_flags_group1(cpu->SR);
        }
        else
        {
            printf("ROR - Rotate right. A(%02X), Cycles:%01lu, Flags: ", cpu->A, cpu->cycles);
            print_flags_group1(cpu->SR);
        }
        break;
    case 0x4:
        printf("STX - Store X at address. X(%02X), Cycles:%01lu, Flags: Not affected\n", cpu->X, cpu->cycles);
        break;
    case 0x5:
        printf("LDX - Load X from address. X(%02X), Cycles:%01lu, Flags: ", cpu->X, cpu->cycles);
        print_flags_group1(cpu->SR);
        break;
    case 0x6:
        printf("DEC - Decrement one. Value before(%02X), Cycles:%01lu, Flags: ", cpu->ram[offset_address], cpu->cycles);
        print_flags_group1(cpu->SR);
        break;
    case 0x7:
        printf("INC - Increment one. Value before(%02X), Cycles:%01lu, Flags: ", cpu->ram[offset_address], cpu->cycles);
        print_flags_group1(cpu->SR);
        break;
    }
}

void trace_instruction_group3(CPU_t *cpu, uint16_t offset_address)
{
    switch (cpu->inst.aaa)
    {
    case 0x0:
        // printf("INVALID OPCODE \n");
        break;
    case 0x1:
        printf("AND Test - A(%02X), Value(%02X), Cycles:%01lu, Flags: ", cpu->A, cpu->ram[offset_address], cpu->cycles);
        print_flags_group1(cpu->SR);
        break;
    case 0x2:
        printf("JMP(Absolut) to (%04X). Cycles:%01lu, Flags: not affected\n", offset_address, cpu->cycles);
        break;
    case 0x3:
        printf("JMP(Indirect) to (%04X). Cycles:%01lu, Flags: not affected\n", read_address_tracer(cpu->ram, offset_address), cpu->cycles);

        break;
    case 0x4:
        // STY
        break;
    case 0x5:
        // LDY
        break;
    case 0x6:
        // CPY
        break;
    case 0x7:
        // CPX
        break;
    }
}
void tracer(CPU_t *cpu, uint16_t offset_address, bool page_cross, uint16_t PC, bool onaddress_group2)
{
    // ADDRESSING MODES AT https://llx.com/Neil/a2/opcodes.html
    (void)page_cross;
    //printf("Address: %04X, Opcode: %02X, Instruction: ", PC, cpu->inst.opcode);
    printf("%04X %02X ", PC, cpu->inst.opcode);
    byte low_nibble = cpu->inst.opcode & 0x0F;
    byte high_nibble = cpu->inst.opcode >> 4;
    if (low_nibble == 0x08)
    {
        // Single byte instruction type 1
        // run_insturction_sb1(cpu);
    }
    if (low_nibble == 0x0A && high_nibble >= 0x08)
    {
        // Single byte instruction type 2
        // run_instruction_sb2(cpu);
    }
    switch (cpu->inst.cc)
    {
    case 0x01: // cc = 01
        trace_instruction_group1(cpu, offset_address);
        break;

    case 0x02:
        trace_instruction_group2(cpu, offset_address, onaddress_group2);
        // printf("Unimplemented opcode\n");
        //  address = decode_addrmode_g2(cpu);
        //  run_insturction_group2(address, cpu);
        break;

    case 0x0:
        trace_instruction_group3(cpu, offset_address);
        break;
    default:
        printf("Unimplemented opcode\n");
        break;
    }
}
