#ifndef MODNES_MAPPER_NROM_HPP
#define MODNES_MAPPER_NROM_HPP

#include "mappers/mapper.hpp"

namespace nes {

struct MapperNROM final : public Mapper {
    MapperNROM() = delete;
    explicit MapperNROM(struct Cartridge& cartridge) noexcept;

    [[nodiscard]] byte read_prg(word address) const noexcept override;
    [[nodiscard]] byte read_chr(word address) const noexcept override;

    void write_prg(word address, byte value) noexcept override;
    void write_chr(word address, byte value) noexcept override;
};

} // nes

#endif //MODNES_MAPPER_NROM_HPP
