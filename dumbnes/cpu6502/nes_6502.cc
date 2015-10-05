/* nes_6502.cpp
 * definitions for 6502 cpu emulation
 * brendan long
 * Feb 2015
 */

#include "nes_6502.h"

namespace dumbnes { namespace cpu6502 {

using namespace dumbnes::memory;
using namespace dumbnes::opcodes;

inline static bool IsSamePage(uint16_t address_a, uint16_t address_b)
{
    return (address_a & 0xFF) != (address_b & 0xFF);
}

Nes6502::Nes6502(const std::shared_ptr<IMemory>& memory)
    :memory_(memory)
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
    auto next_op = OpInfo::Decode((*memory_)[reg_pc_]);
    std::cout << "PC[0x" << std::hex << reg_pc_ << "] "
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
        return reg_a_;
    case OpMode::Relative:
        b1 = mem[reg_pc_+1];
        // b1 is a signed offset to the PC of the next instruction,
        // hence the +2.
        return 2 + ByteNegative(b1) ? (reg_pc_ - b1) : (reg_pc_ + b1);
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
        return reg_pc_+1;
    case OpMode::ZeroPage:
        return mem[reg_pc_+1];
    case OpMode::ZeroPageX:
        return reg_x_ + mem[reg_pc_+1];
    case OpMode::ZeroPageY:
        return reg_y_ + mem[reg_pc_+1];
    case OpMode::Absolute:
        return (uint16_t)mem[reg_pc_+1] << 8 | mem[reg_pc_+2];
    case OpMode::AbsoluteX:
        b1 = mem[reg_pc_+1];
        b2 = mem[reg_pc_+2];
        return reg_x_ + (uint16_t)b2 << 8 | b1;
    case OpMode::AbsoluteY:
        b1 = mem[reg_pc_+1];
        b2 = mem[reg_pc_+2];
        return reg_y_ + (uint16_t)b2 << 8 | b1;
    case OpMode::Indirect:
        b1 = mem[reg_pc_+1];
        b2 = mem[reg_pc_+2];
        indirect_addr16 = (uint16_t)b2 << 8 | b1;
        b1 = mem[indirect_addr16];
        b2 = mem[indirect_addr16 + 1];
        return (uint16_t)b2 << 8 | b1;
    case OpMode::IndirectX:
        indirect_addr8 = reg_x_ + mem[reg_pc_+1];
        b1 = mem[indirect_addr8];
        b2 = mem[indirect_addr8+1];
        return (uint16_t)b2 << 8 | b1;
    case OpMode::IndirectY:
        b1 = mem[reg_pc_+1];
        b2 = mem[reg_pc_+2];
        indirect_addr16 = (uint16_t)b2 << 8 | b1;
        return reg_y_ + indirect_addr16;
    default:
        return 0x0;
    }
}

Nes6502& Nes6502::SetStatus(uint8_t bit, bool val)
{
    if (val) {
        reg_sr_ |= bit;
    } else {
        reg_sr_ &= ~bit;
    }
    return *this;
}

bool Nes6502::GetStatus(uint8_t reg) const
{
    return (reg_sr_ & reg) != 0;
}

bool Nes6502::IsBranchTaken(const OpInfo& op) const
{
    bool taken = false;
    switch(op.mnemonic) {
    case Mnemonic::BPL: taken = !GetStatus(SR_S); break;
    case Mnemonic::BMI: taken =  GetStatus(SR_S); break;
    case Mnemonic::BVC: taken = !GetStatus(SR_V); break;
    case Mnemonic::BVS: taken =  GetStatus(SR_V); break;
    case Mnemonic::BCC: taken = !GetStatus(SR_C); break;
    case Mnemonic::BCS: taken =  GetStatus(SR_C); break;
    case Mnemonic::BNE: taken = !GetStatus(SR_Z); break;
    case Mnemonic::BEQ: taken =  GetStatus(SR_Z); break;
    default:
        throw std::logic_error("Not a branch op!");
    }
    return taken;
}
 

void Nes6502::ProcessOp(const OpInfo& op)
{
    // op_result tracks work-in-progress results, mainly
    // regarding branching logic.
    // op_result.cycles_run is initially the basic cycle_time
    // of the operation, but may increase, like if a branch is taken.
    // It also tracks the next_pc
    OpResult op_result(reg_pc_, op);
    switch (op.mnemonic)
    {
    case Mnemonic::ADC: ProcessADC(op, op_result); break;
    case Mnemonic::AND: ProcessAND(op, op_result); break;
    case Mnemonic::ASL: ProcessASL(op, op_result); break;
    case Mnemonic::BIT: ProcessBIT(op, op_result); break;
    case Mnemonic::BPL: ProcessBPL(op, op_result); break;
    case Mnemonic::BMI: ProcessBMI(op, op_result); break;
    case Mnemonic::BVC: ProcessBVC(op, op_result); break;
    case Mnemonic::BVS: ProcessBVS(op, op_result); break;
    case Mnemonic::BCC: ProcessBCC(op, op_result); break;
    case Mnemonic::BCS: ProcessBCS(op, op_result); break;
    case Mnemonic::BNE: ProcessBNE(op, op_result); break;
    case Mnemonic::BEQ: ProcessBEQ(op, op_result); break;
    case Mnemonic::BRK: ProcessBRK(op, op_result); break;
    case Mnemonic::CLC: ProcessCLC(op, op_result); break;
    case Mnemonic::CLD: ProcessCLD(op, op_result); break;
    case Mnemonic::CLI: ProcessCLI(op, op_result); break;
    case Mnemonic::CLV: ProcessCLV(op, op_result); break;
    case Mnemonic::CMP: ProcessCMP(op, op_result); break;
    case Mnemonic::CPX: ProcessCPX(op, op_result); break;
    case Mnemonic::CPY: ProcessCPY(op, op_result); break;
    case Mnemonic::DEC: ProcessDEC(op, op_result); break;
    case Mnemonic::DEX: ProcessDEX(op, op_result); break;
    case Mnemonic::DEY: ProcessDEY(op, op_result); break;
    case Mnemonic::EOR: ProcessEOR(op, op_result); break;
    case Mnemonic::INC: ProcessINC(op, op_result); break;
    case Mnemonic::INX: ProcessINX(op, op_result); break;
    case Mnemonic::INY: ProcessINY(op, op_result); break;
    case Mnemonic::JMP: ProcessJMP(op, op_result); break;
    case Mnemonic::JSR: ProcessJSR(op, op_result); break;
    case Mnemonic::LDA: ProcessLDA(op, op_result); break;
    case Mnemonic::LDX: ProcessLDX(op, op_result); break;
    case Mnemonic::LDY: ProcessLDY(op, op_result); break;
    case Mnemonic::LSR: ProcessLSR(op, op_result); break;
    case Mnemonic::NOP: ProcessNOP(op, op_result); break;
    case Mnemonic::ORA: ProcessORA(op, op_result); break;
    case Mnemonic::PHA: ProcessPHA(op, op_result); break;
    case Mnemonic::PHP: ProcessPHP(op, op_result); break;
    case Mnemonic::PLA: ProcessPLA(op, op_result); break;
    case Mnemonic::PLP: ProcessPLP(op, op_result); break;
    case Mnemonic::ROL: ProcessROL(op, op_result); break;
    case Mnemonic::ROR: ProcessROR(op, op_result); break;
    case Mnemonic::RTI: ProcessRTI(op, op_result); break;
    case Mnemonic::RTS: ProcessRTS(op, op_result); break;
    case Mnemonic::SBC: ProcessSBC(op, op_result); break;
    case Mnemonic::SEC: ProcessSEC(op, op_result); break;
    case Mnemonic::SED: ProcessSED(op, op_result); break;
    case Mnemonic::SEI: ProcessSEI(op, op_result); break;
    case Mnemonic::STA: ProcessSTA(op, op_result); break;
    case Mnemonic::STX: ProcessSTX(op, op_result); break;
    case Mnemonic::STY: ProcessSTY(op, op_result); break;
    case Mnemonic::TAX: ProcessTAX(op, op_result); break;
    case Mnemonic::TAY: ProcessTAY(op, op_result); break;
    case Mnemonic::TSX: ProcessTSX(op, op_result); break;
    case Mnemonic::TXA: ProcessTXA(op, op_result); break;
    case Mnemonic::TXS: ProcessTXS(op, op_result); break;
    case Mnemonic::TYA: ProcessTYA(op, op_result); break;
    default:
        std::stringstream ss;
        ss << "unrecognized opcode: "
           << std::hex << (long unsigned)op.mnemonic;
        throw std::logic_error(ss.str());
    }
    cycles_run_ += CyclesTaken(op, op_result);
    reg_pc_ = op_result.next_pc;
}


size_t Nes6502::CyclesTaken(OpInfo const& op, OpResult const& result)
{
    size_t incremental_time = result.cycles_run;
    if (op.add_1_for_branch_taken && result.branch_taken)
    {
        incremental_time++;
    }
    if (op.add_1_for_page_crossing && result.page_crossed)
    {
        incremental_time++;
    }
    return incremental_time;
}

int Nes6502::Interrupt( /*TODO code?*/)
{
}

void Nes6502::HelpProcessBranch(const OpInfo& op,
        OpResult& result)
{
    auto operand = FetchOperand(op.mode, *memory_);
    if (result.branch_taken) {
        int8_t branch_displacement = (int8_t)(operand & 0xFF);
        uint16_t target = result.next_pc + branch_displacement;
        result.page_crossed = !IsSamePage(result.next_pc, target);
        result.next_pc = target;
    }
}

/******************************************************************************/
/* Below here, it's all opcode handlers                                       */
/******************************************************************************/

void Nes6502::ProcessADC(const OpInfo& op, OpResult& result)
{
    auto operand = FetchOperand(op.mode, *memory_);
    if (GetStatus(SR_D))
    {
        operand = DecodeBCD(operand);
    }
    auto add_result = reg_a_ + operand;
    SetStatus(SR_S, ByteNegative(add_result));
    SetStatus(SR_V, add_result < reg_a_);
    SetStatus(SR_Z, add_result == 0);
    SetStatus(SR_C, add_result < reg_a_);
    reg_a_ = add_result;
}

void Nes6502::ProcessAND(const OpInfo& op, OpResult& result)
{
    auto operand = FetchOperand(op.mode, *memory_);
    auto and_result = reg_a_ & operand;
    SetStatus(SR_S, ByteNegative(and_result));
    SetStatus(SR_Z, and_result == reg_a_);
    reg_a_ = and_result;
}

void Nes6502::ProcessASL(const OpInfo& op, OpResult& result)
{
    auto operand = FetchOperand(op.mode, *memory_);
    SetStatus(SR_C, (reg_a_ & 0x80) != 0);
    auto shift_result = reg_a_ << 1;
    SetStatus(SR_S, ByteNegative(shift_result));
    SetStatus(SR_Z, shift_result == 0);
    reg_a_ = shift_result;
}

void Nes6502::ProcessBPL(const OpInfo& op, OpResult& result)
{
    result.branch_taken = !GetStatus(SR_S);
    HelpProcessBranch(op, result);
}
void Nes6502::ProcessBMI(const OpInfo& op, OpResult& result)
{
    result.branch_taken = GetStatus(SR_S);
    HelpProcessBranch(op, result);
}
void Nes6502::ProcessBVC(const OpInfo& op, OpResult& result)
{
    result.branch_taken = !GetStatus(SR_V);
    HelpProcessBranch(op, result);
}
void Nes6502::ProcessBVS(const OpInfo& op, OpResult& result)
{
    result.branch_taken = GetStatus(SR_V);
    HelpProcessBranch(op, result);
}
void Nes6502::ProcessBCC(const OpInfo& op, OpResult& result)
{
    result.branch_taken = !GetStatus(SR_C);
    HelpProcessBranch(op, result);
}
void Nes6502::ProcessBCS(const OpInfo& op, OpResult& result)
{
    result.branch_taken = GetStatus(SR_C);
    HelpProcessBranch(op, result);
}
void Nes6502::ProcessBNE(const OpInfo& op, OpResult& result)
{
    result.branch_taken = !GetStatus(SR_Z);
    HelpProcessBranch(op, result);
}
void Nes6502::ProcessBEQ(const OpInfo& op, OpResult& result)
{
    result.branch_taken = GetStatus(SR_Z);
    HelpProcessBranch(op, result);
}

void Nes6502::ProcessBRK(const OpInfo& op, OpResult& result)
{//TODO
}

void Nes6502::ProcessBIT(const OpInfo& op, OpResult& result)
{
    auto operand = FetchOperand(op.mode, *memory_);
    SetStatus(SR_Z, operand == 0);
    SetStatus(SR_S, ByteNegative(operand));
    SetStatus(SR_V, (operand & 0x40) != 0);
}

void Nes6502::ProcessCLC(const OpInfo& op, OpResult& result)
{
    SetStatus(SR_C, false);
}
void Nes6502::ProcessCLD(const OpInfo& op, OpResult& result)
{
    SetStatus(SR_D, false);
}
void Nes6502::ProcessCLI(const OpInfo& op, OpResult& result)
{
    SetStatus(SR_I, false);
}
void Nes6502::ProcessCLV(const OpInfo& op, OpResult& result)
{
    SetStatus(SR_V, false);
}
void Nes6502::ProcessCMP(const OpInfo& op, OpResult& result)
{
    // Compare sets flags as if a subtraction had been carried out.
    // If the value in the accumulator is equal or greater than the compared value,
    // the Carry will be set. 
    // The equal (Z) and sign (S) flags will be set based on equality
    // or lack thereof and the sign (i.e. A>=$80) of the accumulator. 
    auto operand = FetchOperand(op.mode, *memory_);
    SetStatus(SR_Z, (operand == reg_a_));
    SetStatus(SR_C, (reg_a_ >= operand));
    SetStatus(SR_S, ((int)reg_a_ < (int)operand));
}
void Nes6502::ProcessCPX(const OpInfo& op, OpResult& result)
{
    auto operand = FetchOperand(op.mode, *memory_);
    SetStatus(SR_Z, (operand == reg_x_));
    SetStatus(SR_C, (reg_x_ >= operand));
    SetStatus(SR_S, ((int)reg_x_ < (int)operand));
}

void Nes6502::ProcessCPY(const OpInfo& op, OpResult& result)
{
    auto operand = FetchOperand(op.mode, *memory_);
    SetStatus(SR_Z, (operand == reg_y_));
    SetStatus(SR_C, (reg_y_ >= operand));
    SetStatus(SR_S, ((int)reg_y_ < (int)operand));
}

void Nes6502::ProcessDEC(const OpInfo& op, OpResult& result)
{//TODO
}
void Nes6502::ProcessDEX(const OpInfo& op, OpResult& result)
{//TODO
}
void Nes6502::ProcessDEY(const OpInfo& op, OpResult& result)
{//TODO
}
void Nes6502::ProcessEOR(const OpInfo& op, OpResult& result)
{//TODO
}
void Nes6502::ProcessINC(const OpInfo& op, OpResult& result)
{//TODO
}
void Nes6502::ProcessINX(const OpInfo& op, OpResult& result)
{//TODO
}
void Nes6502::ProcessINY(const OpInfo& op, OpResult& result)
{//TODO
}
void Nes6502::ProcessJMP(const OpInfo& op, OpResult& result)
{//TODO
}
void Nes6502::ProcessJSR(const OpInfo& op, OpResult& result)
{//TODO
}

void Nes6502::ProcessLDA(const OpInfo& op, OpResult& result)
{
    auto operand = FetchOperand(op.mode, *memory_);
    reg_a_ = uint8_t(operand & 0xFF);
    SetStatus(SR_S, ByteNegative(reg_a_));
    SetStatus(SR_Z, reg_a_ == 0);
}

void Nes6502::ProcessLDX(const OpInfo& op, OpResult& result)
{
    auto operand = FetchOperand(op.mode, *memory_);
    reg_x_ = uint8_t(operand & 0xFF);
    SetStatus(SR_S, ByteNegative(reg_x_));
    SetStatus(SR_Z, reg_x_ == 0);
}

void Nes6502::ProcessLDY(const OpInfo& op, OpResult& result)
{
    auto operand = FetchOperand(op.mode, *memory_);
    reg_y_ = uint8_t(operand & 0xFF);
    SetStatus(SR_S, ByteNegative(reg_y_));
    SetStatus(SR_Z, reg_y_ == 0);
}

void Nes6502::ProcessLSR(const OpInfo& op, OpResult& result)
{//TODO
}
void Nes6502::ProcessNOP(const OpInfo& op, OpResult& result)
{//TODO
}
void Nes6502::ProcessORA(const OpInfo& op, OpResult& result)
{//TODO
}
void Nes6502::ProcessPHA(const OpInfo& op, OpResult& result)
{//TODO
}
void Nes6502::ProcessPHP(const OpInfo& op, OpResult& result)
{//TODO
}
void Nes6502::ProcessPLA(const OpInfo& op, OpResult& result)
{//TODO
}
void Nes6502::ProcessPLP(const OpInfo& op, OpResult& result)
{//TODO
}
void Nes6502::ProcessROL(const OpInfo& op, OpResult& result)
{//TODO
}
void Nes6502::ProcessROR(const OpInfo& op, OpResult& result)
{//TODO
}
void Nes6502::ProcessRTI(const OpInfo& op, OpResult& result)
{//TODO
}
void Nes6502::ProcessRTS(const OpInfo& op, OpResult& result)
{//TODO
}
void Nes6502::ProcessSBC(const OpInfo& op, OpResult& result)
{//TODO
}

void Nes6502::ProcessSEC(const OpInfo& op, OpResult& result)
{
    SetStatus(SR_C, true);
}

void Nes6502::ProcessSED(const OpInfo& op, OpResult& result)
{//TODO
}
void Nes6502::ProcessSEI(const OpInfo& op, OpResult& result)
{//TODO
}
void Nes6502::ProcessSTA(const OpInfo& op, OpResult& result)
{//TODO
}
void Nes6502::ProcessSTX(const OpInfo& op, OpResult& result)
{//TODO
}
void Nes6502::ProcessSTY(const OpInfo& op, OpResult& result)
{//TODO
}
void Nes6502::ProcessTAX(const OpInfo& op, OpResult& result)
{//TODO
}
void Nes6502::ProcessTAY(const OpInfo& op, OpResult& result)
{//TODO
}
void Nes6502::ProcessTSX(const OpInfo& op, OpResult& result)
{//TODO
}
void Nes6502::ProcessTXA(const OpInfo& op, OpResult& result)
{//TODO
}
void Nes6502::ProcessTXS(const OpInfo& op, OpResult& result)
{//TODO
}
void Nes6502::ProcessTYA(const OpInfo& op, OpResult& result)
{//TODO
}

}}