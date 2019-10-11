#include "exception/EmulatorException.h"
#include "VirtualMachine.h"
#include <stdio.h>
#include <string.h>

Video::Video(VirtualMachine &vm, VideoFairy &videoFairy) :
    VM(vm),
    cartridge(NULL),
    videoFairy(videoFairy),
    isEven(false),
    nowY(0),
    nowX(0),
    spriteHitCnt(0),
    executeNMIonVBlank(false),
    spriteHeight(8),
    patternTableAddressBackground(0),
    patternTableAddress8x8Sprites(0),
    vramIncrementSize(1),
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
    vramAddrRegister(0x0),
    vramAddrReloadRegister(0),
    horizontalScrollBits(0),
    scrollRegisterWritten(false),
    vramAddrRegisterWritten(false) {
  //ctor
  memset(this->screenBuffer, 0x0, screenWidth * screenHeight * sizeof(uint8_t));
}

Video::~Video() {
  //dtor
}

void Video::run(uint16_t clockDelta) {
  this->nowX += clockDelta;
  while (this->nowX >= 341) {
    this->nowY++;
    this->nowX -= 341;
    if (this->nowY <= 240) {
      uint8_t *const lineBuff = screenBuffer[nowY - 1];
      memset(lineBuff, EmptyBit | this->palette[8][0], screenWidth); //0x00: 空
      spriteEval();
      if (this->backgroundVisibility || this->spriteVisibility) {
        // from http://nocash.emubase.de/everynes.htm#pictureprocessingunitppu
        vramAddrRegister = (vramAddrRegister & 0x7BE0) | (vramAddrReloadRegister & 0x041F);
        buildBgLine();
        buildSpriteLine();
        vramAddrRegister += (1 << 12);
        vramAddrRegister += (vramAddrRegister & 0x8000) >> 10;
        vramAddrRegister &= 0x7fff;
        if ((vramAddrRegister & 0x03e0) == 0x3c0) {
          vramAddrRegister &= 0xFC1F;
          vramAddrRegister ^= 0x800;
        }
      }
    } else if (this->nowY == 241) {
      //241: The PPU just idles during this scanline. Despite this, this scanline still occurs before the VBlank flag is set.
      this->videoFairy.dispatchRendering(screenBuffer, this->paletteMask);
      this->nowOnVBnank = true;
      spriteAddr = 0;//and typically contains 00h at the begin of the VBlank periods
    } else if (this->nowY == 242) {
      // NESDEV: These occur during VBlank. The VBlank flag of the PPU is pulled low during scanline 241, so the VBlank NMI occurs here.
      // EVERYNES: http://nocash.emubase.de/everynes.htm#ppudimensionstimings
      // とあるものの…BeNesの実装だともっと後に発生すると記述されてる。詳しくは以下。
      // なお、$2002のレジスタがHIGHになった後にVBLANKを起こさないと「ソロモンの鍵」にてゲームが始まらない。
      // (NMI割り込みがレジスタを読み込みフラグをリセットしてしまう上、NMI割り込みが非常に長く、クリアしなくてもすでにVBLANKが終わった後に返ってくる)
      //nowOnVBlankフラグの立ち上がり後、数クロックでNMIが発生。
      if (executeNMIonVBlank) {
        this->VM.sendNMI();
      }
      this->VM.sendVBlank();
    } else if (this->nowY <= 261) {
      //nowVBlank.
    } else if (this->nowY == 262) {
      this->nowOnVBnank = false;
      this->sprite0Hit = false;
      this->nowY = 0;
      if (!this->isEven) {
        this->nowX++;
      }
      this->isEven = !this->isEven;
      // the reload value is automatically loaded into the Pointer at the end of the vblank period (vertical reload bits)
      // from http://nocash.emubase.de/everynes.htm#pictureprocessingunitppu
      if (this->backgroundVisibility || this->spriteVisibility) {
        this->vramAddrRegister = (vramAddrRegister & 0x041F) | (vramAddrReloadRegister & 0x7BE0);
      }
    } else {
      throw EmulatorException("Invalid scanline") << this->nowY;
    }
  }
}

inline void Video::spriteEval() {
  const uint16_t y = this->nowY - 1;
  uint8_t _spriteHitCnt = 0;
  this->lostSprites = false;
  const uint8_t _sprightHeight = this->spriteHeight;
  bool bigSprite = _sprightHeight == 16;
  const uint16_t spriteTileAddrBase = this->patternTableAddress8x8Sprites;
  for (uint16_t i = 0; i < 256; i += 4) {
    uint16_t spY = readSprite(i) + 1;
    uint16_t spYend = spY + _sprightHeight;
    bool hit = false;
    if (spY <= y && y < spYend) {//Hit!
      if (_spriteHitCnt < Video::defaultSpriteCnt) {
        hit = true;
        struct SpriteSlot &slot = spriteTable[_spriteHitCnt];
        slot.idx = i >> 2;
        slot.y = spY;
        slot.x = readSprite(i + 3);
        if (bigSprite) {
          //8x16
          uint8_t val = readSprite(i + 1);
          slot.tileAddr = (val & 1) << 12 | (val & 0xfe) << 4;
        } else {
          //8x8
          slot.tileAddr = (readSprite(i + 1) << 4) | spriteTileAddrBase;
        }
        const uint8_t attr = readSprite(i + 2);
        slot.paletteNo = 4 | (attr & 3);
        slot.isForeground = (attr & (1 << 5)) == 0;
        slot.flipHorizontal = (attr & (1 << 6)) != 0;
        slot.flipVertical = (attr & (1 << 7)) != 0;
        _spriteHitCnt++;
      } else {
        //本当はもっと複雑な仕様みたいなものの、省略。
        //http://wiki.nesdev.com/w/index.php/PPU_sprite_evaluation
        this->lostSprites = true;
        break;
      }
    }
  }
  //残りは無効化
  this->spriteHitCnt = _spriteHitCnt;
  for (uint16_t i = _spriteHitCnt; i < Video::defaultSpriteCnt; i++) {
    spriteTable[i].y = 255;
  }
}

inline void Video::buildSpriteLine() {
  if (!this->spriteVisibility) {
    return;
  }
  const uint16_t y = this->nowY - 1;
  const uint16_t _spriteHeight = this->spriteHeight;
  bool searchSprite0Hit = !this->sprite0Hit;
  const uint16_t _spriteHitCnt = this->spriteHitCnt;
  uint8_t *const lineBuff = screenBuffer[nowY - 1];
  readVram(this->spriteTable[0].tileAddr); //FIXME: 読み込まないと、MMC4が動かない。
  for (uint8_t i = 0; i < _spriteHitCnt; i++) {
    const struct SpriteSlot &slot = this->spriteTable[i];
    searchSprite0Hit &= (slot.idx == 0);
    uint16_t offY = 0;

    if (slot.flipVertical) {
      offY = _spriteHeight + slot.y - y - 1;
    } else {
      offY = y - slot.y;
    }
    const uint16_t off = slot.tileAddr | ((offY & 0x8) << 1) | (offY & 7);
    const uint8_t firstPlane = readVram(off);
    const uint8_t secondPlane = readVram(off + 8);
    const uint16_t endX = std::min(screenWidth - slot.x, 8);
    const uint8_t layerMask = slot.isForeground ? Video::FrontSpriteBit : Video::BackSpriteBit;
    if (slot.flipHorizontal) {
      for (size_t x = 0; x < endX; x++) {
        const uint8_t color = ((firstPlane >> x) & 1) | (((secondPlane >> x) & 1) << 1);
        uint8_t &target = lineBuff[slot.x + x];
        const bool isEmpty = (target & LayerBitMask) == EmptyBit;
        const bool isBackgroundDrawn = (target & LayerBitMask) == BackgroundBit;
        const bool isSpriteNotDrawn = (target & SpriteLayerBit) == 0;
        if (searchSprite0Hit && (color != 0 && isBackgroundDrawn)) {
          this->sprite0Hit = true;
          searchSprite0Hit = false;
        }
        if (color != 0 && ((!slot.isForeground && isEmpty) || (slot.isForeground && isSpriteNotDrawn))) {
          target = this->palette[slot.paletteNo][color] | layerMask;
        }
      }
    } else {
      for (size_t x = 0; x < endX; x++) {
        const uint8_t color = ((firstPlane >> (7 - x)) & 1) | (((secondPlane >> (7 - x)) & 1) << 1);
        uint8_t &target = lineBuff[slot.x + x];
        const bool isEmpty = (target & LayerBitMask) == EmptyBit;
        const bool isBackgroundDrawn = (target & LayerBitMask) == BackgroundBit;
        const bool isSpriteNotDrawn = (target & SpriteLayerBit) == 0;
        if (searchSprite0Hit && (color != 0 && isBackgroundDrawn)) {
          this->sprite0Hit = true;
          searchSprite0Hit = false;
        }
        if (color != 0 && ((!slot.isForeground && isEmpty) || (slot.isForeground && isSpriteNotDrawn))) {
          target = this->palette[slot.paletteNo][color] | layerMask;
        }
      }
    }
  }
}

inline void Video::buildBgLine() {
  if (!this->backgroundVisibility) {
    return;
  }
  uint8_t *const lineBuff = screenBuffer[nowY - 1];
  uint16_t nameTableAddr = 0x2000 | (vramAddrRegister & 0xfff);
  const uint8_t offY = (vramAddrRegister >> 12);
  uint8_t offX = this->horizontalScrollBits;

  const uint16_t bgTileAddrBase = this->patternTableAddressBackground;

  for (uint16_t renderX = 0;;) {
    const uint16_t tileNo = readVram(nameTableAddr);
    const uint16_t tileYofScreen = (nameTableAddr & 0x03e0) >> 5;
    const uint8_t palNo =
        (
            readVram((nameTableAddr & 0x2f00) | 0x3c0 | ((tileYofScreen & 0x1C) << 1) | ((nameTableAddr >> 2) & 7))
                >> (((tileYofScreen & 2) << 1) | (nameTableAddr & 2))
        ) & 0x3;
    //タイルのサーフェイスデータを取得
    const uint16_t off = bgTileAddrBase | (tileNo << 4) | offY;
    const uint8_t firstPlane = readVram(off);
    const uint8_t secondPlane = readVram(off + 8);
    const uint8_t *const thisPalette = this->palette[palNo];
    //書く！
    for (int8_t x = offX; x < 8; x++) {
      const uint8_t color = ((firstPlane >> (7 - x)) & 1) | (((secondPlane >> (7 - x)) & 1) << 1);
      if (color != 0) {
        lineBuff[renderX] = thisPalette[color] | BackgroundBit;
      }
      renderX++;
      if (renderX >= screenWidth) {
        return;
      }
    }
    if ((nameTableAddr & 0x001f) == 0x001f) {
      nameTableAddr &= 0xFFE0;
      nameTableAddr ^= 0x400;
    } else {
      nameTableAddr++;
    }
    offX = 0;//次からは最初のピクセルから書ける。
  }
}

void Video::onHardReset() {
  //from http://wiki.nesdev.com/w/index.php/PPU_power_up_state
  memset(internalVram, 0, 2048);
  memset(spRam, 0, 256);
  memset(palette, 0, 32);
  nowY = 0;
  nowX = 0;
  //0x2000
  executeNMIonVBlank = false;
  spriteHeight = 8;
  patternTableAddressBackground = 0x0000;
  patternTableAddress8x8Sprites = 0x0000;
  vramIncrementSize = 1;
  //0x2005 & 0x2000
  vramAddrReloadRegister = 0x0000;
  horizontalScrollBits = 0;
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
  //0x2006
  vramAddrRegister = 0;
}

void Video::onReset() {
  //from http://wiki.nesdev.com/w/index.php/PPU_power_up_state
  //0x2000
  executeNMIonVBlank = false;
  spriteHeight = 8;
  patternTableAddressBackground = 0x0000;
  patternTableAddress8x8Sprites = 0x0000;
  vramIncrementSize = 1;
  //0x2005 & 0x2000
  vramAddrReloadRegister = 0x0000;
  horizontalScrollBits = 0;
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
  //0x2007
  vramBuffer = 0;
}

uint8_t Video::readReg(uint16_t addr) {
  switch (addr & 0x07) {
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
      return 0;
//			throw EmulatorException() << "Invalid addr: 0x" << std::hex << addr;
  }
}

void Video::writeReg(uint16_t addr, uint8_t value) {
  switch (addr & 0x07) {
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

inline uint8_t Video::buildPPUStatusRegister() {
  //from http://nocash.emubase.de/everynes.htm#pictureprocessingunitppu
  vramAddrRegisterWritten = false;
  scrollRegisterWritten = false;
  //Reading resets the 1st/2nd-write flipflop (used by Port 2005h and 2006h).
  uint8_t result =
      ((this->nowOnVBnank) ? 128 : 0)
      | ((this->sprite0Hit) ? 64 : 0)
      | ((this->lostSprites) ? 32 : 0);
  this->nowOnVBnank = false;
  return result;
}

inline void Video::analyzePPUControlRegister1(uint8_t value) {
  executeNMIonVBlank = ((value & 0x80) == 0x80) ? true : false;
  spriteHeight = ((value & 0x20) == 0x20) ? 16 : 8;
  patternTableAddressBackground = (value & 0x10) << 8;
  patternTableAddress8x8Sprites = (value & 0x8) << 9;
  vramIncrementSize = ((value & 0x4) == 0x4) ? 32 : 1;
  vramAddrReloadRegister = (vramAddrReloadRegister & 0x73ff) | ((value & 0x3) << 10);
}

inline void Video::analyzePPUControlRegister2(uint8_t value) {
  colorEmphasis = value >> 5; //FIXME: この扱い、どーする？
  spriteVisibility = ((value & 0x10) == 0x10) ? true : false;
  backgroundVisibility = ((value & 0x08) == 0x08) ? true : false;
  spriteClipping = ((value & 0x04) == 0x04) ? false : true;
  backgroundClipping = ((value & 0x2) == 0x02) ? false : true;
  paletteMask = ((value & 0x1) == 0x01) ? 0x30 : 0x3f;
}

inline void Video::analyzePPUBackgroundScrollingOffset(uint8_t value) {
  if (scrollRegisterWritten) { //Y
    vramAddrReloadRegister = (vramAddrReloadRegister & 0x8C1F) | ((value & 0xf8) << 2) | ((value & 7) << 12);
  } else { //X
    vramAddrReloadRegister = (vramAddrReloadRegister & 0xFFE0) | value >> 3;
    horizontalScrollBits = value & 7;
  }
  scrollRegisterWritten = !scrollRegisterWritten;
}

inline void Video::analyzeVramAddrRegister(uint8_t value) {
  if (vramAddrRegisterWritten) {
    vramAddrReloadRegister = (vramAddrReloadRegister & 0x7f00) | value;
    vramAddrRegister = vramAddrReloadRegister & 0x3fff;
  } else {
    vramAddrReloadRegister = (vramAddrReloadRegister & 0x00ff) | ((value & 0x7f) << 8);
  }
  vramAddrRegisterWritten = !vramAddrRegisterWritten;
}

inline void Video::analyzeSpriteAddrRegister(uint8_t value) {
  spriteAddr = value;
}

inline uint8_t Video::readVramDataRegister() {
  if ((vramAddrRegister & 0x3f00) == 0x3f00) {
    const uint8_t ret = readPalette(vramAddrRegister);
    vramBuffer = readVramExternal(vramAddrRegister); //ミラーされてるVRAMにも同時にアクセスしなければならない。
    vramAddrRegister = (vramAddrRegister + vramIncrementSize) & 0x3fff;
    return ret;
  } else {
    const uint8_t ret = vramBuffer;
    vramBuffer = readVramExternal(vramAddrRegister);
    vramAddrRegister = (vramAddrRegister + vramIncrementSize) & 0x3fff;
    return ret;
  }
}

inline void Video::writeVramDataRegister(uint8_t value) {
  writeVram(vramAddrRegister, value);
  vramAddrRegister = (vramAddrRegister + vramIncrementSize) & 0x3fff;
}

inline uint8_t Video::readSpriteDataRegister() {
  return readSprite(spriteAddr); //The address is NOT auto-incremented after <reading> from 2004h.
}

inline void Video::writeSpriteDataRegister(uint8_t value) {
  writeSprite(spriteAddr, value);
  spriteAddr++; //The address is NOT auto-incremented after <reading> from 2004h.
}

void Video::executeDMA(uint8_t value) {
  const uint16_t addrMask = value << 8;
  for (uint16_t i = 0; i < 256; i++) {
    writeSpriteDataRegister(VM.read(addrMask | i));
  }
  this->VM.consumeCpuClock(514);
}

//-------------------- accessor ----------------------------

inline uint8_t Video::readVram(uint16_t addr) const {
  if ((addr & 0x3f00) == 0x3f00) {
    return readPalette(addr);
  } else {
    return readVramExternal(addr);
  }
}

inline void Video::writeVram(uint16_t addr, uint8_t value) {
  if ((addr & 0x3f00) == 0x3f00) {
    writePalette(addr, value);
  } else {
    writeVramExternal(addr, value);
  }
}

inline uint8_t Video::readVramExternal(uint16_t addr) const {
  switch (addr & 0x3000) {
    case 0x0000:
      return this->cartridge->readPatternTableLow(addr);
    case 0x1000:
      return this->cartridge->readPatternTableHigh(addr);
    case 0x2000:
      return this->cartridge->readNameTable(addr);
    case 0x3000:
      return this->cartridge->readNameTable(addr);
    default:
      throw EmulatorException("Invalid vram access");
  }
}

inline void Video::writeVramExternal(uint16_t addr, uint8_t value) {
  switch (addr & 0x3000) {
    case 0x0000:
      this->cartridge->writePatternTableLow(addr, value);
      break;
    case 0x1000:
      this->cartridge->writePatternTableHigh(addr, value);
      break;
    case 0x2000:
      this->cartridge->writeNameTable(addr, value);
      break;
    case 0x3000:
      this->cartridge->writeNameTable(addr, value);
      break;
    default:
      throw EmulatorException("Invalid vram access");
  }
}

inline uint8_t Video::readPalette(uint16_t addr) const {
  if ((addr & 0x3) == 0) {
    return this->palette[8][(addr >> 2) & 3];
  } else {
    return this->palette[((addr >> 2) & 7)][addr & 3];
  }
}

inline void Video::writePalette(uint16_t addr, uint8_t value) {
  if ((addr & 0x3) == 0) {
    this->palette[8][(addr >> 2) & 3] = value & 0x3f;
  } else {
    this->palette[((addr >> 2) & 7)][addr & 3] = value & 0x3f;
  }
}

void Video::connectCartridge(Cartridge *cartridge) {
  this->cartridge = cartridge;
  cartridge->connectInternalVram(this->internalVram);
}

inline uint8_t Video::readSprite(uint16_t addr) const {
  return this->spRam[addr];
}

inline void Video::writeSprite(uint16_t addr, uint8_t value) {
  this->spRam[addr] = value;
}

