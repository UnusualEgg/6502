#pragma once

#include "spdlog/spdlog.h"

typedef unsigned char int8;
typedef unsigned short uint16;

#define printRegs std::cout<<"cpu axy sr:"<<(int)cpu.a<<' '<<(int)cpu.x<<' '<<(int)cpu.y<<' '<<(int)cpu.sr<<'\n'
#define printStack                             \
	spdlog::debug("Printing stack:\n");                \
	for (int i = 0x100; i <= 0x1ff; i++)       \
	{                                          \
		spdlog::debug((int)ins_read(mem, i));       \
		spdlog::debug(", ");       \
	};                                         \
	spdlog::debug("\b\b sp:"); \
	spdlog::debug((int)cpu->sp);\
	spdlog::debug("\n");
//-----------------------------Memory-------------------------
uint16 readaddr(int8 *mem, uint16 addr);
int8 ins_read(int8 *mem, uint16 addr)
{
	if (addr >= 0 && addr <= 0xffff)
	{
		int itmp;
		char ctmp;
		auto a = std::string();
		uint16 ptr;
		
		switch (addr)
		{
		case 0x4000://int
			std::cin>>itmp;
			return itmp;
			break;
		case 0x4001:
			std::cin>>ctmp;
			return ctmp;
			break;
		case 0x4002:
			getline(std::cin,a);
			ptr = readaddr(mem,0x4003);
			for (int i=0;i<a.length();i++) {
				mem[ptr]=a.data()[i];
				ptr++;
			}
			return 1;
		default:
			return mem[addr];
		}
	} 
	else 
	{
		return -1;
	};
}

/// @brief 
/// @param mem 
/// @param rw 
/// @param addr 
/// @param value 
/// @return 
bool ins_write(int8 *mem, bool *rw, uint16 addr, int8 value)
{
	if (rw[addr])
	{
		switch (addr)
		{
		case 0x4000://int
			std::cout<<(int)value;
			break;
		case 0x4001://char
			std::cout<<(char)value;
			break;
		case 0x4002://str (0x4003:lower ptr, 0x4004:higher ptr)
			std::cout<<"print: ptr:"<<(int)readaddr(mem,0x4003)<<std::endl;
			for (int i=readaddr(mem,0x4003);mem[i]!=0;i++){
				std::cout<<(char)mem[i]<<' '<<i<<'\n';
			}
			break;
		default:
			*(mem + addr) = value;
			break;
		}
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
	ins_write(mem, rw, getSP(cpu), cpu->a);
}
void php(int8 *mem, bool *rw, cpustruct *cpu)
{
	cpu->sp--;
	ins_write(mem, rw, getSP(cpu), cpu->sr);
};
void pla(int8 *mem, cpustruct *cpu)
{
	cpu->sp++;
	cpu->a = ins_read(mem, getSP(cpu));
};
void plp(int8 *mem, cpustruct *cpu)
{
	cpu->sp++;
	cpu->sr = ins_read(mem, cpu->pc) & 0b1101111;
};

void pushaddr(int8 *mem, bool *rw, cpustruct *cpu, uint16 addr)
{
	cpu->pc--;
	ins_write(mem, rw, cpu->pc, (int8)((addr & 0xff00) >> 8));
	cpu->pc--;
	ins_write(mem, rw, cpu->pc, (int8)(addr & 0xff));
};

/// @brief 
/// @param mem 
/// @param addr 
/// @return 
uint16 readaddr(int8 *mem, uint16 addr)
{
	return ((((uint16) ins_read(mem, addr + 1)) << 8) + ((uint16) ins_read(mem, addr)));
};

void loadaddrlittle(int8 *mem, cpustruct *cpu, uint16 addr)
{
	spdlog::debug("load addr:{}... {}",addr,readaddr(mem, addr));
	cpu->pc = readaddr(mem, addr);
};

void loadaddrbig(int8 *mem, cpustruct *cpu, uint16 addr)
{
	cpu->pc = (((uint16)ins_read(mem, addr)) << 8) + ((uint16)ins_read(mem, addr + 1));
};

void pushpc(int8 *mem, bool *rw, cpustruct *cpu, uint16 off = 0)
{
	cpu->sp--;
	ins_write(mem, rw, getSP(cpu), (int8)(((cpu->pc + off) & 0xff00) >> 8));
	cpu->sp--;
	ins_write(mem, rw, getSP(cpu), (int8)((cpu->pc + off) & 0xff));
	// printStack
};

void pullpc(int8 *mem, cpustruct *cpu)
{
	cpu->pc = (uint16)ins_read(mem, getSP(cpu)) | (((uint16)ins_read(mem, getSP(cpu) + 1)) << 8);
	cpu->sp += 2;
};
