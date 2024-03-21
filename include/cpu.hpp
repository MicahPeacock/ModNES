#ifndef MODNES_CPU_HPP
#define MODNES_CPU_HPP

#include <array>

#include "cpu_bus.hpp"
#include "cpu_opcodes.hpp"

namespace modnes {

struct CPU {
    explicit CPU(CPUBus& bus) noexcept : bus(bus) {}

    void reset() noexcept;
    void reset(word reset_vector) noexcept;
    void step() noexcept;
    void interrupt(InterruptType type) noexcept;
    void skip_dma_cycles() noexcept;

private:
    bool execute(byte opcode) noexcept;
    void run_interrupt(InterruptType type) noexcept;

    byte fetch_byte() noexcept;
    word fetch_word() noexcept;
    byte read_byte(word address);
    word read_word(word address);
    void write_byte(word address, byte value);
    void write_word(word value, word address);

    void push_byte(byte value) noexcept;
    void push_word(word value) noexcept;
    byte pull_byte() noexcept;
    word pull_word() noexcept;

    word zero_page() noexcept;
    word zero_page_x() noexcept;
    word zero_page_y() noexcept;
    word absolute() noexcept;
    word absolute_x(bool always_cycle = false) noexcept;
    word absolute_y(bool always_cycle = false) noexcept;
    word indirect_x() noexcept;
    word indirect_y(bool always_cycle = false) noexcept;

    constexpr void set_zn(byte reg) noexcept;
    constexpr word sp_to_address() const noexcept;
    static constexpr bool page_crossed(word address1, word address2) noexcept;

    static constexpr byte
            NEGATIVE_BIT  = 0x80,
            OVERFLOW_BIT  = 0x40,
            UNUSED_BIT    = 0x20,
            BREAK_BIT     = 0x10,
            DECIMAL_BIT   = 0x08,
            INTERRUPT_BIT = 0x04,
            ZERO_BIT      = 0x02,
            CARRY_BIT     = 0x01;

    union StatusFlags {
        struct {
            byte c : 1;      // Carry flag
            byte z : 1;      // Zero flag
            byte i : 1;      // Interrupt Disable flag
            byte d : 1;      // Decimal flag
            byte b : 1;      // Break flag
            byte u : 1;      // Unused Constant (1)
            byte v : 1;      // Overflow flag
            byte n : 1;      // Negative flag
        };
        byte value = {};
    };

    word pc = {}; // Program Counter
    byte sp = {}; // Stack Pointer
    byte a  = {}, // Registers
         x  = {},
         y  = {};
    StatusFlags status;

    // NES related members
    word cycles      = {};
    word skip_cycles = {};
    bool pending_nmi = {};
    bool pending_irq = {};

    CPUBus& bus;
};

} // modnes

#endif //MODNES_CPU_HPP
