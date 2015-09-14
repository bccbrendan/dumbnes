#include "op_info.h"


using namespace dumbnes;


std::ostream& operator<<(std::ostream& os, const OpInfo& op)
{
    os << MnemonicString(op.mnemonic) << " "
       << OpModeString(op.mode) << " "
       << "bytelen: " << op.length << " "
       << "time: " << op.time;
    return os;
}

const OpInfo&
OpInfo::Decode(Opcode opcode)
{
    static const std::map<Opcode, OpInfo> decoder
    {
        {0x69, OpInfo(Mnemonic::ADC, OpMode::Immediate, 2, 2)},
        {0x65, OpInfo(Mnemonic::ADC, OpMode::ZeroPage, 2, 3)},
        {0x75, OpInfo(Mnemonic::ADC, OpMode::ZeroPageX, 2, 4)},
        {0x6D, OpInfo(Mnemonic::ADC, OpMode::Absolute, 3, 4)},
        {0x7D, OpInfo(Mnemonic::ADC, OpMode::AbsoluteX, 3, 4, true)},
        {0x79, OpInfo(Mnemonic::ADC, OpMode::AbsoluteY, 3, 4, true)},
        {0x61, OpInfo(Mnemonic::ADC, OpMode::IndirectX, 2, 6)},
        {0x71, OpInfo(Mnemonic::ADC, OpMode::IndirectY, 2, 5, true)},
        {0x29, OpInfo(Mnemonic::AND, OpMode::Immediate, 2, 2)},
        {0x25, OpInfo(Mnemonic::AND, OpMode::ZeroPage, 2, 3)},
        {0x35, OpInfo(Mnemonic::AND, OpMode::ZeroPageX, 2, 4)},
        {0x2D, OpInfo(Mnemonic::AND, OpMode::Absolute, 3, 4)},
        {0x3D, OpInfo(Mnemonic::AND, OpMode::AbsoluteX, 3, 4, true)},
        {0x39, OpInfo(Mnemonic::AND, OpMode::AbsoluteY, 3, 4, true)},
        {0x21, OpInfo(Mnemonic::AND, OpMode::IndirectX, 2, 6)},
        {0x31, OpInfo(Mnemonic::AND, OpMode::IndirectY, 2, 5, true)},
        {0x0A, OpInfo(Mnemonic::ASL, OpMode::Accumulator, 1, 2)},
        {0x06, OpInfo(Mnemonic::ASL, OpMode::ZeroPage, 2, 5)},
        {0x16, OpInfo(Mnemonic::ASL, OpMode::ZeroPageX, 2, 6)},
        {0x0E, OpInfo(Mnemonic::ASL, OpMode::Absolute, 3, 6)},
        {0x1E, OpInfo(Mnemonic::ASL, OpMode::AbsoluteX, 3, 7)},
        {0x24, OpInfo(Mnemonic::BIT, OpMode::ZeroPage, 2, 3)},
        {0x2C, OpInfo(Mnemonic::BIT, OpMode::Absolute, 3, 4)},
        {0x10, OpInfo(Mnemonic::BPL, OpMode::Relative, 2, 2, true, true)},
        {0x30, OpInfo(Mnemonic::BMI, OpMode::Relative, 2, 2, true, true)},
        {0x50, OpInfo(Mnemonic::BVC, OpMode::Relative, 2, 2, true, true)},
        {0x70, OpInfo(Mnemonic::BVS, OpMode::Relative, 2, 2, true, true)},
        {0x90, OpInfo(Mnemonic::BCC, OpMode::Relative, 2, 2, true, true)},
        {0xB0, OpInfo(Mnemonic::BCS, OpMode::Relative, 2, 2, true, true)},
        {0xD0, OpInfo(Mnemonic::BNE, OpMode::Relative, 2, 2, true, true)},
        {0xF0, OpInfo(Mnemonic::BEQ, OpMode::Relative, 2, 2, true, true)},
        {0x00, OpInfo(Mnemonic::BRK, OpMode::Implied, 1, 7)},
        {0xC9, OpInfo(Mnemonic::CMP, OpMode::Immediate, 2, 2)},
        {0xC5, OpInfo(Mnemonic::CMP, OpMode::ZeroPage, 2, 3)},
        {0xD5, OpInfo(Mnemonic::CMP, OpMode::ZeroPageX, 2, 4)},
        {0xCD, OpInfo(Mnemonic::CMP, OpMode::Absolute, 3, 4)},
        {0xDD, OpInfo(Mnemonic::CMP, OpMode::AbsoluteX, 3, 4, true)},
        {0xD9, OpInfo(Mnemonic::CMP, OpMode::AbsoluteY, 3, 4, true)},
        {0xC1, OpInfo(Mnemonic::CMP, OpMode::IndirectX, 2, 6)},
        {0xD1, OpInfo(Mnemonic::CMP, OpMode::IndirectY, 2, 5, true)},
        {0xE0, OpInfo(Mnemonic::CPX, OpMode::Immediate, 2, 2)},
        {0xE4, OpInfo(Mnemonic::CPX, OpMode::ZeroPage, 2, 3)},
        {0xEC, OpInfo(Mnemonic::CPX, OpMode::Absolute, 3, 4)},
        {0xC0, OpInfo(Mnemonic::CPY, OpMode::Immediate, 2, 2)},
        {0xC4, OpInfo(Mnemonic::CPY, OpMode::ZeroPage, 2, 3)},
        {0xCC, OpInfo(Mnemonic::CPY, OpMode::Absolute, 3, 4)},
        {0xC6, OpInfo(Mnemonic::DEC, OpMode::ZeroPage, 2, 5)},
        {0xD6, OpInfo(Mnemonic::DEC, OpMode::ZeroPageX, 2, 6)},
        {0xCE, OpInfo(Mnemonic::DEC, OpMode::Absolute, 3, 6)},
        {0xDE, OpInfo(Mnemonic::DEC, OpMode::AbsoluteX, 3, 7)},
        {0x49, OpInfo(Mnemonic::EOR, OpMode::Immediate, 2, 2)},
        {0x45, OpInfo(Mnemonic::EOR, OpMode::ZeroPage, 2, 3)},
        {0x55, OpInfo(Mnemonic::EOR, OpMode::ZeroPageX, 2, 4)},
        {0x4D, OpInfo(Mnemonic::EOR, OpMode::Absolute, 3, 4)},
        {0x5D, OpInfo(Mnemonic::EOR, OpMode::AbsoluteX, 3, 4, true)},
        {0x59, OpInfo(Mnemonic::EOR, OpMode::AbsoluteY, 3, 4, true)},
        {0x41, OpInfo(Mnemonic::EOR, OpMode::IndirectX, 2, 6)},
        {0x51, OpInfo(Mnemonic::EOR, OpMode::IndirectY, 2, 5, true)},
        {0x18, OpInfo(Mnemonic::CLC, OpMode::Implied, 1, 2)},
        {0x38, OpInfo(Mnemonic::SEC, OpMode::Implied, 1, 2)},
        {0x58, OpInfo(Mnemonic::CLI, OpMode::Implied, 1, 2)},
        {0x78, OpInfo(Mnemonic::SEI, OpMode::Implied, 1, 2)},
        {0xB8, OpInfo(Mnemonic::CLV, OpMode::Implied, 1, 2)},
        {0xD8, OpInfo(Mnemonic::CLD, OpMode::Implied, 1, 2)},
        {0xF8, OpInfo(Mnemonic::SED, OpMode::Implied, 1, 2)},
        {0xE6, OpInfo(Mnemonic::INC, OpMode::ZeroPage, 2, 5)},
        {0xF6, OpInfo(Mnemonic::INC, OpMode::ZeroPageX, 2, 6)},
        {0xEE, OpInfo(Mnemonic::INC, OpMode::Absolute, 3, 6)},
        {0xFE, OpInfo(Mnemonic::INC, OpMode::AbsoluteX, 3, 7)},
        {0x4C, OpInfo(Mnemonic::JMP, OpMode::Absolute, 3, 3)},
        {0x6C, OpInfo(Mnemonic::JMP, OpMode::Indirect, 3, 5)},
        {0x20, OpInfo(Mnemonic::JSR, OpMode::Absolute, 3, 6)},
        {0xA9, OpInfo(Mnemonic::LDA, OpMode::Immediate, 2, 2)},
        {0xA5, OpInfo(Mnemonic::LDA, OpMode::ZeroPage, 2, 3)},
        {0xB5, OpInfo(Mnemonic::LDA, OpMode::ZeroPageX, 2, 4)},
        {0xAD, OpInfo(Mnemonic::LDA, OpMode::Absolute, 3, 4)},
        {0xBD, OpInfo(Mnemonic::LDA, OpMode::AbsoluteX, 3, 4, true)},
        {0xB9, OpInfo(Mnemonic::LDA, OpMode::AbsoluteY, 3, 4, true)},
        {0xA1, OpInfo(Mnemonic::LDA, OpMode::IndirectX, 2, 6)},
        {0xB1, OpInfo(Mnemonic::LDA, OpMode::IndirectY, 2, 5, true)},
        {0xA2, OpInfo(Mnemonic::LDX, OpMode::Immediate, 2, 2)},
        {0xA6, OpInfo(Mnemonic::LDX, OpMode::ZeroPage, 2, 3)},
        {0xB6, OpInfo(Mnemonic::LDX, OpMode::ZeroPageY, 2, 4)},
        {0xAE, OpInfo(Mnemonic::LDX, OpMode::Absolute, 3, 4)},
        {0xBE, OpInfo(Mnemonic::LDX, OpMode::AbsoluteY, 3, 4, true)},
        {0xA0, OpInfo(Mnemonic::LDY, OpMode::Immediate, 2, 2)},
        {0xA4, OpInfo(Mnemonic::LDY, OpMode::ZeroPage, 2, 3)},
        {0xB4, OpInfo(Mnemonic::LDY, OpMode::ZeroPageX, 2, 4)},
        {0xAC, OpInfo(Mnemonic::LDY, OpMode::Absolute, 3, 4)},
        {0xBC, OpInfo(Mnemonic::LDY, OpMode::AbsoluteX, 3, 4, true)},
        {0x4A, OpInfo(Mnemonic::LSR, OpMode::Accumulator, 1, 2)},
        {0x46, OpInfo(Mnemonic::LSR, OpMode::ZeroPage, 2, 5)},
        {0x56, OpInfo(Mnemonic::LSR, OpMode::ZeroPageX, 2, 6)},
        {0x4E, OpInfo(Mnemonic::LSR, OpMode::Absolute, 3, 6)},
        {0x5E, OpInfo(Mnemonic::LSR, OpMode::AbsoluteX, 3, 7)},
        {0xEA, OpInfo(Mnemonic::NOP, OpMode::Implied, 1, 2)},
        {0x09, OpInfo(Mnemonic::ORA, OpMode::Immediate, 2, 2)},
        {0x05, OpInfo(Mnemonic::ORA, OpMode::ZeroPage, 2, 3)},
        {0x15, OpInfo(Mnemonic::ORA, OpMode::ZeroPageX, 2, 4)},
        {0x0D, OpInfo(Mnemonic::ORA, OpMode::Absolute, 3, 4)},
        {0x1D, OpInfo(Mnemonic::ORA, OpMode::AbsoluteX, 3, 4, true)},
        {0x19, OpInfo(Mnemonic::ORA, OpMode::AbsoluteY, 3, 4, true)},
        {0x01, OpInfo(Mnemonic::ORA, OpMode::IndirectX, 2, 6)},
        {0x11, OpInfo(Mnemonic::ORA, OpMode::IndirectY, 2, 5, true)},
        {0xAA, OpInfo(Mnemonic::TAX, OpMode::Implied, 1, 2)},
        {0x8A, OpInfo(Mnemonic::TXA, OpMode::Implied, 1, 2)},
        {0xCA, OpInfo(Mnemonic::DEX, OpMode::Implied, 1, 2)},
        {0xE8, OpInfo(Mnemonic::INX, OpMode::Implied, 1, 2)},
        {0xA8, OpInfo(Mnemonic::TAY, OpMode::Implied, 1, 2)},
        {0x98, OpInfo(Mnemonic::TYA, OpMode::Implied, 1, 2)},
        {0x88, OpInfo(Mnemonic::DEY, OpMode::Implied, 1, 2)},
        {0xC8, OpInfo(Mnemonic::INY, OpMode::Implied, 1, 2)},
        {0x2A, OpInfo(Mnemonic::ROL, OpMode::Accumulator, 1, 2)},
        {0x26, OpInfo(Mnemonic::ROL, OpMode::ZeroPage, 2, 5)},
        {0x36, OpInfo(Mnemonic::ROL, OpMode::ZeroPageX, 2, 6)},
        {0x2E, OpInfo(Mnemonic::ROL, OpMode::Absolute, 3, 6)},
        {0x3E, OpInfo(Mnemonic::ROL, OpMode::AbsoluteX, 3, 7)},
        {0x6A, OpInfo(Mnemonic::ROR, OpMode::Accumulator, 1, 2)},
        {0x66, OpInfo(Mnemonic::ROR, OpMode::ZeroPage, 2, 5)},
        {0x76, OpInfo(Mnemonic::ROR, OpMode::ZeroPageX, 2, 6)},
        {0x6E, OpInfo(Mnemonic::ROR, OpMode::Absolute, 3, 6)},
        {0x7E, OpInfo(Mnemonic::ROR, OpMode::AbsoluteX, 3, 7)},
        {0x40, OpInfo(Mnemonic::RTI, OpMode::Implied, 1, 6)},
        {0x60, OpInfo(Mnemonic::RTS, OpMode::Implied, 1, 6)},
        {0xE9, OpInfo(Mnemonic::SBC, OpMode::Immediate, 2, 2)},
        {0xE5, OpInfo(Mnemonic::SBC, OpMode::ZeroPage, 2, 3)},
        {0xF5, OpInfo(Mnemonic::SBC, OpMode::ZeroPageX, 2, 4)},
        {0xED, OpInfo(Mnemonic::SBC, OpMode::Absolute, 3, 4)},
        {0xFD, OpInfo(Mnemonic::SBC, OpMode::AbsoluteX, 3, 4, true)},
        {0xF9, OpInfo(Mnemonic::SBC, OpMode::AbsoluteY, 3, 4, true)},
        {0xE1, OpInfo(Mnemonic::SBC, OpMode::IndirectX, 2, 6)},
        {0xF1, OpInfo(Mnemonic::SBC, OpMode::IndirectY, 2, 5, true)},
        {0x85, OpInfo(Mnemonic::STA, OpMode::ZeroPage, 2, 3)},
        {0x95, OpInfo(Mnemonic::STA, OpMode::ZeroPageX, 2, 4)},
        {0x8D, OpInfo(Mnemonic::STA, OpMode::Absolute, 3, 4)},
        {0x9D, OpInfo(Mnemonic::STA, OpMode::AbsoluteX, 3, 5)},
        {0x99, OpInfo(Mnemonic::STA, OpMode::AbsoluteY, 3, 5)},
        {0x81, OpInfo(Mnemonic::STA, OpMode::IndirectX, 2, 6)},
        {0x91, OpInfo(Mnemonic::STA, OpMode::IndirectY, 2, 6)},
        {0x9A, OpInfo(Mnemonic::TXS, OpMode::Implied, 1, 2)},
        {0xBA, OpInfo(Mnemonic::TSX, OpMode::Implied, 1, 2)},
        {0x48, OpInfo(Mnemonic::PHA, OpMode::Implied, 1, 3)},
        {0x68, OpInfo(Mnemonic::PLA, OpMode::Implied, 1, 4)},
        {0x08, OpInfo(Mnemonic::PHP, OpMode::Implied, 1, 3)},
        {0x28, OpInfo(Mnemonic::PLP, OpMode::Implied, 1, 4)},
        {0x86, OpInfo(Mnemonic::STX, OpMode::ZeroPage, 2, 3)},
        {0x96, OpInfo(Mnemonic::STX, OpMode::ZeroPageY, 2, 4)},
        {0x8E, OpInfo(Mnemonic::STX, OpMode::Absolute, 3, 4)},
        {0x84, OpInfo(Mnemonic::STY, OpMode::ZeroPage, 2, 3)},
        {0x94, OpInfo(Mnemonic::STY, OpMode::ZeroPageX, 2, 4)},
        {0x8C, OpInfo(Mnemonic::STY, OpMode::Absolute, 3, 4)}
    };
    return decoder.at(opcode);
}

