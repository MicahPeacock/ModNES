#ifndef MODNES_REGISTERS_HPP
#define MODNES_REGISTERS_HPP

#include <cstdint>
#include <functional>

namespace modnes {

enum class IORegisters : word {
    PPUCTRL = 0x2000,
    PPUMASK = 0x2001,
    PPUSTAT = 0x2002,
    OAMADDR = 0x2003,
    OAMDATA = 0x2004,
    PPUSCRL = 0x2005,
    PPUADDR = 0x2006,
    PPUDATA = 0x2007,
    OAMDMA  = 0x4014,
    JOYPAD1 = 0x4016,
    JOYPAD2 = 0x4017,
};

} // modnes


namespace std {

template <>
struct hash<modnes::IORegisters> {
    size_t operator()(const modnes::IORegisters& reg) const noexcept {
        return size_t(reg);
    }
};

} // std

#endif //MODNES_REGISTERS_HPP
