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

void loadaddr(int8 *mem, libbase::cpustruct* cpu, uint16 addr) {
	cpu->pc=((uint16)libbase::read(mem,addr)&0xff00)+(((uint16)libbase::read(mem,addr+1))>>8);
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
	//reset registers
	cpu->a=0;
	cpu->x=0;
	cpu->y=0;
	cpu->pc=0;
	cpu->sr=0;
	cpu->sp=0;
	cpu->bp=0xff;

	std::cout<<"getting starting adress..."<<std::endl;
	//fffc fffd
	start=((unsigned int) libbase::read(mem, (uint16)0xfffc))<<8;//high part
	start+=libbase::read(mem, (uint16)0xfffd);//low part
	cpu->pc=start;
	std::cout<<"start address="<<cpu->pc<<std::endl;
	std::cout<<"Instruction:"<<libbase::read(mem,cpu->pc)<<'\n';
	execute(mem,cpu);
}


int libbase::Emulator::execute(int8 *mem, libbase::cpustruct *cpu) {
	
	for (int cycle = 0; cycle < 100; ++cycle) {
		if (ins.find(libbase::read(mem,cpu->pc))!=ins.end()) {
			std::cout<<std::hex<<"Instruction:"<<(int)libbase::read(mem,cpu->pc)<<" at "<<(int)cpu->pc<<'\n';
			std::cout<<"Instruction:"<<&ins[cpu->pc]<<'\n';
			ins[cpu->pc](mem,cpu);
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
	loadaddr(mem,cpu,0xfffc);
}

void IRQ(int8 *mem, libbase::cpustruct *cpu) {
	//push pc
	pushpc(mem,cpu,2);
	//push processor status register
	php(mem,cpu);
	//set interrupt disable
	updatesr(cpu,cpu->sr|4);//3rd bit /irq disable
	//go to addr (nmi fffe/ffff)
	loadaddr(mem,cpu,0xfffe);
}

void BRK(int8 *mem, libbase::cpustruct *cpu) {
	//push pc
	pushpc(mem,cpu,2);
	//push processor status register
	php(mem,cpu);
	//set interrupt disable
	updatesr(cpu,cpu->sr|20);//3rd bit /irq disable and break bit (16)
	//go to addr (nmi fffe/ffff)
	loadaddr(mem,cpu,0xfffe);
}







//set instructions

// void brk(uint16 pc,int8 *mem)
// void brk(uint16 pc,int8 *mem)

//adc
void adc(int8 *mem, libbase::cpustruct* cpu){
	switch (libbase::read(mem,cpu->pc)){
		case 0x69://immidiate
			int8 tmp=cpu->a;
			cpu->a+=libbase::read(mem, cpu->pc+1)+(cpu->sr&0b00000001);//carry bit
			if (cpu->a<tmp) {
				cpu->sr=cpu->sr|0b000000001;
				cpu->pc+=2;
			} else {
				cpu->sr=cpu->sr&0b111111110;
				cpu->pc+=2;
			}
			std::cout<<"adc "<<libbase::read(mem, cpu->pc+1)+(cpu->sr&0b00000001)<<std::endl;
			break;
	}
}
void jmp(int8 *mem, libbase::cpustruct* cpu) {
	switch (libbase::read(mem,cpu->pc)){
		case 0x4c://absolute
			loadaddr(mem,cpu,cpu->pc+1);
			break;
		case 0x6c://indirect
			loadaddr(mem,cpu,(uint16)libbase::read(mem,cpu->pc+1)+((uint16)libbase::read(mem,cpu->pc+2)<<8));
			break;
	}
}

void lda(int8 *mem, libbase::cpustruct* cpu) {
	std::cout<<"lda:"<<cpu->a<<'\n';
	switch (libbase::read(mem,cpu->pc)){
		case 0xa9://imm
			cpu->a+=libbase::read(mem,cpu->pc+1);
			cpu->pc+=2;
			break;
		case 0x65://zpg
			cpu->a=libbase::read(mem,libbase::read(mem,cpu->pc+1));
			cpu->pc+=2;
			break;
		case 0x6d://absolute
			cpu->a=libbase::read(mem, (((uint16)libbase::read(mem,cpu->pc+2))<<8) + (uint16)libbase::read(mem,cpu->pc+1) );
			cpu->pc+=2;
			break;
	}
}

void setins(libbase::Emulator* emu) {
	emu->ins[0x00]=&BRK;
	emu->ins[0x69]=&adc;
	emu->ins[0x4c]=&jmp;
	emu->ins[0x6c]=&jmp;
	emu->ins[0xa9]=&lda;
	printf("brk:%p\njmp:%p\nlda:%p\n",BRK,jmp,lda);


}