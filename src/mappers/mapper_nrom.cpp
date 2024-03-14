#include "mappers/mapper_nrom.hpp"

namespace modnes {

MapperNROM::MapperNROM(struct Cartridge& cart) noexcept : Mapper(cart) {}

byte MapperNROM::read_prg(word address) const noexcept {
    const word mapped_address = address & 0x3fff;
    return cartridge.read_prg(mapped_address);
}

byte MapperNROM::read_chr(word address) const noexcept {
    return cartridge.read_chr(address);
}

void MapperNROM::write_prg(word address, byte value) noexcept {
    // Do nothing
}

void MapperNROM::write_chr(word address, byte value) noexcept {

}

} // modnes