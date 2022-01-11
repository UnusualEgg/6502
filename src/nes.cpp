#include <iostream>
#include <fstream>
#include <cxxabi.h>
#include <map>
#include "libbase.h"
typedef unsigned char int8;
typedef unsigned short uint16;


//-----------------main----------------------


int main(int argc, char const *argv[])
{
	//memory
	int8 mem[0xffff];
	int8 *ptr = &mem[0];
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
	std::cout<<std::hex<<"len:"<<pflen<<std::endl<<"prg:[";
	for (int i = 0; i < pflen; ++i) {
		std::cout<<((uint)prg[i]&0xff)<<' ';
	}
	std::cout<<"\b]\n";
	//----(prg is in (char)prg array)

	//time to load the prg into memory
	std::cout<<"mem:[";
	for (int i=0;i<pflen;i++){
		mem[i]=prg[i];
		std::cout<<i<<' '<<(uint)libbase::read(ptr,i) <<", ";
	}
	std::cout<<"\b\b]\n";
	for (int i=pflen;i<0xffff;i++){
		mem[i]=0;
	}


	mem[0xfffc]=(int8)startaddr<<8;
	mem[0xfffd]=(int8)startaddr&0x00ff;


	// std::map<char[2], int*> regs;
	// regs[""]
	/* code */
	// std::cout<<"Hello!\n";
	libbase::cpustruct cpu;
	libbase::Emulator emulator;
	// char buff[2];
	// char* bptr=&name[0];
	// bptr=len
	char *name=abi::__cxa_demangle(typeid(pflen).name(), 0, 0, 0);
	std::cout<<"type of len:"<<name<<std::endl;
	// std::cout<<"len:"<<len<<std::endl;
	// Emulator emulator();
	setins(&emulator);
	emulator.RESET(ptr, &cpu);
	std::cout<<"cpu axy:"<<(int)cpu.a<<' '<<(int)cpu.x<<' '<<(int)cpu.y<<'\n';
	// printf("a:%u\n",cpu.a);
	// free(name);
	// std::cout<<emulator.ins[0x4c]<<'\n';
	return 0;
}

