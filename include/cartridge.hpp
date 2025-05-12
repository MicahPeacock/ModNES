#ifndef MODNES_CARTRIDGE_HPP
#define MODNES_CARTRIDGE_HPP

#include <string_view>
#include <vector>

#include "definitions.hpp"

namespace nes {

struct Cartridge {
    static constexpr usize HEADER_SIZE   = 0x10;
    static constexpr usize TRAINER_SIZE  = 0x200;
    static constexpr usize PRG_BANK_SIZE = 0x4000;
    static constexpr usize CHR_BANK_SIZE = 0x2000;

    Cartridge() = delete;
    explicit Cartridge(const byte* bytes);

    [[nodiscard]] byte read_prg(word address) const noexcept { return prg_rom[address]; }
    [[nodiscard]] byte read_chr(word address) const noexcept { return chr_rom[address]; }
    void write_prg(word address, byte value) noexcept { prg_rom[address] = value; }
    void write_chr(word address, byte value) noexcept { chr_rom[address] = value; }

    [[nodiscard]] constexpr byte get_prg_banks() const noexcept { return prg_banks; }
    [[nodiscard]] constexpr byte get_chr_banks() const noexcept { return chr_banks; }
    [[nodiscard]] constexpr byte get_mapper_id() const noexcept { return mapper_id; }
    [[nodiscard]] constexpr byte get_mirroring() const noexcept { return mirroring; }

    static Cartridge from_file(std::string_view path);

private:
    std::vector<byte> prg_rom;
    std::vector<byte> chr_rom;
    byte prg_banks = {};
    byte chr_banks = {};
    byte mapper_id = {};
    byte mirroring = {};
};

} // nes

#endif //MODNES_CARTRIDGE_HPP
