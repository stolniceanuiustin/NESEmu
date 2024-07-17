#include "cpu_core.h"

void init_config(CONFIG_t* config, char* rom_name)
{
    config->rom_name = rom_name;
    config->max_rom_size = 0xFFFF+1;
    config->entry_point = 0x0000; //THIS IS FOR 6502 ROM TEST. FOR NEX WILL NEED TO BE 0x8000
}

bool reset_cpu(CPU_t* cpu)
{
    cpu->A = 0;
    cpu->X = 0;
    cpu->Y = 0;
    cpu->stack_pointer = 0;
    cpu->PC = 0;
    for(int i=0; i<0xFFFF; i++)
        cpu->ram[i] = 0;
    
    return true;
}

bool init_cpu(CPU_t* cpu, CONFIG_t config)
{
    uint16_t entry_point = config.entry_point; //THIS IS WHERE THE ROM IS LOADED
    FILE* rom = fopen(config.rom_name, "rb");
    if(!rom)
    {
        printf("Could not open ROM FILE: %s\n", config.rom_name);
        return false;
    }
    // Get rom size
    fseek(rom, 0, SEEK_END);
    const size_t rom_size = ftell(rom); // Moment C
    rewind(rom);
    
    if(rom_size > config.max_rom_size)
    {
        printf("Rom file is bigger than ram\n");
        fclose(rom);
        return false;
    }

    if(fread(&cpu->ram[entry_point], rom_size, 1, rom) != 1)
    {
        printf("Could not load ROM into RAM\n");
        fclose(rom);
        return false;
    }
    fclose(rom);
    uint16_t pc_point = (cpu->ram[0xFFFD] << 8) | cpu->ram[0xFFFC]; //it should be at 0xFFFD * 256 + 0xFFFC on NES but not sure right now;
    cpu->PC = pc_point;
    cpu->state = RUNNING;
    cpu->stack_pointer = 0xfd;
    cpu->SR = 0x24;
    return true;
}

void hex_dump(CPU_t cpu)
{
    for(int i=0; i<=0xFFFF; i++)
    {
        printf("%02X ", cpu.ram[i]);
    }
}
