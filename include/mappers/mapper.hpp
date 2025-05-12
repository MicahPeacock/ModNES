#ifndef MODNES_MAPPER_HPP
#define MODNES_MAPPER_HPP

#include <memory>

#include "cartridge.hpp"

namespace nes {

enum class MapperType : byte {
    NROM    = 0,
    SXROM   = 1,
    UXROM   = 2,
    CNROM   = 3,
    MMC3    = 4,
    Unknown = 0xff,
};

enum class MirrorType : byte {
    HORIZONTAL  = 0,
    VERTICAL    = 1,
    FOUR_SCREEN = 8,
};

struct Mapper {
    Mapper() = delete;
    explicit Mapper(Cartridge& cartridge) noexcept : cartridge(cartridge) {}
    virtual ~Mapper() = default;

    [[nodiscard]] virtual byte read_prg(word address) const noexcept = 0;
    [[nodiscard]] virtual byte read_chr(word address) const noexcept = 0;

    virtual void write_prg(word address, byte value) noexcept = 0;
    virtual void write_chr(word address, byte value) noexcept = 0;

    [[nodiscard]] constexpr MirrorType get_mirroring() const noexcept { return static_cast<MirrorType>(cartridge.get_mirroring()); }

    virtual void scanline_irq() noexcept {}

protected:
    Cartridge& cartridge;
};

std::unique_ptr<Mapper> make_mapper(Cartridge& cartridge) noexcept;

} // nes

#endif //MODNES_MAPPER_HPP
