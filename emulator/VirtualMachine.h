#ifndef VIRTUALMACHINE_H
#define VIRTUALMACHINE_H

#include <stdint.h>
#include <string>

class VirtualMachine;

class EmulatorException
{
	public:
		EmulatorException(const char* msg);
		EmulatorException(const std::string msg);
		~EmulatorException();
		std::string& getMessage() const;
	private:
		const std::string msg;
};

class NesFile
{
    public:
        enum MirrorType {HORIZONTAL, VERTICAL, FOUR_SCREEN};
        static const uint16_t NES_FORMAT_SIZE = 16;
        static const uint16_t TRAINER_SIZE = 512;
        static const uint16_t PRG_ROM_PAGE_SIZE = 16 * 1024;
        static const uint16_t CHR_ROM_PAGE_SIZE = 8 * 1024;
    public:
        explicit NesFile(const char* filename);
        explicit NesFile(const uint32_t filesize, const uint8_t* data);
        ~NesFile();
        MirrorType getMirrorType() const;

        bool hasTrainer() const;
        bool hasSram() const;
        uint8_t getMapperNo() const;
        uint32_t getPrgSize() const;
        uint32_t getChrSize() const;
        uint8_t readTrainer(uint16_t addr) const;
        uint8_t readPrg(uint16_t page, uint16_t addr) const;
        uint8_t readPrg(uint32_t addr) const;
        uint8_t readChr(uint16_t page, uint16_t addr) const;
        uint8_t readChr(uint32_t addr) const;
    private:
        const std::string& filename;
        uint8_t mapperNo;
        const uint8_t* prgRom;
        const uint8_t* chrRom;
        uint8_t trainer[512];

        MirrorType mirrorType;
        bool trainerFlag;
        bool sramFlag;
        uint32_t prgSize;
        uint32_t chrSize;

        void analyzeFile(const uint8_t* const header, const uint32_t filesize, const uint8_t* data);
};

class Cartridge { //カートリッジデータ＋マッパー
    public:
        static const uint16_t SRAM_SIZE = 8192;
        static Cartridge* loadCartridge(VirtualMachine& vm, const char* filename);
    public:
        explicit Cartridge(VirtualMachine& vm, const NesFile* nesFile = 0);
        virtual ~Cartridge();
        virtual void run(uint16_t clockDelta);
        virtual void onHSync(uint16_t scanline);
        virtual void onVSync();
        virtual void onHardReset();
        virtual void onReset();
        virtual uint8_t readVideo(uint16_t addr);
        virtual void writeVideo(uint16_t addr, uint8_t value);
        virtual uint8_t readCpu(uint16_t addr);
        virtual void writeCpu(uint16_t addr, uint8_t value);
    protected:
        uint8_t readSram(uint16_t addr) const;
        void writeSram(uint16_t addr, uint8_t value);
        const NesFile* const nesFile;
    private:
        VirtualMachine& VM;
        uint8_t sram[SRAM_SIZE];
};

class Audio {
    public:
        explicit Audio(VirtualMachine& vm);
        ~Audio();
        void run(uint16_t clockDelta);
        void onHardReset();
        void onReset();
        uint8_t readReg(uint16_t addr);
        void writeReg(uint16_t addr, uint8_t value);
    protected:
    private:
        VirtualMachine& VM;
};

class Video
{
    public:
        explicit Video(VirtualMachine& vm);
        ~Video();
        void run(uint16_t clockDelta);
        void onHardReset();
        void onReset();
        uint8_t readReg(uint16_t addr);
        void writeReg(uint16_t addr, uint8_t value);
    protected:
    private:
        VirtualMachine& VM;
        uint8_t read(uint16_t addr);
        void write(uint16_t addr, uint8_t value);
};

class Ram
{
    public:
        static const uint16_t WRAM_LENGTH = 2048;
    public:
        explicit Ram(VirtualMachine& vm);
        ~Ram();
        void onHardReset();
        void onReset();
        uint8_t read(uint16_t addr);
        void write(uint16_t addr, uint8_t value);
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
        void run(uint16_t clockDelta);
        void onHardReset();
        void onReset();
        void sendNMI();
        void sendIRQ();
    protected:
    private:
        uint8_t read(uint16_t addr);
        void write(uint16_t addr, uint8_t value);
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
        bool NMI;
        bool IRQ;
        //
        void push(uint8_t val);
        uint8_t pop();
        void consumeClock(uint8_t clock);
        void updateFlagZN(const uint8_t& val);
        //ハードウェア☆割り込み
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

class VirtualMachine
{
    public:
        explicit VirtualMachine();
        ~VirtualMachine();
        void run();
        void sendVideoOut(); //from video to user.
        void sendNMI(); //from video to processor
        void sendIRQ(); //from catreidge and audio to processor.
        void sendHSync(uint16_t scanline); //from video to cartridge
        void sendHardReset(); //from user to all subsystems.
        void sendReset(); //from user to all subsystems.
        void loadCartridge(const char* filename); //from user
        uint8_t read(uint16_t addr); //from processor to ram
        void write(uint16_t addr, uint8_t value); // from processor to ram.
        void consumeCpuClock(uint8_t clock);
        void consumeAudioClock(uint8_t clock);
        class Fairy //connect emulator and ui.
        {
            explicit Fairy();
            virtual ~Fairy();
            virtual void onVideoOut();
        };
    protected:
    private:
        static const int MAIN_CLOCK = 21477272;//21.28MHz(NTSC)
        static const int CPU_CLOCK_FACTOR = 12;
        static const int AUDIO_CLOCK_FACTOR = 12;
        static const int VIDEO_CLOCK_FACTOR = 4;
        static const int CARTRIDGE_CLOCK_FACTOR = 12;
        void consumeClock(uint8_t clock);
        Ram ram;
        Processor processor;
        Audio audio;
        Video video;
        Cartridge* cartridge;

        uint16_t cpuClockDelta;
        uint16_t audioClockDelta;
        uint16_t videoClockDelta;
        uint16_t cartridgeClockDelta;

        bool resetFlag;
        bool hardResetFlag;
};


#endif // VIRTUALMACHINE_H
