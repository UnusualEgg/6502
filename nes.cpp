#include <iostream>
#include <fstream>
#include <cxxabi.h>
#include <map>
#include "include/cpu.hpp"
#include "include/memory.hpp"
#include "include/Emulator.hpp"
#include "include/instructions.hpp"
typedef unsigned char int8;
typedef unsigned short uint16;
#define printRegs std::cout<<"cpu axy sr:"<<(int)cpu.a<<' '<<(int)cpu.x<<' '<<(int)cpu.y<<' '<<(int)cpu.sr<<'\n'


//-----------------main----------------------


int main(int argc, char const *argv[])
{
	//get level
	if (argc>1&&*argv[1]=='d'){
		spdlog::set_level(spdlog::level::debug);
	}
	
	const char secret[] = "Your... MOTHER!";
	std::cout<<"argv:"<<*argv<<std::endl;
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
	std::ifstream pf("prg.bin", std::ifstream::binary);
	pflen = pf.tellg(); //begining
	pf.seekg(0, std::ios::end);
	pflen = pf.tellg()-pflen; //end-begining
	if (pf.good()){
		std::cout<<"good!\n";

	pf.seekg(0);
	char prg[(int)pflen];
	pf.read(&prg[0],pflen);
	pf.close();

	//----(prg is in (char)prg array)

	//time to load the prg into memory
	for (uint i=0;i<pflen and i<0x10000;i++){
		mem[i]=prg[i];//+16 bc header
	}

    std::cout<<std::hex;

//    getSP;
    cpustruct cpu;
	Emulator emulator;

	//check typeof(len)
	char *name=abi::__cxa_demangle(typeid(pflen).name(), 0, 0, 0);
	std::cout<<"type of len:"<<name<<std::endl;

	//set instruction to functions
	setins(&emulator);
	emulator.RESET(ptr, &rw[0], &cpu);
    printRegs;
	} else {
		pf.close();
		std::cout<<"file bad";
	}
	return 0;
}

