#include "cpu_core.h"
#include "cpu_init.h"
int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;
    if(argc < 2)
    {
        printf("Usage: ./%s <rom_name>\n", argv[0]);
        return -1;
    }
    CPU_t cpu = {0};
    CONFIG_t config = {0};
    reset_cpu(&cpu);
    //read ROM functionality
    init_config(&config, argv[1]);
    init_cpu(&cpu, config);
    hex_dump(cpu);
}