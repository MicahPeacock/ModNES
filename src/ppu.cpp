#include "ppu.hpp"

#include <cstring>

namespace nes {

void PPU::reset() noexcept {
    vram.address = temp.address = 0x0000;


}

void PPU::step() noexcept {
    const auto increment_scroll_x = [&]() {
        if (!rendering_enabled()) return;
        if (vram.coarse_x == 0x1f) {
            vram.coarse_x = 0;
            vram.nametable_x = ~vram.nametable_x;
        } else {
            vram.coarse_x++;
        }
    };
    const auto increment_scroll_y = [&]() {
        if (!rendering_enabled()) return;
        if (vram.fine_y < 7) {
            vram.fine_y++;
        } else {
            vram.fine_y = 0;
            if (vram.coarse_y == 0x1d) {
                vram.coarse_y = 0;
                vram.nametable_y = ~vram.nametable_y;
            } else if (vram.coarse_y == 0x1f) {
                vram.coarse_y = 0;
            } else {
                vram.coarse_y++;
            }
        }
    };
    const auto transfer_address_x = [&]() {
        if (!rendering_enabled()) return;
        vram.nametable_x = temp.nametable_x;
        vram.coarse_x    = temp.coarse_x;
    };
    const auto transfer_address_y = [&]() {
        if (!rendering_enabled()) return;
        vram.nametable_y = temp.nametable_y;
        vram.coarse_y    = temp.coarse_y;
        vram.fine_y      = temp.fine_y;
    };
}

void PPU::handle_dma(const byte* page_ptr) noexcept {
    std::memcpy(oam.data() + oam_address, page_ptr, 256 - oam_address);
    if (oam_address) {
        std::memcpy(oam.data(), page_ptr + (256 - oam_address), oam_address);
    }
}

void PPU::set_vblank_callback(std::function<void()>&& callback) {
    vblank_callback = std::move(callback);
}

byte PPU::get_status() noexcept {
    const auto status = stat.value;
    stat.vertical_blank = false;
    first_write = true;
    address_latch = 0;
    return status;
}

byte PPU::get_data() noexcept {
    byte data = bus.read(vram.address);
    if (vram.address >= 0x3f00) {
        std::swap(data, data_buffer);
    }
    vram.address += get_vram_increment();
    return data;
}

void PPU::set_control(byte value) noexcept {
    ctrl.value = value;
    temp.nametable_x = ctrl.nametable_x;
    temp.nametable_y = ctrl.nametable_y;
}

void PPU::set_scroll(byte value) noexcept {
    if (address_latch == 0) {
        x_fine = value & 0x07;
        temp.coarse_x = value >> 3;
        address_latch = 1;
    } else {
        temp.fine_y = value & 0x07;
        temp.coarse_y = value >> 3;
        address_latch = 0;
    }
}

void PPU::set_mask(byte value) noexcept {
    mask.value = value;
}

void PPU::set_oam_address(byte value) noexcept {
    oam_address = value;
}

void PPU::set_data_address(byte value) noexcept {
    if (address_latch == 0) {
        address_latch = 1;
    } else {
        vram.address = temp.address;
        address_latch = 0;
    }
}

void PPU::set_data(byte value) noexcept {
    bus.write(vram.address, value);
    vram.address += get_vram_increment();
}

byte PPU::get_oam_data() const noexcept {
    return oam[oam_address];
}

void PPU::set_oam_data(byte value) noexcept {
    oam[oam_address++] = value;
}

}
