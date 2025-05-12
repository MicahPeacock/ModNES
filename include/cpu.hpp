#ifndef MODNES_CPU_HPP
#define MODNES_CPU_HPP

#include <array>

#include "cpu_opcodes.hpp"
#include "cpu_bus.hpp"

namespace nes {

struct CPU {
    explicit CPU(CPUBus& bus) noexcept : bus(bus) {}

    void reset() noexcept;
    void reset(word reset_vector) noexcept;
    void step() noexcept;
    void interrupt(InterruptType type) noexcept;
    void skip_dma_cycles() noexcept;

private:
    byte execute(Instruction instruction) noexcept;
    void run_interrupt(InterruptType type) noexcept;

    byte fetch_byte() noexcept;
    word fetch_word() noexcept;
    byte read_byte(word address);
    word read_word(word address);
    void write_byte(word address, byte value);
    void write_word(word address, word value);

    void push_byte(byte value) noexcept;
    void push_word(word value) noexcept;
    byte pull_byte() noexcept;
    word pull_word() noexcept;

    word immediate() noexcept;
    word zero_page() noexcept;
    word zero_page_x() noexcept;
    word zero_page_y() noexcept;
    word absolute() noexcept;
    word absolute_x(bool always_cycle = false) noexcept;
    word absolute_y(bool always_cycle = false) noexcept;
    word indirect() noexcept;
    word indirect_x() noexcept;
    word indirect_y(bool always_cycle = false) noexcept;

    void load_register(word address, byte& reg) noexcept;
    void and_op(word address) noexcept;
    void ora_op(word address) noexcept;
    void eor_op(word address) noexcept;
    void branch_if(byte test, byte expected) noexcept;
    void compare(byte operand, byte value) noexcept;
    void adc(byte operand) noexcept;
    void sbc(byte operand) noexcept;
    byte asl(byte operand) noexcept;
    byte lsr(byte operand) noexcept;
    byte rol(byte operand) noexcept;
    byte ror(byte operand) noexcept;
    void push_status() noexcept;
    void pull_status() noexcept;

    constexpr void set_zn(byte reg) noexcept;
    constexpr word sp_to_address() const noexcept;
    static constexpr bool page_crossed(word address1, word address2) noexcept;

    static constexpr auto NMI_VECTOR   = 0xfffa;
    static constexpr auto RESET_VECTOR = 0xfffc;
    static constexpr auto IRQ_VECTOR   = 0xfffe;

    static constexpr byte
            NEGATIVE_BIT  = 1 << 7,
            OVERFLOW_BIT  = 1 << 6,
            UNUSED_BIT    = 1 << 5,
            BREAK_BIT     = 1 << 4,
            DECIMAL_BIT   = 1 << 3,
            INTERRUPT_BIT = 1 << 2,
            ZERO_BIT      = 1 << 1,
            CARRY_BIT     = 1 << 0;

    union status_t {
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
    byte a  = {}, // Accumulator
         x  = {}, // Register X
         y  = {}; // Register Y
    status_t status;

    // NES related members
    word cycles      = {};
    word skip_cycles = {};
    bool pending_nmi = {};
    bool pending_irq = {};

    CPUBus& bus;
};

} // nes

#endif //MODNES_CPU_HPP
