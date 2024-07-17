
#include "cpu_core.h"

const int CLOCK_TIME = 1;

// These functions don't take PC as we need compiler optimisations.
byte read_byte(byte *address)
{
    sleep(CLOCK_TIME);
    return (*address);
}

void write_byte(byte *address, byte value)
{
    // sleep(CLOCK_TIME);
    *address = value;
    return;
}

uint16_t read_address(byte *ram, byte offset)
{
    uint16_t val = read_byte(ram + offset + 1); // little endian
    val <<= 8;
    val |= read_byte(ram + offset);
    return (val);
}

// TODO: Optimise this maybe
byte read_pc(CPU_t *cpu)
{
    byte val = read_byte(cpu->ram + cpu->PC);
    (cpu->PC)++;
    return (val);
}

byte *compute_addr_mode_g1(CPU_t *cpu, bool *page_cross, int* addr_tracer) // Calculates the address based on the addresation mode
{
    uint16_t address = 0; // The first byte must be 0
    (*page_cross) = false;
    switch (cpu->inst.bbb)
    {
    case 0x0: //(ZERO PAGE, X). Next byte + X represents an adress in zero page that needs to be refferenced
        address = read_pc(cpu) + cpu->X;
        address &= 0xFF; // The first byte must be 0
        address = read_address(cpu->ram, address);
        break;

    case 0x01: // ZERO PAGE.
        address = read_pc(cpu);
        break;

    case 0x02: // Immediate. The opperand is in the next byte of memory
        address = (cpu->PC)++;
        break;

    case 0x03: // Absolute. The full 16-bit address
        address = read_address(cpu->ram, cpu->PC);
        cpu->PC += 2;
        break;

    case 0x04: //(zero page), Y; Takes an address from zero page as a pointer then adds Y to that address
        // CAN CROSS PAGES
        uint16_t alligment = 0;
        address = read_pc(cpu) + alligment;
        address &= 0xFF;
        address = read_address(cpu->ram, address);
        if (address + cpu->Y > 0xFF) // We crossed a page
            (*page_cross) = true;
        address += cpu->Y;
        break;

    case 0x05: // zero page, X; basically zeropage_addres + x
        address = read_pc(cpu) + cpu->X;
        address &= 0xFF;
        break;

    case 0x06: // absolute, Y; The full 16-bit address is in memory and we have to add Y
        // CAN CROSS PAGES
        // TODO: im not sure this is right
        address = read_address(cpu->ram, cpu->PC);
        int first_digit_address = address >> 12; // We get the first 4 bits -> one digit in Hexa
        cpu->PC += 2;
        address += cpu->Y;
        int first_digit_address_after_increment = address >> 12;
        if (first_digit_address < first_digit_address_after_increment)
            (*page_cross) = true;
        break;

    case 0x07: // Absolute, X. the same as before
        // CAN CROSS PAGES
        address = read_address(cpu->ram, cpu->PC);
        int first_digit_address_1 = address >> 12;
        cpu->PC += 2;
        address += cpu->X;
        int first_digit_address_after_increment_1 = address >> 12;
        if (first_digit_address_1 < first_digit_address_after_increment_1)
            (*page_cross) = true;
        break;
    }
    // We got the offset of the address but we have to return it as a pointer, so we return it as an offset to ram
    (*addr_tracer) = address;
    return cpu->ram + address;
}

void run_instruction_group1(byte *address, CPU_t *cpu, bool page_cross)
{
    (void)address;
    (void)page_cross;
    switch (cpu->inst.aaa)
    {
    case 0x0:
        // ORA();
        break;
    case 0x1:
        // AND();
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

//SET FLAGS FUNCTIONS


void execute_cpu(CPU_t *cpu)
{
    uint16_t addr_tracer = 0;
    bool page_cross = false;
    
    cpu->inst.opcode = read_pc(cpu);
    if (cpu->inst.opcode == 0)
    {
        printf("Invalid opcode 0\n");
        return;
    }
    cpu->PC += 1;
    cpu->inst.aaa = (0xE0 & cpu->inst.opcode) >> 5; // first 3 bits of the opcode
    cpu->inst.bbb = (0x1C & cpu->inst.opcode) >> 2;
    cpu->inst.cc = (0x03 & cpu->inst.opcode);
    byte low_nibble = cpu->inst.opcode & 0x0F;
    byte high_nibble = cpu->inst.opcode >> 4;
    byte *address = 0; // address will be calculated by some abstract function based on the addressing mode
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

        address = compute_addr_mode_g1(cpu, &page_cross, &addr_tracer);
        run_instruction_group1(address, cpu, page_cross);
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
    }
    #ifdef debug
        tracer(cpu, addr_tracer, page_cross);
    #endif
    //(void)address;
}
