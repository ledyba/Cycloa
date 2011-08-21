#include "VirtualMachine.h"
#include <stdio.h>
#include <string.h>

Video::Video(VirtualMachine& vm):
    VM(vm),
    cartridge(NULL),
    image(),
    isEven(false),
    nowY(0),
    nowX(0),
	executeNMIonVBlank(false),
	spriteHeight(8),
	patternTableAddressBackground(0),
	patternTableAddress8x8Sprites(0),
	vramIncrementSize(1),
	nameTableScrollAddr(0),
	colorEmphasis(0),
	spriteVisibility(false),
	backgroundVisibility(false),
	spriteClipping(false),
	backgroundClipping(false),
	paletteMask(0),
	nowOnVBnank(false),
	sprite0Hit(false),
	lostSprites(false),
	vramBuffer(0),
	spriteAddr(0),
	horizontalScrollOrigin(0),
	verticalScrollOrigin(0),
	vramAddrRegister(0x0),
	scrollRegisterWritten(false),
	vramAddrRegisterWritten(false)
{
    //ctor
}

Video::~Video()
{
    //dtor
}
void Video::run(uint16_t clockDelta)
{
	this->nowX += clockDelta;
	while(this->nowX >= 341){
		this->nowY++;
		this->nowX -= 341;
		if(this->nowY <= 240){
			memset(lineBuff, 0xff, screenWidth); //oxffをBGカラーに後で置き換える
			spriteEval();
			buildBgLine();
			buildSpriteLine();
			fillImage();
		}else if(this->nowY == 241){
			this->nowOnVBnank = true;
			//The PPU just idles during this scanline. Despite this, this scanline still occurs before the VBlank flag is set.
			this->startVBlank();
		}else if(this->nowY <= 261){
			//nowVBlank.
		}else if(this->nowY == 262){
			this->nowOnVBnank = false;
			this->nowY = 0;
			if(!this->isEven){
				this->nowX++;
			}
			this->isEven = !this->isEven;
		}else{
			throw EmulatorException("Invalid scanline") << this->nowY;
		}
		this->VM.sendHSync(this->nowY);
	}
}

void Video::spriteEval()
{
	const uint16_t y = this->nowY-1;
	uint8_t spriteHitCnt = 0;
	this->lostSprites = false;
	const uint8_t _sprightHeight = this->spriteHeight;
	bool bigSprite = _sprightHeight == 16;
	const uint16_t spriteTileAddrBase = this->patternTableAddress8x8Sprites;
	for(uint16_t i=0;i<256;i+=4){
		uint16_t spY = readSprite(i)+1;
		uint16_t spYend = spY+_sprightHeight;
		bool hit = false;
		if(spY <= y && y < spYend){//Hit!
			if(spriteHitCnt < defaultSpriteCnt){
				hit = true;
				struct SpriteSlot& slot = spriteTable[spriteHitCnt];
				slot.y = spY;
				slot.x = readSprite(i+3);
				if(bigSprite){
					//8x16
					uint8_t val = readSprite(i+1);
					slot.tileAddr = (val & 1) << 12 | (val & 0xfe) << 4;
				}else{
					//8x8
					slot.tileAddr = (readSprite(i+1) << 4) + spriteTileAddrBase;
				}
				const uint8_t attr = readSprite(i+2);
				slot.paletteNo = 4 + (attr & 3);
				slot.isForeground = (attr & (1<<5)) == 0;
				slot.flipHorizontal = (attr & (1<<6)) != 0;
				slot.flipVertical = (attr & (1<<7)) != 0;
				spriteHitCnt++;
			}else{
				//本当はもっと複雑な仕様みたいなものの、省略。
				//http://wiki.nesdev.com/w/index.php/PPU_sprite_evaluation
				this->lostSprites = true;
				break;
			}
		}
		if(i==0){
			this->sprite0Hit = hit;
		}
	}
	//残りは無効化
	for(uint16_t i=spriteHitCnt;i<defaultSpriteCnt;i++){
		spriteTable[i].y=255;
	}
}
void Video::buildSpriteLine()
{
	if(!this->spriteVisibility){
		return;
	}
	const uint16_t y = this->nowY-1;
	const uint16_t spriteHeight = this->spriteHeight;
	for(uint8_t i=0;i<defaultSpriteCnt;i++){
		const struct SpriteSlot slot = this->spriteTable[i];
		uint16_t offY = y-slot.y;
		if(slot.y <= y && offY < spriteHeight){
			if(slot.flipVertical){
				offY = spriteHeight-offY-1;
			}
			const uint16_t off = slot.tileAddr+(((offY & 0x8) << 1) + (offY&7));
			const uint8_t firstPlane = readVram(off);
			const uint8_t secondPlane = readVram(off+8);
			const uint16_t endX = std::min(screenWidth-slot.x, 8);
			if(slot.flipHorizontal){
				for(size_t x=0;x<endX;x++){
					const uint8_t color = ((firstPlane >> x) & 1) | (((secondPlane >> x) & 1)<<1);
					uint8_t& target = this->lineBuff[slot.x + x];
					if(color != 0 && (target == 0xff || slot.isForeground)){
						target = this->palette[slot.paletteNo][color];
					}
				}
			}else{
				for(size_t x=0;x<endX;x++){
					const uint8_t color = ((firstPlane >> (7-x)) & 1) | (((secondPlane >> (7-x)) & 1)<<1);
					uint8_t& target = this->lineBuff[slot.x + x];
					if(color != 0 && (target == 0xff || slot.isForeground)){
						target = this->palette[slot.paletteNo][color];
					}
				}
			}
		}
	}
}

void Video::buildBgLine()
{
	const uint16_t y = this->nowY+this->verticalScrollOrigin-1;
	const uint16_t tileY = y>>3;
	const uint16_t tileYofScreen = tileY % 30;
	const uint16_t tileYoff = y & 7;

	const uint16_t offX = this->horizontalScrollOrigin;
	const uint16_t endX = offX+screenWidth;

	const uint16_t bgTileAddrBase = this->patternTableAddressBackground;
	const uint16_t nameTableOffBase = (tileY < 30) ? 0x2000 : 0x2800;

	uint16_t renderX=0;
	for(uint16_t tileX = (offX>>3);tileX<=((endX-1)>>3);tileX++){
		const int16_t tileXoff = -std::min(0, (tileX << 3)-offX);
		const int16_t tileXend = std::min(8,endX-(tileX << 3));
		const uint16_t nameTableOff = nameTableOffBase | ((tileX < 32) ? 0x0000 : 0x0400);

		const uint16_t tileNo = readVram(nameTableOff | ((tileYofScreen << 5) | (tileX & 31)));
		const uint8_t palNo =
				(
					readVram(nameTableOff | 0x3c0 | ((tileYofScreen & 0b11100) << 1) | ((tileX >> 2) & 7))
								>> (((tileYofScreen & 2) << 1) | (tileX & 2))
				) & 0x3;


		const uint16_t off = bgTileAddrBase | (tileNo << 4) | tileYoff;

		const uint8_t firstPlane = readVram(off);
		const uint8_t secondPlane = readVram(off+8);

		for(int16_t x=tileXoff;x<tileXend;x++){
			const uint8_t color = ((firstPlane >> (7-x)) & 1) | (((secondPlane >> (7-x)) & 1)<<1);
			if(color != 0){
				this->lineBuff[renderX] = this->palette[palNo][color];
			}
			renderX++;
		}
	}
}

void Video::fillImage()
{
	uint8_t* const imageBuf = this->image.getPtr(this->nowY-1);
	uint8_t* const lineBuf = this->lineBuff;
	const uint8_t mask = paletteMask;
	const uint8_t bgColor = palette[8][0] & mask;
	uint16_t imageIdx = 0;
	for(uint16_t x = 0;x<screenWidth;x++){
		const uint8_t buf = lineBuf[x];
		const uint8_t palNo = (buf == 0xff) ? bgColor : (buf & mask);
		const uint8_t* color = Video::nesPalette[palNo];
		imageBuf[imageIdx+0] = color[0];
		imageBuf[imageIdx+1] = color[1];
		imageBuf[imageIdx+2] = color[2];
		imageIdx+=3;
	}
}

void Video::onHardReset()
{
    //from http://wiki.nesdev.com/w/index.php/PPU_power_up_state
    memset(internalVram, 0, 2048);
    memset(spRam, 0 , 256);
    memset(palette, 0, 32);
    //0x2000
	executeNMIonVBlank = false;
	spriteHeight = 8;
	patternTableAddressBackground = 0x0000;
	patternTableAddress8x8Sprites = 0x0000;
	vramIncrementSize = 1;
	nameTableScrollAddr = 0x2000;
    //0x2001
	colorEmphasis = 0;
	spriteVisibility = false;
	backgroundVisibility = false;
	spriteClipping = true;
	backgroundClipping = true;
	paletteMask = 0x3f;
	//0x2003
	spriteAddr = 0;
	//0x2005/0x2006
	vramAddrRegisterWritten = false;
	scrollRegisterWritten = false;
	//0x2005
	verticalScrollOrigin = 0;
	horizontalScrollOrigin = 0;
	//0x2006
	vramAddrRegister = 0;
}
void Video::onReset()
{
    //from http://wiki.nesdev.com/w/index.php/PPU_power_up_state
    //0x2000
	executeNMIonVBlank = false;
	spriteHeight = 8;
	patternTableAddressBackground = 0x0000;
	patternTableAddress8x8Sprites = 0x0000;
	vramIncrementSize = 1;
	nameTableScrollAddr = 0x2000;
    //0x2001
	colorEmphasis = 0;
	spriteVisibility = false;
	backgroundVisibility = false;
	spriteClipping = true;
	backgroundClipping = true;
	paletteMask = 0x3f;
	//0x2005/0x2006
	vramAddrRegisterWritten = false;
	scrollRegisterWritten = false;
	//0x2005
	verticalScrollOrigin = 0;
	horizontalScrollOrigin = 0;
	//0x2007
	vramBuffer = 0;
}
uint8_t Video::readReg(uint16_t addr)
{
    const uint16_t regAddr = addr & 0x07;
    switch(regAddr)
    {
    	/* PPU Control and Status Registers */
    	//case 0x00: //2000h - PPU Control Register 1 (W)
    	//case 0x01: //2001h - PPU Control Register 2 (W)
    	case 0x02: //2002h - PPU Status Register (R)
            return buildPPUStatusRegister();
    	/* PPU SPR-RAM Access Registers */
    	//case 0x03: //2003h - SPR-RAM Address Register (W)
    	case 0x04: //2004h - SPR-RAM Data Register (Read/Write)
			return readSpriteDataRegister();
    	/* PPU VRAM Access Registers */
    	//case 0x05: //PPU Background Scrolling Offset (W2)
    	//case 0x06: //VRAM Address Register (W2)
    	case 0x07: //VRAM Read/Write Data Register (RW)
			return readVramDataRegister();
    	default:
			throw EmulatorException() << "Invalid addr: 0x" << std::hex << addr;
    }
}
void Video::writeReg(uint16_t addr, uint8_t value)
{
    const uint16_t regAddr = addr & 0x07;
    switch(regAddr)
    {
    	/* PPU Control and Status Registers */
    	case 0x00: //2000h - PPU Control Register 1 (W)
            analyzePPUControlRegister1(value);
            break;
    	case 0x01: //2001h - PPU Control Register 2 (W)
            analyzePPUControlRegister2(value);
            break;
    	//case 0x02: //2002h - PPU Status Register (R)
    	/* PPU SPR-RAM Access Registers */
    	case 0x03: //2003h - SPR-RAM Address Register (W)
            analyzeSpriteAddrRegister(value);
            break;
    	case 0x04: //2004h - SPR-RAM Data Register (Read/Write)
			writeSpriteDataRegister(value);
			break;
    	/* PPU VRAM Access Registers */
    	case 0x05: //PPU Background Scrolling Offset (W2)
            analyzePPUBackgroundScrollingOffset(value);
            break;
    	case 0x06: //VRAM Address Register (W2)
			analyzeVramAddrRegister(value);
			break;
    	case 0x07: //VRAM Read/Write Data Register (RW)
			writeVramDataRegister(value);
			break;
    	default:
			throw EmulatorException() << "Invalid addr: 0x" << std::hex << addr;
    }
}

uint8_t Video::buildPPUStatusRegister()
{
    //from http://nocash.emubase.de/everynes.htm#pictureprocessingunitppu
	vramAddrRegisterWritten = false;
	scrollRegisterWritten = false;
    //Reading resets the 1st/2nd-write flipflop (used by Port 2005h and 2006h).
    uint8_t result =
            (this->nowOnVBnank) ? 0b10000000 : 0
        |   (this->sprite0Hit) ? 0b01000000 : 0
        |   (this->lostSprites) ? 0b00100000 : 0;
	this->nowOnVBnank = false;
	return result;
}

void Video::analyzePPUControlRegister1(uint8_t value)
{
	executeNMIonVBlank = ((value & 0b10000000) != 0) ? true : false;
	spriteHeight = ((value & 0b100000) != 0) ? 16 : 8;
	patternTableAddressBackground = ((value & 0b10000) != 0) ? 0x1000 : 0x0000;
	patternTableAddress8x8Sprites = ((value & 0b1000) != 0) ? 0x10000 : 0x0000;
	vramIncrementSize = ((value & 0b100) != 0) ? 32 : 1;
	switch(value & 0b11)
	{
		case 0:
			nameTableScrollAddr = 0x2000;
			break;
		case 1:
			nameTableScrollAddr = 0x2400;
			break;
		case 2:
			nameTableScrollAddr = 0x2800;
			break;
		case 3:
			nameTableScrollAddr = 0x2c00;
			break;
		default:
			throw EmulatorException() << "[analyzePPUControlRegister1] Invalid case: 0x" << std::hex << value;
	}
}
void Video::analyzePPUControlRegister2(uint8_t value)
{
	colorEmphasis = value >> 5; //FIXME: この扱い、どーする？
	spriteVisibility = ((value & 0b10000) != 0) ? true : false;
	backgroundVisibility = ((value & 0b1000) != 0) ? true : false;
	spriteClipping = ((value & 0b100) != 0) ? false : true;
	backgroundClipping = ((value & 0b10) != 0) ? false : true;
	paletteMask = ((value & 0b1) != 0) ? 0x30 : 0x3f;
}
void Video::analyzePPUBackgroundScrollingOffset(uint8_t value)
{
	if(scrollRegisterWritten){
		verticalScrollOrigin = value;
	}else{
		horizontalScrollOrigin = value;
	}
	scrollRegisterWritten = !scrollRegisterWritten;
}
void Video::analyzeVramAddrRegister(uint8_t value)
{
	if(vramAddrRegisterWritten){
		vramAddrRegister = (vramAddrRegister & 0xff00) | value;
	} else {
		vramAddrRegister = (vramAddrRegister & 0x00ff) | ((value & 0x3f) << 8);
	}
	vramAddrRegisterWritten = !vramAddrRegisterWritten;
}
void Video::analyzeSpriteAddrRegister(uint8_t value)
{
	spriteAddr = value;
}
uint8_t Video::readVramDataRegister()
{
	const uint8_t ret = vramBuffer;
	bool isPalette = (vramAddrRegister & 0x3f00) == 0x3f00; //パレットは内部RAMであり、ラッチを通過しない！
	vramBuffer = readVram(vramAddrRegister);
	vramAddrRegister = (vramAddrRegister + vramIncrementSize) & 0x3fff;
	return isPalette ? vramBuffer : ret;
}
void Video::writeVramDataRegister(uint8_t value)
{
	writeVram(vramAddrRegister, value);
	vramAddrRegister = (vramAddrRegister + vramIncrementSize) & 0x3fff;
}
uint8_t Video::readSpriteDataRegister()
{
	return readSprite(spriteAddr); //The address is NOT auto-incremented after <reading> from 2004h.
}
void Video::writeSpriteDataRegister(uint8_t value)
{
	writeSprite(spriteAddr, value);
	spriteAddr++; //The address is NOT auto-incremented after <reading> from 2004h.
}

void Video::startVBlank()
{
	spriteAddr = 0;//and typically contains 00h at the begin of the VBlank periods
	this->VM.showVideo(this->image);
	if(executeNMIonVBlank){
		this->VM.sendNMI();
	}
}

void Video::executeDMA(uint8_t value)
{
	uint16_t addrMask = value << 8;
	for(uint16_t i=0;i<256;i++){
		writeSpriteDataRegister(VM.read(addrMask | i));
	}
	this->VM.consumeCpuClock(512);
}

//-------------------- accessor ----------------------------

uint8_t Video::readVram(uint16_t addr) const
{
	if((addr & 0x3f00) == 0x3f00){
		if((addr & 0x3) == 0){
			return this->palette[8][(addr >> 2) & 3];
		}else{
			return this->palette[((addr>>2) & 7)][addr & 3];
		}
	}
	#ifndef DEBUG
	else{
		return this->cartridge->readVideo(addr);
	}
	#endif
	#ifdef DEBUG
	else if(this->cartridge != NULL){
		return this->cartridge->readVideo(addr);
	}else{
		throw new EmulatorException("Maybe cartidge is NULL.");
	}
	#endif
}
void Video::writeVram(uint16_t addr, uint8_t value)
{
	if((addr & 0x3f00) == 0x3f00){
		if((addr & 0x3) == 0){
			this->palette[8][(addr >> 2) & 3] = value & 0x3f;
		}else{
			this->palette[((addr>>2) & 7)][addr & 3] = value & 0x3f;
		}
	}
	#ifndef DEBUG
	else{
		this->cartridge->writeVideo(addr, value);
	}
	#endif
	#ifdef DEBUG
	else if(this->cartridge != NULL){
		this->cartridge->writeVideo(addr, value);
	}else{
		throw new EmulatorException("Maybe cartidge is NULL.");
	}
	#endif
}
void Video::connectCartridge(Cartridge* cartridge)
{
	this->cartridge = cartridge;
	cartridge->connectInternalVram(this->internalVram);
}
uint8_t Video::readSprite(uint16_t addr) const
{
    return this->spRam[addr];
}
void Video::writeSprite(uint16_t addr, uint8_t value)
{
    this->spRam[addr] = value;
}

const uint8_t Video::nesPalette[64][3] =
{
	{0x78,0x78,0x78},{0x20,0x00,0xB0},{0x28,0x00,0xB8},{0x60,0x10,0xA0},
	{0x98,0x20,0x78},{0xB0,0x10,0x30},{0xA0,0x30,0x00},{0x78,0x40,0x00},
	{0x48,0x58,0x00},{0x38,0x68,0x00},{0x38,0x6C,0x00},{0x30,0x60,0x40},
	{0x30,0x50,0x80},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},
	{0xB0,0xB0,0xB0},{0x40,0x60,0xF8},{0x40,0x40,0xFF},{0x90,0x40,0xF0},
	{0xD8,0x40,0xC0},{0xD8,0x40,0x60},{0xE0,0x50,0x00},{0xC0,0x70,0x00},
	{0x88,0x88,0x00},{0x50,0xA0,0x00},{0x48,0xA8,0x10},{0x48,0xA0,0x68},
	{0x40,0x90,0xC0},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},
	{0xFF,0xFF,0xFF},{0x60,0xA0,0xFF},{0x50,0x80,0xFF},{0xA0,0x70,0xFF},
	{0xF0,0x60,0xFF},{0xFF,0x60,0xB0},{0xFF,0x78,0x30},{0xFF,0xA0,0x00},
	{0xE8,0xD0,0x20},{0x98,0xE8,0x00},{0x70,0xF0,0x40},{0x70,0xE0,0x90},
	{0x60,0xD0,0xE0},{0x78,0x78,0x78},{0x00,0x00,0x00},{0x00,0x00,0x00},
	{0xFF,0xFF,0xFF},{0x90,0xD0,0xFF},{0xA0,0xB8,0xFF},{0xC0,0xB0,0xFF},
	{0xE0,0xB0,0xFF},{0xFF,0xB8,0xE8},{0xFF,0xC8,0xB8},{0xFF,0xD8,0xA0},
	{0xFF,0xF0,0x90},{0xC8,0xF0,0x80},{0xA0,0xF0,0xA0},{0xA0,0xFF,0xC8},
	{0xA0,0xFF,0xF0},{0xA0,0xA0,0xA0},{0x00,0x00,0x00},{0x00,0x00,0x00}
};
