#ifndef MODNES_CPU_BUS_HPP
#define MODNES_CPU_BUS_HPP

#include <array>
#include <functional>
#include <unordered_map>

#include "definitions.hpp"
#include "mappers/mapper.hpp"

namespace nes {

static constexpr word
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
    JOYPAD2 = 0x4017;

struct CPUBus {
    CPUBus() = delete;
    explicit CPUBus(Mapper* mapper) noexcept : mapper(mapper) {}

    byte read(word address) const noexcept;
    void write(word address, byte value) noexcept;

    void set_read_callback(word address, std::function<byte(void)>&& callback);
    void set_write_callback(word address, std::function<void(byte)>&& callback);

private:
    std::array<byte, 0x800> ram = {};
    Mapper* mapper;

    std::unordered_map<word, std::function<byte(void)>> read_callbacks;
    std::unordered_map<word, std::function<void(byte)>> write_callbacks;
};

} // nes

#endif //MODNES_CPU_BUS_HPP

