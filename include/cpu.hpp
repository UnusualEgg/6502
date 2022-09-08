#pragma once

typedef unsigned char int8;
typedef unsigned short uint16;



// sign of number (+ or -)
const int8 sign = 0b10000000;
const int8 nbit = 0b10000000; // negative
const int8 vbit = 0b01000000; // overflow
const int8 bbit = 0b00010000; // break
const int8 dbit = 0b00001000; // decimal idk
const int8 ibit = 0b00000100; // interrupt(irq disable)
const int8 zbit = 0b00000010; // zero
const int8 cbit = 0b00000001; // carry

#define chall (int8)0xff;
#define invnbit (int8)(nbit ^ chall);
#define invcbit (int8)(cbit ^ chall);
#define invzbit (int8)(zbit ^ chall);

struct cpustruct;

void pha(int8 *mem, bool *rw, cpustruct *cpu);

void php(int8 *mem, bool *rw, cpustruct *cpu);

void pla(int8 *mem, cpustruct *cpu);

void plp(int8 *mem, cpustruct *cpu);

void pushaddr(int8 *mem, bool *rw, cpustruct *cpu, uint16 addr);

// uint16 readaddr(int8 *mem, uint16 addr);

void pushpc(int8 *mem, bool *rw, cpustruct *cpu, uint16 off);

void pullpc(int8 *mem, cpustruct *cpu);

void loadaddrbig(int8 *mem, cpustruct *cpu, uint16 addr);



//----------------------------CPU-------------------------
struct cpustruct
{
    int8 a;
    int8 x;
    int8 y;
    int8 sr;   // 7 bits long (so it has a function to update it)
    uint16 pc; // 32 bits long
    int8 bp;
    int8 sp;
	bool isHalted;
};

uint16 getSP(cpustruct *cpu) {
    return (uint16)cpu->sp + 0x0100; // page 1
}


int updatesr(cpustruct *cpu, int8 value)
{
	cpu->sr = value;
	return cpu->sr;
	/*
	SR Flags (bit 7 to bit 0)
	N	Negative
	V	Overflow
	-	ignored
	B	Break
	D	Decimal (use BCD for arithmetics)
	I	Interrupt (IRQ disable)
	Z	Zero
	C	Carry
	*/
}


void NMI(int8 *mem, bool *rw, cpustruct *cpu)
{
	// push pc
	pushpc(mem, rw, cpu, 2);
	// push processor status register
	php(mem, rw, cpu);
	// set interrupt disable
	cpu->sr |= ibit; // 3rd bit /irq disable
	// go to addr (nmi fffc/fffd)
	loadaddrbig(mem, cpu, 0xfffc);
}

void IRQ(int8 *mem, bool *rw, cpustruct *cpu)
{
	// push pc
	pushpc(mem, rw, cpu, 2);
	// push processor status register
	php(mem, rw, cpu);
	// set interrupt disable
	cpu->sr |= ibit; // 3rd bit /irq disable
	// go to addr (nmi fffe/ffff)
	loadaddrbig(mem, cpu, 0xfffe);
}

void BRK(int8 *mem, bool *rw, cpustruct *cpu)
{
	// push pc
	pushpc(mem, rw, cpu, 2);
	// push processor status register
	php(mem, rw, cpu);
	// set interrupt disable
	cpu->sr |= ibit | bbit; // 3rd bit /irq disable and break bit (16)
	// go to addr (nmi fffe/ffff)
	loadaddrbig(mem, cpu, 0xfffe);
	std::cout << "BRK!\n";
}

void setsrbits(cpustruct *cpu, int8 tmp, int8 result, int8 mask)
{
	// tmp is number before
	// tmp+(or whatever operation) off=result
	// m is if tmp should go up or down
	int8 tmpsr = 0;
	int8 off = 0;
	// set bits
	// overflow
	// if operands the aren't the same sign as the result
	int8 tmpsign = tmp & sign;
	int8 offsign = off & sign;
	int8 resultsign = result & sign;
	if (tmpsign == offsign && tmpsign != resultsign)
	{
		tmpsr |= vbit;
	}
	else
	{
		off |= vbit;
	}
	// zero
	if (result == 0)
	{
		tmpsr |= zbit; // zero bit 0b10
	}
	else
	{
		off |= zbit;
	}
	// negative
	if (result >= 0b10000000)
	{
		tmpsr |= nbit;
	}
	else
	{
		off |= nbit;
	}
	tmpsr &= mask;
	off &= mask;
	cpu->sr |= tmpsr;
	cpu->sr &= ~off;
} // This function does NOT set the carry bit!
