#include <memory>

#include "mappers/mapper.hpp"
#include "mappers/mapper_nrom.hpp"

namespace modnes {

std::unique_ptr<Mapper> Mapper::create(Cartridge& cartridge) noexcept {
    switch (static_cast<mapper_type>(cartridge.get_mapper())) {
        case mapper_type::NROM:
            return std::make_unique<MapperNROM>(cartridge);
        default:
            break;
    }
    return { nullptr };
}

}