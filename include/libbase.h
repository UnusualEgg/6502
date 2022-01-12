typedef unsigned char int8;
typedef unsigned short uint16;

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
        *(mem+(addr<<1))=value;
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

void loadaddrbig(int8 *mem, libbase::cpustruct* cpu, uint16 addr) {
	cpu->pc=(((uint16)libbase::read(mem,addr+1))<<8)+ ((uint16)libbase::read(mem,addr));
}

void loadaddrlittle(int8 *mem, libbase::cpustruct* cpu, uint16 addr) {
	cpu->pc=(((uint16)libbase::read(mem,addr))<<8)+ ((uint16)libbase::read(mem,addr+1));
}

uint16 readadr(int8 *mem,uint16 addr) {
	return (((uint16)libbase::read(mem,addr+1))<<8)+ ((uint16)libbase::read(mem,addr));
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
	cpu->sr=cpu->sr&0b01111111;
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
	loadaddrbig(mem,cpu,0xfffc);
	std::cout<<"start address="<<cpu->pc<<std::endl;
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
			std::cout<<"Unknown Instruction:"<<libbase::read(mem,cpu->pc)<<" at "<<cpu->pc<<'\n';
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
	updatesr(cpu,cpu->sr|4);//3rd bit /irq disable
	//go to addr (nmi fffc/fffd)
	loadaddrbig(mem,cpu,0xfffc);
}

void IRQ(int8 *mem, libbase::cpustruct *cpu) {
	//push pc
	pushpc(mem,cpu,2);
	//push processor status register
	php(mem,cpu);
	//set interrupt disable
	updatesr(cpu,cpu->sr|4);//3rd bit /irq disable
	//go to addr (nmi fffe/ffff)
	loadaddrbig(mem,cpu,0xfffe);
}

void BRK(int8 *mem, libbase::cpustruct *cpu) {
	//push pc
	pushpc(mem,cpu,2);
	//push processor status register
	php(mem,cpu);
	//set interrupt disable
	updatesr(cpu,cpu->sr|20);//3rd bit /irq disable and break bit (16)
	//go to addr (nmi fffe/ffff)
	loadaddrbig(mem,cpu,0xfffe);
	std::cout<<"BRK!\n";
}







//set instructions

// void brk(uint16 pc,int8 *mem)
// void brk(uint16 pc,int8 *mem)

//adc
void adc(int8 *mem, libbase::cpustruct* cpu){
	int8 tmp=cpu->a;
	switch (libbase::read(mem,cpu->pc)){
		case 0x69://immidiate
			cpu->a+=libbase::read(mem, cpu->pc+1)+(cpu->sr&0b00000001);//carry bit
			if (cpu->a<tmp) {
				cpu->sr=cpu->sr|0b000000001;
			} else {
				cpu->sr=cpu->sr&0b111111110;
			}
			cpu->pc+=2;
			std::cout<<"adc "<<libbase::read(mem, cpu->pc+1)+(cpu->sr&0b00000001)<<std::endl;
			break;
		case 0x6d://absolute
			cpu->a+=libbase::read(mem,readadr(mem,cpu->pc+1))+(cpu->sr&0b01);
			//set the carry bit
			if (cpu->a<tmp) {
				cpu->sr=cpu->sr|0b000000001;
			} else {
				cpu->sr=cpu->sr&0b111111110;
			}
			//----
			cpu->pc+=3;
			break;
	}
}
void jmp(int8 *mem, libbase::cpustruct* cpu) {
	switch (libbase::read(mem,cpu->pc)){
		case 0x4c://absolute
			loadaddrlittle(mem,cpu,cpu->pc+1);
			std::cout<<"jmp "<<(int)cpu->pc<<'\n';
			break;
		case 0x6c://indirect
			loadaddrlittle(mem,cpu,readadr(mem,cpu->pc+1));
			break;
	}
}

void lda(int8 *mem, libbase::cpustruct* cpu) {
	// std::cout<<"lda:"<<(uint)cpu->a<<'\n';
	switch (libbase::read(mem,cpu->pc)){
		case 0xa9://imm
			cpu->a+=libbase::read(mem,cpu->pc+1);
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
			cpu->a=libbase::read(mem, readadr(mem,cpu->pc+1) );
			cpu->pc+=3;
			break;
		case 0xb9://absolute,x
			cpu->a=libbase::read(mem, readadr(mem,cpu->pc+1)+cpu->x );
			cpu->pc+=3;
			break;
		case 0xa1://absolute,y
			cpu->a=libbase::read(mem, readadr(mem,cpu->pc+1)+cpu->y );
			cpu->pc+=3;
			break;
		case 0xb1://(indirect,x)
			cpu->a=libbase::read(mem,readadr(mem, libbase::read(mem,cpu->pc+1)+cpu->x ));
			cpu->pc+=3;
			break;
		case 0xad://(indirect),y
			int8 tmp=cpu->a;
			cpu->a=libbase::read(mem,readadr(mem, libbase::read(mem,cpu->pc+1)))+cpu->y;
			if (cpu->a<tmp) {
				cpu->sr=cpu->sr|0b000000001;
			} else {
				cpu->sr=cpu->sr&0b111111110;
			}
			cpu->pc+=2;
			break;
	}
}

void ins_and(int8 *mem, libbase::cpustruct* cpu) {
	switch (libbase::read(mem,cpu->pc)) {
		case 0x29://imm
			cpu->a&=libbase::read(mem,cpu->pc);
			cpu->pc+=2;
			break;
		case 0x25://zpg
			cpu->a&=libbase::read(mem,libbase::read(mem,cpu->pc+1));
			cpu->pc+=2;
			break;
		case 0x35://zpg,x
			cpu->a&=libbase::read(mem,readadr(mem,cpu->pc+1)+cpu->x);
			cpu->pc+=2;
			break;
		case 0x2d://abs
			cpu->a&=libbase::read(mem,readadr(mem,cpu->pc+1));
			cpu->pc+=2;
			break;
		case 0x3d://abs,x
			cpu->a&=libbase::read(mem,readadr(mem,cpu->pc+1)+cpu->x);
			cpu->pc+=2;
			break;
		case 0x39://abs,y
			cpu->a&=libbase::read(mem,readadr(mem,cpu->pc+1)+cpu->y);
			cpu->pc+=2;
			break;
		case 0x21://(indirect,x)
			cpu->a&=libbase::read(mem,readadr(mem, libbase::read(mem,cpu->pc+1)+cpu->x ));
			cpu->pc+=3;
			break;
		case 0x31://(indirect),y
			cpu->a&=libbase::read(mem,readadr(mem, libbase::read(mem,cpu->pc+1)));
			int8 tmp=cpu->a;
			cpu->a+=cpu->y;
			if (cpu->a<tmp) {
				cpu->sr=cpu->sr|0b000000001;
			} else {
				cpu->sr=cpu->sr&0b111111110;
			}
			cpu->pc+=2;
			break;
	}
}

void hlt(int8 *mem, libbase::cpustruct* cpu) {
	cpu->sp+=0;
}

void setins(libbase::Emulator* emu) {
	emu->ins[0x00]=&BRK;
	emu->ins[0x02]=&hlt;
	emu->ins[0x69]=&adc;
	emu->ins[0x6d]=&adc;
	emu->ins[0x4c]=&jmp;
	emu->ins[0x6c]=&jmp;
	emu->ins[0xa9]=&lda;
	emu->ins[0xa5]=&lda;
	emu->ins[0xad]=&lda;
	printf("ins dict at:%p\n",&emu->ins[0x00]);
	printf("ins dict at:%p\n",&emu->ins[0xa9]);
	printf("ins dict at:%p\n",&emu->ins[0x4c]);
	printf("ins dict at:%p\n",&emu->ins[0x69]);
	printf("ins dict at:%p\n",&emu->ins[0x6c]);
	printf("brk:%p\njmp:%p\nlda:%p\n",BRK,jmp,lda);


}