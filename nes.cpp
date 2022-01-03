#include <iostream>
#include <fstream>
#include <cxxabi.h>
#include <map>

typedef unsigned char int8;
typedef unsigned short uint16;
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
//-----------------------------Memory-------------------------
int8 read(int8 *mem, uint16 addr){
	if (addr>=0&&addr<=0xffff){
		return *(mem+addr);
	}
	return -1;
};
bool write(int8 *mem, uint16 addr, int8 value) {
    if (addr>=0&&addr<=0xffff){
        *(mem+(addr<<1))=value;
        return true;
    } else {
        return false;
    }
}
void pha(int8 *mem,cpustruct* cpu) {
	cpu->sp--;
	write(mem,cpu->sp,cpu->a);
}
void php(int8 *mem,cpustruct* cpu) {
	cpu->sp--;
	write(mem,cpu->sp,cpu->sr);
}
void pla(int8 *mem,cpustruct* cpu) {
	cpu->a=read(mem,cpu->pc);
	cpu->pc++;
}
void plp(int8 *mem,cpustruct* cpu) {
	cpu->sr=read(mem,cpu->pc)&0b1101111;
	cpu->pc++;
}
void pushaddr(int8 *mem, cpustruct* cpu, uint16 addr) {
	cpu->pc--;
	write(mem, cpu->pc, (int8)(addr&0xff));
	cpu->pc--;
	write(mem, cpu->pc, (int8)(addr>>8));
}
int updatesr(cpustruct* cpu, int8 value) {
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

//------------------------------Emulator---------------------------------------------------------
class Emulator {
private:
	// Emulator();

public:
	unsigned int start;
	// int8 tmp;
	std::map<int8, void (*)(int8*,cpustruct*)> ins;
	int updatesr(cpustruct* cpu,int8 value);
	void reset(cpustruct *cpu,int8 *mem);
	int execute(int8 address,cpustruct *cpu,int8 *mem);
};



//------------Constructor---------------

// Emulator::Emulator() {
// 	// reset(cpu,mem);
// };




void Emulator::reset(cpustruct *cpu, int8 *mem) {
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
	start=((unsigned int) read(mem, (uint16)0xfffc))<<8;//high part
	start+=read(mem, (uint16)0xfffd);//low part
	std::cout<<"start address="<<start<<std::endl;
	execute(start,cpu,mem);
}


int Emulator::execute(int8 address,cpustruct *cpu, int8 *mem) {
	cpu->pc=address;
	do {
		if (ins.find(read(mem,cpu->pc))!=ins.end()) {
			ins[cpu->pc](mem,cpu);
		} else {
			std::cout<<"Unknown Instruction:"<<std::hex<<(uint)read(mem,cpu->pc)<<'\n';
			break;
		}
	} while ((cpu->sr&0000100)==0);//interrupt flag
	return 0;
}


//-----------------main----------------------



int main(int argc, char const *argv[])
{
	// class Emulator;
	int8 read(int8 *mem, uint16 addr);
	bool write(int8 *mem, uint16 addr, int8 value);
	//memory
	int8 mem[0xffff];
	int8 *pm = &mem[0];
	std::fill_n(mem,0xffff,0);
	// mem[0]=0x69;

	uint16 startaddr = 0;

	//find length of prg and read prg
	std::streampos pflen;
	std::ifstream pf("prg.bin", std::ifstream::binary);
	pflen = pf.tellg(); //begining
	pf.seekg(0, std::ios::end);
	pflen = pf.tellg()-pflen; //end-begining
	if (pf.good()){
		std::cout<<"good!\n";
	}
	pf.seekg(0);
	char prg[(int)pflen];
	pf.read(&prg[0],pflen);
	pf.close();

	//print length of prg and prg itself
	std::cout<<"len:"<<pflen<<std::endl<<"prg:[";
	for (int i = 0; i < pflen; ++i) {
		std::cout<<std::hex<<(uint)prg[i]<<' ';
	}
	std::cout<<"\b]\n";
	//----(prg is in (char)prg array)

	//time to load the prg into memory
	for (int i=0;i<pflen;i++){
		mem[i]=prg[i];
	}


	mem[0xfffc]=(int8)startaddr<<8;
	mem[0xfffd]=(int8)startaddr&0x00ff;
	int8 *ptr;
	ptr = mem;

	// std::map<char[2], int*> regs;
	// regs[""]
	/* code */
	// std::cout<<"Hello!\n";
	cpustruct cpu;
	Emulator emulator;
	// char buff[2];
	// char* bptr=&name[0];
	// bptr=len
	char *name=abi::__cxa_demangle(typeid(pflen).name(), 0, 0, 0);
	std::cout<<"type of len:"<<name<<std::endl;
	// std::cout<<"len:"<<len<<std::endl;
	// Emulator emulator();
	emulator.reset(&cpu,ptr);
	// printf("a:%u\n",cpu.a);
	// free(name);
	return 0;
}

//set instructions

void brk(int8 *mem,cpustruct* cpu){//00
	pushaddr(mem,cpu,cpu->pc+2);
	cpu->sp--;
	mem[cpu->sp]=cpu->sr|0b0001000;//break flag
	php(mem,cpu);
	cpu->pc++;
	//trigger irq event
}
// void brk(uint16 pc,int8 *mem)
// void brk(uint16 pc,int8 *mem)

//adc
void adc(int8 *mem,cpustruct* cpu){
	int8 tmp=cpu->a;
	cpu->a+=read(mem, cpu->pc+1)+(cpu->sr&0b00000001);//carry bit
	if (cpu->a<tmp) {
		cpu->sr=cpu->sr|0b000000001;
		cpu->pc+=2;
	} else {
		cpu->sr=cpu->sr&0b111111110;
		cpu->pc+=2;
	}
	std::cout<<"adc "<<read(mem, cpu->pc+1)+(cpu->sr&0b00000001)<<std::endl;
}
void setins(Emulator* emu) {
	emu->ins[0x00]=&brk;
	emu->ins[0x69]=&adc;

}