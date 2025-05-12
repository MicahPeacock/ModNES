#ifndef MODNES_CPU_OPCODES_HPP
#define MODNES_CPU_OPCODES_HPP

#include "definitions.hpp"

#include <array>

namespace nes {

enum class InterruptType : byte {
    IRQ,
    NMI
};

enum class AddressMode : byte {
    IMM,
    ZPG, ZPX, ZPY,
    ABS, ABX, ABY,
    IND, IDX, IDY,
    REL, ACC, IMP,
};

enum class Operation : byte {
    ADC, AND, ASL, BCC, BCS, BEQ, BIT, BMI, BNE, BPL, BRK, BVC, BVS, CLC, CLD, CLI, CLV, CMP, CPX,
    CPY, DEC, DEX, DEY, EOR, INC, INX, INY, JMP, JSR, LDA, LDX, LDY, LSR, NOP, ORA, PHA, PHP, PLA,
    PLP, ROL, ROR, RTI, RTS, SBC, SEC, SED, SEI, STA, STX, STY, TAX, TAY, TSX, TXS, TXA, TYA,

    XXX, // Unused Opcode
};

struct Instruction {
    byte opcode;
    Operation operation;
    AddressMode address_mode;
    byte cycles;
};

using InstructionTable = std::array<Instruction, 0x100>;

static constexpr InstructionTable make_instruction_table() {
    using enum Operation;
    using enum AddressMode;
    return InstructionTable {{
        { 0x00, BRK, IMM, 7 }, { 0x01, ORA, IDX, 6 }, { 0x02, XXX, IMP, 0 }, { 0x03, XXX, IMP, 0 }, { 0x04, XXX, IMP, 0 }, { 0x05, ORA, ZPG, 3 }, { 0x06, ASL, ZPG, 5 }, { 0x07, XXX, IMP, 0 }, { 0x08, PHP, IMP, 3 }, { 0x09, ORA, IMM, 2 }, { 0x0a, ASL, ACC, 2 }, { 0x0b, XXX, IMP, 0 }, { 0x0c, XXX, IMP, 0 }, { 0x0d, ORA, ABS, 4 }, { 0x0e, ASL, ABS, 6 }, { 0x0f, XXX, IMP, 0 },
        { 0x10, BPL, REL, 2 }, { 0x11, ORA, IDY, 5 }, { 0x12, XXX, IMP, 0 }, { 0x13, XXX, IMP, 0 }, { 0x14, XXX, IMP, 0 }, { 0x15, ORA, ZPX, 4 }, { 0x16, ASL, ZPX, 6 }, { 0x17, XXX, IMP, 0 }, { 0x18, CLC, IMP, 2 }, { 0x19, ORA, ABY, 4 }, { 0x1a, XXX, IMP, 0 }, { 0x1b, XXX, IMP, 0 }, { 0x1c, XXX, IMP, 0 }, { 0x1d, ORA, ABX, 4 }, { 0x1e, ASL, ABX, 7 }, { 0x1f, XXX, IMP, 0 },
        { 0x20, JSR, ABS, 6 }, { 0x21, AND, IDX, 6 }, { 0x22, XXX, IMP, 0 }, { 0x23, XXX, IMP, 0 }, { 0x24, BIT, ZPG, 3 }, { 0x25, AND, ZPG, 3 }, { 0x26, ROL, ZPG, 5 }, { 0x27, XXX, IMP, 0 }, { 0x28, PLP, IMP, 4 }, { 0x29, AND, IMM, 2 }, { 0x2a, ROL, ACC, 2 }, { 0x2b, XXX, IMP, 0 }, { 0x2c, BIT, ABS, 4 }, { 0x2d, AND, ABS, 4 }, { 0x2e, ROL, ABS, 6 }, { 0x2f, XXX, IMP, 0 },
        { 0x30, BMI, REL, 2 }, { 0x31, AND, IDY, 5 }, { 0x32, XXX, IMP, 0 }, { 0x33, XXX, IMP, 0 }, { 0x34, XXX, IMP, 0 }, { 0x35, AND, ZPX, 4 }, { 0x36, ROL, ZPX, 6 }, { 0x37, XXX, IMP, 0 }, { 0x38, SEC, IMP, 2 }, { 0x39, AND, ABY, 4 }, { 0x3a, XXX, IMP, 0 }, { 0x3b, XXX, IMP, 0 }, { 0x3c, XXX, IMP, 0 }, { 0x3d, AND, ABX, 4 }, { 0x3e, ROL, ABX, 7 }, { 0x3f, XXX, IMP, 0 },
        { 0x40, RTI, IMP, 6 }, { 0x41, EOR, IDX, 6 }, { 0x42, XXX, IMP, 0 }, { 0x43, XXX, IMP, 0 }, { 0x44, XXX, IMP, 0 }, { 0x45, EOR, ZPG, 3 }, { 0x46, LSR, ZPG, 5 }, { 0x47, XXX, IMP, 0 }, { 0x48, PHA, IMP, 3 }, { 0x49, EOR, IMM, 2 }, { 0x4a, LSR, ACC, 2 }, { 0x4b, XXX, IMP, 0 }, { 0x4c, JMP, ABS, 3 }, { 0x4d, EOR, ABS, 4 }, { 0x4e, LSR, ABS, 6 }, { 0x4f, XXX, IMP, 0 },
        { 0x50, BVC, REL, 2 }, { 0x51, EOR, IDY, 5 }, { 0x52, XXX, IMP, 0 }, { 0x53, XXX, IMP, 0 }, { 0x54, XXX, IMP, 0 }, { 0x55, EOR, ZPX, 4 }, { 0x56, LSR, ZPX, 6 }, { 0x57, XXX, IMP, 0 }, { 0x58, CLI, IMP, 2 }, { 0x59, EOR, ABY, 4 }, { 0x5a, XXX, IMP, 0 }, { 0x5b, XXX, IMP, 0 }, { 0x5c, XXX, IMP, 0 }, { 0x5d, EOR, ABX, 4 }, { 0x5e, LSR, ABX, 7 }, { 0x5f, XXX, IMP, 0 },
        { 0x60, RTS, IMP, 6 }, { 0x61, ADC, IDX, 6 }, { 0x62, XXX, IMP, 0 }, { 0x63, XXX, IMP, 0 }, { 0x64, XXX, IMP, 0 }, { 0x65, ADC, ZPG, 3 }, { 0x66, ROR, ZPG, 5 }, { 0x67, XXX, IMP, 0 }, { 0x68, PLA, IMP, 4 }, { 0x69, ADC, IMM, 2 }, { 0x6a, ROR, ACC, 2 }, { 0x6b, XXX, IMP, 0 }, { 0x6c, JMP, IND, 5 }, { 0x6d, ADC, ABS, 4 }, { 0x6e, ROR, ABS, 6 }, { 0x6f, XXX, IMP, 0 },
        { 0x70, BVS, REL, 2 }, { 0x71, ADC, IDY, 5 }, { 0x72, XXX, IMP, 0 }, { 0x73, XXX, IMP, 0 }, { 0x74, XXX, IMP, 0 }, { 0x75, ADC, ZPX, 4 }, { 0x76, ROR, ZPX, 6 }, { 0x77, XXX, IMP, 0 }, { 0x78, SEI, IMP, 2 }, { 0x79, ADC, ABY, 4 }, { 0x7a, XXX, IMP, 0 }, { 0x7b, XXX, IMP, 0 }, { 0x7c, XXX, IMP, 0 }, { 0x7d, ADC, ABX, 4 }, { 0x7e, ROR, ABX, 7 }, { 0x7f, XXX, IMP, 0 },
        { 0x80, XXX, IMP, 0 }, { 0x81, STA, IDX, 6 }, { 0x82, XXX, IMP, 0 }, { 0x83, XXX, IMP, 0 }, { 0x84, STY, ZPG, 3 }, { 0x85, STA, ZPG, 3 }, { 0x86, STX, ZPG, 3 }, { 0x87, XXX, IMP, 0 }, { 0x88, DEY, IMP, 2 }, { 0x89, XXX, IMP, 0 }, { 0x8a, TXA, IMP, 2 }, { 0x8b, XXX, IMP, 0 }, { 0x8c, STY, ABS, 4 }, { 0x8d, STA, ABS, 4 }, { 0x8e, STX, ABS, 4 }, { 0x8f, XXX, IMP, 0 },
        { 0x90, BCC, REL, 2 }, { 0x91, STA, IDY, 6 }, { 0x92, XXX, IMP, 0 }, { 0x93, XXX, IMP, 0 }, { 0x94, STY, ZPX, 4 }, { 0x95, STA, ZPX, 4 }, { 0x96, STX, ZPY, 4 }, { 0x97, XXX, IMP, 0 }, { 0x98, TYA, IMP, 2 }, { 0x99, STA, ABY, 5 }, { 0x9a, TXS, IMP, 2 }, { 0x9b, XXX, IMP, 0 }, { 0x9c, XXX, IMP, 0 }, { 0x9d, STA, ABX, 5 }, { 0x9e, XXX, IMP, 0 }, { 0x9f, XXX, IMP, 0 },
        { 0xa0, LDY, IMM, 2 }, { 0xa1, LDA, IDX, 6 }, { 0xa2, LDX, IMM, 2 }, { 0xa3, XXX, IMP, 0 }, { 0xa4, LDY, ZPG, 3 }, { 0xa5, LDA, ZPG, 3 }, { 0xa6, LDX, ZPG, 3 }, { 0xa7, XXX, IMP, 0 }, { 0xa8, TAY, IMP, 2 }, { 0xa9, LDA, IMM, 2 }, { 0xaa, TAX, IMP, 2 }, { 0xab, XXX, IMP, 0 }, { 0xac, LDY, ABS, 4 }, { 0xad, LDA, ABS, 4 }, { 0xae, LDX, ABS, 4 }, { 0xaf, XXX, IMP, 0 },
        { 0xb0, BCS, REL, 2 }, { 0xb1, LDA, IDY, 5 }, { 0xb2, XXX, IMP, 0 }, { 0xb3, XXX, IMP, 0 }, { 0xb4, LDY, ZPX, 4 }, { 0xb5, LDA, ZPX, 4 }, { 0xb6, LDX, ZPY, 4 }, { 0xb7, XXX, IMP, 0 }, { 0xb8, CLV, IMP, 2 }, { 0xb9, LDA, ABY, 4 }, { 0xba, TSX, IMP, 2 }, { 0xbb, XXX, IMP, 0 }, { 0xbc, LDY, ABX, 4 }, { 0xbd, LDA, ABX, 4 }, { 0xbe, LDX, ABY, 4 }, { 0xbf, XXX, IMP, 0 },
        { 0xc0, CPY, IMM, 2 }, { 0xc1, CMP, IDX, 6 }, { 0xc2, XXX, IMP, 0 }, { 0xc3, XXX, IMP, 0 }, { 0xc4, CPY, ZPG, 3 }, { 0xc5, CMP, ZPG, 3 }, { 0xc6, DEC, ZPG, 5 }, { 0xc7, XXX, IMP, 0 }, { 0xc8, INY, IMP, 2 }, { 0xc9, CMP, IMM, 2 }, { 0xca, DEX, IMP, 2 }, { 0xcb, XXX, IMP, 0 }, { 0xcc, CPY, ABS, 4 }, { 0xcd, CMP, ABS, 4 }, { 0xce, DEC, ABS, 6 }, { 0xcf, XXX, IMP, 0 },
        { 0xd0, BNE, REL, 2 }, { 0xd1, CMP, IDY, 5 }, { 0xd2, XXX, IMP, 0 }, { 0xd3, XXX, IMP, 0 }, { 0xd4, XXX, IMP, 0 }, { 0xd5, CMP, ZPX, 4 }, { 0xd6, DEC, ZPX, 6 }, { 0xd7, XXX, IMP, 0 }, { 0xd8, CLD, IMP, 2 }, { 0xd9, CMP, ABY, 4 }, { 0xda, XXX, IMP, 0 }, { 0xdb, XXX, IMP, 0 }, { 0xdc, XXX, IMP, 0 }, { 0xdd, CMP, ABX, 4 }, { 0xde, DEC, ABX, 7 }, { 0xdf, XXX, IMP, 0 },
        { 0xe0, CPX, IMM, 2 }, { 0xe1, SBC, IDX, 6 }, { 0xe2, XXX, IMP, 0 }, { 0xe3, XXX, IMP, 0 }, { 0xe4, CPX, ZPG, 3 }, { 0xe5, SBC, ZPG, 3 }, { 0xe6, INC, ZPG, 5 }, { 0xe7, XXX, IMP, 0 }, { 0xe8, INX, IMP, 2 }, { 0xe9, SBC, IMM, 2 }, { 0xea, NOP, IMP, 2 }, { 0xeb, SBC, IMM, 2 }, { 0xec, CPX, ABS, 4 }, { 0xed, SBC, ABS, 4 }, { 0xee, INC, ABS, 6 }, { 0xef, XXX, IMP, 0 },
        { 0xf0, BEQ, REL, 2 }, { 0xf1, SBC, IDY, 5 }, { 0xf2, XXX, IMP, 0 }, { 0xf3, XXX, IMP, 0 }, { 0xf4, XXX, IMP, 0 }, { 0xf5, SBC, ZPX, 4 }, { 0xf6, INC, ZPX, 6 }, { 0xf7, XXX, IMP, 0 }, { 0xf8, SED, IMP, 2 }, { 0xf9, SBC, ABY, 4 }, { 0xfa, XXX, IMP, 0 }, { 0xfb, XXX, IMP, 0 }, { 0xfc, XXX, IMP, 0 }, { 0xfd, SBC, ABX, 4 }, { 0xfe, INC, ABX, 7 }, { 0xff, XXX, IMP, 0 },
    }};
}

static constexpr InstructionTable INSTRUCTIONS = make_instruction_table();

} // nes


#endif //MODNES_CPU_OPCODES_HPP
