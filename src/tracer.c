#include "tracer.h"
// WILL TRACE

void print_flags_group1(byte SR)
{
    printf("C:%01d O:%01d Z:%01d N:%01d\n", (SR) & 1, (SR >> 6) & 1, (SR >> 1) & 1, (SR >> 7) & 1);
}

void trace_instruction_group1(CPU_t *cpu, uint16_t addr_tracer)
{
    //PCR STANDS FOR PAGE CROSS, booolean
    switch (cpu->inst.aaa)
    { 
    case 0x0:
        printf("ORA - OR with accumulator. Cycles:%01lu. Flags: ", cpu->cycles);
        print_flags_group1(cpu->SR);
        break;
    case 0x1:
        printf("AND - A(%02d) and M(%02d). Cycles:%01lu. Flags: ", cpu->A, cpu->ram[addr_tracer], cpu->cycles);
        print_flags_group1(cpu->SR);
        break;
    case 0x2:
        printf("EOR - XOR with accumulator. Cycles:%01lu. Flags: ",  cpu->cycles);
        print_flags_group1(cpu->SR);
        break;
    case 0x3:
        // ADC();
        printf("ADC - Add A + M with C. Cycles:%01lu. Flags: ", cpu->cycles);
        print_flags_group1(cpu->SR);
        break;
    case 0x4:
        // STA();
        printf("STA - Store A at M. Cycles:%01lu. Flags: ",  cpu->cycles);
        print_flags_group1(cpu->SR);
        break;
    case 0x5:
        // LDA();
        printf("LDA - Load A(%02X) from M. Cycles:%01lu. Flags: ", cpu->A, cpu->cycles);
        print_flags_group1(cpu->SR);
        break;
    case 0x6:
        // CMP();
        printf("CMP - CMP A(%02X) with M(%02X). Cycles:%01lu. Flags: ",cpu->A, cpu->ram[addr_tracer], cpu->cycles);
        print_flags_group1(cpu->SR);
        break;
    case 0x7:
        // SBC();
        printf("SBC - Sub A with M with borrow. Cycles:%01lu. Flags: ",  cpu->cycles);
        print_flags_group1(cpu->SR);
        break;
    default: printf("Unimplemented opcode\n");
    }
}

void tracer(CPU_t* cpu, uint16_t addr_tracer, bool page_cross, uint16_t PC)
{
    //ADDRESSING MODES AT https://llx.com/Neil/a2/opcodes.html
    (void)page_cross;
    printf("Address: %04X, Opcode: %02X, Instruction: ", PC, cpu->inst.opcode);
    
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
        trace_instruction_group1(cpu, addr_tracer);
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
