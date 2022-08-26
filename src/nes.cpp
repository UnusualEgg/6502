#include <iostream>
#include <fstream>
#include <cxxabi.h>
#include <map>
#include <defs.hpp>
#include <cpu.hpp>
#include <memory.hpp>
#include <Emulator.hpp>
#include <instructions.hpp>
typedef unsigned char int8;
typedef unsigned short uint16;
#define printRegs std::cout<<"cpu axy sr:"<<(int)cpu.a<<' '<<(int)cpu.x<<' '<<(int)cpu.y<<' '<<(int)cpu.sr<<'\n'


//-----------------main----------------------


int main(int argc, char const *argv[])
{
	const char secret[] = "Your... MOTHER!";
	//memory
	int8 mem[0xffff];
	int8 *ptr = &mem[0];
    //read or write
    bool rw[0xffff];
	// set writable
    for (unsigned int i = 0x0; i<=0xffff;i++) {
        if (i>=0xc000) {
            rw[i] = false;
        } else {
            rw[i] = true;
        }
    }

	uint16 startaddr = 0;

	//find length of prg and read prg
	std::streampos pflen;
	std::ifstream pf("prg.nes", std::ifstream::binary);
	pflen = pf.tellg(); //begining
	pf.seekg(0, std::ios::end);
	pflen = pf.tellg()-pflen; //end-begining
	if (pf.good()){
		std::cout<<"good!\n";

	pf.seekg(16);
	char prg[(int)pflen];
	pf.read(&prg[0],pflen);
	pf.close();

	//print length of prg and prg itself
	// std::cout<<std::hex<<"len:"<<pflen<<std::endl<<"prg:[";
	// for (int i = 0; i < pflen; ++i) {
	// 	std::cout<<((uint)prg[i]&0xff)<<' ';
	// }
	// std::cout<<"\b]\n";
	//----(prg is in (char)prg array)

	//time to load the prg into memory
	for (uint i=0;i<pflen and i<0x10000;i++){
		mem[i]=prg[i];//+16 bc header
	}

//	std::cout<<std::hex<<"mem:[";
//	for (uint i=0;;i++) {
//		std::cout<<i<<' '<<(int)libbase::read(ptr,i) <<", ";
//		if (i==0xffff){
//			break;
//		}
//	}
//	std::cout<<"\b\b]\n";
	// for (int i=pflen;i<0xffff;i++){
	// 	mem[i]=0x01;
	// }
    std::cout<<std::hex;

//    getSP;
    cpustruct cpu;
	Emulator emulator;

	//check typeof(len)
	char *name=abi::__cxa_demangle(typeid(pflen).name(), 0, 0, 0);
	std::cout<<"type of len:"<<name<<std::endl;
	// std::cout<<"len:"<<len<<std::endl;

	//set instruction to functions
	setins(&emulator);
	emulator.RESET(ptr, &rw[0], &cpu);
//    std::cout<<"cpu axy sr:"<<(int)cpu.a<<' '<<(int)cpu.x<<' '<<(int)cpu.y<<' '<<(int)cpu.sr<<'\n';
    printRegs;
	// printf("a:%u\n",cpu.a);
	// free(name);
	// std::cout<<emulator.ins[0x4c]<<'\n';
	} else {
		pf.close();
		std::cout<<"file bad";
	}
	return 0;
}

