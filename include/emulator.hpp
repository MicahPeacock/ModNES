#ifndef MODNES_EMULATOR_HPP
#define MODNES_EMULATOR_HPP

#include "cpu.hpp"
#include "ppu.hpp"
#include "cpu_bus.hpp"
#include "ppu_bus.hpp"

namespace nes {

struct Emulator {
    explicit Emulator(Cartridge& cartridge);

    void handle_dma(byte page);

private:
    Cartridge cartridge;
    std::unique_ptr<Mapper> mapper;
    CPUBus cpu_bus;
    PPUBus ppu_bus;
    CPU cpu;
    PPU ppu;
};

} // namespace nes

#endif //MODNES_EMULATOR_HPP
