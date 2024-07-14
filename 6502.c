#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include "6502.h"
#include "operations.h"
const int CLOCK_TIME = 1;

byte read_byte(byte* address)
{
    sleep(CLOCK_TIME);
    return (*address);
}

void write_byte(byte* address, byte value)
{
    sleep(CLOCK_TIME);
    *address = value;
    return;
}

uint16_t read_address(CPU_t* cpu, byte offset)
{
    uint16_t val = read_byte(cpu->ram+offset+1); //little endian
    val <<= 8;
    val |= read_byte(cpu->ram+offset);
    return(val);
}

byte read_pc(CPU_t* cpu)
{
    byte val = read_byte(cpu->ram + cpu->PC);
    (cpu->PC)++;
    return(val);
}

byte* decode_addrmode_g1(CPU_t* cpu) //Calculates the address based on the addresation mode
{
    uint16_t address = 0; //The first byte must be 0
    switch(cpu->inst.bbb)
    {
        case 0x0: //(ZERO PAGE, X). Next byte + X represents an adress in zero page that needs to be refferenced
            address = read_pc(cpu) + cpu->X; 
            address &= 0xFF;        //The first byte must be 0
            address = read_address(cpu, address);
            break;

        case 0x01: //ZERO PAGE.
            address = read_pc(cpu);
            break;
        
        case 0x02: //Immediate. The opperand is in the next byte of memory
            address = (cpu->PC)++;
            break;
        
        case 0x03: //Absolute. The full 16-bit address 
            address = read_address(cpu, cpu->PC);
            cpu->PC += 2;
            break;
        
        case 0x04: //(zero page), Y; Takes an address from zero page as a pointer then adds Y to that address
            uint16_t alligment = 0;
            address = read_pc(cpu) + alligment;
            address &= 0xFF;
            address = read_address(cpu, address);
            address += cpu->Y; //TODO: check overflow
            break;

        case 0x05: //zero page, X; basically zeropage_addres + x
            address = read_pc(cpu) + cpu->X;
            address &= 0xFF;
            break;
        
        case 0x06: //absolute, Y; The full 16-bit address is in memory and we have to add Y
            address = read_address(cpu, cpu->PC);
            cpu->PC += 2;
            address += cpu->Y;
            break;
        
        case 0x07: //Absolute, X. the same as before
            address = read_address(cpu, cpu->PC);
            cpu->PC += 2;
            address += cpu->X;
            break;
        
        //We got the offset of the address but we have to return it as a pointer, so we add it to ram
        return cpu->ram + address;
    }
}

void emulate_instruction(CPU_t* cpu)
{
    cpu->inst.opcode = read_pc(cpu);
    if(cpu->inst.opcode == 0)
    {
        printf("Invalid opcode 0\n");
        return;
    }
    cpu->PC += 1;
    cpu->inst.aaa = (0xE0 & cpu->inst.opcode) >> 5;//first 3 bits of the opcode 
    cpu->inst.bbb = (0x1C & cpu->inst.opcode) >> 2;
    cpu->inst.cc  = (0x03 & cpu->inst.opcode);
    byte low_nibble = cpu->inst.opcode & 0x0F;
    byte high_nibble = cpu->inst.opcode >> 4;
    byte* address; //address will be calculated by some abstract function based on the addressing mode
    if(low_nibble == 0x08)
    {
        //Single byte instruction type 1
        //run_insturction_sb1(cpu);
    }
    if(low_nibble == 0x0A && high_nibble >= 0x08)
    {
        //Single byte instruction type 2 
        //run_instruction_sb2(cpu);
    }
    switch(cpu->inst.cc)
    {
        case 0x01: //cc = 01
            address = decode_addrmode_g1(cpu);
            //run_instruction_group1(address, cpu);
            break;
        case 0x02:
            //address = decode_addrmode_g2(cpu);
            //run_insturction_group2(address, cpu);
            break;
        case 0x0:
            //Branching operations: xxy 100 00. deci if(cc == 00 and bb == 100 => branching)
            //xx indicates a flag. 00 = negative, o1 = overflow, 10 = carry, 11 = zero
            if(cpu->inst.bbb == 0x4)
            {
                //branching.. will do later 
            }
    }
}



