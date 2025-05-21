#ifndef MODNES_PPU_BUS_HPP
#define MODNES_PPU_BUS_HPP

#include "definitions.hpp"
#include "mappers/mapper.hpp"

#include <array>

namespace nes {

struct PPUBus {
    explicit PPUBus(Mapper* mapper) noexcept : mapper(mapper) {}

    byte read(word address) const noexcept;
    void write(word address, byte value) noexcept;

private:
    std::array<byte, 0x800> ram = {};
    std::array<byte, 0x20> palette   = {};
    std::array<word, 0x04> nametable = {};

    Mapper* mapper;
};

} // namespace nes

#endif //MODNES_PPU_BUS_HPP
