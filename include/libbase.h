typedef unsigned char int8;
typedef unsigned short uint16;

#define sign 0b10000000//sign of number (+ or -)
#define nbit 0b10000000//negative
#define vbit 0b01000000//overflow
#define bbit 0b00010000//break
#define dbit 0b00001000//decimal idk
#define ibit 0b00000100//interrupt(irq disable)
#define zbit 0b00000010//zero
#define cbit 0b00000001//carry

namespace libbase
{
//----------------------------CPU-------------------------
	struct cpustruct
{
	int8 a;
	int8 x;
	int8 y;
	int8 sr; //7 bits long (so it has a function to update it)
	uint16 pc; //32 bits long
	int8 bp;
	int8 sp;
} cpu6502;


class Emulator {
private:
	// Emulator();

public:
	unsigned int start;
	// int8 tmp;
	std::map<int8, void (*)(int8*,libbase::cpustruct*)> ins;
	int updatesr(libbase::cpustruct* cpu,int8 value);
	void RESET(int8 *mem, libbase::cpustruct *cpu);
	int execute(int8 *mem, libbase::cpustruct *cpu);
};



//-----------------------------Memory-------------------------
int8 read(int8 *mem, uint16 addr){
	if (addr>=0&&addr<=0xffff){
		return mem[addr];
	} else {
		return -1;
	}
};
bool write(int8 *mem, uint16 addr, int8 value) {
    if (addr>=0&&addr<=0xffff){
        *(mem+addr)=value;
        return true;
    } else {
        return false;
    }
}
}




void pha(int8 *mem, libbase::cpustruct* cpu) {
	cpu->sp--;
	libbase::write(mem,cpu->sp,cpu->a);
}
void php(int8 *mem, libbase::cpustruct* cpu) {
	cpu->sp--;
	libbase::write(mem,cpu->sp,cpu->sr);
}
void pla(int8 *mem, libbase::cpustruct* cpu) {
	cpu->a=libbase::read(mem,cpu->pc);
	cpu->pc++;
}
void plp(int8 *mem, libbase::cpustruct* cpu) {
	cpu->sr=libbase::read(mem,cpu->pc)&0b1101111;
	cpu->pc++;
}
void pushaddr(int8 *mem, libbase::cpustruct* cpu, uint16 addr) {
	cpu->pc--;
	libbase::write(mem, cpu->pc, (int8)((addr&0xff00)>>8));
	cpu->pc--;
	libbase::write(mem, cpu->pc, (int8)(addr&0xff));
}

uint16 readaddr(int8 *mem,uint16 addr) {
	// std::cout<<"readaddr:"<<addr<<" got:"<<(((uint16)libbase::read(mem,addr+1))<<8)+ ((uint16)libbase::read(mem,addr))<<'/'<<(uint16)mem[addr+1]<<(uint16)mem[addr]<<' ';
	return (((uint16)libbase::read(mem,addr+1))<<8)+ ((uint16)libbase::read(mem,addr));
}

void loadaddrlittle(int8 *mem, libbase::cpustruct* cpu, uint16 addr) {
	std::cout<<"load addr:"<<addr<<"... "<<readaddr(mem,addr)<<'\n';
	cpu->pc=readaddr(mem,addr);
}

void loadaddrbig(int8 *mem, libbase::cpustruct* cpu, uint16 addr) {
	cpu->pc=(((uint16)libbase::read(mem,addr))<<8)+ ((uint16)libbase::read(mem,addr+1));
}

void pushpc(int8 *mem, libbase::cpustruct* cpu, uint16 off) {
	cpu->pc--;
	libbase::write(mem, cpu->sp, (int8)(((cpu->pc+off)&0xff00)>>8));
	cpu->pc--;
	libbase::write(mem, cpu->sp, (int8)((cpu->pc+off)&0xff));
}

void pullpc(int8 *mem, libbase::cpustruct* cpu) {
	cpu->pc=(uint16)libbase::read(mem,cpu->sp)|(((uint16)libbase::read(mem,cpu->sp+1))<<8);
	cpu->sp+=2;
}

int updatesr(libbase::cpustruct* cpu, int8 value) {
	cpu->sr=value;
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

void libbase::Emulator::RESET(int8 *mem, libbase::cpustruct *cpu) {
	std::cout<<"RESET\n";
	//reset registers
	cpu->a=0;
	cpu->x=0;
	cpu->y=0;
	cpu->pc=0;
	cpu->sr=0;
	cpu->sp=0xff;
	cpu->bp=0xff;

	std::cout<<"getting starting adress..."<<std::endl;
	//fffc fffd
 	// uint16 start=((uint16)libbase::read(mem,0xfffc))<<8;
	// start+=((uint16)libbase::read(mem,0xfffd));
	// cpu->pc=start;
	loadaddrlittle(mem,cpu,0xfffc);
	std::cout<<"start address="<<cpu->pc<<' '<<(int)mem[0xfffc]<<std::endl;
	execute(mem,cpu);
}


int libbase::Emulator::execute(int8 *mem, libbase::cpustruct *cpu) {
	
	for (int cycle = 0; cycle < 1000; ++cycle) {
		//check if valid opcode
		if (ins.find(libbase::read(mem,cpu->pc))!=ins.end()) {
			// std::cout<<std::hex<<"Instruction:"<<(uint)libbase::read(mem,cpu->pc)<<'/'<<(int)mem[cpu->pc]<<" at "<<(int)cpu->pc<<'\n';
			// std::cout<<"Instruction method loc:"<<&ins[cpu->pc]<<"\n\n";
			ins[libbase::read(mem,cpu->pc)](mem,cpu);
		} else {
			std::cout<<"Unknown Instruction:"<<(int)libbase::read(mem,cpu->pc)<<" at "<<cpu->pc<<'\n';
			break;
		}
	}
	return 0;
}

void NMI(int8 *mem, libbase::cpustruct *cpu) {
	//push pc
	pushpc(mem,cpu,2);
	//push processor status register
	php(mem,cpu);
	//set interrupt disable
	cpu->sr|=ibit;//3rd bit /irq disable
	//go to addr (nmi fffc/fffd)
	loadaddrbig(mem,cpu,0xfffc);
}

void IRQ(int8 *mem, libbase::cpustruct *cpu) {
	//push pc
	pushpc(mem,cpu,2);
	//push processor status register
	php(mem,cpu);
	//set interrupt disable
	cpu->sr|=ibit;//3rd bit /irq disable
	//go to addr (nmi fffe/ffff)
	loadaddrbig(mem,cpu,0xfffe);
}

void BRK(int8 *mem, libbase::cpustruct *cpu) {
	//push pc
	pushpc(mem,cpu,2);
	//push processor status register
	php(mem,cpu);
	//set interrupt disable
	cpu->sr|=ibit|bbit;//3rd bit /irq disable and break bit (16)
	//go to addr (nmi fffe/ffff)
	loadaddrbig(mem,cpu,0xfffe);
	std::cout<<"BRK!\n";
}





void setsrbits(libbase::cpustruct *cpu, int8 tmp,int8 result, int8 mask) {
	//tmp is number before
	//tmp+(or whatever operation) off=result
	//m is if tmp should go up or down
	int8 tmpsr=0;
	int8 off=0;
	//set bits
	//overflow
	//if operands the aren't the same sign as the result
	int8 tmpsign=tmp&sign;
	int8 offsign=off&sign;
	int8 resultsign=result&sign;
	if (tmpsign==offsign && tmpsign!=resultsign) {
		tmpsr|=vbit;
	} else {
		off|=vbit;
	}
	//zero
	if (result==0) {
		tmpsr|=zbit;//zero bit 0b10
	} else {
		off|=zbit;
	}
	//negative
	if (result>=0b10000000) {
		tmpsr|=nbit;
	} else {
		off|=nbit;
	}
	tmpsr&=mask;
	off&=mask;
	cpu->sr|=tmpsr;
	cpu->sr&=~off;
} //This function does NOT set the carry bit!



//INSTRUCTION FUNCTIONS

//DONE
void adc(int8 *mem, libbase::cpustruct* cpu){
	int8 tmp=cpu->a;
	long result;
	switch (libbase::read(mem,cpu->pc)){
		case 0x69://immidiate
			result=(int)cpu->a+(int)libbase::read(mem, cpu->pc+1)+(int)(cpu->sr&cbit);
			//set the carry bit
			if (result|0b100000000) {
				cpu->sr|=cbit;
			} else {
				cpu->sr&=~cbit;
			}
			cpu->a=(int8)result;
			setsrbits(cpu, tmp,cpu->a,nbit|zbit|vbit);
			cpu->pc+=2;
			std::cout<<"adc "<<libbase::read(mem, cpu->pc+1)+(cpu->sr&0b00000001)<<std::endl;
			break;
		case 0x65://zpg
			result=(int)cpu->a+(int)libbase::read(mem,libbase::read(mem,cpu->pc+1))+(int)(cpu->sr&0b01);
			//set carry bit
			if (result|0b100000000) {
				cpu->sr|=cbit;
			} else {
				cpu->sr&=~cbit;
			}
			cpu->a=(int8)result;
			//set other bits
			setsrbits(cpu, tmp,cpu->a,nbit|zbit|vbit);
			cpu->pc+=2;
			break;
		case 0x75://zpg,x
			result=(int)cpu->a+(int)libbase::read(mem,libbase::read(mem,cpu->pc+1)+cpu->x)+(int)(cpu->sr&0b01);
			//set carry bit
			if (result|0b100000000) {
				cpu->sr|=cbit;
			} else {
				cpu->sr&=~cbit;
			}
			cpu->a=(int8)result;
			//set other bits
			setsrbits(cpu, tmp,cpu->a,nbit|zbit|vbit);
			cpu->pc+=2;
			break;
		case 0x6d://absolute
			result=(int)cpu->a+(int)libbase::read(mem,readaddr(mem,cpu->pc+1))+(int)(cpu->sr&0b01);
			//set carry bit
			if (result|0b100000000) {
				cpu->sr|=cbit;
			} else {
				cpu->sr&=~cbit;
			}
			cpu->a=(int8)result;
			//set other bits
			setsrbits(cpu, tmp,cpu->a,nbit|zbit|vbit);
			cpu->pc+=3;
			break;
		case 0x7d://absolute,x
			result=(int)cpu->a+(int)libbase::read(mem,readaddr(mem,cpu->pc+1)+cpu->x)+(int)(cpu->sr&0b01);
			//set carry bit
			if (result|0b100000000) {
				cpu->sr|=cbit;
			} else {
				cpu->sr&=~cbit;
			}
			cpu->a=(int8)result;
			//set other bits
			setsrbits(cpu, tmp,cpu->a,nbit|zbit|vbit);
			cpu->pc+=3;
			break;
		case 0x79://abs,y
			result=(int)cpu->a+(int)libbase::read(mem,readaddr(mem,cpu->pc+1)+cpu->y)+(int)(cpu->sr&0b01);
			//set carry bit
			if (result|0b100000000) {
				cpu->sr|=cbit;
			} else {
				cpu->sr&=~cbit;
			}
			cpu->a=(int8)result;
			//set other bits
			setsrbits(cpu, tmp,cpu->a,nbit|zbit|vbit);
			cpu->pc+=3;
			break;
		case 0x61://ind+offx
			//						           -----pointing to      --------operand----
			result=(int)cpu->a+(int)readaddr(mem,readaddr(mem,libbase::read(mem,cpu->pc+1))+cpu->x)+(int)(cpu->sr&0b01);
			//set carry bit
			if (result|0b100000000) {
				cpu->sr|=cbit;
			} else {
				cpu->sr&=~cbit;
			}
			cpu->a=(int8)result;
			//set other bits
			setsrbits(cpu, tmp,cpu->a,nbit|zbit|vbit);
			cpu->pc+=2;
			break;
		case 0x71://ind+y
			result=(int)cpu->a+(int)readaddr(mem, readaddr(mem,libbase::read(mem,cpu->pc+1)))+(int)cpu->y+(int)(cpu->sr&0b01);
			//set carry bit
			if (result|0b100000000) {
				cpu->sr|=cbit;
			} else {
				cpu->sr&=~cbit;
			}
			cpu->a=(int8)result;
			//set other bits
			setsrbits(cpu, tmp,cpu->a,nbit|zbit|vbit);
			cpu->pc+=2;
			break;
	}
}

//DONE
void ins_and(int8 *mem, libbase::cpustruct* cpu) {
	int8 tmp=cpu->a;
	switch (libbase::read(mem,cpu->pc)) {
		case 0x29://imm
			cpu->a&=libbase::read(mem,cpu->pc);
			setsrbits(cpu, tmp,cpu->a,nbit|zbit);
			cpu->pc+=2;
			break;
		case 0x25://zpg
			cpu->a&=libbase::read(mem,libbase::read(mem,cpu->pc+1));
			setsrbits(cpu, tmp,cpu->a,nbit|zbit);
			cpu->pc+=2;
			break;
		case 0x35://zpg,x
			cpu->a&=libbase::read(mem,libbase::read(mem,cpu->pc+1)+cpu->x);
			setsrbits(cpu, tmp,cpu->a,nbit|zbit);
			cpu->pc+=2;
			break;
		case 0x2d://abs
			cpu->a&=libbase::read(mem,readaddr(mem,cpu->pc+1));
			setsrbits(cpu, tmp,cpu->a,nbit|zbit);
			cpu->pc+=3;
			break;
		case 0x3d://abs,x
			cpu->a&=libbase::read(mem,readaddr(mem,cpu->pc+1)+cpu->x);
			setsrbits(cpu, tmp,cpu->a,nbit|zbit);
			cpu->pc+=3;
			break;
		case 0x39://abs,y
			cpu->a&=libbase::read(mem,readaddr(mem,cpu->pc+1)+cpu->y);
			setsrbits(cpu, tmp,cpu->a,nbit|zbit);
			cpu->pc+=3;
			break;
		case 0x21://(indirect,x)
			cpu->a&=libbase::read(mem,readaddr(mem, libbase::read(mem,cpu->pc+1)+cpu->x ));
			setsrbits(cpu, tmp,cpu->a,nbit|zbit);
			cpu->pc+=2;
			break;
		case 0x31://(indirect),y
			cpu->a&=libbase::read(mem,readaddr(mem, libbase::read(mem,cpu->pc+1)));
			setsrbits(cpu, tmp,cpu->a,nbit|zbit);
			cpu->pc+=2;
			break;
	}
}

//DONE
void asl(int8 *mem, libbase::cpustruct* cpu) {
	int8 tmp;
	int result;
	switch (libbase::read(mem,cpu->pc)) {
		case 0x0a://accumulator
			tmp=cpu->a;
			result=(int)cpu->a<<1;
			//set carry bit
			if (result|0b100000000) {
				cpu->sr|=cbit;
			} else {
				cpu->sr&=~cbit;
			}
			cpu->a=(int8)result;
			//set other bits
			setsrbits(cpu, tmp,cpu->a,nbit|zbit|vbit);
			cpu->pc+=1;
			break;
		case 0x07://zpg
			tmp=libbase::read(mem,cpu->pc+1);
			result=(int)libbase::read(mem,cpu->pc+1)<<1;
			//set carry bit
			if (result|0b100000000) {
				cpu->sr|=cbit;
			} else {
				cpu->sr&=~cbit;
			}
			libbase::write(mem,tmp,(int8)result);
			cpu->pc+=2;
			break;
		case 0x16://zpg,x
			tmp=libbase::read(mem,libbase::read(mem,cpu->pc+1)+cpu->x);
			result=libbase::read(mem,libbase::read(mem,cpu->pc+1)+cpu->x)<<1;
			//set carry bit
			if (result|0b100000000) {
				cpu->sr|=cbit;
			} else {
				cpu->sr&=~cbit;
			}
			libbase::write(mem,tmp,(int8)result);
			cpu->pc+=2;
			break;
		case 0x0e://abs
			tmp=libbase::read(mem, readaddr(mem,cpu->pc+1));
			result=libbase::read(mem, readaddr(mem,cpu->pc+1))<<1;
			//set carry bit
			if (result|0b100000000) {
				cpu->sr|=cbit;
			} else {
				cpu->sr&=~cbit;
			}
			libbase::write(mem,tmp,(int8)result);
			cpu->pc+=2;
			break;
		case 0x1e://abs,x
			tmp=libbase::read(mem, readaddr(mem,cpu->pc+1)+cpu->x);
			result=libbase::read(mem, readaddr(mem,cpu->pc+1)+cpu->x)<<1;
			//set carry bit
			if (result|0b100000000) {
				cpu->sr|=cbit;
			} else {
				cpu->sr&=~cbit;
			}
			libbase::write(mem,tmp,(int8)result);
			cpu->pc+=2;
			break;
	}
}

void bcc(int8 *mem, libbase::cpustruct* cpu) {//branch on carry clear
	switch (libbase::read(mem,cpu->pc)) {
		case 0x90://relative
			if (cpu->sr&cbit==0) {
				int8 off=libbase::read(mem,cpu->pc+1);
				if ((off&0b10000000)!=0){
					//add1 then invert
					cpu->pc-=~(off+1);
				} else {
					//else add 3?
					cpu->pc+=off+3;
				}
			} else {
				cpu->pc+=2
			}
			break;
	}
}

void bcs(int8 *mem, libbase::cpustruct* cpu) {//branch on carry set
	switch (libbase::read(mem,cpu->pc)) {
		case 0xb0://relative
			if (cpu->sr&cbit==1) {
				int8 off=libbase::read(mem,cpu->pc+1);
				if ((off&0b10000000)!=0){
					//add1 then invert
					cpu->pc-=~(off+1);
				} else {
					//else add 3?
					cpu->pc+=off+3;
				}
			} else {
				cpu->pc+=2
			}
			break;
	}
}

void beq(int8 *mem, libbase::cpustruct* cpu) {//branch on equal zero
	switch (libbase::read(mem,cpu->pc)) {
		case 0xf0://relative
			if (cpu->sr&zbit==1) {
				int8 off=libbase::read(mem,cpu->pc+1);
				if ((off&0b10000000)!=0){
					//add1 then invert
					cpu->pc-=~(off+1);
				} else {
					//else add 3?
					cpu->pc+=off+3;
				}
			} else {
				cpu->pc+=2
			}
			break;
	}
}

void bit(int8 *mem, libbase::cpustruct* cpu) {//Test Bits in Memory with Accumulator
	switch (libbase::read(mem,cpu->pc)) {
		case 0x24://zpg
			int8 m = libbase::read(mem,cpu->pc+1);
			if (m&nbit!=0){
				cpu->sr|=nbit;
			}
			if (m&zbit!=0){
				cpu->sr|=zbit;
			}
			if ((m&cpu->a)==0){
				cpu->sr!=zbit;
			} else {
				cpu->sr&(~zbit);
			}
			cpu->pc+=2;
			break;
		case 0x2c://abs
			int8 m = libbase::read(mem, readaddr(mem,cpu->pc+1));
			if (m&nbit!=0){
				cpu->sr|=nbit;
			}
			if (m&zbit!=0){
				cpu->sr|=zbit;
			}
			if ((m&cpu->a)==0){
				cpu->sr!=zbit;
			} else {
				cpu->sr&(~zbit);
			}
			cpu->pc+=3;
			break;
			
	}
}

void bmi(int8 *mem, libbase::cpustruct* cpu) {//branch on result minus
	switch (libbase::read(mem,cpu->pc)) {
		case 0x30://relative
			if (cpu->sr&nbit==1) {
				int8 off=libbase::read(mem,cpu->pc+1);
				if ((off&0b10000000)!=0){
					//add1 then invert
					cpu->pc-=~(off+1);
				} else {
					//else add 3?
					cpu->pc+=off+3;
				}
			} else {
				cpu->pc+=2
			}
			break;
	}
}

void bne(int8 *mem, libbase::cpustruct* cpu) {//branch on result not zero
	switch (libbase::read(mem,cpu->pc)) {
		case 0x10://relative
			if (cpu->sr&zbit==0) {
				int8 off=libbase::read(mem,cpu->pc+1);
				if ((off&0b10000000)!=0){
					//add1 then invert
					cpu->pc-=~(off+1);
				} else {
					//else add 3?
					cpu->pc+=off+3;
				}
			} else {
				cpu->pc+=2
			}
			break;
	}
}

void bpl(int8 *mem, libbase::cpustruct* cpu) {//branch on result plus
	switch (libbase::read(mem,cpu->pc)) {
		case 0xd0://relative
			if (cpu->sr&nbit==0) {
				int8 off=libbase::read(mem,cpu->pc+1);
				if ((off&0b10000000)!=0){
					//add1 then invert
					cpu->pc-=~(off+1);
				} else {
					//else add 3?
					cpu->pc+=off+3;
				}
			} else {
				cpu->pc+=2
			}
			break;
	}
}

void bvc(int8 *mem, libbase::cpustruct* cpu) {//branch on overflow clear
	switch (libbase::read(mem,cpu->pc)) {
		case 0x50://relative
			if (cpu->sr&vbit==0) {
				int8 off=libbase::read(mem,cpu->pc+1);
				if ((off&0b10000000)!=0){
					//add1 then invert
					cpu->pc-=~(off+1);
				} else {
					//else add 3?
					cpu->pc+=off+3;
				}
			} else {
				cpu->pc+=2
			}
			break;
	}
}

void bvs(int8 *mem, libbase::cpustruct* cpu) {//branch on overflow set
	switch (libbase::read(mem,cpu->pc)) {
		case 0x70://relative
			if (cpu->sr&vbit==0) {
				int8 off=libbase::read(mem,cpu->pc+1);
				if ((off&0b10000000)!=0){
					//add1 then invert
					cpu->pc-=~(off+1);
				} else {
					//else add 3?
					cpu->pc+=off+3;
				}
			} else {
				cpu->pc+=2
			}
			break;
	}
}

void clc(int8 *mem, libbase::cpustruct* cpu) {//clear carry flag
	switch (libbase::read(mem,cpu->pc)) {
		case 0x18://relative
			cpu->sr&= ~cbit;
			cpu->pc+=1;
			break;
	}
}

void cld(int8 *mem, libbase::cpustruct* cpu) {//clear decimal flag
	switch (libbase::read(mem,cpu->pc)) {
		case 0xd8://relative
			cpu->sr&= ~dbit;
			cpu->pc+=1;
			break;
	}
}

void cli(int8 *mem, libbase::cpustruct* cpu) {//clear irq flag
	switch (libbase::read(mem,cpu->pc)) {
		case 0x58://relative
			cpu->sr&= ~ibit;
			cpu->pc+=1;
			break;
	}
}

void clv(int8 *mem, libbase::cpustruct* cpu) {//clear overflow flag
	switch (libbase::read(mem,cpu->pc)) {
		case 0x58://relative
			cpu->sr&= ~vbit;
			cpu->pc+=1;
			break;
	}
}

void cmp(int8 *mem, libbase::cpustruct* cpu) {
	switch (libbase::read(mem,cpu->pc)) {
		case 0xc9://immidiate
			result=cpu->a-libbase::read(mem,cpu->pc+1);
			//set the carry bit
			if (result|0b100000000) {
				cpu->sr|=cbit;
			} else {
				cpu->sr&=~cbit;
			}
			setsrbits(cpu, cpu->a,result,nbit|zbit);
			cpu->pc+=2;
			break;
		case 0xc5://zpg
			result=cpu->a-libbase::read(libbase::read(mem,cpu->pc+1));
			//set the carry bit
			if (result|0b100000000) {
				cpu->sr|=cbit;
			} else {
				cpu->sr&=~cbit;
			}
			setsrbits(cpu, cpu->a,result,nbit|zbit);
			cpu->pc+=2;
			break;
		case 0xd5://zpg,x
			result=cpu->a-libbase::read(libbase::read(mem,cpu->pc+1)+cpu->x);
			//set the carry bit
			if (result|0b100000000) {
				cpu->sr|=cbit;
			} else {
				cpu->sr&=~cbit;
			}
			setsrbits(cpu, cpu->a,result,nbit|zbit);
			cpu->pc+=2;
			break;
		case 0xcd://absolute
			result=cpu->a-libbase::read(readaddr(mem,cpu->pc+1));
			//set the carry bit
			if (result|0b100000000) {
				cpu->sr|=cbit;
			} else {
				cpu->sr&=~cbit;
			}
			setsrbits(cpu, cpu->a,result,nbit|zbit);
			cpu->pc+=3;
			break;
		case 0xdd://absolute,x
			result=cpu->a-libbase::read(readaddr(mem,cpu->pc+1)+cpu->x);
			//set the carry bit
			if (result|0b100000000) {
				cpu->sr|=cbit;
			} else {
				cpu->sr&=~cbit;
			}
			setsrbits(cpu, cpu->a,result,nbit|zbit);
			cpu->pc+=3;
			break;
		case 0xd9://absolute,y
			result=cpu->a-libbase::read(readaddr(mem,cpu->pc+1)+cpu->y);
			//set the carry bit
			if (result|0b100000000) {
				cpu->sr|=cbit;
			} else {
				cpu->sr&=~cbit;
			}
			setsrbits(cpu, cpu->a,result,nbit|zbit);
			cpu->pc+=3;
			break;
		case 0xc1://indirect+xoff
			result=cpu->a-libbase::read(mem,readaddr(mem, libbase::read(mem,cpu->pc+1)+cpu->x));
			//set the carry bit
			if (result|0b100000000) {
				cpu->sr|=cbit;
			} else {
				cpu->sr&=~cbit;
			}
			setsrbits(cpu, cpu->a,result,nbit|zbit);
			cpu->pc+=2;
			break;
		case 0xd1://indirect+y
			result=cpu->a-libbase::read(mem,readaddr(mem, libbase::read(mem,cpu->pc+1)))+cpu->y;
			//set the carry bit
			if (result|0b100000000) {
				cpu->sr|=cbit;
			} else {
				cpu->sr&=~cbit;
			}
			setsrbits(cpu, cpu->a,result,nbit|zbit);
			cpu->pc+=2;
			break;
	}
}

void cpx(int8 *mem, libbase::cpustruct* cpu) {
	switch (libbase::read(mem,cpu->pc)) {
		case 0xe0://immidiate
			result=cpu->x-libbase::read(mem,cpu->pc+1);
			//set the carry bit
			if (result|0b100000000) {
				cpu->sr|=cbit;
			} else {
				cpu->sr&=~cbit;
			}
			setsrbits(cpu, cpu->x,result,nbit|zbit);
			cpu->pc+=2;
			break;
		case 0xe4://zpg
			result=cpu->x-libbase::read(libbase::read(mem,cpu->pc+1));
			//set the carry bit
			if (result|0b100000000) {
				cpu->sr|=cbit;
			} else {
				cpu->sr&=~cbit;
			}
			setsrbits(cpu, cpu->x,result,nbit|zbit);
			cpu->pc+=2;
			break;
		case 0xec://absolute
			result=cpu->x-libbase::read(readaddr(mem,cpu->pc+1));
			//set the carry bit
			if (result|0b100000000) {
				cpu->sr|=cbit;
			} else {
				cpu->sr&=~cbit;
			}
			setsrbits(cpu, cpu->x,result,nbit|zbit);
			cpu->pc+=3;
			break;
	}
}

void cpy(int8 *mem, libbase::cpustruct* cpu) {
	switch (libbase::read(mem,cpu->pc)) {
		case 0xc0://immidiate
			result=cpu->y-libbase::read(mem,cpu->pc+1);
			//set the carry bit
			if (result|0b100000000) {
				cpu->sr|=cbit;
			} else {
				cpu->sr&=~cbit;
			}
			setsrbits(cpu, cpu->y,result,nbit|zbit);
			cpu->pc+=2;
			break;
		case 0xc4://zpg
			result=cpu->y-libbase::read(libbase::read(mem,cpu->pc+1));
			//set the carry bit
			if (result|0b100000000) {
				cpu->sr|=cbit;
			} else {
				cpu->sr&=~cbit;
			}
			setsrbits(cpu, cpu->y,result,nbit|zbit);
			cpu->pc+=2;
			break;
		case 0xcc://absolute
			result=cpu->y-libbase::read(mem, readaddr(mem,cpu->pc+1));
			//set the carry bit
			if (result|0b100000000) {
				cpu->sr|=cbit;
			} else {
				cpu->sr&=~cbit;
			}
			setsrbits(cpu, cpu->y,result,nbit|zbit);
			cpu->pc+=3;
			break;
	}
}

void dec(int8 *mem, libbase::cpustruct* cpu) {
	switch (libbase::read(mem,cpu->pc)) {
		case 0xc6://zpg
			int8 num = libbase::read(libbase::read(mem,cpu->pc+1));
			int8 addr = libbase::read(mem,cpu->pc+1);
			int8 result = num-1;
			libbase::write(mem,addr,result);
			setsrbits(cpu,num,result,nbit|zbit);
			cpu->pc+=2;
			break;
		case 0xd6://zpg,x
			int8 num = libbase::read(libbase::read(mem,cpu->pc+1)+cpu->x);
			int8 addr = libbase::read(mem,cpu->pc+1);
			int8 result = num-1;
			libbase::write(mem,addr,result);
			setsrbits(cpu,num,result,nbit|zbit);
			cpu->pc+=2;
			break;
		case 0xce://abs
			int8 num = libbase::read(readaddr(mem,cpu->pc+1));
			int8 addr = libbase::read(mem,cpu->pc+1);
			int8 result = num-1;
			libbase::write(mem,addr,result);
			setsrbits(cpu,num,result,nbit|zbit);
			cpu->pc+=3;
			break;
		case 0xde://abs,x
			int8 num = libbase::read(readaddr(mem,cpu->pc+1)+cpu->x);
			int8 addr = libbase::read(mem,cpu->pc+1);
			int8 result = num-1;
			libbase::write(mem,addr,result);
			setsrbits(cpu,num,result,nbit|zbit);
			cpu->pc+=3;
			break;
	}
}

void dex(int8 *mem, libbase::cpustruct* cpu) {
	switch (libbase::read(mem,cpu->pc)) {
		case 0xca://implied
			cpu->x--;
			setsrbits(cpu,0,cpu->x,nbit|zbit);
			cpu->pc+=1;
	}
}

void dey(int8 *mem, libbase::cpustruct* cpu) {
	switch (libbase::read(mem,cpu->pc)) {
		case 0x88://implied
			cpu->y--;
			setsrbits(cpu,0,cpu->y,nbit|zbit);
			cpu->pc+=1;
	}
}






void jmp(int8 *mem, libbase::cpustruct* cpu) {
	switch (libbase::read(mem,cpu->pc)){
		case 0x4c://absolute
			loadaddrlittle(mem,cpu,cpu->pc+1);
			std::cout<<"jmp "<<(int)cpu->pc<<'\n';
			break;
		case 0x6c://indirect
			std::cout<<"jmp "<<readaddr(mem,readaddr(mem,cpu->pc+1))<<'\n';
			//lda					arg pts to:	--------arg------------
			loadaddrlittle(mem,cpu,readaddr(mem,cpu->pc+1));
			break;
	}
}

void lda(int8 *mem, libbase::cpustruct* cpu) {
	// std::cout<<"lda:"<<(uint)cpu->a<<'\n';
	switch (libbase::read(mem,cpu->pc)){
		case 0xa9://imm
			cpu->a=libbase::read(mem,cpu->pc+1);
			std::cout<<"load "<<(int)libbase::read(mem,cpu->pc+1)<<"in a:"<<(int)cpu->a<<'\n';
			cpu->pc+=2;
			break;
		case 0xa5://zpg
			cpu->a=libbase::read(mem,libbase::read(mem,cpu->pc+1));
			cpu->pc+=2;
			break;
		case 0xb5://zpg,x
			cpu->a+=libbase::read(mem,cpu->pc+1+cpu->x);
			cpu->pc+=2;
			break;
		case 0xbd://absolute
			cpu->a=libbase::read(mem, readaddr(mem,cpu->pc+1) );
			cpu->pc+=3;
			break;
		case 0xb9://absolute,x
			cpu->a=libbase::read(mem, readaddr(mem,cpu->pc+1)+cpu->x );
			cpu->pc+=3;
			break;
		case 0xa1://absolute,y
			cpu->a=libbase::read(mem, readaddr(mem,cpu->pc+1)+cpu->y );
			cpu->pc+=3;
			break;
		case 0xb1://(indirect,x)
			cpu->a=libbase::read(mem,readaddr(mem, libbase::read(mem,cpu->pc+1)+cpu->x ));
			cpu->pc+=3;
			break;
		case 0xad://(indirect),y
			int8 tmp=cpu->a;
			cpu->a=libbase::read(mem,readaddr(mem, libbase::read(mem,cpu->pc+1)))+cpu->y;
			if (cpu->a<tmp) {
				cpu->sr=cpu->sr|0b000000001;
			} else {
				cpu->sr=cpu->sr&0b111111110;
			}
			cpu->pc+=2;
			break;
	}
}


void ldx(int8 *mem, libbase::cpustruct* cpu) {
	switch (libbase::read(mem,cpu->pc)) {
		case 0xa2://imm(byte)
			cpu->x=libbase::read(mem,cpu->pc+1);
			cpu->pc+=2;
			break;
		case 0xa6://zpg
			cpu->x&=libbase::read(mem,libbase::read(mem,cpu->pc+1));
			cpu->pc+=2;
			break;
		case 0xb6://zpg,y
			cpu->x&=libbase::read(mem,libbase::read(mem,cpu->pc+1)+cpu->y);
			cpu->pc+=2;
			break;
		case 0xae://abs
			cpu->x&=libbase::read(mem,readaddr(mem,cpu->pc+1));
			cpu->pc+=3;
			break;
		case 0xbe://abs,y
			cpu->x&=libbase::read(mem,readaddr(mem,cpu->pc+1)+cpu->y);
			cpu->pc+=3;
			break;
	}
}

void ldy(int8 *mem, libbase::cpustruct* cpu) {
	switch (libbase::read(mem,cpu->pc)) {
		case 0xa0://imm(byte)
			cpu->y=libbase::read(mem,cpu->pc+1);
			cpu->pc+=2;
			break;
		case 0xa4://zpg
			cpu->y&=libbase::read(mem,libbase::read(mem,cpu->pc+1));
			cpu->pc+=2;
			break;
		case 0xb4://zpg,x
			cpu->y&=libbase::read(mem,libbase::read(mem,cpu->pc+1)+cpu->x);
			cpu->pc+=2;
			break;
		case 0xac://abs
			cpu->y&=libbase::read(mem,readaddr(mem,cpu->pc+1));
			cpu->pc+=3;
			break;
		case 0xbc://abs,x
			cpu->y&=libbase::read(mem,readaddr(mem,cpu->pc+1)+cpu->x);
			cpu->pc+=3;
			break;
	}
}

void sta(int8 *mem, libbase::cpustruct* cpu) {
	switch (libbase::read(mem,cpu->pc)) {
		case 0x85:
			std::cout<<"sta:"<<(int)cpu->a<<" at "<<(int)libbase::read(mem,cpu->pc+1)<<"... ";
			libbase::write(mem,libbase::read(mem,cpu->pc+1),cpu->a);
			std::cout<<(int)libbase::read(mem,libbase::read(mem,cpu->pc+1))<<'\n';
			cpu->pc+=2;
			break;
	}
}

void hlt(int8 *mem, libbase::cpustruct* cpu) {
	cpu->sp+=0;
}

void setins(libbase::Emulator* emu) {
	//ror rotates number (keeps data)(with carry bit)
	//lsr shifts with a zero(loses partial data)

	//done
	emu->ins[0x69]=&adc;
	emu->ins[0x65]=&adc;
	emu->ins[0x75]=&adc;
	emu->ins[0x6d]=&adc;
	emu->ins[0x7d]=&adc;
	emu->ins[0x79]=&adc;
	emu->ins[0x61]=&adc;
	emu->ins[0x71]=&adc;
	//done
	emu->ins[0x29]=&ins_and;
	emu->ins[0x25]=&ins_and;
	emu->ins[0x35]=&ins_and;
	emu->ins[0x2d]=&ins_and;
	emu->ins[0x3d]=&ins_and;
	emu->ins[0x39]=&ins_and;
	emu->ins[0x21]=&ins_and;
	emu->ins[0x31]=&ins_and;
	//done
	emu->ins[0x0a]=&asl;
	emu->ins[0x06]=&asl;
	emu->ins[0x16]=&asl;
	emu->ins[0x0e]=&asl;
	emu->ins[0x1e]=&asl;
	//done
	emu->ins[0x90]=&bcc;
	emu->ins[0xb0]=&bcs;
	emu->ins[0xf0]=&beq;
	emu->ins[0x24]=&bit;
	emu->ins[0x2c]=&bit;
	emu->ins[0x30]=&bmi;
	emu->ins[0xd0]=&bne;
	emu->ins[0x10]=&bpl;
	emu->ins[0x00]=&BRK;
	emu->ins[0x50]=&bvc;
	emu->ins[0x70]=&bvs;
	emu->ins[0x18]=&clc;
	emu->ins[0xd8]=&cld;
	emu->ins[0x58]=&cli;
	emu->ins[0xb8]=&clv;
	
	emu->ins[0xc9]=&cmp;
	emu->ins[0xc5]=&cmp;
	emu->ins[0xd5]=&cmp;
	emu->ins[0xcd]=&cmp;
	emu->ins[0xdd]=&cmp;
	emu->ins[0xd9]=&cmp;
	emu->ins[0xc1]=&cmp;
	emu->ins[0xd1]=&cmp;

	emu->ins[0xe0]=&cpx;
	emu->ins[0xe4]=&cpx;
	emu->ins[0xec]=&cpx;

	emu->ins[0xc0]=&cpy;
	emu->ins[0xc4]=&cpy;
	emu->ins[0xcc]=&cpy;

	emu->ins[0xc6]=&dec;
	emu->ins[0xd6]=&dec;
	emu->ins[0xce]=&dec;
	emu->ins[0xde]=&dec;

	emu->ins[0xca]=&dex;

	emu->ins[0x88]=&dey;





	emu->ins[0x02]=&hlt;
	emu->ins[0x4c]=&jmp;
	emu->ins[0x6c]=&jmp;
	emu->ins[0xa9]=&lda;
	emu->ins[0xa5]=&lda;
	emu->ins[0xad]=&lda;
	emu->ins[0xa2]=&ldx;
	emu->ins[0x85]=&sta;
	printf("ins dict at:%p\n",&emu->ins[0x00]);
	printf("ins dict at:%p\n",&emu->ins[0xa9]);
	printf("ins dict at:%p\n",&emu->ins[0x4c]);
	printf("ins dict at:%p\n",&emu->ins[0x69]);
	printf("ins dict at:%p\n",&emu->ins[0x6c]);
	printf("brk:%p\njmp:%p\nlda:%p\n",BRK,jmp,lda);


}