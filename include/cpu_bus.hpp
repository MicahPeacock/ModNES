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
    template <typename Callback>
    using callback_table = std::unordered_map<word, Callback>;

    CPUBus() = delete;
    explicit CPUBus(Mapper* mapper) noexcept : mapper(mapper) {}

    byte read(word address) const noexcept;
    void write(word address, byte value) noexcept;

    void set_read_callbacks(callback_table<std::function<byte(void)>>&& callbacks);
    void set_write_callbacks(callback_table<std::function<void(byte)>>&& callbacks);

    const byte* get_page_ptr(byte page) const noexcept;

private:
    std::array<byte, 0x800> ram = {};
    Mapper* mapper;

    callback_table<std::function<byte(void)>> read_callbacks;
    callback_table<std::function<void(byte)>> write_callbacks;
};

} // nes

#endif //MODNES_CPU_BUS_HPP

