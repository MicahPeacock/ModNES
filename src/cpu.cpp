#include "cpu.hpp"

namespace nes {

void CPU::reset() noexcept {
    reset(RESET_VECTOR);
}

void CPU::reset(word reset_vector) noexcept {
    pc = read_word(reset_vector);
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
        run_interrupt(InterruptType::NMI);
        pending_irq = pending_nmi = false;
        return;
    }
    if(pending_irq) {
        run_interrupt(InterruptType::IRQ);
        pending_irq = pending_nmi = false;
        return;
    }

    const auto opcode = fetch_byte();
    const auto instruction = INSTRUCTIONS[opcode];
    if(instruction.cycles && execute(instruction)) {
        skip_cycles += instruction.cycles;
    } else {
        // unrecognized opcode
    }
}

void CPU::interrupt(InterruptType type) noexcept {
    switch (type) {
        case InterruptType::NMI:
            pending_nmi = true;
            break;
        case InterruptType::IRQ:
            pending_irq = true;
            break;
        default:
            break;
    }
}

void CPU::skip_dma_cycles() noexcept {
    skip_cycles += 513 + (cycles & 0x1);
}

byte CPU::execute(Instruction instruction) noexcept {


    const bool always_cycle = instruction.operation == Operation::STA;
    const word address = [&]() -> word {
        using enum AddressMode;
        switch (instruction.address_mode) {
            case IMM: return immediate();
            case ZPG: return zero_page();
            case ZPX: return zero_page_x();
            case ZPY: return zero_page_y();
            case ABS: return absolute();
            case ABX: return absolute_x(always_cycle);
            case ABY: return absolute_y(always_cycle);
            case IND: return indirect();
            case IDX: return indirect_x();
            case IDY: return indirect_y(always_cycle);
            default:  return 0;
        }
    }();
    switch (instruction.operation) {
        using enum Operation;
        case LDA: {
            load_register(address, a);
            break;
        }
        case LDX: {
            load_register(address, x);
            break;
        }
        case LDY: {
            load_register(address, y);
            break;
        }
        case STA: {
            write_byte(address, a);
            break;
        }
        case STX: {
            write_byte(address, x);
            break;
        }
        case STY: {
            write_byte(address, y);
            break;
        }
        case TSX: {
            x = sp;
            set_zn(x);
            break;
        }
        case TXS: {
            sp = x;
            break;
        }
        case PHA: {
            push_byte(a);
            break;
        }
        case PLA: {
            a = pull_byte();
            set_zn(a);
            break;
        }
        case PHP: {
            push_status();
            break;
        }
        case PLP: {
            pull_status();
            break;
        }
        case JMP: {
            pc = address;
            break;
        }
        case JSR: {
            push_word(pc - 1);
            pc = address;
            break;
        }
        case RTS: {
            pc = pull_word() + 1;
            break;
        }
        case AND: {
            and_op(address);
            break;
        }
        case ORA: {
            ora_op(address);
            break;
        }
        case EOR: {
            eor_op(address);
            break;
        }
        case BIT: {
            const byte value = read_byte(address);
            status.z = !(a & value);
            status.n = (value & NEGATIVE_BIT) != 0;
            status.v = (value & OVERFLOW_BIT) != 0;
            break;
        }
        case TAX: {
            x = a;
            set_zn(x);
            break;
        }
        case TAY: {
            y = a;
            set_zn(y);
            break;
        }
        case TXA: {
            a = x;
            set_zn(a);
            break;
        }
        case TYA: {
            a = y;
            set_zn(a);
            break;
        }
        case INC: {
            const byte value = read_byte(address) + 1;
            write_byte(address, value);
            set_zn(value);
            break;
        }
        case INX: {
            x++;
            set_zn(x);
            break;
        }
        case INY: {
            y++;
            set_zn(y);
            break;
        }
        case DEC: {
            const byte value = read_byte(address) - 1;
            write_byte(address, value);
            set_zn(value);
            break;
        }
        case DEX: {
            x--;
            set_zn(x);
            break;
        }
        case DEY: {
            y--;
            set_zn(y);
            break;
        }
        case BCC: {
            branch_if(status.c, false);
            break;
        }
        case BCS: {
            branch_if(status.c, true);
            break;
        }
        case BEQ: {
            branch_if(status.z, true);
            break;
        }
        case BMI: {
            branch_if(status.n, true);
            break;
        }
        case BNE: {
            branch_if(status.z, false);
            break;
        }
        case BPL: {
            branch_if(status.n, false);
            break;
        }
        case BVC: {
            branch_if(status.v, false);
            break;
        }
        case BVS: {
            branch_if(status.v, true);
            break;
        }
        case CLC: {
            status.c = 0;
            break;
        }
        case SEC: {
            status.c = true;
            break;
        }
        case CLD: {
            status.d = 0;
            break;
        }
        case SED: {
            status.d = true;
            break;
        }
        case CLI: {
            status.i = false;
            break;
        }
        case SEI: {
            status.i = true;
            break;
        }
        case CLV: {
            status.v = false;
            break;
        }
        case ADC: {
            const byte operand = read_byte(address);
            adc(operand);
            break;
        }
        case SBC: {
            const byte operand = read_byte(address);
            sbc(operand);
            break;
        }
        case CMP: {
            const byte operand = read_byte(address);
            compare(operand, a);
            break;
        }
        case CPX: {
            const byte operand = read_byte(address);
            compare(operand, x);
            break;
        }
        case CPY: {
            const byte operand = read_byte(address);
            compare(operand, y);
            break;
        }
        case ASL: {
            if (instruction.address_mode == AddressMode::ACC) {
                a = asl(a);
            } else {
                const byte operand = read_byte(address);
                const byte value = asl(operand);
                write_byte(address, value);
            }
            break;
        }
        case LSR: {
            if (instruction.address_mode == AddressMode::ACC) {
                a = lsr(a);
            } else {
                const byte operand = read_byte(address);
                const byte value = lsr(operand);
                write_byte(address, value);
            }
            break;
        }
        case ROL: {
            if (instruction.address_mode == AddressMode::ACC) {
                a = rol(a);
            } else {
                const byte operand = read_byte(address);
                const byte value = rol(operand);
                write_byte(address, value);
            }
            break;
        }
        case ROR: {
            if (instruction.address_mode == AddressMode::ACC) {
                a = ror(a);
            } else {
                const byte operand = read_byte(address);
                const byte value = ror(operand);
                write_byte(address, value);
            }
            break;
        }
        case NOP: {
            break;
        }
        case BRK: {
            push_word(pc + 1);
            push_status();
            pc = read_word(IRQ_VECTOR);
            status.b = true;
            status.i = true;
            break;
        }
        case RTI: {
            pull_status();
            pc = pull_word();
            break;
        }
        default: {
            throw std::runtime_error("Unhandled opcode: 0x" + std::to_string(instruction.opcode));
        }
    }
    return instruction.cycles;
}

void CPU::run_interrupt(InterruptType type) noexcept {
    if (status.i && type != InterruptType::NMI)
        return;
    push_word(pc);

    const byte flags = status.value | UNUSED_BIT | BREAK_BIT;
    push_byte(flags);
    status.i = true;

    switch (type) {
        case InterruptType::IRQ:
            pc = read_word(IRQ_VECTOR);
            break;
        case InterruptType::NMI:
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
    return static_cast<word>(0x100 | sp);
}

constexpr bool CPU::page_crossed(word address1, word address2) noexcept {
    return (address1 & 0xff00) != (address2 & 0xff00);
}

word CPU::immediate() noexcept {
    return pc++;
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
    const word abs_address = absolute();
    const word abs_address_x = abs_address + x;
    if (page_crossed(abs_address, abs_address_x) || always_cycle) {
        ++skip_cycles;
    }
    return abs_address_x;
}

word CPU::absolute_y(bool always_cycle) noexcept {
    const word abs_address = absolute();
    const word abs_address_y = abs_address + y;
    if (page_crossed(abs_address, abs_address_y) || always_cycle) {
        ++skip_cycles;
    }
    return abs_address_y;
}

word CPU::indirect() noexcept {
    return read_word(absolute());
}

word CPU::indirect_x() noexcept {
    return read_word(zero_page_x());
}

word CPU::indirect_y(bool always_cycle) noexcept {
    const word ind_address = read_word(zero_page());
    const word ind_address_y = ind_address + y;
    if (page_crossed(ind_address, ind_address_y) || always_cycle) {
        ++skip_cycles;
    }
    return ind_address_y;
}

void CPU::load_register(word address, byte& reg) noexcept {
    reg = read_byte(address);
    set_zn(reg);
}

void CPU::and_op(word address) noexcept {
    a &= read_byte(address);
    set_zn(a);
}

void CPU::ora_op(word address) noexcept {
    a |= read_byte(address);
    set_zn(a);
}

void CPU::eor_op(word address) noexcept {
    a ^= read_byte(address);
    set_zn(a);
}

void CPU::branch_if(byte test, byte expected) noexcept {
    const auto offset = static_cast<sbyte>(fetch_byte());
    if (test != expected) {
        return;
    }
    const word new_pc = pc + offset;
    if (page_crossed(new_pc, pc)) {
        ++skip_cycles;
    }
    pc = new_pc;
}

void CPU::compare(byte operand, byte value) noexcept {
    const byte temp = value - operand;
    status.n = (temp & NEGATIVE_BIT) != 0;
    status.z = value == operand;
    status.c = value >= operand;
}

void CPU::adc(byte operand) noexcept {
    const bool same_signs = !((a ^ operand) & NEGATIVE_BIT);
    const word sum = a + operand + status.c;
    a = static_cast<byte>(sum & 0xff);
    set_zn(a);
    status.c = sum > 0xff;
    status.v = same_signs && ((a ^ operand) & NEGATIVE_BIT);
}

void CPU::sbc(byte operand) noexcept {
    adc(~operand);
}

byte CPU::asl(byte operand) noexcept {
    status.c = (operand & NEGATIVE_BIT) != 0;
    const byte result = operand << 1;
    set_zn(result);
    return result;
}

byte CPU::lsr(byte operand) noexcept {
    status.c = (operand & CARRY_BIT) != 0;
    const byte result = operand >> 1;
    set_zn(result);
    return result;
}

byte CPU::rol(byte operand) noexcept {
    const byte new_first = status.c ? CARRY_BIT : 0;
    status.c = (operand & NEGATIVE_BIT) != 0;
    const byte result = (operand << 1) | new_first;
    set_zn(result);
    return result;
}

byte CPU::ror(byte operand) noexcept {
    const bool old_first = (operand & CARRY_BIT) != 0;
    operand >>= 1;
    if(status.c) {
        operand |= NEGATIVE_BIT;
    }
    status.c = old_first;
    set_zn(operand);
    return operand;
}

void CPU::push_status() noexcept {
    push_byte(status.value | BREAK_BIT | UNUSED_BIT);
}

void CPU::pull_status() noexcept {
    status.value = pull_byte();
    status.b = false;
    status.u = false;
}

} // nes