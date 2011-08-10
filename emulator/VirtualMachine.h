#ifndef VIRTUALMACHINE_H
#define VIRTUALMACHINE_H

#include <stdint.h>

class VirtualMachine;

class Cartridge { //カートリッジデータ＋マッパー
    public:
        explicit Cartridge(VirtualMachine& vm);
        virtual ~Cartridge();
    protected:
    private:
        VirtualMachine& VM;
};

class Audio {
    public:
        explicit Audio(VirtualMachine& vm);
        ~Audio();
    protected:
    private:
        VirtualMachine& VM;
};

class Video
{
    public:
        explicit Video(VirtualMachine& vm);
        ~Video();
    protected:
    private:
        VirtualMachine& VM;
};

class Ram
{
    public:
        explicit Ram(VirtualMachine& vm);
        ~Ram();
        static const uint16_t WRAM_LENGTH = 2 * 1024;
    protected:
    private:
        VirtualMachine& VM;
        uint8_t wram[WRAM_LENGTH]; //2KB WRAM
};

class Processor
{
    public:
        explicit Processor(VirtualMachine& vm);
        ~Processor();
        void startup();
        void run();
        void sendReset();
        void sendNMI();
        void sendIRQ();
    protected:
    private:
        //定数
        static const uint8_t ZNFlagCache[0x100];
        static const uint8_t CycleTable[0x100];
        static const uint8_t FLAG_C = 0b00000001;
        static const uint8_t FLAG_Z = 0b00000010;
        static const uint8_t FLAG_I = 0b00000100;
        static const uint8_t FLAG_D = 0b00001000;
        static const uint8_t FLAG_B = 0b00010000; //not used in NES
        static const uint8_t FLAG_V = 0b01000000;
        static const uint8_t FLAG_N = 0b10000000;
        //
        VirtualMachine& VM;
        uint8_t A;
        uint8_t X;
        uint8_t Y;
        uint16_t PC;
        uint8_t SP;
        uint8_t P;
        //
        bool Reset;
        bool NMI;
        bool IRQ;
        //
        void push(uint8_t val);
        uint8_t pop();
        void consumeClock(uint8_t clock);
        void updateFlagZN(const uint8_t& val);
        //ハードウェア☆割り込み
        void onReset();
        void onNMI();
        void onIRQ();

        //アドレッシングモード
        uint16_t addrImmediate();
        uint16_t addrAbsolute();
        uint16_t addrZeroPage();
        uint16_t addrZeroPageIdxX();
        uint16_t addrZeroPageIdxY();
        uint16_t addrAbsoluteIdxX();
        uint16_t addrAbsoluteIdxY();
        uint16_t addrRelative();
        uint16_t addrIndirectX();
        uint16_t addrIndirextY();
        uint16_t addrAbsoluteIndirect();

        //命令一覧 from http://nesdev.parodius.com/opcodes.txt
        void BCC(uint16_t addr);
        void BCS(uint16_t addr);
        void BEQ(uint16_t addr);
        void BNE(uint16_t addr);
        void BVC(uint16_t addr);
        void BVS(uint16_t addr);
        void BPL(uint16_t addr);
        void BMI(uint16_t addr);

        void LDA(uint16_t addr);
        void LDY(uint16_t addr);
        void LDX(uint16_t addr);
        void STA(uint16_t addr);
        void STY(uint16_t addr);
        void STX(uint16_t addr);

        void TXA();
        void TYA();
        void TXS();
        void TAY();
        void TAX();
        void TSX();

        void PHP();
        void PLP();
        void PHA();
        void PLA();

        void ADC(uint16_t addr);
        void SBC(uint16_t addr);
        void CPX(uint16_t addr);
        void CPY(uint16_t addr);
        void CMP(uint16_t addr);

        void AND(uint16_t addr);
        void EOR(uint16_t addr);
        void ORA(uint16_t addr);
        void BIT(uint16_t addr);

        void ASL();
        void ASL(uint16_t addr);
        void LSR();
        void LSR(uint16_t addr);
        void ROL();
        void ROL(uint16_t addr);
        void ROR();
        void ROR(uint16_t addr);

        void INX();
        void INY();
        void INC(uint16_t addr);
        void DEX();
        void DEY();
        void DEC(uint16_t addr);

        void CLC();
        void CLI();
        void CLV();
        void CLD();
        void SEC();
        void SEI();
        void SED();

        void NOP();
        void BRK();

        void JSR(uint16_t addr);
        void JMP(uint16_t addr);
        void RTI();
        void RTS();
};

class IOPort
{
    public:
        explicit IOPort(VirtualMachine& vm);
        ~IOPort();
        uint8_t read(uint16_t addr);
        void write(uint16_t addr, uint8_t value);

    protected:
    private:
        VirtualMachine& VM;
};

class VirtualMachine
{
    public:
        explicit VirtualMachine();
        ~VirtualMachine();
        IOPort ioPort;
        Ram ram;
        Processor processor;
    protected:
    private:
};


#endif // VIRTUALMACHINE_H
