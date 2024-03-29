#include "cpu.hpp"

namespace modnes {

void CPU::reset() noexcept {
    reset(RESET_VECTOR);
}

void CPU::reset(word reset_vector) noexcept {
    pc = reset_vector;
    sp = 0xfd;
    a = x = y = 0;
    status.c = status.z = status.d = status.b = status.v = status.n = 0;
    status.i = true;
    cycles = skip_cycles = 0;
}

void CPU::step() noexcept {
    ++cycles;
    if (skip_cycles-- > 1)
        return;
    skip_cycles = 0;

    if(pending_nmi) {
        run_interrupt(InterruptType::nmi);
        pending_irq = pending_nmi = false;
        return;
    }
    if(pending_irq) {
        run_interrupt(InterruptType::irq);
        pending_irq = pending_nmi = false;
        return;
    }

    const auto opcode = fetch_byte();
    const auto cycle_length = modnes::OPERATION_CYCLES[opcode];
    if(cycle_length && execute(opcode)) {
        skip_cycles += cycle_length;
    } else {
        // unrecognized opcode
    }
}

void CPU::interrupt(InterruptType type) noexcept {
    switch (type) {
        case InterruptType::nmi:
            pending_nmi = true;
            break;
        case InterruptType::irq:
            pending_irq = true;
            break;
        default:
            break;
    }
}

void CPU::skip_dma_cycles() noexcept {
    skip_cycles += 513;
    skip_cycles += (cycles & 0x1);
}

bool CPU::execute(byte opcode) noexcept {
    const auto load_register = [&](word address, byte& reg) {
        reg = read_byte(address);
        set_zn(reg);
    };
    const auto and_op = [&](word address) {
        a &= read_byte(address);
        set_zn(a);
    };
    const auto ora_op = [&](word address) {
        a |= read_byte(address);
        set_zn(a);
    };
    const auto eor_op = [&](word address) {
        a ^= read_byte(address);
        set_zn(a);
    };
    const auto branch_if = [&](bool test, bool expected) {
        const auto offset = static_cast<sbyte>(fetch_byte());
        if (test == expected) {
            const word old_pc = pc;
            pc += offset;
            const bool page_changed = (pc & 0xff00) != (old_pc & 0xff00);
            if (page_changed) {
                ++skip_cycles;
            }
        }
    };
    const auto compare = [&](byte operand, byte value) {
        const byte temp = value - operand;
        status.n = (temp & NEGATIVE_BIT) != 0;
        status.z = value == operand;
        status.c = value >= operand;
    };
    const auto adc = [&](byte operand) {
        const bool same_signs = !((a ^ operand) & NEGATIVE_BIT);
        const word sum = a + operand + status.c;
        a = static_cast<byte>(sum & 0xff);
        set_zn(a);
        status.c = sum > 0xff;
        status.v = same_signs && ((a ^ operand) & NEGATIVE_BIT);
    };
    const auto sbc = [&](byte operand) {
        adc(~operand);
    };
    const auto asl = [&](byte operand) {
        status.c = (operand & NEGATIVE_BIT) != 0;
        const byte result = operand << 1;
        set_zn(result);
        return result;
    };
    const auto lsr = [&](byte operand) {
        status.c = (operand & CARRY_BIT) != 0;
        const byte result = operand >> 1;
        set_zn(result);
        return result;
    };
    const auto rol = [&](byte operand) {
        const byte new_first = status.c ? CARRY_BIT : 0;
        status.c = (operand & NEGATIVE_BIT) != 0;
        const byte result = (operand << 1) | new_first;
        set_zn(result);
        return result;
    };
    const auto ror = [&](byte operand) {
        const bool old_first = (operand & CARRY_BIT) != 0;
        operand >>= 1;
        if(status.c) {
            operand |= NEGATIVE_BIT;
        }
        status.c = old_first;
        set_zn(operand);
        return operand;
    };
    const auto push_status = [&]() {
        const byte stack_status = status.value | BREAK_BIT | UNUSED_BIT;
        push_byte(stack_status);
    };
    const auto pull_status = [&]() {
        status.value = pull_byte();
        status.b = false;
        status.u = false;
    };

    switch (static_cast<Opcode>(opcode)) {
        case Opcode::lda_imm: {
                a = fetch_byte();
                set_zn(a);
            }
            break;
        case Opcode::lda_zpg: {
                const word address = zero_page();
                load_register(address, a);
            }
            break;
        case Opcode::lda_zpx: {
                const word address = zero_page_x();
                load_register(address, a);
            }
            break;
        case Opcode::lda_abs: {
                const word address = absolute();
                load_register(address, a);
            }
            break;
        case Opcode::lda_abx: {
                const word address = absolute_x();
                load_register(address, a);
            }
            break;
        case Opcode::lda_aby: {
                const word address = absolute_y();
                load_register(address, a);
            }
            break;
        case Opcode::lda_idx: {
                const word address = indirect_x();
                load_register(address, a);
            }
            break;
        case Opcode::lda_idy: {
                const word address = indirect_y();
                load_register(address, a);
            }
            break;
        case Opcode::ldx_imm: {
                x = fetch_byte();
                set_zn(x);
            }
            break;
        case Opcode::ldx_zpg: {
                const word address = zero_page();
                load_register(address, x);
            }
            break;
        case Opcode::ldx_zpx: {
                const word address = zero_page_x();
                load_register(address, x);
            }
            break;
        case Opcode::ldx_abs: {
                const word address = absolute();
                load_register(address, x);
            }
            break;
        case Opcode::ldx_aby: {
                const word address = absolute_y();
                load_register(address, x);
            }
            break;
        case Opcode::ldy_imm: {
                y = fetch_byte();
                set_zn(y);
            }
            break;
        case Opcode::ldy_zpg: {
                const word address = zero_page();
                load_register(address, y);
            }
            break;
        case Opcode::ldy_zpx: {
                const word address = zero_page_x();
                load_register(address, y);
            }
            break;
        case Opcode::ldy_abs: {
                const word address = absolute();
                load_register(address, y);
            }
            break;
        case Opcode::ldy_abx: {
                const word address = absolute_x();
                load_register(address, y);
            }
            break;
        case Opcode::sta_zpg: {
                const word address = zero_page();
                write_byte(address, a);
            }
            break;
        case Opcode::sta_zpx: {
                const word address = zero_page_x();
                write_byte(address, a);
            }
            break;
        case Opcode::sta_abs: {
                const word address = absolute();
                write_byte(address, a);
            }
            break;
        case Opcode::sta_abx: {
                const word address = absolute_x(true);
                write_byte(address, a);
            }
            break;
        case Opcode::sta_aby: {
                const word address = absolute_y(true);
                write_byte(address, a);
            }
            break;
        case Opcode::sta_idx: {
                const word address = indirect_x();
                write_byte(address, a);
            }
            break;
        case Opcode::sta_idy: {
                const word address = indirect_y(true);
                write_byte(address, a);
            }
            break;
        case Opcode::stx_zpg: {
                const word address = zero_page();
                write_byte(address, x);
            }
            break;
        case Opcode::stx_zpy: {
                const word address = zero_page_y();
                write_byte(address, x);
            }
            break;
        case Opcode::stx_abs: {
                const word address = absolute();
                write_byte(address, x);
            }
            break;
        case Opcode::sty_zpg: {
                const word address = zero_page();
                write_byte(address, y);
            }
            break;
        case Opcode::sty_zpx: {
                const word address = zero_page_x();
                write_byte(address, y);
            }
            break;
        case Opcode::sty_abs: {
                const word address = absolute();
                write_byte(address, y);
            }
            break;
        case Opcode::tsx: {
                x = sp;
                set_zn(x);
            }
            break;
        case Opcode::txs: {
                sp = x;
            }
            break;
        case Opcode::pha: {
                push_byte(a);
            }
            break;
        case Opcode::pla: {
                a = pull_byte();
                set_zn(a);
            }
            break;
        case Opcode::php: {
                push_status();
            }
            break;
        case Opcode::plp: {
                pull_status();
            }
            break;
        case Opcode::jmp: {
                pc = absolute();
            }
            break;
        case Opcode::jmp_ind: {
                const word address = absolute();
                pc = read_word(address);
            }
            break;
        case Opcode::jsr: {
                const word subroutine_address = fetch_word();
                push_word(pc - 1);
                pc = subroutine_address;
            }
            break;
        case Opcode::rts: {
                const word return_address = pull_word();
                pc = return_address + 1;
            }
            break;
        case Opcode::and_imm: {
                a &= fetch_byte();
                set_zn(a);
            }
            break;
        case Opcode::and_zpg: {
                const word address = zero_page();
                and_op(address);
            }
            break;
        case Opcode::and_zpx: {
                const word address = zero_page_x();
                and_op(address);
            }
            break;
        case Opcode::and_abs: {
                const word address = absolute();
                and_op(address);
            }
            break;
        case Opcode::and_abx: {
                const word address = absolute_x();
                and_op(address);
            }
            break;
        case Opcode::and_aby: {
                const word address = absolute_y();
                and_op(address);
            }
            break;
        case Opcode::and_idx: {
                const word address = indirect_x();
                and_op(address);
            }
            break;
        case Opcode::and_idy: {
                const word address = indirect_y();
                and_op(address);
            }
            break;
        case Opcode::ora_imm: {
                a |= fetch_byte();
                set_zn(a);
            }
            break;
        case Opcode::ora_zpg: {
                const word address = zero_page();
                ora_op(address);
            }
            break;
        case Opcode::ora_zpx: {
                const word address = zero_page_x();
                ora_op(address);
            }
            break;
        case Opcode::ora_abs: {
                const word address = absolute();
                ora_op(address);
            }
            break;
        case Opcode::ora_abx: {
                const word address = absolute_x();
                ora_op(address);
            }
            break;
        case Opcode::ora_aby: {
                const word address = absolute_y();
                ora_op(address);
            }
            break;
        case Opcode::ora_idx: {
                const word address = indirect_x();
                ora_op(address);
            }
            break;
        case Opcode::ora_idy: {
                const word address = indirect_y();
                ora_op(address);
            }
            break;
        case Opcode::eor_imm: {
                a ^= fetch_byte();
                set_zn(a);
            }
            break;
        case Opcode::eor_zpg: {
                const word address = zero_page();
                eor_op(address);
            }
            break;
        case Opcode::eor_zpx: {
                const word address = zero_page_x();
                eor_op(address);
            }
            break;
        case Opcode::eor_abs: {
                const word address = absolute();
                eor_op(address);
            }
            break;
        case Opcode::eor_abx: {
                const word address = absolute_x();
                eor_op(address);
            }
            break;
        case Opcode::eor_aby: {
                const word address = absolute_y();
                eor_op(address);
            }
            break;
        case Opcode::eor_idx: {
                const word address = indirect_x();
                eor_op(address);
            }
            break;
        case Opcode::eor_idy: {
                const word address = indirect_y();
                eor_op(address);
            }
            break;
        case Opcode::bit_zpg: {
                const word address = zero_page();
                const byte value = read_byte(address);
                status.z = !(a & value);
                status.n = (value & NEGATIVE_BIT) != 0;
                status.v = (value & OVERFLOW_BIT) != 0;
            }
            break;
        case Opcode::bit_abs: {
                const word address = absolute();
                const byte value = read_byte(address);
                status.z = !(a & value);
                status.n = (value & NEGATIVE_BIT) != 0;
                status.v = (value & OVERFLOW_BIT) != 0;
            }
            break;
        case Opcode::tax: {
                x = a;
                set_zn(x);
            }
            break;
        case Opcode::tay: {
                y = a;
                set_zn(y);
            }
            break;
        case Opcode::txa: {
                a = x;
                set_zn(a);
            }
            break;
        case Opcode::tya: {
                a = y;
                set_zn(a);
            }
            break;
        case Opcode::inx: {
                ++x;
                set_zn(x);
            }
            break;
        case Opcode::iny: {
                ++y;
                set_zn(y);
            }
            break;
        case Opcode::dex: {
                --x;
                set_zn(x);
            }
            break;
        case Opcode::dey: {
                --y;
                set_zn(y);
            }
            break;
        case Opcode::dec_zpg: {
                const word address = zero_page();
                const byte value = read_byte(address) - 1;
                write_byte(address, value);
                set_zn(value);
            }
            break;
        case Opcode::dec_zpx: {
                const word address = zero_page_x();
                const byte value = read_byte(address) - 1;
                write_byte(address, value);
                set_zn(value);
            }
            break;
        case Opcode::dec_abs: {
                const word address = absolute();
                const byte value = read_byte(address) - 1;
                write_byte(address, value);
                set_zn(value);
            }
            break;
        case Opcode::dec_abx: {
                const word address = absolute_x(true);
                const byte value = read_byte(address) - 1;
                write_byte(address, value);
                set_zn(value);
            }
            break;
        case Opcode::inc_zpg: {
                const word address = zero_page();
                const byte value = read_byte(address) + 1;
                write_byte(address, value);
                set_zn(value);
            }
            break;
        case Opcode::inc_zpx: {
                const word address = zero_page_x();
                const byte value = read_byte(address) + 1;
                write_byte(address, value);
                set_zn(value);
            }
            break;
        case Opcode::inc_abs: {
                const word address = absolute();
                const byte value = read_byte(address) + 1;
                write_byte(address, value);
                set_zn(value);
            }
            break;
        case Opcode::inc_abx: {
                const word address = absolute_x(true);
                const byte value = read_byte(address) + 1;
                write_byte(address, value);
                set_zn(value);
            }
            break;
        case Opcode::beq: {
                branch_if(status.z, true);
            }
            break;
        case Opcode::bne: {
                branch_if(status.z, false);
            }
            break;
        case Opcode::bcs: {
                branch_if(status.c, true);
            }
            break;
        case Opcode::bcc: {
                branch_if(status.c, false);
            }
            break;
        case Opcode::bmi: {
                branch_if(status.n, true);
            }
            break;
        case Opcode::bpl: {
                branch_if(status.n, false);
            }
            break;
        case Opcode::bvc: {
                branch_if(status.v, false);
            }
            break;
        case Opcode::bvs: {
                branch_if(status.v, true);
            }
            break;
        case Opcode::clc: {
                status.c = false;
            }
            break;
        case Opcode::sec: {
                status.c = true;
            }
            break;
        case Opcode::cld: {
                status.d = false;
            }
            break;
        case Opcode::sed: {
                status.d = true;
            }
            break;
        case Opcode::cli: {
                status.i = false;
            }
            break;
        case Opcode::sei: {
                status.i = true;
            }
            break;
        case Opcode::clv: {
                status.v = false;
            }
            break;
        case Opcode::adc: {
                const byte operand = fetch_byte();
                adc(operand);
            }
            break;
        case Opcode::adc_zpg: {
                const word address = zero_page();
                const byte operand = read_byte(address);
                adc(operand);
            }
            break;
        case Opcode::adc_zpx: {
                const word address = zero_page_x();
                const byte operand = read_byte(address);
                adc(operand);
            }
            break;
        case Opcode::adc_abs: {
                const word address = absolute();
                const byte operand = read_byte(address);
                adc(operand);
            }
            break;
        case Opcode::adc_abx: {
                const word address = absolute_x();
                const byte operand = read_byte(address);
                adc(operand);
            }
            break;
        case Opcode::adc_aby: {
                const word address = absolute_y();
                const byte operand = read_byte(address);
                adc(operand);
            }
            break;
        case Opcode::adc_idx: {
                const word address = indirect_x();
                const byte operand = read_byte(address);
                adc(operand);
            }
            break;
        case Opcode::adc_idy: {
                const word address = indirect_y();
                const byte operand = read_byte(address);
                adc(operand);
            }
            break;
        case Opcode::sbc: {
                const byte operand = fetch_byte();
                sbc(operand);
            }
            break;
        case Opcode::sbc_zpg: {
                const word address = zero_page();
                const byte operand = read_byte(address);
                sbc(operand);
            }
            break;
        case Opcode::sbc_zpx: {
                const word address = zero_page_x();
                const byte operand = read_byte(address);
                sbc(operand);
            }
            break;
        case Opcode::sbc_abs: {
                const word address = absolute();
                const byte operand = read_byte(address);
                sbc(operand);
            }
            break;
        case Opcode::sbc_abx: {
                const word address = absolute_x();
                const byte operand = read_byte(address);
                sbc(operand);
            }
            break;
        case Opcode::sbc_aby: {
                const word address = absolute_y();
                const byte operand = read_byte(address);
                sbc(operand);
            }
            break;
        case Opcode::sbc_idx: {
                const word address = indirect_x();
                const byte operand = read_byte(address);
                sbc(operand);
            }
            break;
        case Opcode::sbc_idy: {
                const word address = indirect_y();
                const byte operand = read_byte(address);
                sbc(operand);
            }
            break;
        case Opcode::cmp: {
                const byte operand = fetch_byte();
                compare(operand, a);
            }
            break;
        case Opcode::cmp_zpg: {
                const word address = zero_page();
                const byte operand = read_byte(address);
                compare(operand, a);
            }
            break;
        case Opcode::cmp_zpx: {
                const word address = zero_page_x();
                const byte operand = read_byte(address);
                compare(operand, a);
            }
            break;
        case Opcode::cmp_abs: {
                const word address = absolute();
                const byte operand = read_byte(address);
                compare(operand, a);
            }
            break;
        case Opcode::cmp_abx: {
                const word address = absolute_x();
                const byte operand = read_byte(address);
                compare(operand, a);
            }
            break;
        case Opcode::cmp_aby: {
                const word address = absolute_y();
                const byte operand = read_byte(address);
                compare(operand, a);
            }
            break;
        case Opcode::cmp_idx: {
                const word address = indirect_x();
                const byte operand = read_byte(address);
                compare(operand, a);
            }
            break;
        case Opcode::cmp_idy: {
                const word address = indirect_y();
                const byte operand = read_byte(address);
                compare(operand, a);
            }
            break;
        case Opcode::cpx: {
                const byte operand = fetch_byte();
                compare(operand, x);
            }
            break;
        case Opcode::cpy: {
                const byte operand = fetch_byte();
                compare(operand, y);
            }
            break;
        case Opcode::cpx_zpg: {
                const word address = zero_page();
                const byte operand = read_byte(address);
                compare(operand, x);
            }
            break;
        case Opcode::cpy_zpg: {
                const word address = zero_page();
                const byte operand = read_byte(address);
                compare(operand, y);
            }
            break;
        case Opcode::cpx_abs: {
                const word address = absolute();
                const byte operand = read_byte(address);
                compare(operand, x);
            }
            break;
        case Opcode::cpy_abs: {
                const word address = absolute();
                const byte operand = read_byte(address);
                compare(operand, y);
            }
            break;
        case Opcode::asl: {
                a = asl(a);
            }
            break;
        case Opcode::asl_zpg: {
                const word address = zero_page();
                const byte operand = read_byte(address);
                const byte result = asl(operand);
                write_byte(address, result);
            }
            break;
        case Opcode::asl_zpx: {
                const word address = zero_page_x();
                const byte operand = read_byte(address);
                const byte result = asl(operand);
                write_byte(address, result);
            }
            break;
        case Opcode::asl_abs: {
                const word address = absolute();
                const byte operand = read_byte(address);
                const byte result = asl(operand);
                write_byte(address, result);
            }
            break;
        case Opcode::asl_abx: {
                const word address = absolute_x();
                const byte operand = read_byte(address);
                const byte result = asl(operand);
                write_byte(address, result);
            }
            break;
        case Opcode::lsr: {
                a = lsr(a);
            }
            break;
        case Opcode::lsr_zpg: {
                const word address = zero_page();
                const byte operand = read_byte(address);
                const byte result = lsr(operand);
                write_byte(address, result);
            }
            break;
        case Opcode::lsr_zpx: {
                const word address = zero_page_x();
                const byte operand = read_byte(address);
                const byte result = lsr(operand);
                write_byte(address, result);
            }
            break;
        case Opcode::lsr_abs: {
                const word address = absolute();
                const byte operand = read_byte(address);
                const byte result = lsr(operand);
                write_byte(address, result);
            }
            break;
        case Opcode::lsr_abx: {
                const word address = absolute_x(true);
                const byte operand = read_byte(address);
                const byte result = lsr(operand);
                write_byte(address, result);
            }
            break;
        case Opcode::rol: {
                a = rol(a);
            }
            break;
        case Opcode::rol_zpg: {
                const word address = zero_page();
                const byte operand = read_byte(address);
                const byte result = rol(operand);
                write_byte(address, result);
            }
            break;
        case Opcode::rol_zpx: {
                const word address = zero_page_x();
                const byte operand = read_byte(address);
                const byte result = rol(operand);
                write_byte(address, result);
            }
            break;
        case Opcode::rol_abs: {
                const word address = absolute();
                const byte operand = read_byte(address);
                const byte result = rol(operand);
                write_byte(address, result);
            }
            break;
        case Opcode::rol_abx: {
                const word address = absolute_x();
                const byte operand = read_byte(address);
                const byte result = rol(operand);
                write_byte(address, result);
            }
            break;
        case Opcode::ror: {
                a = ror(a);
            }
            break;
        case Opcode::ror_zpg: {
                const word address = zero_page();
                const byte operand = read_byte(address);
                const byte result = ror(operand);
                write_byte(address, result);
            }
            break;
        case Opcode::ror_zpx: {
                const word address = zero_page_x();
                const byte operand = read_byte(address);
                const byte result = ror(operand);
                write_byte(address, result);
            }
            break;
        case Opcode::ror_abs: {
                const word address = absolute();
                const byte operand = read_byte(address);
                const byte result = ror(operand);
                write_byte(address, result);
            }
            break;
        case Opcode::ror_abx: {
                const word address = absolute_x();
                const byte operand = read_byte(address);
                const byte result = ror(operand);
                write_byte(address, result);
            }
            break;
        case Opcode::nop: {

            }
            break;
        case Opcode::brk: {
                push_word(pc + 1);
                push_status();
                pc = read_word(IRQ_VECTOR);
                status.b = true;
                status.i = true;
            }
            break;
        case Opcode::rti: {
                pull_status();
                pc = pull_word();
            }
            break;
        default:
            return false;
    }
    return true;
}

void CPU::run_interrupt(InterruptType type) noexcept {
    if (status.i && type != InterruptType::nmi)
        return;
    push_word(pc);

    const byte flags = status.value | UNUSED_BIT | BREAK_BIT;
    push_byte(flags);
    status.i = true;

    switch (type) {
        case InterruptType::irq:
            pc = read_word(IRQ_VECTOR);
            break;
        case InterruptType::nmi:
            pc = read_word(NMI_VECTOR);
            break;
    }
    skip_cycles += 6;
}

byte CPU::fetch_byte() noexcept {
    return bus.read(pc++);
}

word CPU::fetch_word() noexcept {
    const byte lo = fetch_byte();
    const byte hi = fetch_byte();
    return lo | (hi << 8);
}

byte CPU::read_byte(word address) {
    return bus.read(address);
}

word CPU::read_word(word address) {
    const byte lo = read_byte(address);
    const byte hi = read_byte(address + 1);
    return lo | (hi << 8);
}

void CPU::write_byte(word address, byte value) {
    bus.write(address, value);
}

void CPU::write_word(word value, word address) {
    bus.write(address, value & 0xff);
    bus.write(address + 1, value >> 8);
}

void CPU::push_byte(byte value) noexcept {
    write_byte(sp_to_address(), value);
    --sp;
}

void CPU::push_word(word value) noexcept {
    push_byte(value >> 8);
    push_byte(value & 0xff);
}

byte CPU::pull_byte() noexcept {
    ++sp;
    return read_byte(sp_to_address());
}

word CPU::pull_word() noexcept {
    const auto value = read_word(sp_to_address() + 1);
    sp += 2;
    return value;
}

constexpr void CPU::set_zn(byte reg) noexcept {
    status.z = (reg == 0);
    status.n = (reg & NEGATIVE_BIT) > 0;
}

constexpr word CPU::sp_to_address() const noexcept {
    return 0x100 | sp;
}

constexpr bool CPU::page_crossed(word address1, word address2) noexcept {
    return (address1 & 0xff00) != (address2 & 0xff00);
}

word CPU::zero_page() noexcept {
    return static_cast<word>(fetch_byte());
}

word CPU::zero_page_x() noexcept {
    return zero_page() + x;
}

word CPU::zero_page_y() noexcept {
    return zero_page() + y;
}

word CPU::absolute() noexcept {
    return fetch_word();
}

word CPU::absolute_x(bool always_cycle) noexcept {
    const word abs_address = fetch_word();
    const word abs_address_x = abs_address + x;
    if (page_crossed(abs_address, abs_address_x) || always_cycle) {
        ++skip_cycles;
    }
    return abs_address_x;
}

word CPU::absolute_y(bool always_cycle) noexcept {
    const word abs_address = fetch_word();
    const word abs_address_y = abs_address + y;
    if (page_crossed(abs_address, abs_address_y) || always_cycle) {
        ++skip_cycles;
    }
    return abs_address_y;
}

word CPU::indirect_x() noexcept {
    const word zpg_address = zero_page_x();
    return read_word(zpg_address);
}

word CPU::indirect_y(bool always_cycle) noexcept {
    const word zpg_address = zero_page();
    const word ind_address = read_word(zpg_address);
    const word ind_address_y = ind_address + y;
    if (page_crossed(ind_address, ind_address_y) || always_cycle) {
        ++skip_cycles;
    }
    return ind_address_y;
}

} // modnes