#include "ppu_bus.hpp"

namespace nes {

byte PPUBus::read(word address) const noexcept {
    address = address & 0x3fff;
    switch (address) {
        case 0x0000 ... 0x1fff:
            return ram[address & 0x7ff];
        case 0x2000 ... 0x3eff:
            return 0;
        case 0x3f00 ... 0x3fff:
            return 0;
        default:
            return 0;
    }
}

void PPUBus::write(word address, byte value) noexcept {
    address = address & 0x3fff;
    switch (address) {
        case 0x0000 ... 0x1fff:
            ram[address & 0x7ff] = value;
            break;
        case 0x2000 ... 0x3eff:
        case 0x3f00 ... 0x3fff:
        default:
            break;
    }
}

} // nes