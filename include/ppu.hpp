#ifndef PPU_HPP
#define PPU_HPP

#include <functional>

#include "definitions.hpp"
#include "ppu_bus.hpp"

namespace nes {

static constexpr auto SCANLINE_CYCLE_LENGTH = 341;
static constexpr auto SCANLINE_CYCLE_END    = 340;
static constexpr auto SCANLINE_FRAME_END    = 261;
static constexpr auto SCANLINE_VISIBLE_DOTS = 256;
static constexpr auto VISIBLE_SCANLINES     = 240;

static constexpr auto SCREEN_SIZE = SCANLINE_VISIBLE_DOTS * VISIBLE_SCANLINES;
static constexpr auto OAM_SIZE         = 0x100;
static constexpr auto OAM_CACHE_SIZE   = 0x08;
static constexpr auto ATTRIBUTE_OFFSET = 0x3C0;


struct PPU {
    explicit PPU(PPUBus& bus) noexcept : bus(bus) {}

    void reset() noexcept;
    void step() noexcept;

    void set_vblank_callback(std::function<void()>&& callback);
    void handle_dma(const byte* page_ptr) noexcept;

    byte get_status() noexcept;
    byte get_data() noexcept;
    byte get_oam_data() const noexcept;

    void set_control(byte value) noexcept;
    void set_scroll(byte value) noexcept;
    void set_mask(byte value) noexcept;
    void set_oam_data(byte value) noexcept;

    void set_oam_address(byte value) noexcept;
    void set_data_address(byte value) noexcept;
    void set_data(byte value) noexcept;

private:

    [[nodiscard]] constexpr byte get_vram_increment() const noexcept {
        return ctrl.increment_mode ? 0x20 : 0x01;
    }

    [[nodiscard]] constexpr bool rendering_enabled() const noexcept {
        return mask.show_background || mask.show_sprites;
    }

    union ppu_ctrl_t {
        struct {
            byte nametable_x        : 1;
            byte nametable_y        : 1;
            byte increment_mode     : 1;
            byte pattern_sprite     : 1;
            byte pattern_background : 1;
            byte sprite_size        : 1;
            byte slave_mode         : 1;
            byte generate_nmi       : 1;
        };
        byte value = {};
    };

    union ppu_mask_t {
        struct {
            byte grey_scale      : 1;
            byte hide_background : 1;
            byte hide_sprites    : 1;
            byte show_background : 1;
            byte show_sprites    : 1;
            byte emphasize_red   : 1;
            byte emphasize_green : 1;
            byte emphasize_blue  : 1;
        };
        byte value = {};
    };

    union ppu_stat_t {
        struct {
            byte unused          : 5;
            byte sprite_overflow : 1;
            byte sprite_zero     : 1;
            byte vertical_blank  : 1;
        };
        byte value = {};
    };

    union ppu_register_t {
        struct {
            word coarse_x    : 5;
            word coarse_y    : 5;
            word nametable_x : 1;
            word nametable_y : 1;
            word fine_y      : 3;
            word unused      : 1;
        };
        struct {
            word lo_byte     : 8;
            word hi_byte     : 8;
        };
        word address = {};
    };

    struct sprite_t {
        byte y;
        byte id;
        byte attr;
        byte x;
    };

    static constexpr std::array<usize, 0x40> PALETTE = {
            0xff545454, 0xff001e74, 0xff081090, 0xff300088, 0xff440064, 0xff5c0030, 0xff540400, 0xff3c1800,
            0xff202a00, 0xff083a00, 0xff004000, 0xff003c00, 0xff00323c, 0xff000000, 0xff000000, 0xff000000,
            0xff989698, 0xff084cc4, 0xff3032ec, 0xff5c1ee4, 0xff8814b0, 0xffa01464, 0xff982220, 0xff783c00,
            0xff545a00, 0xff287200, 0xff087c00, 0xff007628, 0xff006678, 0xff000000, 0xff000000, 0xff000000,
            0xffeceeec, 0xff4c9aec, 0xff787cec, 0xffb062ec, 0xffe454ec, 0xffec58b4, 0xffec6a64, 0xffd48820,
            0xffa0aa00, 0xff74c400, 0xff4cd020, 0xff38cc6c, 0xff38b4cc, 0xff3c3c3c, 0xff000000, 0xff000000,
            0xffeceeec, 0xffa8ccec, 0xffbcbcec, 0xffd4b2ec, 0xffecaeec, 0xffecaed4, 0xffecb4b0, 0xffe4c490,
            0xffccd278, 0xffb4de78, 0xffa8e290, 0xff98e2b4, 0xffa0d6e4, 0xffa0a2a0, 0xff000000, 0xff000000,
    };

    std::array<byte, 0x100> oam       = {};
    std::array<byte, 0x008> oam_cache = {};
    byte oam_address      = {};
    byte oam_cache_length = {};

    ppu_register_t vram;
    ppu_register_t temp;

    ppu_ctrl_t ctrl;
    ppu_mask_t mask;
    ppu_stat_t stat;

    byte data_buffer = {};
    byte x_fine      = {};
    bool first_write   = {};
    byte address_latch = {};

    usize cycles   = {};
    usize scanline = {};

    std::function<void()> vblank_callback = [] {};

    PPUBus& bus;
};

} // namespace nes

#endif //PPU_HPP
