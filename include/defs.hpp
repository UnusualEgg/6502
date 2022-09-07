#pragma once

#include <spdlog/spdlog.h>
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

#define sign (int8)0b10000000 // sign of number (+ or -)
#define nbit (int8)0b10000000 // negative
#define vbit (int8)0b01000000 // overflow
#define bbit (int8)0b00010000 // break
#define dbit (int8)0b00001000 // decimal idk
#define ibit (int8)0b00000100 // interrupt(irq disable)
#define zbit (int8)0b00000010 // zero
#define cbit (int8)0b00000001 // carry

#define chall (int8)0xff
#define invnbit (int8)(nbit ^ chall)
#define invcbit (int8)(cbit ^ chall)
#define invzbit (int8)(zbit ^ chall)
