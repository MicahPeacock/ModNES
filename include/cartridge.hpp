#ifndef MODNES_CARTRIDGE_HPP
#define MODNES_CARTRIDGE_HPP

#include <vector>
#include <fstream>
#include <filesystem>

#include "definitions.hpp"

namespace modnes {

namespace fs = std::filesystem;

struct INESHeader {
    byte nes_id[4];
    byte prg_banks;
    byte chr_banks;
    byte flags6;
    byte flags7;
    byte prg_ram_size;
    byte flags9;
    byte flags10;
    byte unused[5];
};


struct Cartridge {
    using rom_type = std::vector<byte>;

    Cartridge() = delete;
    explicit Cartridge(const fs::path& rom_path);

    [[nodiscard]] byte read_prg(word address) const noexcept { return prg_rom[address]; }
    [[nodiscard]] byte read_chr(word address) const noexcept { return chr_rom[address]; }

    void write_prg(word address, byte value) noexcept { prg_rom[address] = value; }
    void write_chr(word address, byte value) noexcept { chr_rom[address] = value; }

    [[nodiscard]] constexpr byte get_mapper() const noexcept { return mapper; }
    [[nodiscard]] constexpr byte get_mirroring() const noexcept { return mirror; }

    [[nodiscard]] constexpr bool has_extended_ram() const noexcept { return extended_ram; }

private:
    rom_type prg_rom;
    rom_type chr_rom;

    byte mapper = {};
    byte mirror = {};
    bool extended_ram = {};
};

}
#endif //MODNES_CARTRIDGE_HPP
