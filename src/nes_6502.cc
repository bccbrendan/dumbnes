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

uint16_t Nes6502::FetchOperand(const OpMode& opmode, IMemory& mem)
{
    // reference: http://nesdev.com/6502.txt
    uint8_t b1 = 0;
    switch(opmode)
    {
    case OpMode::Immediate:
    case OpMode::ZeroPage:
    case OpMode::ZeroPageX:
    case OpMode::ZeroPageY:
    case OpMode::Absolute:
    case OpMode::AbsoluteX:
    case OpMode::AbsoluteY:
    case OpMode::Indirect:
    case OpMode::IndirectX:
    case OpMode::IndirectY:
        return mem[DecodeAddress(opmode, mem)];
    case OpMode::Accumulator:
        return _regA;
    case OpMode::Relative:
        b1 = mem[_regPC+1];
        // b1 is a signed offset to the PC of the next instruction,
        // hence the +2.
        return 2 + ByteNegative(b1) ? (_regPC - b1) : (_regPC + b1);
    default:
        // TODO throw exception?
        return 0; // not really valid - no operand.
    }
}

/* Compute, load, indirectly determine, etc
 * the address of an operand, or store target. */
uint16_t
Nes6502::DecodeAddress(const OpMode& opmode, IMemory& mem)
{
    // reference: http://nesdev.com/6502.txt
    uint8_t b1 = 0x0; // 1st byte after current instruction
    uint8_t b2 = 0x0; // 2nd byte after current instruction
    uint16_t indirect_addr16 = 0x0;
    uint16_t indirect_addr8 = 0x0;
    switch(opmode)
    {
    case OpMode::Immediate:
        return _regPC+1;
    case OpMode::ZeroPage:
        return mem[_regPC+1];
    case OpMode::ZeroPageX:
        return _regX + mem[_regPC+1];
    case OpMode::ZeroPageY:
        return _regY + mem[_regPC+1];
    case OpMode::Absolute:
        return (uint16_t)mem[_regPC+1] << 8 | mem[_regPC+2]; 
    case OpMode::AbsoluteX:
        b1 = mem[_regPC+1];
        b2 = mem[_regPC+2];
        return _regX + (uint16_t)b2 << 8 | b1;
    case OpMode::AbsoluteY:
        b1 = mem[_regPC+1];
        b2 = mem[_regPC+2];
        return _regY + (uint16_t)b2 << 8 | b1;
    case OpMode::Indirect:
        b1 = mem[_regPC+1];
        b2 = mem[_regPC+2];
        indirect_addr16 = (uint16_t)b2 << 8 | b1;
        b1 = mem[indirect_addr16];
        b2 = mem[indirect_addr16 + 1];
        return (uint16_t)b2 << 8 | b1;
    case OpMode::IndirectX:
        indirect_addr8 = _regX + mem[_regPC+1];
        b1 = mem[indirect_addr8];
        b2 = mem[indirect_addr8+1];
        return (uint16_t)b2 << 8 | b1;
    case OpMode::IndirectY:
        b1 = mem[_regPC+1];
        b2 = mem[_regPC+2];
        indirect_addr16 = (uint16_t)b2 << 8 | b1;
        return _regY + indirect_addr16;
    default:
        return 0x0;
    }
}

Nes6502& Nes6502::SetStatus(uint8_t bit, bool val)
{
    if (val) {
        _regSR |= bit;
    } else {
        _regSR &= ~bit;
    }
    return *this;
}

bool Nes6502::GetStatus(uint8_t reg) const
{
    return (_regSR & reg) != 0;
}

void Nes6502::ProcessOp(const OpInfo& op)
{
    uint8_t operand = 0;
    uint8_t result = 0;
    switch (op.mnemonic)
    {
    case Mnemonic::ADC: 
        operand = FetchOperand(op.mode, *_memory);
        if (GetStatus(SR_D))
        {
            operand = DecodeBCD(operand);
        }
        result = _regA + operand;
        SetStatus(SR_S, ByteNegative(result));
        SetStatus(SR_V, result < _regA);
        SetStatus(SR_Z, result == 0);
        SetStatus(SR_C, result < _regA);
        _regA = result;
        break;
    case Mnemonic::AND:
        operand = FetchOperand(op.mode, *_memory);
        result = _regA & operand;
        SetStatus(SR_S, ByteNegative(result));
        SetStatus(SR_Z, result == _regA);
        break;
    case Mnemonic::ASL:
        operand = FetchOperand(op.mode, *_memory);
        SetStatus(SR_C, (_regA & 0x80) != 0);
        result = _regA << 1;
        SetStatus(SR_S, ByteNegative(result));
        SetStatus(SR_Z, result == 0);
        _regA = result;
        break;
    case Mnemonic::BIT:
        operand = FetchOperand(op.mode, *_memory);
        SetStatus(SR_Z, operand == 0);
        SetStatus(SR_S, ByteNegative(operand));
        SetStatus(SR_V, (operand & 0x40) != 0);
        break;

    case Mnemonic::BCC:
    case Mnemonic::BCS:
    case Mnemonic::BEQ:
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
        SetStatus(SR_S, ByteNegative(_regA));
        SetStatus(SR_Z, _regA == 0);
        break;
    case Mnemonic::LDX:
        _regX = uint8_t(operand & 0xFF);
        SetStatus(SR_S, ByteNegative(_regX));
        SetStatus(SR_Z, _regX == 0);
       break;
    case Mnemonic::LDY:
        _regY = uint8_t(operand & 0xFF);
        SetStatus(SR_S, ByteNegative(_regY));
        SetStatus(SR_Z, _regY == 0);
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

