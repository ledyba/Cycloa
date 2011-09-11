#include "exception/EmulatorException.h"
#include "VirtualMachine.h"
#include <stdio.h>

Processor::Processor(VirtualMachine& vm):
    VM(vm),
	A(0),
	X(0),
	Y(0),
	PC(0),
	SP(0),
	P(0),
	NMI(false),
	IRQ(false),
	needStatusRewrite(false),
	newStatus(0)
{
}

void Processor::onHardReset()
{
    //from http://wiki.nesdev.com/w/index.php/CPU_power_up_state
    this->P = 0x24;
    this->A = 0x0;
    this->X = 0x0;
    this->Y = 0x0;
    this->SP = 0xfd;
    this->write(0x4017, 0x00);
    this->write(0x4015, 0x00);
    for(uint16_t i=0x4000;i<=0x4000;i++){
        this->write(i, 0x00);
    }
    this->PC = (read(0xFFFC) | (read(0xFFFD) << 8));

    this->NMI = false;
    this->IRQ = false;
    this->needStatusRewrite = false;
    this->newStatus = 0;
}

void Processor::onReset()
{
    //from http://wiki.nesdev.com/w/index.php/CPU_power_up_state
    //from http://crystal.freespace.jp/pgate1/nes/nes_cpu.htm
    consumeClock(6);
    this->SP -= 0x03;
    this->P |= FLAG_I;
    this->write(0x4015, 0x0);
    this->PC = (read(0xFFFC) | (read(0xFFFD) << 8));

    this->NMI = false;
    this->IRQ = false;
    this->needStatusRewrite = false;
    this->newStatus = 0;
}

void Processor::sendNMI()
{
	this->NMI = true;
}
void Processor::reserveIRQ()
{
	this->IRQ = true;
}

void Processor::releaseIRQ()
{
	this->IRQ = false;
}

void Processor::run(uint16_t clockDelta)
{
    this->P |= FLAG_ALWAYS_SET;

    if(this->NMI){
        this->onNMI();
    }else if(this->IRQ){
        this->onIRQ();
    }

    if(needStatusRewrite){
    	this->P = newStatus;
    	needStatusRewrite = false;
    }

    uint8_t opcode = this->read(this->PC);
    #ifdef CPUTRACE
    char flag[9];
    flag[0] = (this->P & FLAG_N) ? 'N' : 'n';
    flag[1] = (this->P & FLAG_V) ? 'V' : 'v';
    flag[2] = (this->P & FLAG_ALWAYS_SET) ? 'U' : 'u';
    flag[3] = (this->P & FLAG_B) ? 'B' : 'b';
    flag[4] = (this->P & FLAG_D) ? 'D' : 'd';
    flag[5] = (this->P & FLAG_I) ? 'I' : 'i';
    flag[6] = (this->P & FLAG_Z) ? 'Z' : 'z';
    flag[7] = (this->P & FLAG_C) ? 'C' : 'c';
    flag[8] = '\0';
    printf("%04x op:%02x a:%02x x:%02x y:%02x sp:%02x p:%s IRQ?:%s NMI?:%s\n", this->PC, opcode, this->A, this->X, this->Y, this->SP, flag, IRQ ? "on" : "off", NMI ? "on" : "off");
    fflush(stdout);
    #endif
    this->PC++;

    switch(opcode){
        case 0x00: // BRK
            this->BRK();
            break;
        case 0x01: // ORA - (Indirect,X)
            this->ORA(addrIndirectX());
            break;
        //case 0x02: // Future Expansion
        //case 0x03: // Future Expansion
        //case 0x04: // Future Expansion
        case 0x05: // ORA - Zero Page
            this->ORA(addrZeroPage());
            break;
        case 0x06: // ASL - Zero Page
            this->ASL(addrZeroPage());
            break;
        //case 0x07: // Future Expansion
        case 0x08: // PHP
            this->PHP();
            break;
        case 0x09: // ORA - Immediate
            this->ORA(addrImmediate());
            break;
        case 0x0A: // ASL - Accumulator
            this->ASL();
            break;
        //case 0x0B: // Future Expansion
        //case 0x0C: // Future Expansion
        case 0x0D: // ORA - Absolute
            this->ORA(addrAbsolute());
            break;
        case 0x0E: // ASL - Absolute
            this->ASL(addrAbsolute());
            break;
        //case 0x0F: // Future Expansion
        case 0x10: // BPL
            this->BPL(addrRelative());
            break;
        case 0x11: // ORA - (Indirect),Y
            this->ORA(addrIndirectY());
            break;
        //case 0x12: // Future Expansion
        //case 0x13: // Future Expansion
        //case 0x14: // Future Expansion
        case 0x15: // ORA - Zero Page,X
            this->ORA(addrZeroPageIdxX());
            break;
        case 0x16: // ASL - Zero Page,X
            this->ASL(addrZeroPageIdxX());
            break;
        //case 0x17: // Future Expansion
        case 0x18: // CLC
            this->CLC();
            break;
        case 0x19: // ORA - Absolute,Y
            this->ORA(addrAbsoluteIdxY());
            break;
        //case 0x1A: // Future Expansion
        //case 0x1B: // Future Expansion
        //case 0x1C: // Future Expansion
        case 0x1D: // ORA - Absolute,X
            this->ORA(addrAbsoluteIdxX());
            break;
        case 0x1E: // ASL - Absolute,X
            this->ASL(addrAbsoluteIdxX());
            break;
        //case 0x1F: // Future Expansion
        case 0x20: // JSR - Absolute
            this->JSR(addrAbsolute());
            break;
        case 0x21: // AND - (Indirect,X)
            this->AND(addrIndirectX());
            break;
        //case 0x22: // Future Expansion
        //case 0x23: // Future Expansion
        case 0x24: // BIT - Zero Page
            this->BIT(addrZeroPage());
            break;
        case 0x25: // AND - Zero Page
            this->AND(addrZeroPage());
            break;
        case 0x26: // ROL - Zero Page
            this->ROL(addrZeroPage());
            break;
        //case 0x27: // Future Expansion
        case 0x28: // PLP
            this->PLP();
            break;
        case 0x29: // AND - Immediate
            this->AND(addrImmediate());
            break;
        case 0x2A: // ROL - Accumulator
            this->ROL();
            break;
        //case 0x2B: // Future Expansion
        case 0x2C: // BIT - Absolute
            this->BIT(addrAbsolute());
            break;
        case 0x2D: // AND - Absolute
            this->AND(addrAbsolute());
            break;
        case 0x2E: // ROL - Absolute
            this->ROL(addrAbsolute());
            break;
        //case 0x2F: // Future Expansion
        case 0x30: // BMI
            this->BMI(addrRelative());
            break;
        case 0x31: // AND - (Indirect),Y
            this->AND(addrIndirectY());
            break;
        //case 0x32: // Future Expansion
        //case 0x33: // Future Expansion
        //case 0x34: // Future Expansion
        case 0x35: // AND - Zero Page,X
            this->AND(addrZeroPageIdxX());
            break;
        case 0x36: // ROL - Zero Page,X
            this->ROL(addrZeroPageIdxX());
            break;
        //case 0x37: // Future Expansion
        case 0x38: // SEC
            this->SEC();
            break;
        case 0x39: // AND - Absolute,Y
            this->AND(addrAbsoluteIdxY());
            break;
        //case 0x3A: // Future Expansion
        //case 0x3B: // Future Expansion
        //case 0x3C: // Future Expansion
        case 0x3D: // AND - Absolute,X
            this->AND(addrAbsoluteIdxX());
            break;
        case 0x3E: // ROL - Absolute,X
            this->ROL(addrAbsoluteIdxX());
            break;
        //case 0x3F: // Future Expansion
        case 0x40: // RTI
            this->RTI();
            break;
        case 0x41: // EOR - (Indirect,X)
            this->EOR(addrIndirectX());
            break;
        //case 0x42: // Future Expansion
        //case 0x43: // Future Expansion
        //case 0x44: // Future Expansion
        case 0x45: // EOR - Zero Page
            this->EOR(addrZeroPage());
            break;
        case 0x46: // LSR - Zero Page
            this->LSR(addrZeroPage());
            break;
        //case 0x47: // Future Expansion
        case 0x48: // PHA
            this->PHA();
            break;
        case 0x49: // EOR - Immediate
            this->EOR(addrImmediate());
            break;
        case 0x4A: // LSR - Accumulator
            this->LSR();
            break;
        //case 0x4B: // Future Expansion
        case 0x4C: // JMP - Absolute
            this->JMP(addrAbsolute());
            break;
        case 0x4D: // EOR - Absolute
            this->EOR(addrAbsolute());
            break;
        case 0x4E: // LSR - Absolute
            this->LSR(addrAbsolute());
            break;
        //case 0x4F: // Future Expansion
        case 0x50: // BVC
            this->BVC(addrRelative());
            break;
        case 0x51: // EOR - (Indirect),Y
            this->EOR(addrIndirectY());
            break;
        //case 0x52: // Future Expansion
        //case 0x53: // Future Expansion
        //case 0x54: // Future Expansion
        case 0x55: // EOR - Zero Page,X
            this->EOR(addrZeroPageIdxX());
            break;
        case 0x56: // LSR - Zero Page,X
            this->LSR(addrZeroPageIdxX());
            break;
        //case 0x57: // Future Expansion
        case 0x58: // CLI
            this->CLI();
            break;
        case 0x59: // EOR - Absolute,Y
            this->EOR(addrAbsoluteIdxY());
            break;
        //case 0x5A: // Future Expansion
        //case 0x5B: // Future Expansion
        //case 0x5C: // Future Expansion
        case 0x5D: // EOR - Absolute,X
            this->EOR(addrAbsoluteIdxX());
            break;
        case 0x5E: // LSR - Absolute,X
            this->LSR(addrAbsoluteIdxX());
            break;
        //case 0x5F: // Future Expansion
        case 0x60: // RTS
            this->RTS();
            break;
        case 0x61: // ADC - (Indirect,X)
            this->ADC(addrIndirectX());
            break;
        //case 0x62: // Future Expansion
        //case 0x63: // Future Expansion
        //case 0x64: // Future Expansion
        case 0x65: // ADC - Zero Page
            this->ADC(addrZeroPage());
            break;
        case 0x66: // ROR - Zero Page
            this->ROR(addrZeroPage());
            break;
        //case 0x67: // Future Expansion
        case 0x68: // PLA
            this->PLA();
            break;
        case 0x69: // ADC - Immediate
            this->ADC(addrImmediate());
            break;
        case 0x6A: // ROR - Accumulator
            this->ROR();
            break;
        //case 0x6B: // Future Expansion
        case 0x6C: // JMP - Indirect
            this->JMP(this->addrAbsoluteIndirect());
            break;
        case 0x6D: // ADC - Absolute
            this->ADC(addrAbsolute());
            break;
        case 0x6E: // ROR - Absolute
            this->ROR(addrAbsolute());
            break;
        //case 0x6F: // Future Expansion
        case 0x70: // BVS
            this->BVS(addrRelative());
            break;
        case 0x71: // ADC - (Indirect),Y
            this->ADC(addrIndirectY());
            break;
        //case 0x72: // Future Expansion
        //case 0x73: // Future Expansion
        //case 0x74: // Future Expansion
        case 0x75: // ADC - Zero Page,X
            this->ADC(addrZeroPageIdxX());
            break;
        case 0x76: // ROR - Zero Page,X
            this->ROR(addrZeroPageIdxX());
            break;
        //case 0x77: // Future Expansion
        case 0x78: // SEI
            this->SEI();
            break;
        case 0x79: // ADC - Absolute,Y
            this->ADC(addrAbsoluteIdxY());
            break;
        //case 0x7A: // Future Expansion
        //case 0x7B: // Future Expansion
        //case 0x7C: // Future Expansion
        case 0x7D: // ADC - Absolute,X
            this->ADC(addrAbsoluteIdxX());
            break;
        case 0x7E: // ROR - Absolute,X
            this->ROR(addrAbsoluteIdxX());
            break;
        //case 0x7F: // Future Expansion
        //case 0x80: // Future Expansion
        case 0x81: // STA - (Indirect,X)
            this->STA(addrIndirectX());
            break;
        //case 0x82: // Future Expansion
        //case 0x83: // Future Expansion
        case 0x84: // STY - Zero Page
            this->STY(addrZeroPage());
            break;
        case 0x85: // STA - Zero Page
            this->STA(addrZeroPage());
            break;
        case 0x86: // STX - Zero Page
            this->STX(addrZeroPage());
            break;
        //case 0x87: // Future Expansion
        case 0x88: // DEY
            this->DEY();
            break;
        //case 0x89: // Future Expansion
        case 0x8A: // TXA
            this->TXA();
            break;
        //case 0x8B: // Future Expansion
        case 0x8C: // STY - Absolute
            this->STY(addrAbsolute());
            break;
        case 0x8D: // STA - Absolute
            this->STA(addrAbsolute());
            break;
        case 0x8E: // STX - Absolute
            this->STX(addrAbsolute());
            break;
        //case 0x8F: // Future Expansion
        case 0x90: // BCC
            this->BCC(addrRelative());
            break;
        case 0x91: // STA - (Indirect),Y
            this->STA(addrIndirectY());
            break;
        //case 0x92: // Future Expansion
        //case 0x93: // Future Expansion
        case 0x94: // STY - Zero Page,X
            this->STY(addrZeroPageIdxX());
            break;
        case 0x95: // STA - Zero Page,X
            this->STA(addrZeroPageIdxX());
            break;
        case 0x96: // STX - Zero Page,Y
            this->STX(addrZeroPageIdxY());
            break;
        //case 0x97: // Future Expansion
        case 0x98: // TYA
            this->TYA();
            break;
        case 0x99: // STA - Absolute,Y
            this->STA(addrAbsoluteIdxY());
            break;
        case 0x9A: // TXS
            this->TXS();
            break;
        //case 0x9B: // Future Expansion
        //case 0x9C: // Future Expansion
        case 0x9D: // STA - Absolute,X
            this->STA(addrAbsoluteIdxX());
            break;
        //case 0x9E: // Future Expansion
        //case 0x9F: // Future Expansion
        case 0xA0: // LDY - Immediate
            this->LDY(addrImmediate());
            break;
        case 0xA1: // LDA - (Indirect,X)
            this->LDA(addrIndirectX());
            break;
        case 0xA2: // LDX - Immediate
            this->LDX(addrImmediate());
            break;
        //case 0xA3: // Future Expansion
        case 0xA4: // LDY - Zero Page
            this->LDY(addrZeroPage());
            break;
        case 0xA5: // LDA - Zero Page
            this->LDA(addrZeroPage());
            break;
        case 0xA6: // LDX - Zero Page
            this->LDX(addrZeroPage());
            break;
        //case 0xA7: // Future Expansion
        case 0xA8: // TAY
            this->TAY();
            break;
        case 0xA9: // LDA - Immediate
            this->LDA(addrImmediate());
            break;
        case 0xAA: // TAX
            this->TAX();
            break;
        //case 0xAB: // Future Expansion
        case 0xAC: // LDY - Absolute
            this->LDY(addrAbsolute());
            break;
        case 0xAD: // LDA - Absolute
            this->LDA(addrAbsolute());
            break;
        case 0xAE: // LDX - Absolute
            this->LDX(addrAbsolute());
            break;
        //case 0xAF: // Future Expansion
        case 0xB0: // BCS
            this->BCS(addrRelative());
            break;
        case 0xB1: // LDA - (Indirect),Y
            this->LDA(addrIndirectY());
            break;
        //case 0xB2: // Future Expansion
        //case 0xB3: // Future Expansion
        case 0xB4: // LDY - Zero Page,X
            this->LDY(addrZeroPageIdxX());
            break;
        case 0xB5: // LDA - Zero Page,X
            this->LDA(addrZeroPageIdxX());
            break;
        case 0xB6: // LDX - Zero Page,Y
            this->LDX(addrZeroPageIdxY());
            break;
        //case 0xB7: // Future Expansion
        case 0xB8: // CLV
            this->CLV();
            break;
        case 0xB9: // LDA - Absolute,Y
            this->LDA(addrAbsoluteIdxY());
            break;
        case 0xBA: // TSX
            this->TSX();
            break;
        //case 0xBB: // Future Expansion
        case 0xBC: // LDY - Absolute,X
            this->LDY(addrAbsoluteIdxX());
            break;
        case 0xBD: // LDA - Absolute,X
            this->LDA(addrAbsoluteIdxX());
            break;
        case 0xBE: // LDX - Absolute,Y
            this->LDX(addrAbsoluteIdxY());
            break;
        //case 0xBF: // Future Expansion
        case 0xC0: // CPY - Immediate
            this->CPY(addrImmediate());
            break;
        case 0xC1: // CMP - (Indirect,X)
            this->CMP(addrIndirectX());
            break;
        //case 0xC2: // Future Expansion
        //case 0xC3: // Future Expansion
        case 0xC4: // CPY - Zero Page
            this->CPY(addrZeroPage());
            break;
        case 0xC5: // CMP - Zero Page
            this->CMP(addrZeroPage());
            break;
        case 0xC6: // DEC - Zero Page
            this->DEC(addrZeroPage());
            break;
        //case 0xC7: // Future Expansion
        case 0xC8: // INY
            this->INY();
            break;
        case 0xC9: // CMP - Immediate
            this->CMP(addrImmediate());
            break;
        case 0xCA: // DEX
            this->DEX();
            break;
        //case 0xCB: // Future Expansion
        case 0xCC: // CPY - Absolute
            this->CPY(addrAbsolute());
            break;
        case 0xCD: // CMP - Absolute
            this->CMP(addrAbsolute());
            break;
        case 0xCE: // DEC - Absolute
            this->DEC(addrAbsolute());
            break;
        //case 0xCF: // Future Expansion
        case 0xD0: // BNE
            this->BNE(addrRelative());
            break;
        case 0xD1: // CMP   (Indirect),Y
            this->CMP(addrIndirectY());
            break;
        //case 0xD2: // Future Expansion
        //case 0xD3: // Future Expansion
        //case 0xD4: // Future Expansion
        case 0xD5: // CMP - Zero Page,X
            this->CMP(addrZeroPageIdxX());
            break;
        case 0xD6: // DEC - Zero Page,X
            this->DEC(addrZeroPageIdxX());
            break;
        //case 0xD7: // Future Expansion
        case 0xD8: // CLD
            this->CLD();
            break;
        case 0xD9: // CMP - Absolute,Y
            this->CMP(addrAbsoluteIdxY());
            break;
        //case 0xDA: // Future Expansion
        //case 0xDB: // Future Expansion
        //case 0xDC: // Future Expansion
        case 0xDD: // CMP - Absolute,X
            this->CMP(addrAbsoluteIdxX());
            break;
        case 0xDE: // DEC - Absolute,X
            this->DEC(addrAbsoluteIdxX());
            break;
        //case 0xDF: // Future Expansion
        case 0xE0: // CPX - Immediate
            this->CPX(addrImmediate());
            break;
        case 0xE1: // SBC - (Indirect,X)
            this->SBC(addrIndirectX());
            break;
        //case 0xE2: // Future Expansion
        //case 0xE3: // Future Expansion
        case 0xE4: // CPX - Zero Page
            this->CPX(addrZeroPage());
            break;
        case 0xE5: // SBC - Zero Page
            this->SBC(addrZeroPage());
            break;
        case 0xE6: // INC - Zero Page
            this->INC(addrZeroPage());
            break;
        //case 0xE7: // Future Expansion
        case 0xE8: // INX
            this->INX();
            break;
        case 0xE9: // SBC - Immediate
            this->SBC(addrImmediate());
            break;
        case 0xEA: // NOP
            this->NOP();
            break;
        //case 0xEB: // Future Expansion
        case 0xEC: // CPX - Absolute
            this->CPX(addrAbsolute());
            break;
        case 0xED: // SBC - Absolute
            this->SBC(addrAbsolute());
            break;
        case 0xEE: // INC - Absolute
            this->INC(addrAbsolute());
            break;
        //case 0xEF: // Future Expansion
        case 0xF0: // BEQ
            this->BEQ(addrRelative());
            break;
        case 0xF1: // SBC - (Indirect),Y
            this->SBC(addrIndirectY());
            break;
        //case 0xF2: // Future Expansion
        //case 0xF3: // Future Expansion
        //case 0xF4: // Future Expansion
        case 0xF5: // SBC - Zero Page,X
            this->SBC(addrZeroPageIdxX());
            break;
        case 0xF6: // INC - Zero Page,X
            this->INC(addrZeroPageIdxX());
            break;
        //case 0xF7: // Future Expansion
        case 0xF8: // SED
            this->SED();
            break;
        case 0xF9: // SBC - Absolute,Y
            this->SBC(addrAbsoluteIdxY());
            break;
        //case 0xFA: // Future Expansion
        //case 0xFB: // Future Expansion
        //case 0xFC: // Future Expansion
        case 0xFD: // SBC - Absolute,X
            this->SBC(addrAbsoluteIdxX());
            break;
        case 0xFE: // INC - Absolute,X
            this->INC(addrAbsoluteIdxX());
            break;
        //case 0xFF: // Future Expansion
        default:
            uint16_t opcodeBig = opcode;
            uint16_t opcodePC = this->PC-1;
            throw EmulatorException("[FIXME] Invalid opcode: 0x") << std::hex << opcodeBig << " in 0x" << opcodePC;
    }
    consumeClock(CycleTable[opcode]);
}

Processor::~Processor()
{
}


// -- 割り込みなう --

inline void Processor::onNMI()
{
    //from http://crystal.freespace.jp/pgate1/nes/nes_cpu.htm
    //from http://nesdev.parodius.com/6502_cpu.txt
    consumeClock(7);
    this->P &= ~FLAG_B;
    push(static_cast<uint8_t>((this->PC >> 8) & 0xFF));
    push(static_cast<uint8_t>(this->PC & 0xFF));
    push(this->P);
    this->P |= FLAG_I;
    this->PC = (read(0xFFFA) | (read(0xFFFB) << 8));
    this->NMI = false;
}
inline void Processor::onIRQ()
{
    //from http://crystal.freespace.jp/pgate1/nes/nes_cpu.htm
    //from http://nesdev.parodius.com/6502_cpu.txt
    if((this->P & FLAG_I) == FLAG_I){
        return;
    }
    consumeClock(7);
    this->P &= ~FLAG_B;
    push(static_cast<uint8_t>((this->PC >> 8) & 0xFF));
    push(static_cast<uint8_t>(this->PC & 0xFF));
    push(this->P);
    this->P |= FLAG_I;
    this->PC = (read(0xFFFE) | (read(0xFFFF) << 8));
}


// -- よく使いそうな関数 --

inline uint8_t Processor::read(uint16_t addr)
{
    return VM.read(addr);
}
inline void Processor::write(uint16_t addr, uint8_t value){
    VM.write(addr, value);
}


inline void Processor::push(uint8_t val)
{
    write(0x0100 | this->SP, val);
    this->SP--;
}
inline uint8_t Processor::pop()
{
    this->SP++;
    return read(0x0100 | this->SP);
}


inline void Processor::updateFlagZN(const uint8_t val)
{
    this->P = (this->P & 0x7D) | ZNFlagCache[val];
}

inline void Processor::consumeClock(uint8_t clock)
{
    VM.consumeCpuClock(clock);
}

// -- 以下、アドレッシングモード --
inline uint16_t Processor::addrImmediate()
{
    const uint16_t addr = this->PC;
    this->PC++;
    return addr;
}
inline uint16_t Processor::addrAbsolute()
{
    uint16_t addr = read(this->PC);
    this->PC++;
    addr = addr | (read(this->PC) << 8);
    this->PC++;
    return addr;
}

inline uint16_t Processor::addrZeroPage()
{
    uint16_t addr = static_cast<uint16_t>(read(this->PC));
    this->PC++;
    return addr;
}
inline uint16_t Processor::addrZeroPageIdxX()
{
    uint8_t val = read(this->PC) + this->X;
    this->PC++;
    return static_cast<uint16_t>(val);
}
inline uint16_t Processor::addrZeroPageIdxY()
{
    uint8_t val = read(this->PC) + this->Y;
    this->PC++;
    return static_cast<uint16_t>(val);
}
inline uint16_t Processor::addrAbsoluteIdxX()
{
    uint16_t orig = read(this->PC);
    this->PC++;
    orig = orig | (read(this->PC) << 8);
    this->PC++;
    const uint16_t addr = orig + this->X;
    if(((addr ^ orig) & 0x0100) != 0){
        consumeClock(1);
    }
    return addr;
}
inline uint16_t Processor::addrAbsoluteIdxY()
{
    uint16_t orig = read(this->PC);
    this->PC++;
    orig = orig | (read(this->PC) << 8);
    this->PC++;
    const uint16_t addr = orig + this->Y;
    if(((addr ^ orig) & 0x0100) != 0){
        consumeClock(1);
    }
    return addr;
}
inline uint16_t Processor::addrRelative()
{
    int8_t offset = static_cast<int8_t>(read(this->PC));
    this->PC++;
    return this->PC + offset;
}
inline uint16_t Processor::addrIndirectX()
{
    uint8_t idx = read(this->PC) + this->X;
    this->PC++;
    uint16_t addr = read(idx);
    idx++;
    addr = addr | (read(idx) << 8);
    return addr;
}
inline uint16_t Processor::addrIndirectY()
{
    uint8_t idx = read(this->PC);
    this->PC++;
    uint16_t orig = read(idx);
    idx++;
    orig = orig | (read(idx) << 8);
    const uint16_t addr = orig + this->Y;
    if(((addr ^ orig) & 0x0100) != 0){
        consumeClock(1);
    }
    return addr;
}
inline uint16_t Processor::addrAbsoluteIndirect() // used only in JMP
{
    uint16_t srcAddr = read(this->PC);
    this->PC++;
    srcAddr = srcAddr | (read(this->PC) << 8);
    this->PC++;
    return read(srcAddr) | (read((srcAddr & 0xff00) | ((srcAddr+1) & 0x00ff)) << 8); //bug of NES
}

//-- 以下、個別命令 --
inline void Processor:: LDA(uint16_t addr)
{
    this->A = this->read(addr);
    updateFlagZN(this->A);
}
inline void Processor:: LDY(uint16_t addr)
{
    this->Y = this->read(addr);
    updateFlagZN(this->Y);
}
inline void Processor:: LDX(uint16_t addr)
{
    this->X = this->read(addr);
    updateFlagZN(this->X);
}

inline void Processor::STA(uint16_t addr)
{
    this->write(addr, this->A);
}
inline void Processor::STX(uint16_t addr)
{
    this->write(addr, this->X);
}
inline void Processor::STY(uint16_t addr)
{
    this->write(addr, this->Y);
}

inline void Processor:: TXA()
{
    this->A = this->X;
    updateFlagZN(this->A);
}
inline void Processor:: TYA()
{
    this->A = this->Y;
    updateFlagZN(this->A);
}
inline void Processor:: TXS()
{
    this->SP = this->X;
}
inline void Processor:: TAY()
{
    this->Y = this->A;
    updateFlagZN(this->Y);
}
inline void Processor:: TAX()
{
    this->X = this->A;
    updateFlagZN(this->X);
}
inline void Processor:: TSX()
{
    this->X = this->SP;
    updateFlagZN(this->X);
}

inline void Processor:: PHP()
{
    push(this->P | FLAG_B); // bug of 6502! from http://crystal.freespace.jp/pgate1/nes/nes_cpu.htm
}
inline void Processor:: PLP()
{
	const uint8_t newP = pop();
	if((this->P & FLAG_I) == FLAG_I && (newP & FLAG_I) == 0){
		this->needStatusRewrite = true;
		this->newStatus =newP;
	}else{
		this->P = newP;
	}
}
inline void Processor:: PHA()
{
    push(this->A);
}
inline void Processor:: PLA()
{
    this->A = pop();
    this->updateFlagZN(this->A);
}

inline void Processor:: ADC(uint16_t addr)
{
    const uint8_t val = read(addr);
    const uint16_t result = this->A + val + (this->P & FLAG_C);
    const uint8_t newA = static_cast<uint8_t>(result & 0xff);
	this->P = (this->P & ~(FLAG_V | FLAG_C))
		| ((((this->A ^ val) & 0x80) ^ 0x80) & ((this->A ^ newA) & 0x80)) >> 1 //set V flag //いまいちよくわかってない（
		| ((result >> 8) & FLAG_C); //set C flag
	this->A = newA;
	updateFlagZN(this->A);
}
inline void Processor:: SBC(uint16_t addr)
{
    const uint8_t val = read(addr);
    const uint16_t result = this->A - val - ((this->P & FLAG_C) ^ FLAG_C);
    const uint8_t newA = static_cast<uint8_t>(result & 0xff);
	this->P = (this->P & ~(FLAG_V | FLAG_C))
		| ((this->A ^ val) & (this->A ^ newA) & 0x80) >> 1 //set V flag //いまいちよくわかってない（
		| (((result >> 8) & FLAG_C) ^ FLAG_C);

	this->A = newA;
	updateFlagZN(this->A);
}
inline void Processor:: CPX(uint16_t addr)
{
    const uint16_t val = this->X - read(addr);
    const uint8_t val8 = static_cast<uint8_t>(val & 0xff);
    updateFlagZN(val8);
    this->P = (this->P & 0xfe) | (((val >> 8) & 0x1) ^ 0x1);
}
inline void Processor:: CPY(uint16_t addr)
{
    const uint16_t val = this->Y - read(addr);
    const uint8_t val8 = static_cast<uint8_t>(val & 0xff);
    updateFlagZN(val8);
    this->P = (this->P & 0xfe) | (((val >> 8) & 0x1) ^ 0x1);
}
inline void Processor:: CMP(uint16_t addr)
{
    const uint16_t val = this->A - read(addr);
    const uint8_t val8 = static_cast<uint8_t>(val & 0xff);
    updateFlagZN(val8);
    this->P = (this->P & 0xfe) | (((val >> 8) & 0x1) ^ 0x1);
}

inline void Processor::AND(uint16_t addr)
{
    this->A &= read(addr);
    updateFlagZN(this->A);
}
inline void Processor::EOR(uint16_t addr)
{
    this->A ^= read(addr);
    updateFlagZN(this->A);
}
inline void Processor::ORA(uint16_t addr)
{
    this->A |= read(addr);
    updateFlagZN(this->A);
}
inline void Processor::BIT(uint16_t addr)
{
    uint8_t val = read(addr);
    this->P = (this->P & (0xff & ~(FLAG_V | FLAG_N | FLAG_Z)))
        | (val & (FLAG_V | FLAG_N))
        | (ZNFlagCache[this->A & val] & FLAG_Z);
}


inline void Processor:: ASL()
{
    this->P = (this->P & 0xFE) | this->A >> 7;
    this->A <<= 1;
    updateFlagZN(this->A);
}
inline void Processor:: ASL(uint16_t addr)
{
    uint8_t val = this->read(addr);
    this->P = (this->P & 0xFE) | val >> 7;
    val <<= 1;
    this->write(addr, val);
    updateFlagZN(val);
}
inline void Processor:: LSR()
{
    this->P = (this->P & 0xFE) | (this->A & 0x01);
    this->A >>= 1;
    updateFlagZN(this->A);
}
inline void Processor:: LSR(uint16_t addr)
{
    uint8_t val = this->read(addr);
    this->P = (this->P & 0xFE) | (val & 0x01);
    val >>= 1;
    this->write(addr, val);
    updateFlagZN(val);
}
inline void Processor:: ROL()
{
    const uint8_t carry = this->A >> 7;
    this->A = (this->A << 1) | (this->P & 0x01);
    this->P = (this->P & 0xFE) | carry;
    updateFlagZN(this->A);
}
inline void Processor:: ROL(uint16_t addr)
{
    uint8_t val = this->read(addr);
    const uint8_t carry = val >> 7;
    val = (val << 1) | (this->P & 0x01);
    this->P = (this->P & 0xFE) | carry;
    updateFlagZN(val);
    this->write(addr, val);
}
inline void Processor:: ROR()
{
    const uint8_t carry = this->A & 0x01;
    this->A = (this->A >> 1) | ((this->P & 0x01) << 7);
    this->P = (this->P & 0xFE) | carry;
    updateFlagZN(this->A);
}
inline void Processor:: ROR(uint16_t addr)
{
    uint8_t val = this->read(addr);
    const uint8_t carry = val & 0x01;
    val = (val >> 1) | ((this->P & 0x01) << 7);
    this->P = (this->P & 0xFE) | carry;
    updateFlagZN(val);
    this->write(addr, val);
}

inline void Processor:: INX()
{
    this->X++;
    updateFlagZN(this->X);
}
inline void Processor:: INY()
{
    this->Y++;
    updateFlagZN(this->Y);
}
inline void Processor:: INC(uint16_t addr)
{
    uint8_t val = read(addr);
    val++;
    updateFlagZN(val);
    write(addr, val);
}
inline void Processor:: DEX()
{
    this->X--;
    updateFlagZN(this->X);
}
inline void Processor:: DEY()
{
    this->Y--;
    updateFlagZN(this->Y);
}
inline void Processor:: DEC(uint16_t addr)
{
    uint8_t val = read(addr);
    val--;
    updateFlagZN(val);
    write(addr, val);
}

inline void Processor:: CLC()
{
    this->P &= ~(FLAG_C);
}
inline void Processor:: CLI()
{
	// http://twitter.com/#!/KiC6280/status/112348378100281344
	// http://twitter.com/#!/KiC6280/status/112351125084180480
	this->needStatusRewrite = true;
	this->newStatus = this->P & ~(FLAG_I);
    //this->P &= ~(FLAG_I);
}
inline void Processor:: CLV()
{
    this->P &= ~(FLAG_V);
}
inline void Processor:: CLD()
{
    this->P &= ~(FLAG_D);
}
inline void Processor:: SEC()
{
    this->P |= FLAG_C;
}
inline void Processor:: SEI()
{
    this->P |= FLAG_I;
}
inline void Processor:: SED()
{
    this->P |= FLAG_D;
}

inline void Processor:: NOP()
{
    //NOP。そう、何もしない。
}
inline void Processor:: BRK()
{
	//NES ON FPGAには、
	//「割り込みが確認された時、Iフラグがセットされていれば割り込みは無視します。」
	//…と合ったけど、他の資料だと違う。http://nesdev.parodius.com/6502.txt
	//DQ4はこうしないと、動かない。
	/*
	if((this->P & FLAG_I) == FLAG_I){
		return;
	}*/
    this->PC++;
    push(static_cast<uint8_t>((this->PC >> 8) & 0xFF));
    push(static_cast<uint8_t>(this->PC & 0xFF));
    this->P |= FLAG_B;
    push(this->P);
    this->P |= FLAG_I;
    this->PC = (read(0xFFFE) | (read(0xFFFF) << 8));
}

inline void Processor:: BCC(uint16_t addr)
{
    if((this->P & FLAG_C) == 0){
        this->PC = addr;
    }
}
inline void Processor:: BCS(uint16_t addr)
{
    if((this->P & FLAG_C) == FLAG_C){
        this->PC = addr;
    }
}
inline void Processor:: BEQ(uint16_t addr)
{
    if((this->P & FLAG_Z) == FLAG_Z){
        this->PC = addr;
    }
}
inline void Processor:: BNE(uint16_t addr)
{
    if((this->P & FLAG_Z) == 0){
        this->PC = addr;
    }
}
inline void Processor:: BVC(uint16_t addr)
{
    if((this->P & FLAG_V) == 0){
        this->PC = addr;
    }
}
inline void Processor:: BVS(uint16_t addr)
{
    if((this->P & FLAG_V) == FLAG_V){
        this->PC = addr;
    }
}
inline void Processor:: BPL(uint16_t addr)
{
    if((this->P & FLAG_N) == 0){
        this->PC = addr;
    }
}
inline void Processor:: BMI(uint16_t addr)
{
    if((this->P & FLAG_N) == FLAG_N){
        this->PC = addr;
    }
}
inline void Processor:: JSR(uint16_t addr)
{
    this->PC--;
    push(static_cast<uint8_t>((this->PC >> 8) & 0xFF));
    push(static_cast<uint8_t>(this->PC & 0xFF));
    this->PC = addr;
}
inline void Processor:: JMP(uint16_t addr)
{
    this->PC = addr;
}
inline void Processor:: RTI()
{
    this->P = pop();
    this->PC = pop();
    this->PC = this->PC | (pop() << 8);
}
inline void Processor:: RTS()
{
    this->PC = pop();
    this->PC = (this->PC + (pop() << 8)) + 1;
}

// -- cache --

const uint8_t Processor::ZNFlagCache[0x100] = {
    0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
};

const uint8_t Processor::CycleTable[0x100] ={
	  7, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6,
	  2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 6, 7,
	  6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6,
	  2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 6, 7,
	  6, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6,
	  2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 6, 7,
	  6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6,
	  2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 6, 7,
	  2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
	  2, 5, 2, 6, 4, 4, 4, 4, 2, 4, 2, 5, 5, 4, 5, 5,
	  2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
	  2, 5, 2, 5, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4,
	  2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
	  2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 6, 7,
	  2, 6, 3, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
	  2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 6, 7
};
