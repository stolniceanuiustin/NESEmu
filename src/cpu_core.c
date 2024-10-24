#include "cpu_core.h"
#include "cpu_operations.h"
#include "tracer.h"
const int CLOCK_TIME = 1;

// These functions don't take PC as we need compiler optimisations.
byte read_byte(byte *address)
{
    // sleep(CLOCK_TIME);
    return (*address);
}

void write_byte(byte *address, byte value)
{
    // sleep(CLOCK_TIME);
    sleep(0.1);
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
uint16_t read_abs_address(byte *ram, uint16_t offset)
{
    uint16_t val = read_byte(ram + offset + 1);
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

bool compute_addr_mode_g23(CPU_t *cpu, bool *page_cross, uint16_t *offset_address, byte **address_to_return)
{
    uint16_t address = 0;
    (*page_cross) = false;
    if(cpu->inst.opcode != 0x004C && cpu->inst.opcode != 0x0060) // EXCLUDE THE JUMPS;
    {
    switch (cpu->inst.bbb)
    {
    case 0x0: // #immediate
        address = (cpu->PC)++;
        break;

    case 0x1: // zero page
        address = read_pc(cpu);
        break;

    case 0x2: // accumulator. there is no address
        return false;

    case 0x3: // absolute
        address = read_abs_address(cpu->ram, cpu->PC);
        cpu->PC += 2;
        break;

    case 0x4:
        printf("INVALID OPCODE\n");
        break;

    case 0x5: // zero page, x
        // STX, LDX use zero page, y not x
        if ((cpu->inst.aaa == 0x4 || cpu->inst.aaa == 0x5) && cpu->inst.cc == 0x2)
        {
            address = read_pc(cpu) + cpu->Y;
            address &= 0xFF;
        }
        else
        {
            address = read_pc(cpu) + cpu->X;
            address &= 0xFF;
        }
        break;

    case 0x6:
        printf("INVALID OPCODE");
        break;

    case 0x7: // absolute, x
        address = read_address(cpu->ram, cpu->PC);
        cpu->PC += 2;
        // LTX: absolute, y instead of X
        if (cpu->inst.aaa == 0x7 && cpu->inst.cc == 0x2)
        {
            int first_digit_address = address >> 12; // We get the first 4 bits -> one digit in Hexa
            address += cpu->Y;
            int first_digit_address_after_increment = address >> 12;
            if (first_digit_address < first_digit_address_after_increment)
                (*page_cross) = true;
            break;
        }
        else
            address += cpu->X;
        break;
    }
    }
    (*offset_address) = address;
    (*address_to_return) = cpu->ram + address;

    return true;
}

byte *compute_addr_mode_g1(CPU_t *cpu, bool *page_cross, uint16_t *offset_address) // Calculates the address based on the addresation mode
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
        address = read_abs_address(cpu->ram, cpu->PC);
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
    (*offset_address) = address;
    return cpu->ram + address;
}
void run_instruction_group1(byte *address, CPU_t *cpu, bool page_cross)
{
    switch (cpu->inst.aaa)
    {
    case 0x0:
        ORA(cpu, address, page_cross);
        break;
    case 0x1:
        AND(cpu, address, page_cross);
        break;
    case 0x2:
        EOR(cpu, address, page_cross);
        break;
    case 0x3:
        ADC(cpu, address, page_cross);
        break;
    case 0x4:
        STA(cpu, address);
        break;
    case 0x5:
        LDA(cpu, address, page_cross);
        break;
    case 0x6:
        CMP(cpu, address, page_cross);
        break;
    case 0x7:
        SBC(cpu, address, page_cross);
        break;
    }
}

void run_instruction_group2(byte *address, CPU_t *cpu, bool page_cross, bool accumulator)
{
    switch (cpu->inst.aaa)
    {
    case 0x0:
        ASL(cpu, address, accumulator);
        break;
    case 0x1:
        ROL(cpu, address, accumulator);
        break;
    case 0x2:
        LSR(cpu, address, accumulator);
        break;
    case 0x3:
        ROR(cpu, address, accumulator);
        break;
    case 0x4:
        STX(cpu, address);
        break;
    case 0x5:
        LDX(cpu, address, page_cross);
        break;
    case 0x6:
        DEC(cpu, address);
        break;
    case 0x7:
        INC(cpu, address);
        break;
    }
}

void run_instruction_group3(byte *address, CPU_t *cpu, bool page_cross, uint16_t offset_address)
{
    uint16_t jump_address = 0;
    (void)page_cross;
    (void)offset_address;
    switch (cpu->inst.aaa)
    {
    case 0x0:
        printf("INVALID OPCODE \n");
        break;
    case 0x1:
        BIT(cpu, address);
        break;
    case 0x2:
        jump_address = read_abs_address(cpu->ram, cpu->PC);
        cpu->PC += 2;
        JMP_abs(cpu, jump_address);
        break;
    case 0x3:
        jump_address = read_abs_address(cpu->ram, cpu->PC);
        cpu->PC += 2;
        JMP_indirect(cpu, jump_address);
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

int execute_cpu(CPU_t *cpu)
{
    // printf("%d\n", cpu->PC);
    //SLEEP CODE(have no idea)
    struct timespec ts;
    ts.tv_sec = 0;            // Seconds
    ts.tv_nsec = 500000000L;  // Nanoseconds (0.5 seconds)

    nanosleep(&ts, NULL);
    
    
    bool onaddress_group2 = false;
    uint16_t offset_address = 0;
    uint16_t original_pc = cpu->PC;
    bool page_cross = false;

    if (original_pc == 0xFFFF)
    {
        printf("END OF PROGRAM \n");
        cpu->state = QUIT;
        return -1;
    }
    cpu->inst.opcode = read_pc(cpu);
    if (cpu->inst.opcode == 0)
    {
        // printf("%d : Opcode shouldn't be 0\n", cpu->PC);
        return 1;
    }

    // cpu->PC += 1;
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
    case 0x01: // cc = 1
        address = compute_addr_mode_g1(cpu, &page_cross, &offset_address);
        run_instruction_group1(address, cpu, page_cross);
        break;
    case 0x02: // cc = 10
        onaddress_group2 = compute_addr_mode_g23(cpu, &page_cross, &offset_address, &address);
        if (onaddress_group2 == true)
            run_instruction_group2(address, cpu, page_cross, 0); // Not accumulator, on address
        else
            run_instruction_group2(NULL, cpu, page_cross, 1); // On accumulator
        break;
    case 0x0: // cc = 00
        compute_addr_mode_g23(cpu, &page_cross, &offset_address, &address);
        run_instruction_group3(address, cpu, page_cross, offset_address);
        break;
    }

    tracer(cpu, offset_address, page_cross, original_pc, onaddress_group2);
    (void)original_pc;
    return 1;
}
