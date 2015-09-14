/* nes_6502.cpp
 * definitions for 6502 cpu emulation
 * brendan long
 * Feb 2015
 */

#include "nes_6502.h"
using namespace dumbnes;

Nes6502::Nes6502(const std::shared_ptr<IMemory>& memory)
    :_memory(memory)
{
}

Nes6502::~Nes6502(void)
{
}

void Nes6502::Reset (void)
{
}

int Nes6502::Step(void)
{
    auto next_op = OpInfo::Decode((*_memory)[_regPC]);
    std::cout << "PC[0x" << std::hex << _regPC << "] "
        << next_op << std::endl;
    ProcessOp(next_op);
}

uint16_t Nes6502::FetchOperand(const OpMode& opmode)
{
    switch(opmode)
    {
    case OpMode::Immediate:
        // TODO handle wrap
        return (*_memory)[_regPC+1];
    case OpMode::ZeroPage:
    case OpMode::ZeroPageX:
    case OpMode::ZeroPageY:
    case OpMode::Accumulator:
    case OpMode::Absolute:
    case OpMode::AbsoluteX:
    case OpMode::AbsoluteY:
    case OpMode::Indirect:
    case OpMode::IndirectX:
    case OpMode::IndirectY:
    case OpMode::Relative:
    case OpMode::Implied:
        break;
    }
}

void Nes6502::CheckSr_S(uint8_t result)
{
    if (result & 0x80) {
        _regSR |= SrCode::S;
    } else {
        _regSR &= ~SrCode::S;
    }
}

void Nes6502::CheckSr_Z(uint8_t result)
{
    if (result == 0) {
        _regSR |= SrCode::Z;
    } else {
        _regSR &= ~SrCode::Z;
    }
}

void Nes6502::ProcessOp(const OpInfo& op)
{
    uint16_t operand = FetchOperand(op.mode);
    switch (op.mnemonic)
    {
    case Mnemonic::ADC: 
        break;
    case Mnemonic::AND:
    case Mnemonic::ASL:
    case Mnemonic::BCC:
    case Mnemonic::BCS:
    case Mnemonic::BEQ:
    case Mnemonic::BIT:
    case Mnemonic::BMI:
    case Mnemonic::BNE:
    case Mnemonic::BPL:
    case Mnemonic::BRK:
    case Mnemonic::BVC:
    case Mnemonic::BVS:
    case Mnemonic::CLC:
    case Mnemonic::CLD:
    case Mnemonic::CLI:
    case Mnemonic::CLV:
    case Mnemonic::CMP:
    case Mnemonic::CPX:
    case Mnemonic::CPY:
    case Mnemonic::DEC:
    case Mnemonic::DEX:
    case Mnemonic::DEY:
    case Mnemonic::EOR:
    case Mnemonic::INC:
    case Mnemonic::INX:
    case Mnemonic::INY:
    case Mnemonic::JMP:
    case Mnemonic::JSR:
    case Mnemonic::LDA:
        _regA = uint8_t(operand & 0xFF);
        CheckSr_S(_regA);
        CheckSr_Z(_regA);
        break;
    case Mnemonic::LDX:
        _regX = uint8_t(operand & 0xFF);
        CheckSr_S(_regX);
        CheckSr_Z(_regX);
        break;
    case Mnemonic::LDY:
        _regY = uint8_t(operand & 0xFF);
        CheckSr_S(_regY);
        CheckSr_Z(_regY);
        break;
    case Mnemonic::LSR:
    case Mnemonic::NOP:
    case Mnemonic::ORA:
    case Mnemonic::PHA:
    case Mnemonic::PHP:
    case Mnemonic::PLA:
    case Mnemonic::PLP:
    case Mnemonic::ROL:
    case Mnemonic::ROR:
    case Mnemonic::RTI:
    case Mnemonic::RTS:
    case Mnemonic::SBC:
    case Mnemonic::SEC:
    case Mnemonic::SED:
    case Mnemonic::SEI:
    case Mnemonic::STA:
    case Mnemonic::STX:
    case Mnemonic::STY:
    case Mnemonic::TAX:
    case Mnemonic::TAY:
    case Mnemonic::TSX:
    case Mnemonic::TXA:
    case Mnemonic::TXS:
    case Mnemonic::TYA:
        break;
    }
}

int Nes6502::Interrupt( /*TODO code?*/)
{
}

