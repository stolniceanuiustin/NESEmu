#ifndef HEADERGUARD_CPUTYPES
#define HEADERGUARD_CPUTYPES
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#define RESET_ADDRESS 0xFFFC


typedef uint8_t byte;
typedef enum {QUIT, RUNNING, PAUSED}emulator_state_t;
typedef struct
{
    uint8_t aaa;
    uint8_t bbb;
    uint8_t cc;
    byte opcode;

}INSTRUCTION_t;

typedef enum{
    NEGATIVE    = 1 << 7,
    OVERFLOW    = 1 << 6,
    BREAK       = 1 << 4,
    DECIMAL_    = 1 << 3,
    INTERRUPT   = 1 << 2,
    ZERO        = 1 << 1,
    CARRY       = 1
} ProcessorFlag;

typedef struct 
{
    byte A; //Accumulator
    byte X; //X register
    byte Y; //Y register
    byte SR; //STATUS REGISTER
    uint16_t PC; //Program counter. updated automatically as instructions are executed
    byte stack[256]; //located in memory from $0100-$01FF
    byte stack_pointer; //push -> decrement, pop -> increment(like in x86) 
    byte ram[65536]; //$0000-$07FF internal ram. //
    size_t cycles;
    INSTRUCTION_t inst;
    emulator_state_t state;    /* data */
}CPU_t;

typedef struct{
    char* rom_name;
    size_t max_rom_size; //0x8000
    int entry_point;
}CONFIG_t;

#endif