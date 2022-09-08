#include "cpu.hpp"
#include "memory.hpp"

typedef unsigned char int8;
typedef unsigned short uint16;

#define printRegs std::cout<<"cpu axy sr:"<<(int)cpu.a<<' '<<(int)cpu.x<<' '<<(int)cpu.y<<' '<<(int)cpu.sr<<'\n'


#ifndef __Emulator
#define __Emulator
class Emulator
{
public:
    unsigned int start;//starting address

    int updatesr(cpustruct *cpu, int8 value);

    void RESET(int8 *mem, bool *rw, cpustruct *cpu);

    int execute(int8 *mem, bool *rw, cpustruct *cpu);

    std::map<int8, void (*)(int8 *, bool *, cpustruct *)> ins;
    // int8 ins;

};


void Emulator::RESET(int8 *mem, bool *rw, cpustruct *cpu)
{
    spdlog::info("RESET");
    // reset registers
    cpu->a = 0;
    cpu->x = 0;
    cpu->y = 0;
    cpu->pc = 0;
    cpu->sr = 0;
    cpu->sp = 0xff;
    cpu->bp = 0xff;
    cpu->isHalted = false;


    loadaddrlittle(mem, cpu, 0xfffc);
    spdlog::info("start address={}",cpu->pc);
    execute(mem, rw, cpu);
};

int Emulator::execute(int8 *mem, bool *rw, cpustruct *cpu)
{
    spdlog::debug("execute");
    for (int cycle = 0; cycle < 1000; ++cycle)
    {
        if (cpu->isHalted) {break;}
        // check if valid opcode
        if (ins.find(ins_read(mem, cpu->pc)) != ins.end())
        {
            //execute function opcode
            ins[ins_read(mem, cpu->pc)](mem, rw, cpu);
        }
        else //if invalid opcode
        {
            std::cout << "Unknown Instruction:" << (int)ins_read(mem, cpu->pc) << " at " << cpu->pc << '\n';
            break;
        }
    }
    return 0;
}
#endif