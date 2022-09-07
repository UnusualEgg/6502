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
    std::cout << "RESET\n";
    // reset registers
    cpu->a = 0;
    cpu->x = 0;
    cpu->y = 0;
    cpu->pc = 0;
    cpu->sr = 0;
    cpu->sp = 0xff;
    cpu->bp = 0xff;

    std::cout << "getting starting adress..." << std::endl;
    // fffc fffd
    //  uint16 start=((uint16)read(mem,0xfffc))<<8;
    //  start+=((uint16)read(mem,0xfffd));
    //  cpu->pc=start;

    loadaddrlittle(mem, cpu, 0xfffc);
    std::cout << "start address=" << cpu->pc << ' ' << (int)mem[0xfffc] << " um " << (int)mem[0xfffd] << std::endl;
    execute(mem, rw, cpu);
};

int Emulator::execute(int8 *mem, bool *rw, cpustruct *cpu)
{
    std::cout << "execute\n";
    for (int cycle = 0; cycle < 1000; ++cycle)
    {
        // check if valid opcode
        //        std::cout<<"[ins]"<<(long)ins[read(mem,cpu->pc)]<<" pc:"<<cpu->pc<<'\n';
        if (ins.find(read(mem, cpu->pc)) != ins.end())
        {
            // std::cout<<std::hex<<"Instruction:"<<(uint)read(mem,cpu->pc)<<'/'<<(int)mem[cpu->pc]<<" at "<<(int)cpu->pc<<'\n';
            // std::cout<<"Instruction method loc:"<<&ins[cpu->pc]<<"\n\n";

            ins[read(mem, cpu->pc)](mem, rw, cpu);
            //            std::cout<<"pc after:"<<(int)cpu->pc<<'\n';
        }
        else
        {
            std::cout << "Unknown Instruction:" << (int)read(mem, cpu->pc) << " at " << cpu->pc << '\n';
            break;
        }
    }
    return 0;
}
#endif