#ifndef VIRTUALMACHINE_H
#define VIRTUALMACHINE_H

#include <cstddef>
#include <iostream>
#include <stdint.h>
#include <string.h>
#include "file/NesFile.h"
#include "fairy/VideoFairy.h"

class VirtualMachine;

class Cartridge { //カートリッジデータ＋マッパー
	public:
		static const uint16_t SRAM_SIZE = 8192;
		static Cartridge* loadCartridge(VirtualMachine& vm, const char* filename);
	public:
		explicit Cartridge(VirtualMachine& vm, const NesFile* nesFile = 0);
		virtual ~Cartridge();
		virtual void run(uint16_t clockDelta);
		virtual void onHBlank(uint16_t scanline);
		virtual void onVBlank();
		virtual void onHardReset();
		virtual void onReset();

		/* from PPU */
		virtual uint8_t readPatternTableHigh(uint16_t addr) const;
		virtual void writePatternTableHigh(uint16_t addr, uint8_t val);

		virtual uint8_t readPatternTableLow(uint16_t addr) const;
		virtual void writePatternTableLow(uint16_t addr, uint8_t val);

		virtual uint8_t readNameTable(uint16_t addr) const;
		virtual void writeNameTable(uint16_t addr, uint8_t val);

		/* from CPU */
		virtual uint8_t readRegisterArea(uint16_t addr);
		virtual void writeRegisterArea(uint16_t addr, uint8_t val);

		virtual uint8_t readSaveArea(uint16_t addr);
		virtual void writeSaveArea(uint16_t addr, uint8_t val);

		virtual uint8_t readBankHigh(uint16_t addr);
		virtual void writeBankHigh(uint16_t addr, uint8_t val);

		virtual uint8_t readBankLow(uint16_t addr);
		virtual void writeBankLow(uint16_t addr, uint8_t val);

		void connectInternalVram(uint8_t* internalVram);
		void changeMirrorType(NesFile::MirrorType mirrorType);
	protected:
		inline uint8_t readSram(uint16_t addr) const
		{
			return this->sram[addr & 0x1fff];
		}
		inline void writeSram(uint16_t addr, uint8_t value)
		{
			this->sram[addr & 0x1fff] = value;
		}
		const NesFile* const nesFile;
	private:
		VirtualMachine& VM;
		uint8_t sram[SRAM_SIZE];
		NesFile::MirrorType mirrorType;
		uint8_t* vramMirroring[4];
		uint8_t* internalVram;
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

class Video
{
	public:
		explicit Video(VirtualMachine& vm, VideoFairy& videoFairy);
		~Video();
		void run(uint16_t clockDelta);
		void onHardReset();
		void onReset();
		uint8_t readReg(uint16_t addr);
		void writeReg(uint16_t addr, uint8_t value);
		void executeDMA(uint8_t value);
		void connectCartridge(Cartridge* cartridge);
		static const int screenWidth = 256;
		static const int screenHeight = 240;
	protected:
	private:
		static const uint8_t nesPalette[64][3];
		static const int clockPerScanline = 341;
		static const int scanlinePerScreen = 262;
		static const int defaultSpriteCnt = 8;
		VirtualMachine& VM;
		Cartridge* cartridge;
	   VideoFairy& videoFairy;
		bool isEven;
		uint16_t nowY;
		uint16_t nowX;
		uint8_t spRam[256];
		uint8_t internalVram[2048];
		uint8_t palette[9][4];

		/* Rendering */
		struct SpriteSlot {
			uint8_t idx;
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
		inline uint8_t readVram(uint16_t addr) const;
		inline void writeVram(uint16_t addr, uint8_t value);
		inline uint8_t readVramExternal(uint16_t addr) const;
		inline void writeVramExternal(uint16_t addr, uint8_t value);
		inline uint8_t readPalette(uint16_t addr) const;
		inline void writePalette(uint16_t addr, uint8_t value);
		inline uint8_t readSprite(uint16_t addr) const;
		inline void writeSprite(uint16_t addr, uint8_t value);

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
		uint16_t vramAddrRegisterBuffer;
		bool scrollRegisterWritten;
		bool vramAddrRegisterWritten;

		inline void analyzePPUControlRegister1(uint8_t value);
		inline void analyzePPUControlRegister2(uint8_t value);
		inline void analyzePPUBackgroundScrollingOffset(uint8_t value);
		inline uint8_t buildPPUStatusRegister();
		inline uint8_t readVramDataRegister();
		inline uint8_t readSpriteDataRegister();
		inline void analyzeVramAddrRegister(uint8_t value);
		inline void analyzeSpriteAddrRegister(uint8_t value);
		inline void writeVramDataRegister(uint8_t value);
		inline void writeSpriteDataRegister(uint8_t value);

		inline void startVBlank();
};

class Ram
{
	public:
		static const uint16_t WRAM_LENGTH = 2048;
	public:
		explicit Ram(VirtualMachine& vm) : VM(vm)
		{}
		~Ram(){}
		inline void onHardReset()
		{
			//from http://wiki.nesdev.com/w/index.php/CPU_power_up_state
			memset(wram, 0xff, WRAM_LENGTH);
			wram[0x8] = 0xf7;
			wram[0x9] = 0xef;
			wram[0xa] = 0xdf;
			wram[0xf] = 0xbf;
		}
		inline void onReset()
		{

		}
		inline uint8_t read(uint16_t addr)
		{
			return wram[addr & 0x7ff];
		}
		inline void write(uint16_t addr, uint8_t value)
		{
			wram[addr & 0x7ff] = value;
		}
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
		inline uint8_t read(uint16_t addr);
		inline void write(uint16_t addr, uint8_t value);
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
		inline void push(uint8_t val);
		inline uint8_t pop();
		inline void consumeClock(uint8_t clock);
		inline void updateFlagZN(const uint8_t val);
		//ハードウェア☆割り込み
		inline void onNMI();
		inline void onIRQ();

		//アドレッシングモード
		inline uint16_t addrImmediate();
		inline uint16_t addrAbsolute();
		inline uint16_t addrZeroPage();
		inline uint16_t addrZeroPageIdxX();
		inline uint16_t addrZeroPageIdxY();
		inline uint16_t addrAbsoluteIdxX();
		inline uint16_t addrAbsoluteIdxY();
		inline uint16_t addrRelative();
		inline uint16_t addrIndirectX();
		inline uint16_t addrIndirectY();
		inline uint16_t addrAbsoluteIndirect();

		//命令一覧 from http://nesdev.parodius.com/opcodes.txt
		inline void BCC(uint16_t addr);
		inline void BCS(uint16_t addr);
		inline void BEQ(uint16_t addr);
		inline void BNE(uint16_t addr);
		inline void BVC(uint16_t addr);
		inline void BVS(uint16_t addr);
		inline void BPL(uint16_t addr);
		inline void BMI(uint16_t addr);

		inline void LDA(uint16_t addr);
		inline void LDY(uint16_t addr);
		inline void LDX(uint16_t addr);
		inline void STA(uint16_t addr);
		inline void STY(uint16_t addr);
		inline void STX(uint16_t addr);

		inline void TXA();
		inline void TYA();
		inline void TXS();
		inline void TAY();
		inline void TAX();
		inline void TSX();

		inline void PHP();
		inline void PLP();
		inline void PHA();
		inline void PLA();

		inline void ADC(uint16_t addr);
		inline void SBC(uint16_t addr);
		inline void CPX(uint16_t addr);
		inline void CPY(uint16_t addr);
		inline void CMP(uint16_t addr);

		inline void AND(uint16_t addr);
		inline void EOR(uint16_t addr);
		inline void ORA(uint16_t addr);
		inline void BIT(uint16_t addr);

		inline void ASL();
		inline void ASL(uint16_t addr);
		inline void LSR();
		inline void LSR(uint16_t addr);
		inline void ROL();
		inline void ROL(uint16_t addr);
		inline void ROR();
		inline void ROR(uint16_t addr);

		inline void INX();
		inline void INY();
		inline void INC(uint16_t addr);
		inline void DEX();
		inline void DEY();
		inline void DEC(uint16_t addr);

		inline void CLC();
		inline void CLI();
		inline void CLV();
		inline void CLD();
		inline void SEC();
		inline void SEI();
		inline void SED();

		inline void NOP();
		inline void BRK();

		inline void JSR(uint16_t addr);
		inline void JMP(uint16_t addr);
		inline void RTI();
		inline void RTS();
};

class VirtualMachine
{
	public:
		explicit VirtualMachine(VideoFairy& videoFairy);
		~VirtualMachine();
		void run();
		void sendNMI(); //from video to processor
		void sendIRQ(); //from cartridge and audio to processor.
		void sendHBlank(uint16_t scanline); //from video to cartridge
		void sendVBlank(); //from video
		void sendHardReset(); //from user to all subsystems.
		void sendReset(); //from user to all subsystems.
		void loadCartridge(const char* filename); //from user
		uint8_t read(uint16_t addr); //from processor to ram
		void write(uint16_t addr, uint8_t value); // from processor to ram.
		void consumeCpuClock(uint32_t clock);
		void consumeAudioClock(uint32_t clock);
	protected:
	private:
		static const int MAIN_CLOCK = 21477272;//21.28MHz(NTSC)
		static const int CPU_CLOCK_FACTOR = 12;
		static const int AUDIO_CLOCK_FACTOR = 12;
		static const int VIDEO_CLOCK_FACTOR = 4;
		static const int CARTRIDGE_CLOCK_FACTOR = 12;
		void consumeClock(uint32_t clock);
		VideoFairy& videoFairy;
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
