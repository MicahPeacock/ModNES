#include "cartridge.hpp"

namespace modnes {

Cartridge::Cartridge(const fs::path& rom_path) {
    const auto read_rom_data = [](auto& file, auto size) {
        Cartridge::rom_type data(size, 0);
        if (!file.read(reinterpret_cast<char*>(&data[0]), size)) {
            throw std::runtime_error("Error reading ROM data from file.");
        }
        return data;
    };

    std::ifstream file { rom_path, std::ios_base::binary | std::ios_base::in };
    if (!file) {
        throw std::runtime_error("Error opening NES file.");
    }

    INESHeader header = {};
    if (!file.read(reinterpret_cast<char*>(&header), sizeof(INESHeader))) {
        throw std::runtime_error("Error reading iNES header from file.");
    }
    if (!header.prg_banks) {
        throw std::runtime_error("No PRG ROM banks read from file.");
    }
    if (header.flags6 & 0x4) {
        throw std::runtime_error("Trainer is not supported.");
    }
    if ((header.flags10 & 0x3) == 0x2 || (header.flags10 & 0x1)) {
        throw std::runtime_error("PAL ROM is not supported.");
    }
    mirror       = (header.flags6 & 0x08) ?: (header.flags6 & 0x01);
    mapper       = (header.flags7 & 0xf0) | ((header.flags6 & 0xf0) >> 4);
    extended_ram = (header.flags6 & 0x02) > 0;

    prg_rom = read_rom_data(file, header.prg_banks * 0x4000);
    chr_rom = read_rom_data(file, header.chr_banks * 0x2000);
}

}