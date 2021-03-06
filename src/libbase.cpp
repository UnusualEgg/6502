#include <iostream>
#include <fstream>
#include <cxxabi.h>
#include <map>
#include "../include/libbase.h"

typedef unsigned char int8;
typedef unsigned short uint16;

int8 mem[0xffff];
int8 *ptr=&mem[0];



//------------------------------libbase::Emulator---------------------------------------------------------



//------------Constructor---------------

// libbase::Emulator::libbase::Emulator() {
// 	// reset(mem,cpu);
// };






/*addressing modes

A	Accumulator	OPC A	operand is AC (implied single byte instruction)
abs	absolute	OPC $LLHH	operand is address $HHLL *
abs,X	absolute, X-indexed	OPC $LLHH,X	operand is address; effective address is address incremented by X with carry **
abs,Y	absolute, Y-indexed	OPC $LLHH,Y	operand is address; effective address is address incremented by Y with carry **
#	immediate	OPC #$BB	operand is byte BB
impl	implied	OPC	operand implied
ind	indirect	OPC ($LLHH)	operand is address; effective address is contents of word at address: C.w($HHLL)
X,ind	X-indexed, indirect	OPC ($LL,X)	operand is zeropage address; effective address is word in (LL + X, LL + X + 1), inc. without carry: C.w($00LL + X)
ind,Y	indirect, Y-indexed	OPC ($LL),Y	operand is zeropage address; effective address is word in (LL, LL + 1) incremented by Y with carry: C.w($00LL) + Y
rel	relative	OPC $BB	branch target is PC + signed offset BB ***
zpg	zeropage	OPC $LL	operand is zeropage address (hi-byte is zero, address = $00LL)
zpg,X	zeropage, X-indexed	OPC $LL,X	operand is zeropage address; effective address is address incremented by X without carry **
zpg,Y	zeropage, Y-indexed	OPC $LL,Y	operand is zeropage address; effective address is address incremented by Y without carry **


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