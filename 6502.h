#ifndef HEADERGUARD_6502
#define HEADERGUARD_6502
typedef uint8_t byte;

byte read_byte(byte* address);
void write_byte(byte* address, byte value);
uint16_t read_address(CPU_t* cpu, byte offset);
void emulate_instruction(CPU_t* cpu);


typedef struct
{
    uint8_t aaa;
    uint8_t bbb;
    uint8_t cc;
    byte opcode;

}INSTRUCTION_t;


typedef struct 
{
    byte A; //Accumulator
    byte X; //X register
    byte Y; //Y register
    uint16_t PC; //Program counter. updated automatically as instructions are executed
    byte stack[256]; //located in memory from $0100-$01FF
    byte stack_pointer; //push -> decrement, pop -> increment(like in x86) 
    byte ram[65536]; //$0000-$07FF internal ram. //
    bool carry_flag;            //theoretically , all the flags are part of the processor status register 
    bool zero_flag;
    bool interrupt_disable;
    bool decimal_mode;
    bool break_comand;
    bool overflow_flag;
    bool negative_flag;
    INSTRUCTION_t inst;
    /* data */
}CPU_t;


#endif