#include <climits>
#include "defs.hpp"
#include "Emulator.hpp"

using spdlog::debug;
using spdlog::info;




/// INSTRUCTION FUNCTIONS
// DONE
void adc(int8 *mem, bool *rw, cpustruct *cpu)
{
	int8 tmp = cpu->a;
	long result;
	switch (isn_read(mem, cpu->pc))
	{
	case 0x69: // immidiate
		result = (int)cpu->a + (int)isn_read(mem, cpu->pc + 1) + (int)(cpu->sr & cbit);
		// set the carry bit
		if (result & 0b1111111100000000)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= ~cbit;
		}
		cpu->a = (int8)result;
		setsrbits(cpu, tmp, cpu->a, nbit | zbit | vbit);
		cpu->pc += 2;
		break;
	case 0x65: // zpg
		result = (int)cpu->a + (int)isn_read(mem, isn_read(mem, cpu->pc + 1)) + (int)(cpu->sr & 0b01);
		// set carry bit
		if (result & 0b1111111100000000)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= ~cbit;
		}
		cpu->a = (int8)result;
		// set other bits
		setsrbits(cpu, tmp, cpu->a, nbit | zbit | vbit);
		cpu->pc += 2;
		break;
	case 0x75: // zpg,x
		result = (int)cpu->a + (int)isn_read(mem, isn_read(mem, cpu->pc + 1) + cpu->x) + (int)(cpu->sr & 0b01);
		// set carry bit
		if (result & 0b1111111100000000)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= ~cbit;
		}
		cpu->a = (int8)result;
		// set other bits
		setsrbits(cpu, tmp, cpu->a, nbit | zbit | vbit);
		cpu->pc += 2;
		break;
	case 0x6d: // absolute
		result = (int)cpu->a + (int)isn_read(mem, readaddr(mem, cpu->pc + 1)) + (int)(cpu->sr & 0b01);
		// set carry bit
		if (result & 0b1111111100000000)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= ~cbit;
		}
		cpu->a = (int8)result;
		// set other bits
		setsrbits(cpu, tmp, cpu->a, nbit | zbit | vbit);
		cpu->pc += 3;
		break;
	case 0x7d: // absolute,x
		result = (int)cpu->a + (int)isn_read(mem, readaddr(mem, cpu->pc + 1) + cpu->x) + (int)(cpu->sr & 0b01);
		// set carry bit
		if (result & 0b1111111100000000)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= ~cbit;
		}
		cpu->a = (int8)result;
		// set other bits
		setsrbits(cpu, tmp, cpu->a, nbit | zbit | vbit);
		cpu->pc += 3;
		break;
	case 0x79: // abs,y
		result = (int)cpu->a + (int)isn_read(mem, readaddr(mem, cpu->pc + 1) + cpu->y) + (int)(cpu->sr & 0b01);
		// set carry bit
		if (result & 0b100000000)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= ~cbit;
		}
		cpu->a = (int8)result;
		// set other bits
		setsrbits(cpu, tmp, cpu->a, nbit | zbit | vbit);
		cpu->pc += 3;
		break;
	case 0x61: // ind+offx
		//						           -----pointing to      --------operand----
		result = (int)cpu->a + (int)readaddr(mem, readaddr(mem, isn_read(mem, cpu->pc + 1)) + cpu->x) + (int)(cpu->sr & 0b01);
		// set carry bit
		if (result & 0b100000000)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= ~cbit;
		}
		cpu->a = (int8)result;
		// set other bits
		setsrbits(cpu, tmp, cpu->a, nbit | zbit | vbit);
		cpu->pc += 2;
		break;
	case 0x71: // ind+y
		result = (int)cpu->a + (int)readaddr(mem, readaddr(mem, isn_read(mem, cpu->pc + 1))) + (int)cpu->y + (int)(cpu->sr & 0b01);
		// set carry bit
		if (result & 0b100000000)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= ~cbit;
		}
		cpu->a = (int8)result;
		// set other bits
		setsrbits(cpu, tmp, cpu->a, nbit | zbit | vbit);
		cpu->pc += 2;
		break;
	}
}

// DONE
void ins_and(int8 *mem, bool *rw, cpustruct *cpu)
{
	int8 tmp = cpu->a;
	switch (isn_read(mem, cpu->pc))
	{
	case 0x29: // imm
		cpu->a &= isn_read(mem, cpu->pc);
		setsrbits(cpu, tmp, cpu->a, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0x25: // zpg
		cpu->a &= isn_read(mem, isn_read(mem, cpu->pc + 1));
		setsrbits(cpu, tmp, cpu->a, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0x35: // zpg,x
		cpu->a &= isn_read(mem, isn_read(mem, cpu->pc + 1) + cpu->x);
		setsrbits(cpu, tmp, cpu->a, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0x2d: // abs
		cpu->a &= isn_read(mem, readaddr(mem, cpu->pc + 1));
		setsrbits(cpu, tmp, cpu->a, nbit | zbit);
		cpu->pc += 3;
		break;
	case 0x3d: // abs,x
		cpu->a &= isn_read(mem, readaddr(mem, cpu->pc + 1) + cpu->x);
		setsrbits(cpu, tmp, cpu->a, nbit | zbit);
		cpu->pc += 3;
		break;
	case 0x39: // abs,y
		cpu->a &= isn_read(mem, readaddr(mem, cpu->pc + 1) + cpu->y);
		setsrbits(cpu, tmp, cpu->a, nbit | zbit);
		cpu->pc += 3;
		break;
	case 0x21: //(indirect,x)
		cpu->a &= isn_read(mem, readaddr(mem, isn_read(mem, cpu->pc + 1) + cpu->x));
		setsrbits(cpu, tmp, cpu->a, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0x31: //(indirect),y
		cpu->a &= isn_read(mem, readaddr(mem, isn_read(mem, cpu->pc + 1)));
		setsrbits(cpu, tmp, cpu->a, nbit | zbit);
		cpu->pc += 2;
		break;
	}
}

// DONE
void asl(int8 *mem, bool *rw, cpustruct *cpu)
{
	int8 tmp;
	int result;
	switch (isn_read(mem, cpu->pc))
	{
	case 0x0a: // accumulator
		tmp = cpu->a;
		result = (int)cpu->a << 1;
		// set carry bit
		if (result & 0b1111111100000000)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= ~cbit;
		}
		cpu->a = (int8)result;
		// set other bits
		setsrbits(cpu, tmp, cpu->a, nbit | zbit | vbit);
		cpu->pc += 1;
		break;
	case 0x07: // zpg
		tmp = isn_read(mem, cpu->pc + 1);
		result = (int)isn_read(mem, cpu->pc + 1) << 1;
		// set carry bit
		if (result & 0b1111111100000000)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= ~cbit;
		}
		ins_write(mem, rw, tmp, (int8)result);
		cpu->pc += 2;
		break;
	case 0x16: // zpg,x
		tmp = isn_read(mem, isn_read(mem, cpu->pc + 1) + cpu->x);
		result = isn_read(mem, isn_read(mem, cpu->pc + 1) + cpu->x) << 1;
		// set carry bit
		if (result & 0b1111111100000000)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= ~cbit;
		}
		ins_write(mem, rw, tmp, (int8)result);
		cpu->pc += 2;
		break;
	case 0x0e: // abs
		tmp = isn_read(mem, readaddr(mem, cpu->pc + 1));
		result = isn_read(mem, readaddr(mem, cpu->pc + 1)) << 1;
		// set carry bit
		if (result & 0b1111111100000000)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= ~cbit;
		}
		ins_write(mem, rw, tmp, (int8)result);
		cpu->pc += 2;
		break;
	case 0x1e: // abs,x
		tmp = isn_read(mem, readaddr(mem, cpu->pc + 1) + cpu->x);
		result = isn_read(mem, readaddr(mem, cpu->pc + 1) + cpu->x) << 1;
		// set carry bit
		if (result & 0b1111111100000000)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= ~cbit;
		}
		ins_write(mem, rw, tmp, (int8)result);
		cpu->pc += 2;
		break;
	}
}

void bcc(int8 *mem, bool *rw, cpustruct *cpu)
{ // branch on carry clear
	switch (isn_read(mem, cpu->pc))
	{
	case 0x90: // relative
		if ((cpu->sr & cbit) == 0)
		{
			int8 off = isn_read(mem, cpu->pc + 1);
			if ((off & 0b10000000) != 0)
			{
				// add1 then invert
				cpu->pc -= ~(off + 1);
			}
			else
			{
				// else add 3?
				cpu->pc += off + 3;
			}
		}
		else
		{
			cpu->pc += 2;
		}
		break;
	}
}

void bcs(int8 *mem, bool *rw, cpustruct *cpu)
{ // branch on carry set
	switch (isn_read(mem, cpu->pc))
	{
	case 0xb0: // relative
		if ((cpu->sr & cbit) == 1)
		{
			int8 off = isn_read(mem, cpu->pc + 1);
			if ((off & 0b10000000) != 0)
			{
				// add1 then invert
				cpu->pc -= ~(off + 1);
			}
			else
			{
				// else add 3?
				cpu->pc += off + 3;
			}
		}
		else
		{
			cpu->pc += 2;
		}
		break;
	}
}

void beq(int8 *mem, bool *rw, cpustruct *cpu)
{ // branch on equal zero
	switch (isn_read(mem, cpu->pc))
	{
	case 0xf0: // relative
		if ((cpu->sr & zbit) == 1)
		{
			int8 off = isn_read(mem, cpu->pc + 1);
			if ((off & 0b10000000) != 0)
			{
				// add1 then invert
				cpu->pc -= ~(off + 1);
			}
			else
			{
				// else add 3?
				cpu->pc += off + 3;
			}
		}
		else
		{
			cpu->pc += 2;
		}
		break;
	}
}

void bit(int8 *mem, bool *rw, cpustruct *cpu)
{ // Test Bits in Memory with Accumulator
	int8 m;
	switch (isn_read(mem, cpu->pc))
	{
	case 0x24: // zpg
		m = isn_read(mem, cpu->pc + 1);
		if ((m & nbit) != 0)
		{
			cpu->sr |= nbit;
		}
		if ((m & vbit) != 0)
		{
			cpu->sr |= vbit;
		}
		if ((m & cpu->a) == 0)
		{
			cpu->sr |= zbit;
		}
		else
		{
			cpu->sr &= (~zbit);
		}
		cpu->pc += 2;
		break;
	case 0x2c: // abs
		m = isn_read(mem, readaddr(mem, cpu->pc + 1));
		if ((m & nbit) != 0)
		{
			cpu->sr |= nbit;
		}
		if ((m & vbit) != 0)
		{
			cpu->sr |= vbit;
		}
		if ((m & cpu->a) == 0)
		{
			cpu->sr |= zbit;
		}
		else
		{
			cpu->sr &= (~zbit);
		}
		cpu->pc += 3;
		break;
	}
}

void bmi(int8 *mem, bool *rw, cpustruct *cpu)
{ // branch on result minus
	switch (isn_read(mem, cpu->pc))
	{
	case 0x30: // relative
		if ((cpu->sr & nbit) == 1)
		{
			int8 off = isn_read(mem, cpu->pc + 1);
			if ((off & 0b10000000) != 0)
			{
				// add1 then invert
				cpu->pc -= ~(off + 1);
			}
			else
			{
				// else add 3?
				cpu->pc += off + 3;
			}
		}
		else
		{
			cpu->pc += 2;
		}
		break;
	}
}

void bne(int8 *mem, bool *rw, cpustruct *cpu)
{ // branch on result not zero
	switch (isn_read(mem, cpu->pc))
	{
	case 0x10: // relative
		if ((cpu->sr & zbit) == 0)
		{
			int8 off = isn_read(mem, cpu->pc + 1);
			if ((off & 0b10000000) != 0)
			{
				// add1 then invert
				cpu->pc -= ~(off + 1);
			}
			else
			{
				// else add 3?
				cpu->pc += off + 3;
			}
		}
		else
		{
			cpu->pc += 2;
		}
		break;
	}
}

void bpl(int8 *mem, bool *rw, cpustruct *cpu)
{ // branch on result plus
	switch (isn_read(mem, cpu->pc))
	{
	case 0xd0: // relative
		if ((cpu->sr & nbit) == 0)
		{
			int8 off = isn_read(mem, cpu->pc + 1);
			if ((off & 0b10000000) != 0)
			{
				// add1 then invert
				cpu->pc -= ~(off + 1);
			}
			else
			{
				// else add 3?
				cpu->pc += off + 3;
			}
		}
		else
		{
			cpu->pc += 2;
		}
		break;
	}
}

void bvc(int8 *mem, bool *rw, cpustruct *cpu)
{ // branch on overflow clear
	switch (isn_read(mem, cpu->pc))
	{
	case 0x50: // relative
		if ((cpu->sr & vbit) == 0)
		{
			int8 off = isn_read(mem, cpu->pc + 1);
			if ((off & 0b10000000) != 0)
			{
				// add1 then invert
				cpu->pc -= ~(off + 1);
			}
			else
			{
				// else add 3?
				cpu->pc += off + 3;
			}
		}
		else
		{
			cpu->pc += 2;
		}
		break;
	}
}

void bvs(int8 *mem, bool *rw, cpustruct *cpu)
{ // branch on overflow set
	switch (isn_read(mem, cpu->pc))
	{
	case 0x70: // relative
		if ((cpu->sr & vbit) == 0)
		{
			int8 off = isn_read(mem, cpu->pc + 1);
			if ((off & 0b10000000) != 0)
			{
				// add1 then invert
				cpu->pc -= ~(off + 1);
			}
			else
			{
				// else add 3?
				cpu->pc += off + 3;
			}
		}
		else
		{
			cpu->pc += 2;
		}
		break;
	}
}

void clc(int8 *mem, bool *rw, cpustruct *cpu)
{ // clear carry flag
	switch (isn_read(mem, cpu->pc))
	{
	case 0x18: // relative
		cpu->sr &= ~cbit;
		cpu->pc += 1;
		break;
	}
}

void cld(int8 *mem, bool *rw, cpustruct *cpu)
{ // clear decimal flag
	switch (isn_read(mem, cpu->pc))
	{
	case 0xd8: // relative
		cpu->sr &= ~dbit;
		cpu->pc += 1;
		break;
	}
}

void cli(int8 *mem, bool *rw, cpustruct *cpu)
{ // clear irq flag
	switch (isn_read(mem, cpu->pc))
	{
	case 0x58: // relative
		cpu->sr &= ~ibit;
		cpu->pc += 1;
		break;
	}
}

void clv(int8 *mem, bool *rw, cpustruct *cpu)
{ // clear overflow flag
	switch (isn_read(mem, cpu->pc))
	{
	case 0x58: // relative
		cpu->sr &= ~vbit;
		cpu->pc += 1;
		break;
	}
}

void cmp(int8 *mem, bool *rw, cpustruct *cpu)
{
	uint16 result;
	switch (isn_read(mem, cpu->pc))
	{
	case 0xc9: // immidiate
		result = (uint16)cpu->a - (uint16)isn_read(mem, cpu->pc + 1);
		// set the carry bit
		if (result & 0b1111111100000000)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= ~cbit;
		}
		setsrbits(cpu, cpu->a, result, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0xc5: // zpg
		result = (uint16)cpu->a - (uint16)isn_read(mem, isn_read(mem, cpu->pc + 1));
		// set the carry bit
		if (result & 0b1111111100000000)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= ~cbit;
		}
		setsrbits(cpu, cpu->a, result, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0xd5: // zpg,x
		result = (uint16)cpu->a - (uint16)isn_read(mem, isn_read(mem, cpu->pc + 1) + cpu->x);
		// set the carry bit
		if (result & 0b1111111100000000)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= ~cbit;
		}
		setsrbits(cpu, cpu->a, result, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0xcd: // absolute
		result = (uint16)cpu->a - (uint16)isn_read(mem, readaddr(mem, cpu->pc + 1));
		// set the carry bit
		if (result & 0b1111111100000000)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= ~cbit;
		}
		setsrbits(cpu, cpu->a, result, nbit | zbit);
		cpu->pc += 3;
		break;
	case 0xdd: // absolute,x
		result = (uint16)cpu->a - (uint16)isn_read(mem, readaddr(mem, cpu->pc + 1) + cpu->x);
		// set the carry bit
		if (result & 0b1111111100000000)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= ~cbit;
		}
		setsrbits(cpu, cpu->a, result, nbit | zbit);
		cpu->pc += 3;
		break;
	case 0xd9: // absolute,y
		result = (uint16)cpu->a - (uint16)isn_read(mem, readaddr(mem, cpu->pc + 1) + cpu->y);
		// set the carry bit
		if (result & 0b1111111100000000)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= ~cbit;
		}
		setsrbits(cpu, cpu->a, result, nbit | zbit);
		cpu->pc += 3;
		break;
	case 0xc1: // indirect+xoff
		result = (uint16)cpu->a - (uint16)isn_read(mem, readaddr(mem, isn_read(mem, cpu->pc + 1) + cpu->x));
		// set the carry bit
		if (result & 0b1111111100000000)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= ~cbit;
		}
		setsrbits(cpu, cpu->a, result, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0xd1: // indirect+y
		result = (uint16)cpu->a - (uint16)isn_read(mem, readaddr(mem, isn_read(mem, cpu->pc + 1))) + (uint16)cpu->y;
		// set the carry bit
		if (result & 0b1111111100000000)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= ~cbit;
		}
		setsrbits(cpu, cpu->a, result, nbit | zbit);
		cpu->pc += 2;
		break;
	}
}

void cpx(int8 *mem, bool *rw, cpustruct *cpu)
{
	uint16 result;
	switch (isn_read(mem, cpu->pc))
	{
	case 0xe0: // immidiate
		result = (uint16)cpu->x - (uint16)isn_read(mem, cpu->pc + 1);
		// set the carry bit
		if (result & 0b1111111100000000)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= ~cbit;
		}
		setsrbits(cpu, cpu->x, result, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0xe4: // zpg
		result = (uint16)cpu->x - (uint16)isn_read(mem, isn_read(mem, cpu->pc + 1));
		// set the carry bit
		if (result & 0b1111111100000000)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= ~cbit;
		}
		setsrbits(cpu, cpu->x, result, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0xec: // absolute
		result = (uint16)cpu->x - (uint16)isn_read(mem, readaddr(mem, cpu->pc + 1));
		// set the carry bit
		if (result & 0b1111111100000000)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= ~cbit;
		}
		setsrbits(cpu, cpu->x, result, nbit | zbit);
		cpu->pc += 3;
		break;
	}
}

void cpy(int8 *mem, bool *rw, cpustruct *cpu)
{
	uint16 result;
	switch (isn_read(mem, cpu->pc))
	{
	case 0xc0: // immidiate
		result = (uint16)cpu->y - (uint16)isn_read(mem, cpu->pc + 1);
		// set the carry bit
		if (result & 0b100000000)
		{
			setsrbits(cpu, cpu->y, result, nbit | zbit);
			cpu->pc += 2;
			break;
		case 0xc4: // zpg
			result = (uint16)cpu->y - (uint16)isn_read(mem, isn_read(mem, cpu->pc + 1));
			// set the carry bit
			if (result & 0b1111111100000000)
			{
				cpu->sr |= cbit;
			}
			else
			{
				cpu->sr &= ~cbit;
			}
			setsrbits(cpu, cpu->y, result, nbit | zbit);
			cpu->pc += 2;
			break;
		case 0xcc: // absolute
			result = (uint16)cpu->y - (uint16)isn_read(mem, readaddr(mem, cpu->pc + 1));
			// set the carry bit
			if (result & 0b1111111100000000)
			{
				cpu->sr |= cbit;
			}
			else
			{
				cpu->sr &= ~cbit;
			}
			setsrbits(cpu, cpu->y, result, nbit | zbit);
			cpu->pc += 3;
			break;
		}
	}
}

void dec(int8 *mem, bool *rw, cpustruct *cpu)
{
	int8 num, addr, result;
	switch (isn_read(mem, cpu->pc))
	{
	case 0xc6: // zpg
		num = isn_read(mem, isn_read(mem, cpu->pc + 1));
		addr = isn_read(mem, cpu->pc + 1);
		result = num - 1;
		ins_write(mem, rw, addr, result);
		setsrbits(cpu, num, result, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0xd6: // zpg,x
		num = isn_read(mem, isn_read(mem, cpu->pc + 1) + cpu->x);
		addr = isn_read(mem, cpu->pc + 1);
		result = num - 1;
		ins_write(mem, rw, addr, result);
		setsrbits(cpu, num, result, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0xce: // abs
		num = isn_read(mem, readaddr(mem, cpu->pc + 1));
		addr = isn_read(mem, cpu->pc + 1);
		result = num - 1;
		ins_write(mem, rw, addr, result);
		setsrbits(cpu, num, result, nbit | zbit);
		cpu->pc += 3;
		break;
	case 0xde: // abs,x
		num = isn_read(mem, readaddr(mem, cpu->pc + 1) + cpu->x);
		addr = isn_read(mem, cpu->pc + 1);
		result = num - 1;
		ins_write(mem, rw, addr, result);
		setsrbits(cpu, num, result, nbit | zbit);
		cpu->pc += 3;
		break;
	}
}

void dex(int8 *mem, bool *rw, cpustruct *cpu)
{
	switch (isn_read(mem, cpu->pc))
	{
	case 0xca: // implied
		cpu->x--;
		setsrbits(cpu, 0, cpu->x, nbit | zbit);
		cpu->pc += 1;
		break;
	}
}

void dey(int8 *mem, bool *rw, cpustruct *cpu)
{
	switch (isn_read(mem, cpu->pc))
	{
	case 0x88: // implied
		cpu->y--;
		setsrbits(cpu, 0, cpu->y, nbit | zbit);
		cpu->pc += 1;
		break;
	}
}

void eor(int8 *mem, bool *rw, cpustruct *cpu)
{
	switch (isn_read(mem, cpu->pc))
	{
	case 0x49: // imm
		cpu->a ^= isn_read(mem, cpu->pc + 1);
		setsrbits(cpu, 0, cpu->a, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0x45: // zpg
		cpu->a ^= isn_read(mem, isn_read(mem, cpu->pc + 1));
		setsrbits(cpu, 0, cpu->a, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0x55: // zpg,x
		cpu->a ^= isn_read(mem, isn_read(mem, cpu->pc + 1) + cpu->x);
		setsrbits(cpu, 0, cpu->a, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0x4d: // abs
		cpu->a ^= isn_read(mem, readaddr(mem, cpu->pc + 1));
		setsrbits(cpu, 0, cpu->a, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0x5d: // abs,x
		cpu->a ^= isn_read(mem, readaddr(mem, cpu->pc + 1) + cpu->x);
		setsrbits(cpu, 0, cpu->a, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0x59: // abs,t
		cpu->a ^= isn_read(mem, readaddr(mem, cpu->pc + 1) + cpu->y);
		setsrbits(cpu, 0, cpu->a, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0x41: // indirect+xoff
		cpu->a ^= isn_read(mem, readaddr(mem, isn_read(mem, cpu->pc + 1) + cpu->x));
		setsrbits(cpu, 0, cpu->a, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0x51: // indirect+y
		cpu->a ^= isn_read(mem, readaddr(mem, isn_read(mem, cpu->pc + 1))) + cpu->y;
		setsrbits(cpu, 0, cpu->a, nbit | zbit);
		cpu->pc += 2;
		break;
	}
}

void inc(int8 *mem, bool *rw, cpustruct *cpu)
{
	int8 num, addr, result;
	switch (isn_read(mem, cpu->pc))
	{
	case 0xe6: // zpg
		num = isn_read(mem, isn_read(mem, cpu->pc + 1));
		addr = isn_read(mem, cpu->pc + 1);
		result = num + 1;
		ins_write(mem, rw, addr, result);
		setsrbits(cpu, num, result, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0xf6: // zpg,x
		num = isn_read(mem, isn_read(mem, cpu->pc + 1) + cpu->x);
		addr = isn_read(mem, cpu->pc + 1);
		result = num + 1;
		ins_write(mem, rw, addr, result);
		setsrbits(cpu, num, result, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0xee: // abs
		num = isn_read(mem, readaddr(mem, cpu->pc + 1));
		addr = isn_read(mem, cpu->pc + 1);
		result = num + 1;
		ins_write(mem, rw, addr, result);
		setsrbits(cpu, num, result, nbit | zbit);
		cpu->pc += 3;
		break;
	case 0xfe: // abs,x
		num = isn_read(mem, readaddr(mem, cpu->pc + 1) + cpu->x);
		addr = isn_read(mem, cpu->pc + 1);
		result = num + 1;
		ins_write(mem, rw, addr, result);
		setsrbits(cpu, num, result, nbit | zbit);
		cpu->pc += 3;
		break;
	}
}

void inx(int8 *mem, bool *rw, cpustruct *cpu)
{
	switch (isn_read(mem, cpu->pc))
	{
	case 0xe8: // implied
		cpu->x++;
		setsrbits(cpu, 0, cpu->x, nbit | zbit);
		cpu->pc += 1;
		break;
	}
}

void iny(int8 *mem, bool *rw, cpustruct *cpu)
{
	switch (isn_read(mem, cpu->pc))
	{
	case 0xc8: // implied
		cpu->y++;
		setsrbits(cpu, 0, cpu->y, nbit | zbit);
		cpu->pc += 1;
		break;
	}
}

void jmp(int8 *mem, bool *rw, cpustruct *cpu)
{
	switch (isn_read(mem, cpu->pc))
	{
	case 0x4c: // absolute
		loadaddrlittle(mem, cpu, cpu->pc + 1);
		debug("jmp ");
		debug((int)cpu->pc);
		debug("\n");
		break;
	case 0x6c: // indirect
		debug("jmp ");
		debug(readaddr(mem, readaddr(mem, cpu->pc + 1)));
		debug("\n");
		// lda					arg pts to:	--------arg------------
		loadaddrlittle(mem, cpu, readaddr(mem, cpu->pc + 1));
		break;
	}
}

void jsr(int8 *mem, bool *rw, cpustruct *cpu)
{
	switch (isn_read(mem, cpu->pc))
	{
	case 0x20:
		pushpc(mem, rw, cpu, +2);
		debug("[jsr] to {} pc:{}",readaddr(mem, cpu->pc + 1),cpu->pc);
		loadaddrlittle(mem, cpu, cpu->pc + 1);
		break;
	}
}

void lda(int8 *mem, bool *rw, cpustruct *cpu)
{
	switch (isn_read(mem, cpu->pc))
	{
	case 0xa9: // imm
		cpu->a = isn_read(mem, cpu->pc + 1);
		debug("load {} in a:{}",(int)isn_read(mem, cpu->pc + 1),(int)cpu->a);
		setsrbits(cpu, 0, cpu->a, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0xa5: // zpg
		cpu->a = isn_read(mem, isn_read(mem, cpu->pc + 1));
		setsrbits(cpu, 0, cpu->a, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0xb5: // zpg,x
		cpu->a += isn_read(mem, cpu->pc + 1 + cpu->x);
		setsrbits(cpu, 0, cpu->a, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0xbd: // absolute
		cpu->a = isn_read(mem, readaddr(mem, cpu->pc + 1));
		setsrbits(cpu, 0, cpu->a, nbit | zbit);
		cpu->pc += 3;
		break;
	case 0xb9: // absolute,x
		cpu->a = isn_read(mem, readaddr(mem, cpu->pc + 1) + cpu->x);
		setsrbits(cpu, 0, cpu->a, nbit | zbit);
		cpu->pc += 3;
		break;
	case 0xa1: // absolute,y
		cpu->a = isn_read(mem, readaddr(mem, cpu->pc + 1) + cpu->y);
		setsrbits(cpu, 0, cpu->a, nbit | zbit);
		cpu->pc += 3;
		break;
	case 0xb1: //(indirect,x)
		cpu->a = isn_read(mem, readaddr(mem, isn_read(mem, cpu->pc + 1) + cpu->x));
		setsrbits(cpu, 0, cpu->a, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0xad: //(indirect),y
		cpu->a = isn_read(mem, readaddr(mem, isn_read(mem, cpu->pc + 1))) + cpu->y;
		setsrbits(cpu, 0, cpu->a, nbit | zbit);
		cpu->pc += 2;
		break;
	}
}

void ldx(int8 *mem, bool *rw, cpustruct *cpu)
{
	switch (isn_read(mem, cpu->pc))
	{
	case 0xa2: // imm(byte)
		cpu->x = isn_read(mem, cpu->pc + 1);
		setsrbits(cpu, 0, cpu->x, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0xa6: // zpg
		cpu->x &= isn_read(mem, isn_read(mem, cpu->pc + 1));
		setsrbits(cpu, 0, cpu->x, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0xb6: // zpg,y
		cpu->x &= isn_read(mem, isn_read(mem, cpu->pc + 1) + cpu->y);
		setsrbits(cpu, 0, cpu->x, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0xae: // abs
		cpu->x &= isn_read(mem, readaddr(mem, cpu->pc + 1));
		setsrbits(cpu, 0, cpu->x, nbit | zbit);
		cpu->pc += 3;
		break;
	case 0xbe: // abs,y
		cpu->x &= isn_read(mem, readaddr(mem, cpu->pc + 1) + cpu->y);
		setsrbits(cpu, 0, cpu->x, nbit | zbit);
		cpu->pc += 3;
		break;
	}
}

void ldy(int8 *mem, bool *rw, cpustruct *cpu)
{
	switch (isn_read(mem, cpu->pc))
	{
	case 0xa0: // imm(byte)
		cpu->y = isn_read(mem, cpu->pc + 1);
		setsrbits(cpu, 0, cpu->y, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0xa4: // zpg
		cpu->y &= isn_read(mem, isn_read(mem, cpu->pc + 1));
		setsrbits(cpu, 0, cpu->y, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0xb4: // zpg,x
		cpu->y &= isn_read(mem, isn_read(mem, cpu->pc + 1) + cpu->x);
		setsrbits(cpu, 0, cpu->y, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0xac: // abs
		cpu->y &= isn_read(mem, readaddr(mem, cpu->pc + 1));
		setsrbits(cpu, 0, cpu->y, nbit | zbit);
		cpu->pc += 3;
		break;
	case 0xbc: // abs,x
		cpu->y &= isn_read(mem, readaddr(mem, cpu->pc + 1) + cpu->x);
		setsrbits(cpu, 0, cpu->y, nbit | zbit);
		cpu->pc += 3;
		break;
	}
}

void lsr(int8 *mem, bool *rw, cpustruct *cpu)
{
	int8 n;
	uint16 result;
	switch (isn_read(mem, cpu->pc))
	{
	case 0x4a: // accumulator
		result = cpu->a >> 1;
		if ((cpu->a & 1) != 0)
		{
			cpu->sr |= cbit;
			debug("set SR:");
			debug((int)cpu->sr);
			debug("\n");
		}
		else
		{
			cpu->sr &= invcbit; // set cbit 0
			cpu->sr |= cbit;
			debug("set SR:");
			debug((int)cpu->sr);
			debug("\n");
		}
		cpu->a = (int8)result;
		setsrbits(cpu, 0, cpu->a, zbit);
		cpu->sr &= invnbit; // set nbit 0
		debug("set SR:");
		debug((int)CHAR_MIN);
		debug("\n");
		cpu->pc += 1;
		break;
	case 0x46: // zpg
		n = isn_read(mem, isn_read(mem, cpu->pc + 1));
		result = n >> 1;
		if (n & 1)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= cbit ^ CHAR_MIN;
		}
		setsrbits(cpu, 0, result, zbit);
		cpu->sr &= invnbit; // set nbit 0
		ins_write(mem, rw, isn_read(mem, cpu->pc + 1), result);
		cpu->pc += 2;
		break;
	case 0x56: // zpg,x
		n = isn_read(mem, isn_read(mem, cpu->pc + 1) + cpu->x);
		result = n >> 1;
		if (n & 1)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= cbit ^ CHAR_MIN;
		}
		setsrbits(cpu, 0, result, zbit);
		cpu->sr &= invnbit; // set nbit 0
		ins_write(mem, rw, isn_read(mem, cpu->pc + 1), result);
		cpu->pc += 2;
		break;
	case 0x4e: // abs
		n = isn_read(mem, readaddr(mem, cpu->pc + 1));
		result = n >> 1;
		if (n & 1)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= cbit ^ CHAR_MIN;
		}
		setsrbits(cpu, 0, result, zbit);
		cpu->sr &= invnbit; // set nbit 0
		ins_write(mem, rw, isn_read(mem, cpu->pc + 1), result);
		cpu->pc += 3;
		break;
	case 0x5e: // abs,x
		n = isn_read(mem, readaddr(mem, cpu->pc + 1) + cpu->x);
		result = n >> 1;
		if (n & 1)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= cbit ^ CHAR_MIN;
		}
		setsrbits(cpu, 0, result, zbit);
		cpu->sr &= invnbit; // set nbit 0
		ins_write(mem, rw, isn_read(mem, cpu->pc + 1), result);
		cpu->pc += 3;
		break;
	}
}

void nop(int8 *mem, bool *rw, cpustruct *cpu)
{
	switch (isn_read(mem, cpu->pc))
	{
	case 0xea:
		cpu->pc += 1;
		break;
	}
}

void ora(int8 *mem, bool *rw, cpustruct *cpu)
{
	int8 tmp = cpu->a;
	switch (isn_read(mem, cpu->pc))
	{
	case 0x09: // immediate
		cpu->a |= isn_read(mem, cpu->pc + 1);
		setsrbits(cpu, tmp, cpu->a, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0x05: // zpg
		cpu->a |= isn_read(mem, isn_read(mem, cpu->pc + 1));
		setsrbits(cpu, tmp, cpu->a, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0x15: // zpg,x
		cpu->a |= isn_read(mem, isn_read(mem, cpu->pc + 1) + cpu->x);
		setsrbits(cpu, tmp, cpu->a, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0x0d: // absolute
		cpu->a |= isn_read(mem, readaddr(mem, cpu->pc + 1));
		setsrbits(cpu, tmp, cpu->a, nbit | zbit);
		cpu->pc += 3;
		break;
	case 0x1d: // absolute,x
		cpu->a |= isn_read(mem, readaddr(mem, cpu->pc + 1) + cpu->x);
		setsrbits(cpu, tmp, cpu->a, nbit | zbit);
		cpu->pc += 3;
		break;
	case 0x19: // absolute,y
		cpu->a |= isn_read(mem, readaddr(mem, cpu->pc + 1) + cpu->y);
		setsrbits(cpu, tmp, cpu->a, nbit | zbit);
		cpu->pc += 3;
		break;
	case 0x01: // indirect+xoff
		cpu->a |= isn_read(mem, readaddr(mem, isn_read(mem, cpu->pc + 1) + cpu->x));
		setsrbits(cpu, tmp, cpu->a, nbit | zbit);
		cpu->pc += 2;
		break;
	case 0x11: // indirect+y
		cpu->a |= isn_read(mem, readaddr(mem, isn_read(mem, cpu->pc + 1))) + cpu->y;
		setsrbits(cpu, tmp, cpu->a, nbit | zbit);
		cpu->pc += 2;
		break;
	}
}

void _pha(int8 *mem, bool *rw, cpustruct *cpu)
{
	switch (isn_read(mem, cpu->pc))
	{
	case 0x48: // implied
		_pha(mem, rw, cpu);
		cpu->pc += 1;
		break;
	}
}

void _php(int8 *mem, bool *rw, cpustruct *cpu)
{
	switch (isn_read(mem, cpu->pc))
	{
	case 0x08: // implied
		_php(mem, rw, cpu);
		cpu->pc += 1;
		break;
	}
}

void pla(int8 *mem, bool *rw, cpustruct *cpu)
{
	switch (isn_read(mem, cpu->pc))
	{
	case 0x68: // implied
		debug("[pla] SP:");
		debug((int)getSP(cpu));
		debug(", pointing to ");
		debug((int)isn_read(mem, getSP(cpu) + 1));
		debug("\n");
		pla(mem, cpu);
		printStack
			cpu->pc += 1;
		break;
	}
}

void plp(int8 *mem, bool *rw, cpustruct *cpu)
{
	switch (isn_read(mem, cpu->pc))
	{
	case 0x28: // implied
		plp(mem, cpu);
		cpu->pc += 1;
		break;
	}
}

void rol(int8 *mem, bool *rw, cpustruct *cpu)
{
	uint16 result;
	uint16 addr;
	int8 n;
	switch (isn_read(mem, cpu->pc))
	{
	case 0x2a: // accumulator
		result = cpu->a << 1;
		result |= cpu->sr & cbit;
		if (cpu->a & 0x80)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= invcbit;
		}
		cpu->a = (int8)result;
		cpu->pc += 1;
		break;
	case 0x26: // zpg
		addr = isn_read(mem, cpu->pc + 1);
		n = isn_read(mem, addr);
		result = n << 1;
		result |= cpu->sr & cbit;
		if (n & 0x80)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= invcbit;
		}
		ins_write(mem, rw, addr, (int8)result);
		cpu->pc += 2;
		break;
	case 0x36: // zpg,x
		addr = isn_read(mem, cpu->pc + 1) + cpu->x;
		n = isn_read(mem, addr);
		result = n << 1;
		result |= cpu->sr & cbit;
		if (n & 0x80)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= invcbit;
		}
		ins_write(mem, rw, addr, (int8)result);
		cpu->pc += 2;
		break;
	case 0x2e: // absolute
		addr = readaddr(mem, cpu->pc + 1);
		n = isn_read(mem, addr);
		result = n << 1;
		result |= cpu->sr & cbit;
		if (n & 0x80)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= invcbit;
		}
		ins_write(mem, rw, addr, (int8)result);
		cpu->pc += 3;
		break;
	case 0x3e: // absolute,x
		addr = readaddr(mem, cpu->pc + 1) + cpu->x;
		n = isn_read(mem, addr);
		result = n << 1;
		result |= cpu->sr & cbit;
		if (n & 0x80)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= invcbit;
		}
		ins_write(mem, rw, addr, (int8)result);
		cpu->pc += 3;
		break;
	}
}

void ror(int8 *mem, bool *rw, cpustruct *cpu)
{
	int8 result;
	uint16 addr;
	int8 n;
	switch (isn_read(mem, cpu->pc))
	{
	case 0x6a: // accumulator
		result = cpu->a >> 1;
		result |= cpu->sr & cbit << 7;
		if (cpu->a & 0x01)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= invcbit;
		}
		cpu->a = result;
		cpu->pc += 1;
		break;
	case 0x66: // zpg
		addr = isn_read(mem, cpu->pc + 1);
		n = isn_read(mem, addr);
		result = n >> 1;
		result |= (cpu->sr & cbit) << 7;
		if (n & 0x01)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= invcbit;
		}
		ins_write(mem, rw, addr, result);
		cpu->pc += 2;
		break;
	case 0x76: // zpg,x
		addr = isn_read(mem, cpu->pc + 1) + cpu->x;
		n = isn_read(mem, addr);
		result = n >> 1;
		result |= cpu->sr & cbit << 7;
		if (n & 0x01)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= invcbit;
		}
		ins_write(mem, rw, addr, result);
		cpu->pc += 2;
		break;
	case 0x6e: // absolute
		addr = readaddr(mem, cpu->pc + 1);
		n = isn_read(mem, addr);
		result = n >> 1;
		result |= cpu->sr & cbit << 7;
		if (n & 0x01)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= invcbit;
		}
		ins_write(mem, rw, addr, result);
		cpu->pc += 3;
		break;
	case 0x7e: // absolute,x
		addr = readaddr(mem, cpu->pc + 1) + cpu->x;
		n = isn_read(mem, addr);
		result = n >> 1;
		result |= cpu->sr & cbit << 7;
		if (n & 0x01)
		{
			cpu->sr |= cbit;
		}
		else
		{
			cpu->sr &= invcbit;
		}
		ins_write(mem, rw, addr, result);
		cpu->pc += 3;
		break;
	}
}

void rti(int8 *mem, bool *rw, cpustruct *cpu)
{
	switch (isn_read(mem, cpu->pc))
	{
	case 0x40: // implied
		// pull sr from stack (ignore bit 5)
		cpu->sr = 0;
		cpu->sr |= isn_read(mem, getSP(cpu)) & 0b11101111; // ignore 5th bit
		pullpc(mem, cpu);
		break;
	}
}

void rts(int8 *mem, bool *rw, cpustruct *cpu)
{
	switch (isn_read(mem, cpu->pc))
	{
	case 0x60:
		loadaddrlittle(mem, cpu, getSP(cpu));
		cpu->pc += 1;
	}
}
//
// void rts(int8 *mem, bool *rw, cpustruct* cpu) {
//    switch (read(mem, cpu->pc)) {

void sei(int8 *mem, bool *rw, cpustruct *cpu)
{
	switch (isn_read(mem, cpu->pc))
	{
	case 0x78:
		cpu->sr |= ibit;
		cpu->pc += 1;
		break;
	}
}

void sta(int8 *mem, bool *rw, cpustruct *cpu)
{
	switch (isn_read(mem, cpu->pc))
	{
	case 0x85://zpg
		debug("sta:{} at {}...",(int)cpu->a, (int)isn_read(mem, cpu->pc + 1));
		ins_write(mem, rw, isn_read(mem, cpu->pc + 1), cpu->a);
		cpu->pc += 2;
		break;
	case 0x8d://abs
		debug("sta:{} to {}", (int)cpu->a,(int)readaddr(mem,cpu->pc+1));
		ins_write(mem, rw, readaddr(mem,cpu->pc+1),cpu->a);
		cpu->pc += 3;
	}

}

void hlt(int8 *mem, bool *rw, cpustruct *cpu)
{
	cpu->sp += 0;
	info("HLT");
	cpu->isHalted=true;
}

void setins(Emulator *emu)
{
	// ror rotates number (keeps data)(with carry bit)
	// lsr shifts with a zero(loses partial data)

	// done
	emu->ins[0x69] = &adc;
	emu->ins[0x65] = &adc;
	emu->ins[0x75] = &adc;
	emu->ins[0x6d] = &adc;
	emu->ins[0x7d] = &adc;
	emu->ins[0x79] = &adc;
	emu->ins[0x61] = &adc;
	emu->ins[0x71] = &adc;
	// done
	emu->ins[0x29] = &ins_and;
	emu->ins[0x25] = &ins_and;
	emu->ins[0x35] = &ins_and;
	emu->ins[0x2d] = &ins_and;
	emu->ins[0x3d] = &ins_and;
	emu->ins[0x39] = &ins_and;
	emu->ins[0x21] = &ins_and;
	emu->ins[0x31] = &ins_and;
	// done
	emu->ins[0x0a] = &asl;
	emu->ins[0x06] = &asl;
	emu->ins[0x16] = &asl;
	emu->ins[0x0e] = &asl;
	emu->ins[0x1e] = &asl;
	// done
	emu->ins[0x90] = &bcc;
	emu->ins[0xb0] = &bcs;
	emu->ins[0xf0] = &beq;

	emu->ins[0x24] = &bit;
	emu->ins[0x2c] = &bit;

	emu->ins[0x30] = &bmi;
	emu->ins[0xd0] = &bne;
	emu->ins[0x10] = &bpl;

	emu->ins[0x00] = &BRK;

	emu->ins[0x50] = &bvc;
	emu->ins[0x70] = &bvs;

	emu->ins[0x18] = &clc;
	emu->ins[0xd8] = &cld;
	emu->ins[0x58] = &cli;
	emu->ins[0xb8] = &clv;

	emu->ins[0xc9] = &cmp;
	emu->ins[0xc5] = &cmp;
	emu->ins[0xd5] = &cmp;
	emu->ins[0xcd] = &cmp;
	emu->ins[0xdd] = &cmp;
	emu->ins[0xd9] = &cmp;
	emu->ins[0xc1] = &cmp;
	emu->ins[0xd1] = &cmp;

	emu->ins[0xe0] = &cpx;
	emu->ins[0xe4] = &cpx;
	emu->ins[0xec] = &cpx;

	emu->ins[0xc0] = &cpy;
	emu->ins[0xc4] = &cpy;
	emu->ins[0xcc] = &cpy;

	emu->ins[0xc6] = &dec;
	emu->ins[0xd6] = &dec;
	emu->ins[0xce] = &dec;
	emu->ins[0xde] = &dec;

	emu->ins[0xca] = &dex;

	emu->ins[0x88] = &dey;

	emu->ins[0x49] = &eor;
	emu->ins[0x45] = &eor;
	emu->ins[0x55] = &eor;
	emu->ins[0x4d] = &eor;
	emu->ins[0x5d] = &eor;
	emu->ins[0x59] = &eor;
	emu->ins[0x41] = &eor;
	emu->ins[0x51] = &eor;

	emu->ins[0xe6] = &inc;
	emu->ins[0xf6] = &inc;
	emu->ins[0xee] = &inc;
	emu->ins[0xfe] = &inc;

	emu->ins[0xe8] = &inx;

	emu->ins[0xc8] = &iny;

	emu->ins[0x4c] = &jmp;
	emu->ins[0x6c] = &jmp;

	emu->ins[0x20] = &jsr;

	emu->ins[0xa9] = &lda;
	emu->ins[0xa5] = &lda;
	emu->ins[0xb5] = &lda;
	emu->ins[0xad] = &lda;
	emu->ins[0xbd] = &lda;
	emu->ins[0xb9] = &lda;
	emu->ins[0xa1] = &lda;
	emu->ins[0xb1] = &lda;

	emu->ins[0xa2] = &ldx;
	emu->ins[0xa6] = &ldx;
	emu->ins[0xb6] = &ldx;
	emu->ins[0xae] = &ldx;
	emu->ins[0xbe] = &ldx;

	emu->ins[0xa0] = &ldy;
	emu->ins[0xa4] = &ldy;
	emu->ins[0xb4] = &ldy;
	emu->ins[0xac] = &ldy;
	emu->ins[0xbc] = &ldy;

	emu->ins[0x4a] = &lsr;
	emu->ins[0x46] = &lsr;
	emu->ins[0x56] = &lsr;
	emu->ins[0x4e] = &lsr;
	emu->ins[0x5e] = &lsr;

	emu->ins[0xea] = &nop;

	emu->ins[0x09] = &ora;
	emu->ins[0x05] = &ora;
	emu->ins[0x15] = &ora;
	emu->ins[0x0d] = &ora;
	emu->ins[0x1d] = &ora;
	emu->ins[0x19] = &ora;
	emu->ins[0x01] = &ora;
	emu->ins[0x11] = &ora;

	emu->ins[0x48] = &_pha;

	emu->ins[0x08] = &_php;

	emu->ins[0x68] = &pla;

	emu->ins[0x28] = &plp;

	emu->ins[0x2a] = &rol;
	emu->ins[0x26] = &rol;
	emu->ins[0x3a] = &rol;
	emu->ins[0x2e] = &rol;
	emu->ins[0x3e] = &rol;

	emu->ins[0x6a] = &ror;
	emu->ins[0x66] = &ror;
	emu->ins[0x76] = &ror;
	emu->ins[0x6e] = &ror;
	emu->ins[0x7e] = &ror;

	emu->ins[0x40] = &rti;

	emu->ins[0x60] = &rts;

	emu->ins[0x78] = &sei;

	emu->ins[0x02] = &hlt;
	
	emu->ins[0x85] = &sta;
	emu->ins[0x8d] = &sta;
}

/*
this sets the carry bit
 if (result&0b1111111100000000) {
 cpu->sr|=cbit;
 } else {
 cpu->sr&=~cbit;
 }
 cpu->a=(int8)result;
*/