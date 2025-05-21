#include "emulator.hpp"

namespace nes {

Emulator::Emulator(Cartridge& cartridge) : cartridge(cartridge), mapper(make_mapper(cartridge)), cpu_bus(mapper.get()), ppu_bus(mapper.get()), cpu(cpu_bus), ppu(ppu_bus) {
    cpu_bus.set_read_callbacks({
        { PPUSTAT, [&]() -> byte { return ppu.get_status();   } },
        { PPUDATA, [&]() -> byte { return ppu.get_data();     } },
        { JOYPAD1, [&]() -> byte { return 0; } },
        { JOYPAD2, [&]() -> byte { return 0; } },
        { OAMDATA, [&]() -> byte { return ppu.get_oam_data(); } },
    });
    cpu_bus.set_write_callbacks({
        { PPUCTRL, [&](byte val) { ppu.set_control(val);      } },
        { PPUMASK, [&](byte val) { ppu.set_mask(val);         } },
        { OAMADDR, [&](byte val) { ppu.set_oam_address(val);  } },
        { PPUADDR, [&](byte val) { ppu.set_data_address(val); } },
        { PPUSCRL, [&](byte val) { ppu.set_scroll(val);       } },
        { PPUDATA, [&](byte val) { ppu.set_data(val);         } },
        { OAMDMA,  [&](byte val) { handle_dma(val);           } },
        { JOYPAD1, [&](byte val) {} },
        { JOYPAD2, [&](byte val) {} },
        { OAMDATA, [&](byte val) { ppu.set_oam_data(val);     } },
    });

    ppu.set_vblank_callback([&]() {
        cpu.interrupt(InterruptType::NMI);
    });
}

void Emulator::handle_dma(byte page) {
    cpu.skip_dma_cycles();
    if (const auto* page_ptr = cpu_bus.get_page_ptr(page)) {
        ppu.handle_dma(page_ptr);
    } else {

    }
}

}
