#include "cpu_bus.hpp"

namespace nes {

byte CPUBus::read(word address) const noexcept {
    switch (address) {
        case 0x0000 ... 0x1fff:
            return ram[address & 0x7ff];
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
        case 0x6000 ... 0x7fff:
            // Extended RAM not supported yet
            return 0;
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
            if (write_callbacks.contains(address)) {
                write_callbacks.at(address)(value);
            }
            break;
        }
        case 0x4000 ... 0x4017: {
            if (write_callbacks.contains(address)) {
                write_callbacks.at(address)(value);
            }
            break;
        }
        case 0x6000 ... 0x7fff:
            // Extended RAM not supported yet
            break;
        case 0x8000 ... 0xffff:
            mapper->write_prg(address, value);
            break;
        default:
            break;
    }
}

void CPUBus::set_read_callback(word address, std::function<byte(void)>&& callback) {
    read_callbacks[address] = std::move(callback);
}

void CPUBus::set_write_callback(word address, std::function<void(byte)>&& callback) {
    write_callbacks[address] = std::move(callback);
}


} // nes