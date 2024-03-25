<h1>ModNES (Modern C++ NES)</h1>

This is yet another C++ NES emulator on the internet. However this emulator is written with a focus on readbility, simplicity, and (if possible) efficiency.

This personal project is primarily used as a learning experience on both about how retro video games work under the hood, as well as delving into more features of modern C++.

This project will utilize SDL 2.0 as the graphical engine as it is open source, cross-platform, and has extensive documentation.

<h3>What's done</h3>

+ Emulate the CPU
  + Build abstraction for NES specific 6502 chip (with cycle counting and interrupt support)
  + Emulate basic 6502 instruction set
  + Add connection to the NES System Bus

+ Parsing an iNES ROM
  + Build abstraction for NES cartridge
  + Handle errors on file parsing (throwing exceptions for now)
  
+ Emulate Mappers
  + Build Mapper 0 (NROM)
 
+ Emulate the CPU Bus
  + Handle memory accesses to system RAM and cartridge ROM
  + Added support for system callbacks for PPU and APU

<h3>To Do</h3>

+ Add Logging
  + Log CPU traces
  + Log errors and warnings
    
+ Build program loop
  + Cleanly create/destroy SDL game instances
  + Handle keyboard/controller events

+ Emulate the PPU
  + Build abstractions for both the 2C02 PPU and its respective picture bus
  + Render textures to the SDL game instance
  + Add scrolling support
  + Add PPU interrupts to CPU system bus

+ Emulate the APU
  + Build abstractions for various 2A02 channels and envelopes
  + (Still researching steps)
 
+ Add more NES mapper support
+ Add extended RAM support
 
+ Modern C++ clean up
  + Try to make certain aspects of the library constexpr-enabled
  + Compile with clang-tidy flags to check for vulnerabilities
