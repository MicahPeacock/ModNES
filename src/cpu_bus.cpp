#include "cpu_bus.hpp"

namespace modnes {

byte CPUBus::read(word address) const noexcept {
    switch (address) {
        case 0x0000 ... 0x1fff:
            return ram[address & 0x7ff];
        case 0x2000 ... 0x2007: {
            const auto itr = read_callbacks.find(static_cast<IORegisters>(address));
            return itr != read_callbacks.end() ? (itr->second)() : 0;
        }
        case 0x4000 ... 0x4017: {
            const auto itr = read_callbacks.find(static_cast<IORegisters>(address));
            return itr != read_callbacks.end() ? (itr->second)() : 0;
        }
        case 0x6000 ... 0x7fff:
            return mapper->has_extended_ram() ? extended_ram[address - 0x6000] : 0;
        case 0x8000 ... 0xffff:
            return mapper->read_prg(address);
        default:
            return 0;
    }
}

void CPUBus::write(word address, byte value) noexcept {
    switch (address) {
        case 0x0000 ... 0x1fff:
            ram[address & 0x7ff] = value;
            break;
        case 0x2000 ... 0x2007: {
            const auto itr = write_callbacks.find(static_cast<IORegisters>(address));
            if (itr != write_callbacks.end())
                (itr->second)(value);
            break;
        }
        case 0x4000 ... 0x4017: {
            const auto itr = write_callbacks.find(static_cast<IORegisters>(address));
            if (itr != write_callbacks.end())
                (itr->second)(value);
            break;
        }
        case 0x6000 ... 0x7fff: {
            if (mapper->has_extended_ram())
                extended_ram[address - 0x6000] = value;
            break;
        }
        case 0x8000 ... 0xffff:
            mapper->write_prg(address, value);
            break;
        default:
            break;
    }
}

bool CPUBus::set_mapper(Mapper* mpr) noexcept {
    if (!mpr) return false;
    mapper = mpr;
    return true;
}

bool CPUBus::set_read_callback(IORegisters reg, std::function<byte(void)>&& callback) {
    return read_callbacks.emplace(reg, std::move(callback)).second;
}

bool CPUBus::set_write_callback(IORegisters reg, std::function<void(byte)>&& callback) {
    return write_callbacks.emplace(reg, std::move(callback)).second;
}

const byte* CPUBus::page_ptr(byte page) const noexcept {
    const word address = page << 8;
    switch (address) {
        case 0x0000 ... 0x1fff:
            return &ram[address & 0x7ff];
        case 0x6000 ... 0x7fff:
            return mapper->has_extended_ram() ? &extended_ram[address - 0x6000] : nullptr;
        default:
            return nullptr;
    }
}

} // modnes