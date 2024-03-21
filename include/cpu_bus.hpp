#ifndef MODNES_CPU_BUS_HPP
#define MODNES_CPU_BUS_HPP

#include <utility>

#include "definitions.hpp"
#include "mappers/mapper.hpp"
#include "ppu.hpp"
#include "registers.hpp"
#include "controller.hpp"

namespace modnes {

struct CPUBus {
    CPUBus() = default;
    explicit CPUBus(Mapper* mpr) noexcept : mapper(mpr) {}

    byte read(word address) const noexcept;
    void write(word address, byte value) noexcept;

    void set_mapper(Mapper* mpr) noexcept;
    void set_read_callback(IORegisters reg, std::function<byte(void)>&& callback);
    void set_write_callback(IORegisters reg, std::function<void(byte)>&& callback);
    const byte* page_ptr(byte page) const noexcept;

private:
    std::array<byte, 0x0800> ram = {};
    Mapper* mapper = nullptr;

    std::unordered_map<IORegisters, std::function<byte(void)>> read_callbacks;
    std::unordered_map<IORegisters, std::function<void(byte)>> write_callbacks;
};

} // modnes

#endif //MODNES_CPU_BUS_HPP
