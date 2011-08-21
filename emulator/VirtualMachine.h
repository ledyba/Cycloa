#ifndef VIRTUALMACHINE_H
#define VIRTUALMACHINE_H

#include <iostream>
#include <stdint.h>
#include <string>
#include <sstream>

class VirtualMachine;

class EmulatorException
{
	public:
		EmulatorException();
		EmulatorException(const char* fmsg);
		EmulatorException(const std::string& fmsg);
		EmulatorException(const EmulatorException& src);
		~EmulatorException();
	private:
		std::stringstream msg;
	public:
		const std::string getMessage() const;
		template<typename T> EmulatorException& operator<<(T& val)
		{
			this->msg << val;
			return *this;
		}
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

        virtual uint8_t readVideo(uint16_t addr) const;
        virtual void writeVideo(uint16_t addr, uint8_t value);

        virtual uint8_t readPatternTable(uint16_t addr) const;
        virtual void writePatternTable(uint16_t addr, uint8_t val);
        virtual uint8_t readNameTable(uint16_t addr) const;
        virtual void writeNameTable(uint16_t addr, uint8_t val);

        virtual uint8_t readCpu(uint16_t addr);
        virtual void writeCpu(uint16_t addr, uint8_t value);

        void connectInternalVram(uint8_t* internalVram);
    protected:
        uint8_t readSram(uint16_t addr) const;
        void writeSram(uint16_t addr, uint8_t value);
        const NesFile* const nesFile;
    private:
        VirtualMachine& VM;
        uint8_t sram[SRAM_SIZE];
        uint8_t* vramMirroring[4];
        uint8_t fourScreenVram[4096];
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

class ImageBuffer
{
	public:
		explicit ImageBuffer(uint16_t actualWidth=256, uint16_t actualHeight=240, uint16_t width=256, uint16_t height=256, uint16_t bytesPerPixel=3);
		~ImageBuffer();
		uint8_t& operator[](uint32_t pos);
		uint8_t* getPtr();
		const uint8_t* getPtr() const;
		uint8_t* getPtr(uint16_t y);
		uint32_t getPos(uint16_t x, uint16_t y) const;
		uint16_t getBytesPerPixel() const;
		uint32_t getTotalSize() const;
		uint16_t getWidth() const;
		uint16_t getHeight() const;
		uint16_t getActualWidth() const;
		uint16_t getActualHeight() const;
	private:
		uint8_t* const data;
		const uint16_t bytesPerPixel;
		const uint16_t actualWidth;
		const uint16_t actualHeight;
		const uint16_t height;
		const uint16_t width;
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
        void executeDMA(uint8_t value);
        void connectCartridge(Cartridge* cartridge);
    protected:
    private:
		static const uint8_t nesPalette[64][3];
		static const int screenWidth = 256;
		static const int screenHeight = 240;
		static const int clockPerScanline = 341;
		static const int scanlinePerScreen = 262;
		static const int defaultSpriteCnt = 8;
        VirtualMachine& VM;
        Cartridge* cartridge;
        ImageBuffer image;
        bool isEven;
        uint16_t nowY;
        uint16_t nowX;
        uint8_t spRam[256];
        uint8_t internalVram[2048];
        uint8_t palette[9][4];

        /* Rendering */
        struct SpriteSlot {
        	uint8_t y;
        	uint8_t x;
        	uint8_t paletteNo;
        	uint16_t tileAddr;
        	bool isForeground;
        	bool flipHorizontal;
        	bool flipVertical;
        } spriteTable[defaultSpriteCnt];
        uint8_t lineBuff[screenWidth];
        void spriteEval();
        void buildSpriteLine();
        void buildBgLine();
        void fillImage();

        /* IO */

        uint8_t readVram(uint16_t addr) const;
        void writeVram(uint16_t addr, uint8_t value);
        uint8_t readSprite(uint16_t addr) const;
        void writeSprite(uint16_t addr, uint8_t value);

		/* PPU Control Register 1 */
		bool executeNMIonVBlank;
		uint8_t spriteHeight;
		uint16_t patternTableAddressBackground;
		uint16_t patternTableAddress8x8Sprites;
		uint8_t vramIncrementSize;
		uint16_t nameTableScrollAddr;

		/* PPU Control Register 2 */
		uint8_t colorEmphasis;
		bool spriteVisibility;
		bool backgroundVisibility;
		bool spriteClipping;
		bool backgroundClipping;
		uint8_t paletteMask;

		/* PPU Status Register */
        bool nowOnVBnank;
        bool sprite0Hit;
        bool lostSprites;

        /* addressControl */
        uint8_t vramBuffer;
        uint8_t spriteAddr;
        uint8_t horizontalScrollOrigin;
        uint8_t verticalScrollOrigin;
        uint16_t vramAddrRegister;
        bool scrollRegisterWritten;
        bool vramAddrRegisterWritten;

        void analyzePPUControlRegister1(uint8_t value);
        void analyzePPUControlRegister2(uint8_t value);
        void analyzePPUBackgroundScrollingOffset(uint8_t value);
        uint8_t buildPPUStatusRegister();
        uint8_t readVramDataRegister();
        uint8_t readSpriteDataRegister();
        void analyzeVramAddrRegister(uint8_t value);
        void analyzeSpriteAddrRegister(uint8_t value);
        void writeVramDataRegister(uint8_t value);
        void writeSpriteDataRegister(uint8_t value);

        void startVBlank();
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
        static const uint8_t FLAG_ALWAYS_SET = 0b00100000;
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
        uint16_t addrIndirectY();
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

class EmulatorFairy
{
	public:
		explicit EmulatorFairy(){}
		virtual ~EmulatorFairy(){}
		virtual void onVSync(const ImageBuffer& img){}
	protected:
	private:
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
        void showVideo(const ImageBuffer& img); //from video
        void consumeCpuClock(uint32_t clock);
        void consumeAudioClock(uint32_t clock);
        void setFairy(EmulatorFairy* const fairy);
    protected:
    private:
        static const int MAIN_CLOCK = 21477272;//21.28MHz(NTSC)
        static const int CPU_CLOCK_FACTOR = 12;
        static const int AUDIO_CLOCK_FACTOR = 12;
        static const int VIDEO_CLOCK_FACTOR = 4;
        static const int CARTRIDGE_CLOCK_FACTOR = 12;
        void consumeClock(uint32_t clock);
        EmulatorFairy* fairy;
        Ram ram;
        Processor processor;
        Audio audio;
        Video video;
        Cartridge* cartridge;

        uint32_t cpuClockDelta;
        uint32_t audioClockDelta;
        uint32_t videoClockDelta;
        uint32_t cartridgeClockDelta;

        bool resetFlag;
        bool hardResetFlag;
};

#endif // VIRTUALMACHINE_H
