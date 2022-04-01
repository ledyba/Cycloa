# Cycloa - a NES emulator

Cycloa is a NES emulator implemented in C++. the documents "How to make a NES emulator without hardware reverse-engineering" is available on [/book/](https://github.com/ledyba/Cycloa/tree/master/book) (in Japanese)

CycloaはC++で記述されたファミコンエミュレータです。作り方についての同人誌[「エミュレータはソフトなマホウ　ハードウェア解析なしでつくるファミコンエミュレータ」](https://github.com/ledyba/Cycloa/tree/master/book)もあります。

# JavaScript Version

The JavaScript version of Cycloa is available:

このエミュレータのJavaScriptバージョンもあります。

 - https://app.7io.org/CycloaJS/ (Demo)
 - https://github.com/ledyba/CycloaJS (Sourcecode)

# How to build

## UNIX-like systems

```bash
git clone --recurse-submodules --recursive git@github.com:ledyba/Cycloa.git
cd Cycloa
mkdir build && cd build
cmake ..
make Cycloa
```

# License

Cycloa is licensed under AGPL v3 or later.
