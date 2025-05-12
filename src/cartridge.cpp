#include <fstream>
#include "cartridge.hpp"

namespace nes {

Cartridge::Cartridge(const byte* bytes) {
    if (std::string_view { reinterpret_cast<const char*>(bytes), 4 } != "NES\x1A") {
        throw std::runtime_error("File is not in iNES file format");
    }
    if (!bytes[4]) {
        throw std::runtime_error("No PRG ROM banks read from file.");
    }
    if (bytes[10] & 0x02 || bytes[10] & 0x1) {
        throw std::runtime_error("PAL ROM is not supported.");
    }

    const auto get_mapper_id = [](const byte* data) -> byte {
        const byte lo = data[6] & 0xf0;
        const byte hi = data[7] & 0xf0;
        return lo | hi << 4;
    };
    const auto get_mirroring = [](const byte* data) -> byte {
        return data[6] & 0x08 ?: data[6] & 0x01;
    };
    prg_banks = bytes[4];
    chr_banks = bytes[5];
    mapper_id = get_mapper_id(bytes);
    mirroring = get_mirroring(bytes);

    const usize skip_trainer = (bytes[6] & 0x04) != 0 ? TRAINER_SIZE : 0;
    const usize prg_rom_size = static_cast<usize>(prg_banks) * PRG_BANK_SIZE;
    const usize chr_rom_size = static_cast<usize>(chr_banks) * CHR_BANK_SIZE;

    const byte* prg_rom_start = bytes + HEADER_SIZE + skip_trainer;
    const byte* chr_rom_start = prg_rom_start + prg_rom_size;
    prg_rom = { prg_rom_start, prg_rom_start + prg_rom_size };
    chr_rom = { chr_rom_start, chr_rom_start + chr_rom_size };
}

Cartridge Cartridge::from_file(const std::string_view path) {
    const std::size_t extension_start = path.find_last_of('.');
    const std::string_view extension = path.substr(extension_start);
    if(extension != ".nes") {
        throw std::runtime_error("Invalid file extension. File must have the extension '.nes'");
    }
    std::ifstream file(path.data(), std::ios::in | std::ios::binary);
    std::vector<byte> rom_data = { std::istreambuf_iterator(file), std::istreambuf_iterator<char>() };
    return Cartridge(rom_data.data());
}

} // nes