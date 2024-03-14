#include <memory>

#include "mappers/mapper.hpp"
#include "mappers/mapper_nrom.hpp"

namespace modnes {

std::unique_ptr<Mapper> Mapper::create(Cartridge& cartridge) noexcept {
    switch (static_cast<MapperType>(cartridge.get_mapper())) {
        case MapperType::NROM:
            return std::make_unique<MapperNROM>(cartridge);
        default:
            return { nullptr };
    }
}

} // modnes