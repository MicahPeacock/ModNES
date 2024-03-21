#ifndef MODNES_CPU_OPCODES_HPP
#define MODNES_CPU_OPCODES_HPP

#include "definitions.hpp"

namespace modnes {

static constexpr auto NMI_VECTOR   = 0xfffa;
static constexpr auto RESET_VECTOR = 0xfffc;
static constexpr auto IRQ_VECTOR   = 0xfffe;

enum class Opcode {
    lda_imm = 0xa9,
    lda_zpg = 0xa5,
    lda_zpx = 0xb5,
    lda_abs = 0xad,
    lda_abx = 0xbd,
    lda_aby = 0xb9,
    lda_idx = 0xa1,
    lda_idy = 0xb1,

    ldx_imm = 0xa2,
    ldx_zpg = 0xa6,
    ldx_zpx = 0xb6,
    ldx_abs = 0xae,
    ldx_aby = 0xbe,

    ldy_imm = 0xa0,
    ldy_zpg = 0xa4,
    ldy_zpx = 0xb4,
    ldy_abs = 0xac,
    ldy_abx = 0xbc,

    sta_zpg = 0x85,
    sta_zpx = 0x95,
    sta_abs = 0x8d,
    sta_abx = 0x9d,
    sta_aby = 0x99,
    sta_idx = 0x81,
    sta_idy = 0x91,

    stx_zpg = 0x86,
    stx_zpy = 0x96,
    stx_abs = 0x8e,

    sty_zpg = 0x84,
    sty_zpx = 0x94,
    sty_abs = 0x8c,

    tsx     = 0xba,
    txs     = 0x9a,
    pha     = 0x48,
    pla     = 0x68,
    php     = 0x08,
    plp     = 0x28,

    jmp     = 0x4c,
    jmp_ind = 0x6c,
    jsr     = 0x20,
    rts     = 0x60,

    and_imm = 0x29,
    and_zpg = 0x25,
    and_zpx = 0x35,
    and_abs = 0x2d,
    and_abx = 0x3d,
    and_aby = 0x39,
    and_idx = 0x21,
    and_idy = 0x31,

    ora_imm = 0x09,
    ora_zpg = 0x05,
    ora_zpx = 0x15,
    ora_abs = 0x0d,
    ora_abx = 0x1d,
    ora_aby = 0x19,
    ora_idx = 0x01,
    ora_idy = 0x11,

    eor_imm = 0x49,
    eor_zpg = 0x45,
    eor_zpx = 0x55,
    eor_abs = 0x4d,
    eor_abx = 0x5d,
    eor_aby = 0x59,
    eor_idx = 0x41,
    eor_idy = 0x51,

    bit_zpg = 0x24,
    bit_abs = 0x2c,

    tax     = 0xaa,
    tay     = 0xa8,
    txa     = 0x8a,
    tya     = 0x98,

    inx     = 0xe8,
    iny     = 0xc8,
    dey     = 0x88,
    dex     = 0xca,
    dec_zpg = 0xc6,
    dec_zpx = 0xd6,
    dec_abs = 0xce,
    dec_abx = 0xde,
    inc_zpg = 0xe6,
    inc_zpx = 0xf6,
    inc_abs = 0xee,
    inc_abx = 0xfe,

    beq     = 0xf0,
    bne     = 0xd0,
    bcs     = 0xb0,
    bcc     = 0x90,
    bmi     = 0x30,
    bpl     = 0x10,
    bvc     = 0x50,
    bvs     = 0x70,

    clc     = 0x18,
    sec     = 0x38,
    cld     = 0xd8,
    sed     = 0xf8,
    cli     = 0x58,
    sei     = 0x78,
    clv     = 0xb8,

    adc     = 0x69,
    adc_zpg = 0x65,
    adc_zpx = 0x75,
    adc_abs = 0x6d,
    adc_abx = 0x7d,
    adc_aby = 0x79,
    adc_idx = 0x61,
    adc_idy = 0x71,

    sbc     = 0xe9,
    sbc_abs = 0xed,
    sbc_zpg = 0xe5,
    sbc_zpx = 0xf5,
    sbc_abx = 0xfd,
    sbc_aby = 0xf9,
    sbc_idx = 0xe1,
    sbc_idy = 0xf1,

    cmp     = 0xc9,
    cmp_zpg = 0xc5,
    cmp_zpx = 0xd5,
    cmp_abs = 0xcd,
    cmp_abx = 0xdd,
    cmp_aby = 0xd9,
    cmp_idx = 0xc1,
    cmp_idy = 0xd1,

    cpx     = 0xe0,
    cpy     = 0xc0,
    cpx_zpg = 0xe4,
    cpy_zpg = 0xc4,
    cpx_abs = 0xec,
    cpy_abs = 0xcc,

    asl     = 0x0a,
    asl_zpg = 0x06,
    asl_zpx = 0x16,
    asl_abs = 0x0e,
    asl_abx = 0x1e,

    lsr     = 0x4a,
    lsr_zpg = 0x46,
    lsr_zpx = 0x56,
    lsr_abs = 0x4e,
    lsr_abx = 0x5e,

    rol     = 0x2a,
    rol_zpg = 0x26,
    rol_zpx = 0x36,
    rol_abs = 0x2e,
    rol_abx = 0x3e,

    ror     = 0x6a,
    ror_zpg = 0x66,
    ror_zpx = 0x76,
    ror_abs = 0x6e,
    ror_abx = 0x7e,

    nop     = 0xea,
    brk     = 0x00,
    rti     = 0x40,
};

constexpr auto operator+(Opcode op) noexcept {
    return static_cast<std::underlying_type_t<Opcode>>(op);
}

enum class InterruptType : byte {
    irq,
    nmi
};

// 0 implies unused Opcode
static constexpr std::array<byte, 0x100> OPERATION_CYCLES = {
        7, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 0, 4, 6, 0,
        2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
        6, 6, 0, 0, 3, 3, 5, 0, 4, 2, 2, 0, 4, 4, 6, 0,
        2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
        6, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 3, 4, 6, 0,
        2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
        6, 6, 0, 0, 0, 3, 5, 0, 4, 2, 2, 0, 5, 4, 6, 0,
        2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
        0, 6, 0, 0, 3, 3, 3, 0, 2, 0, 2, 0, 4, 4, 4, 0,
        2, 6, 0, 0, 4, 4, 4, 0, 2, 5, 2, 0, 0, 5, 0, 0,
        2, 6, 2, 0, 3, 3, 3, 0, 2, 2, 2, 0, 4, 4, 4, 0,
        2, 5, 0, 0, 4, 4, 4, 0, 2, 4, 2, 0, 4, 4, 4, 0,
        2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0,
        2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
        2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 2, 4, 4, 6, 0,
        2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
};

} // modnes

#endif //MODNES_CPU_OPCODES_HPP
