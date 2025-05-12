#include "mappers/mapper.hpp"
#include "mappers/mapper_nrom.hpp"

namespace nes {

std::unique_ptr<Mapper> make_mapper(Cartridge& cartridge) noexcept {
    switch (static_cast<MapperType>(cartridge.get_mapper_id())) {
        case MapperType::NROM:
            return std::make_unique<MapperNROM>(cartridge);
        default:
            return { nullptr };
    }
}

} // nes