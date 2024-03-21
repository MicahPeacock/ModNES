#ifndef MODNES_CARTRIDGE_HPP
#define MODNES_CARTRIDGE_HPP

#include <vector>
#include <fstream>
#include <filesystem>

#include "definitions.hpp"

namespace modnes {

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

namespace fs = std::filesystem;

struct Cartridge {
    Cartridge() = delete;
    explicit Cartridge(const fs::path& rom_path);

    [[nodiscard]] byte read_prg(word address) const noexcept { return prg_rom[address]; }
    [[nodiscard]] byte read_chr(word address) const noexcept { return chr_rom[address]; }

    void write_prg(word address, byte value) noexcept { prg_rom[address] = value; }
    void write_chr(word address, byte value) noexcept { chr_rom[address] = value; }

    [[nodiscard]] byte get_prg_banks() const noexcept { return static_cast<byte>(prg_rom.size() / 0x4000); }
    [[nodiscard]] byte get_chr_banks() const noexcept { return static_cast<byte>(chr_rom.size() / 0x2000); }

    [[nodiscard]] constexpr byte get_mapper() const noexcept { return mapper; }
    [[nodiscard]] constexpr byte get_mirroring() const noexcept { return mirror; }

private:
    std::vector<byte> prg_rom;
    std::vector<byte> chr_rom;
    byte mapper = {};
    byte mirror = {};
};

} // modnes

#endif //MODNES_CARTRIDGE_HPP
