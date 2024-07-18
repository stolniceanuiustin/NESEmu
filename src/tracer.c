#include "tracer.h"
// WILL TRACE

void print_flags(byte SR)
{
    for(int i=7; i>=0; i--)
    {
        byte byte1 = (SR >> i) & 1;
        printf("%u", byte1);
    }
    printf("\n");
}

void trace_instruction_group1(CPU_t *cpu, bool page_cross)
{
    //PCR STANDS FOR PAGE CROSS, booolean
    switch (cpu->inst.aaa)
    { 
    case 0x0:
        printf("ORA - OR with accumulator. PCR:%01d. Cycles:%01lu. Flags: ", (int)page_cross, cpu->cycles);
        print_flags(cpu->SR);
        break;
    case 0x1:
        printf("AND - with accumulator. PCR:%01d. Cycles:%01lu\n. Flags: ", (int)page_cross, cpu->cycles);
        print_flags(cpu->SR);
        break;
    case 0x2:
        printf("EOR - XOR with accumulator. PCR:%01d. Cycles:%01lu\n. Flags: ", (int)page_cross, cpu->cycles);
        print_flags(cpu->SR);
        break;
    case 0x3:
        // ADC();
        printf("ADC - Add A + M with C. PCR:%01d. Cycles:%01lu\n. Flags: ", (int)page_cross, cpu->cycles);
        print_flags(cpu->SR);
        break;
    case 0x4:
        // STA();
        printf("STA - Store A at M. PCR:%01d. Cycles:%01lu\n. Flags: ", (int)page_cross, cpu->cycles);
        print_flags(cpu->SR);
        break;
    case 0x5:
        // LDA();
        printf("LDA - Load A from M. PCR:%01d. Cycles:%01lu\n. Flags: ", (int)page_cross, cpu->cycles);
        print_flags(cpu->SR);
        break;
    case 0x6:
        // CMP();
        printf("CMP - CMP A with M. PCR:%01d. Cycles:%01lu\n. Flags: ", (int)page_cross, cpu->cycles);
        print_flags(cpu->SR);
        break;
    case 0x7:
        // SBC();
        printf("SBC - Sub A with M with borrow. PCR:%01d. Cycles:%01lu\n. Flags: ", (int)page_cross, cpu->cycles);
        print_flags(cpu->SR);
        break;
    default: printf("Unimplemented opcode\n");
    }
}

void tracer(CPU_t* cpu, uint16_t addr_tracer, bool page_cross, uint16_t PC)
{
    //ADDRESSING MODES AT https://llx.com/Neil/a2/opcodes.html
    (void)addr_tracer;
    printf("Address: %04X, Opcode: %02X, AddrMode: %03X, Instruction: ", PC, cpu->inst.opcode, cpu->inst.bbb);
    
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
        printf("Unimplemented opcode\n");
        // address = decode_addrmode_g2(cpu);
        // run_insturction_group2(address, cpu);
        break;

    case 0x0:
        // Branching operations: xxy 100 00. deci if(cc == 00 and bb == 100 => branching)
        // xx indicates a flag. 00 = negative, o1 = overflow, 10 = carry, 11 = zero
        printf("Unimplemented opcode\n");
        if (cpu->inst.bbb == 0x4)
        {
            // branching.. will do later
            break;
        }
        break;
    default:
        printf("Unimplemented opcode\n");
        break;
    }
}
