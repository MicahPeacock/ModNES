#ifndef MODNES_MAPPER_HPP
#define MODNES_MAPPER_HPP

#include <memory>

#include "cartridge.hpp"
#include "definitions.hpp"

namespace modnes {

enum class mapper_type : byte {
    NROM    = 0,
    Unknown = 0xff,
};

struct Mapper {
    Mapper() = delete;

    explicit Mapper(Cartridge& cartridge) noexcept : cartridge(cartridge) {}
    virtual ~Mapper() = default;

    [[nodiscard]] virtual byte read_prg(word address) const noexcept = 0;
    [[nodiscard]] virtual byte read_chr(word address) const noexcept = 0;

    virtual void write_prg(word address, byte value) noexcept = 0;
    virtual void write_chr(word address, byte value) noexcept = 0;

    [[nodiscard]] constexpr byte get_mirroring() const noexcept { return cartridge.get_mirroring(); }
    [[nodiscard]] constexpr bool has_extended_ram() const noexcept { return cartridge.has_extended_ram(); }

    constexpr virtual void scanline_irq() noexcept {}

    static std::unique_ptr<Mapper> create(Cartridge& cart) noexcept;

protected:
    Cartridge& cartridge;
};

}

#endif //MODNES_MAPPER_HPP
