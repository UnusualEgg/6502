#pragma once

#include "spdlog/spdlog.h"

typedef unsigned char int8;
typedef unsigned short uint16;

#define printRegs std::cout<<"cpu axy sr:"<<(int)cpu.a<<' '<<(int)cpu.x<<' '<<(int)cpu.y<<' '<<(int)cpu.sr<<'\n'
#define printStack                             \
	spdlog::debug("Printing stack:\n");                \
	for (int i = 0x100; i <= 0x1ff; i++)       \
	{                                          \
		spdlog::debug((int)read(mem, i));       \
		spdlog::debug(", ");       \
	};                                         \
	spdlog::debug("\b\b sp:"); \
	spdlog::debug((int)cpu->sp);\
	spdlog::debug("\n");
//-----------------------------Memory-------------------------
int8 read(int8 *mem, uint16 addr)
{
	if (addr >= 0 && addr <= 0xffff)
	{
		return mem[addr];
	} 
	else 
	{
		return -1;
	};
}

bool write(int8 *mem, bool *rw, uint16 addr, int8 value)
{
	if (rw[addr])
	{
		*(mem + addr) = value;
		return true;
	}
	else
	{
		return false;
	};
}


void pha(int8 *mem, bool *rw, cpustruct *cpu) 
{
	cpu->sp--;
	write(mem, rw, getSP(cpu), cpu->a);
}
void php(int8 *mem, bool *rw, cpustruct *cpu)
{
	cpu->sp--;
	write(mem, rw, getSP(cpu), cpu->sr);
};
void pla(int8 *mem, cpustruct *cpu)
{
	cpu->sp++;
	cpu->a = read(mem, getSP(cpu));
};
void plp(int8 *mem, cpustruct *cpu)
{
	cpu->sp++;
	cpu->sr = read(mem, cpu->pc) & 0b1101111;
};

void pushaddr(int8 *mem, bool *rw, cpustruct *cpu, uint16 addr)
{
	cpu->pc--;
	write(mem, rw, cpu->pc, (int8)((addr & 0xff00) >> 8));
	cpu->pc--;
	write(mem, rw, cpu->pc, (int8)(addr & 0xff));
};

uint16 readaddr(int8 *mem, uint16 addr)
{
	return ((((uint16) read(mem, addr + 1)) << 8) + ((uint16) read(mem, addr)));
};

void loadaddrlittle(int8 *mem, cpustruct *cpu, uint16 addr)
{
	spdlog::debug("load addr:");
	spdlog::debug(addr);
	spdlog::debug("... ");
	spdlog::debug(readaddr(mem, addr));
	spdlog::debug("\n");
	cpu->pc = readaddr(mem, addr);
};

void loadaddrbig(int8 *mem, cpustruct *cpu, uint16 addr)
{
	cpu->pc = (((uint16)read(mem, addr)) << 8) + ((uint16)read(mem, addr + 1));
};

void pushpc(int8 *mem, bool *rw, cpustruct *cpu, uint16 off = 0)
{
	cpu->sp--;
	std::cout << write(mem, rw, getSP(cpu), (int8)(((cpu->pc + off) & 0xff00) >> 8));
	cpu->sp--;
	std::cout << write(mem, rw, getSP(cpu), (int8)((cpu->pc + off) & 0xff));
	printStack
};

void pullpc(int8 *mem, cpustruct *cpu)
{
	cpu->pc = (uint16)read(mem, getSP(cpu)) | (((uint16)read(mem, getSP(cpu) + 1)) << 8);
	cpu->sp += 2;
};
