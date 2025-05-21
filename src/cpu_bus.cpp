#include "cpu_bus.hpp"

namespace nes {

byte CPUBus::read(word address) const noexcept {
    switch (address) {
        case 0x0000 ... 0x1fff: {
            return ram[address & 0x7ff];
        }
        case 0x2000 ... 0x2007: {
            if (read_callbacks.contains(address)) {
                return read_callbacks.at(address)();
            }
            return 0;
        }
        case 0x4000 ... 0x4017: {
            if (read_callbacks.contains(address)) {
                return read_callbacks.at(address)();
            }
            return 0;
        }
        case 0x6000 ... 0x7fff: {
            // Extended RAM not supported yet
            return 0;
        }
        case 0x8000 ... 0xffff: {
            return mapper->read_prg(address);
        }
        default: {
            return 0;
        }
    }
}

void CPUBus::write(word address, byte value) noexcept {
    switch (address) {
        case 0x0000 ... 0x1fff: {
            ram[address & 0x7ff] = value;
            return;
        }
        case 0x2000 ... 0x2007: {
            if (write_callbacks.contains(address)) {
                write_callbacks.at(address)(value);
            }
            return;
        }
        case 0x4000 ... 0x4017: {
            if (write_callbacks.contains(address)) {
                write_callbacks.at(address)(value);
            }
            return;
        }
        case 0x6000 ... 0x7fff: {
            return;
        }
        case 0x8000 ... 0xffff: {
            mapper->write_prg(address, value);
        }
        default: {
            return;
        }
    }
}

const byte* CPUBus::get_page_ptr(byte page) const noexcept {
    if (const word address = page << 8; address <= 0x1fff) {
        return &ram[address & 0x7ff];
    }
    return nullptr;
}

void CPUBus::set_read_callbacks(std::unordered_map<word, std::function<byte(void)>>&& callbacks) {
    read_callbacks = std::move(callbacks);
}

void CPUBus::set_write_callbacks(std::unordered_map<word, std::function<void(byte)>>&& callbacks) {
    write_callbacks = std::move(callbacks);
}

} // nes