#include "cpu_core.c"
#include "tracer.h"
// WILL TRACE

void trace_instruction_group1(CPU_t *cpu, bool page_cross)
{
    switch (cpu->inst.aaa)
    {
    case 0x0:
        printf("ORA - XOR on accumulator\n. Page cross:%01d\n. Cycles:%01d", (int)page_cross, cpu->cycles);
        break;
    case 0x1:
        printf("AND - with accumulator. Page cross:%01d\n. Cycles:%01d", (int)page_cross, cpu->cycles);
        break;
    case 0x2:
        // EOR();
        break;
    case 0x3:
        // ADC();
        break;
    case 0x4:
        // STA();
        break;
    case 0x5:
        // LDA();
        break;
    case 0x6:
        // CMP();
        break;
    case 0x7:
        // SBC();
        break;
    }
}

void tracer(CPU_t *cpu, uint16_t addr_tracer, bool page_cross)
{
    //ADDRESSING MODES AT https://llx.com/Neil/a2/opcodes.html
    printf("Address: %04X, AddrMode: %03X, Instruction: ", addr_tracer, cpu->inst.bbb);
    
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
        trace_instruction_group1(cpu, page_cross);
        break;

    case 0x02:
        // address = decode_addrmode_g2(cpu);
        // run_insturction_group2(address, cpu);
        break;

    case 0x0:
        // Branching operations: xxy 100 00. deci if(cc == 00 and bb == 100 => branching)
        // xx indicates a flag. 00 = negative, o1 = overflow, 10 = carry, 11 = zero
        if (cpu->inst.bbb == 0x4)
        {
            // branching.. will do later
        }
    default:
        break;
    }
}
