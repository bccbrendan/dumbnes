/* nes_6502.cpp
 * definitions for 6502 cpu emulation
 * brendan long
 * Feb 2015
 */

/* super reference here:
 * http://homepage.ntlworld.com/cyborgsystems/CS_Main/6502/6502.htm
 */
#include "dumbnes_includes.h"
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
    , nmi_req_(false)
{
    reg_sp_ = 0xFD;
}

Nes6502::~Nes6502(void)
{
}

void Nes6502::Reset (void)
{
    reg_sp_ = 0xFD - 3;
    reg_a_ = 0;
    reg_x_ = 0;
    reg_y_ = 0;
    reg_pc_ = memory_->R16(0xFFFC);
    nmi_req_ = false;
}

void Nes6502::SetNmiReq(bool set) {
    nmi_req_ = set;
    LOG(DEBUG) << "nmi: " << set;
}

size_t Nes6502::AsmStep(void)
{
    size_t cycles_run = 0;
    bool had_nmi = nmi_req_;
    if (nmi_req_) {
        cycles_run += NMI();
    }
    auto next_op = OpInfo::Decode(memory_->R(reg_pc_));
    LOG(DEBUG) << std::hex << "PC[0x"  << reg_pc_ << "] " << next_op;
    cycles_run += ProcessOp(next_op);
    LOG(DEBUG) << std::hex << "processed op " << (had_nmi ? " and NMI " : "") << " in " << int(cycles_run) << " cycles";
    return cycles_run;
}

size_t Nes6502::NMI(void)
{
    // behavior documented here: 
    // https://www.pagetable.com/?p=410
    size_t cycles_taken = 2; 
    PushByte(reg_pc_ >> 8);
    PushByte(reg_pc_ & 0xFF);
    PushByte(reg_sr_);
    SetStatus(SR_I, 1);
    reg_pc_ = memory_->R16(0xFFFA);
    nmi_req_ = false;
    LOG(DEBUG) << "Handling NMI - jump to 0x" << reg_pc_;
    return cycles_taken;
}

uint8_t Nes6502::FetchOperand8(const OpMode& opmode,
        IMemory& mem,
        OpResult &result)
{
    // reference: http://nesdev.com/6502.txt
    uint8_t b1 = 0;
    uint8_t operand = 0;
    uint16_t address = 0;
    switch(opmode)
    {
    case OpMode::Accumulator:
        operand = reg_a_;
        break;
    case OpMode::Immediate:
        operand = mem.R(DecodeAddress(opmode, mem, result));
        break;
    case OpMode::ZeroPage:
        operand = mem.R(reg_pc_+1);
        break;
    // the _value_ read
    case OpMode::Absolute:
        address = mem.R16(reg_pc_+1);
        operand = mem.R(address);
        break;
    case OpMode::AbsoluteX:
        address = reg_x_ + mem.R16(reg_pc_+1);
        operand = mem.R(address);
        break;
    case OpMode::AbsoluteY:
        address = reg_y_ + mem.R16(reg_pc_+1);
        operand = mem.R(address);
        break;
    case OpMode::ZeroPageX:
    case OpMode::ZeroPageY:
    case OpMode::Relative:
    case OpMode::Implied: // no operand
    case OpMode::Indirect:
    case OpMode::IndirectX:
    case OpMode::IndirectY:
    default:
        throw std::logic_error("Invalid operand mode");
    }
    LOG(DEBUG) << std::hex << "operand: 0x" << int(operand);
    return operand;
}

uint16_t Nes6502::FetchOperand16(const OpMode& opmode,
        IMemory& mem,
        OpResult &result)
{
    // reference: http://nesdev.com/6502.txt
    uint8_t b1 = 0;
    uint16_t address = 0;
    uint16_t operand = 0;
    int8_t offset = 0;
    switch(opmode)
    {
    case OpMode::Absolute:
        // Absolute for LDA: operand is absolute address to load, like $3000
        // Immediate for LDA: operand is _value_ to load, like #$44
        // Absolute for STA - address to store to.
        operand = mem.R16(reg_pc_+1);
        break;
    case OpMode::Indirect:
        operand = DecodeAddress(opmode, mem, result);
        break;
    case OpMode::AbsoluteX:
    case OpMode::AbsoluteY:
        operand = DecodeAddress(opmode, mem, result);
        break;
    case OpMode::ZeroPageX:
    case OpMode::ZeroPageY:
    case OpMode::Relative:
        b1 = mem.R(reg_pc_+1);
        offset = static_cast<int8_t>(b1);  // get signed version
        // hence the +2.
        operand = 2 + reg_pc_ + offset;
        break;
    case OpMode::IndirectX:
    case OpMode::IndirectY:
        address = DecodeAddress(opmode, mem, result);
        operand = address;
        break;
    case OpMode::Accumulator:  // 8 bit
    case OpMode::Immediate:  // 8 bit
    case OpMode::Implied:   // no operand
    case OpMode::ZeroPage: // 8 bit
    default:
        throw std::logic_error("Invalid operand mode");
    }
    LOG(DEBUG) << "operand: 0x" << std::hex << operand;
    return operand;
}


/* Compute, load, indirectly determine, etc
 * the address of an opand, load, or store target. */
uint16_t
Nes6502::DecodeAddress(const OpMode& opmode, IMemory& mem, OpResult &result)
{
    // reference: http://nesdev.com/6502.txt
    uint8_t b1 = 0x0; // 1st byte after current instruction
    uint8_t b2 = 0x0; // 2nd byte after current instruction
    uint16_t indirect_addr16 = 0x0;
    uint16_t indirect_addr = 0x0;
    uint16_t address = 0;
    switch(opmode)
    {
        // NOTE - for Immediate, this returns the address where we can get the operand.
        // but for 'Absolute, this returns the address where we can get the address, where we can get the operand.
        // TODO - clean that up
        case OpMode::Immediate:
            address = reg_pc_+1;
            break;
        case OpMode::ZeroPage:
            address = mem.R(reg_pc_+1);
            break;
        case OpMode::ZeroPageX:
            address = reg_x_ + mem.R(reg_pc_+1);
            break;
        case OpMode::ZeroPageY:
            address = reg_y_ + mem.R(reg_pc_+1);
            break;
        case OpMode::Absolute:
            address = mem.R16(reg_pc_+1);
            break;
        case OpMode::AbsoluteX:
            address = reg_x_ + mem.R16(reg_pc_+1);
            break;
        case OpMode::AbsoluteY:
            address = reg_y_ + mem.R16(reg_pc_+1);
            break;
        case OpMode::Indirect:
            indirect_addr16 = mem.R16(reg_pc_+1);
            address = mem.R16(indirect_addr16);
            break;
        case OpMode::IndirectX:
            indirect_addr = reg_x_ + mem.R(reg_pc_+1);
            address = mem.R16(indirect_addr);
            break;
        case OpMode::IndirectY:
            indirect_addr = reg_y_ + mem.R(reg_pc_+1);
            address = mem.R16(indirect_addr);
            break;
        default:
            LOG(WARNING) << "Invalid opmode";
            address = 0x0;
    }
    result.page_crossed |= !IsSamePage(result.next_pc, address);
    return address;
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
 

size_t Nes6502::ProcessOp(const OpInfo& op)
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
    auto cycles_taken = CyclesTaken(op, op_result);
    cycles_run_ += cycles_taken;
    reg_pc_ = op_result.next_pc;
    LOG(DEBUG) << "nextpc: " << std::hex << op_result.next_pc << " op.cycles_taken: " << cycles_taken;
    return cycles_taken;
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

void Nes6502::HelpProcessBranch(const OpInfo& op,
        OpResult& result)
{
    uint16_t target = FetchOperand16(op.mode, *memory_, result);
    if (result.branch_taken) {
        result.page_crossed |= !IsSamePage(result.next_pc, target);
        result.next_pc = target;
    }
}
/*from:
 *https://sites.google.com/site/6502asembly/
 *6502-architecture/registerstack-pointer
 */
void Nes6502::PushByte(uint8_t b)
{
    /* The first data are stored at $01FF and so on. Pushing data to the stack
     * causes the Stack Pointer (SP) to be decremented towards from $FF to $00
     * (mapping to address $01FF to $0100). Conversely pulling bytes causes 
     * it to be incremented towards form $00 to $FF.
     */
    LOG(DEBUG) << "push stack pointer: " << std::hex << int(reg_sp_) << "--";
    memory_->W(0x100 + reg_sp_, b);
    reg_sp_--;
}

uint8_t Nes6502::PopByte(void)
{
    LOG(DEBUG) << "pop stack pointer: " << std::hex << int(reg_sp_) << "++";
    reg_sp_++;
    return memory_->R(0x100 + reg_sp_);
}


/******************************************************************************/
/* Below here, it's all opcode handlers                                       */
/******************************************************************************/

void Nes6502::ProcessADC(const OpInfo& op, OpResult& result)
{
    // TODO detect page boundary crossings!!!
    uint8_t addend = reg_a_;
    uint8_t augend = FetchOperand8(op.mode, *memory_, result);
    uint16_t sum = addend + augend + (GetStatus(SR_C) ? 1 : 0);
    SetStatus(SR_V, ByteNegative(addend) != ByteNegative(sum));
    SetStatus(SR_S, ByteNegative(addend));
    SetStatus(SR_Z, sum == 0);
    if (GetStatus(SR_D))
    {
        sum = DecodeBCD(addend)
            + DecodeBCD(augend)
            + (GetStatus(SR_C) ? 1 : 0);
        SetStatus(SR_C, sum > 99);
    }
    else
    {
        SetStatus(SR_C, sum > 255);
    }
    reg_a_ = (uint8_t)(sum & 0xFF);
}

void Nes6502::ProcessAND(const OpInfo& op, OpResult& result)
{
    uint8_t operand = FetchOperand8(op.mode, *memory_, result);
    uint8_t and_result = reg_a_ & operand;
    SetStatus(SR_S, ByteNegative(and_result));
    SetStatus(SR_Z, and_result == reg_a_);
    reg_a_ = and_result;
}

void Nes6502::ProcessASL(const OpInfo& op, OpResult& result)
{
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
{
    uint16_t return_address = reg_pc_ + 1;
    PushByte((uint8_t)(return_address >> 8));
    PushByte((uint8_t)(return_address & 0xFF));
    uint16_t l = memory_->R(0xFFFE);
    uint16_t h = memory_->R(0xFFFF) << 8;
    /// TODO set B flag
    // TODO push P 
    result.next_pc = h | l;
}

void Nes6502::ProcessBIT(const OpInfo& op, OpResult& result)
{
    uint16_t operand_addr = 0;
    if (op.mode == OpMode::ZeroPage) {
        operand_addr = memory_->R(reg_pc_ + 1);
    } else if (op.mode == OpMode::Absolute) {
        operand_addr = memory_->R16(reg_pc_ + 1);
    } else {
        throw std::logic_error("invalid op.mode for BIT");
    }
    // operand_addr should be reg_pc + 1, and take 1 or two bytes
    uint8_t operand = memory_->R(operand_addr);
    LOG(DEBUG) << "BIT $" << std::hex << operand_addr << " : " << int(operand);
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
    uint8_t operand = FetchOperand8(op.mode, *memory_, result);
    SetStatus(SR_Z, (operand == reg_a_));
    SetStatus(SR_C, (reg_a_ >= operand));
    SetStatus(SR_S, ((int)reg_a_ < (int)operand));
}
void Nes6502::ProcessCPX(const OpInfo& op, OpResult& result)
{
    uint8_t operand = FetchOperand8(op.mode, *memory_, result);
    SetStatus(SR_Z, (operand == reg_x_));
    SetStatus(SR_C, (reg_x_ >= operand));
    SetStatus(SR_S, ((int)reg_x_ < (int)operand));
}

void Nes6502::ProcessCPY(const OpInfo& op, OpResult& result)
{
    uint8_t operand = FetchOperand8(op.mode, *memory_, result);
    SetStatus(SR_Z, (operand == reg_y_));
    SetStatus(SR_C, (reg_y_ >= operand));
    SetStatus(SR_S, ((int)reg_y_ < (int)operand));
}

void Nes6502::ProcessDEC(const OpInfo& op, OpResult& result)
{
    auto target_address = DecodeAddress(op.mode, *memory_, result);
    auto dec_result =  memory_->R(target_address) - 1;
    SetStatus(SR_Z, (dec_result == 0));
    SetStatus(SR_S, (ByteNegative(dec_result)));
    memory_->W(target_address, dec_result);
}

void Nes6502::ProcessDEX(const OpInfo& op, OpResult& result)
{
    auto dec_result =  reg_x_ - 1;
    SetStatus(SR_Z, (dec_result == 0));
    SetStatus(SR_S, (ByteNegative(dec_result)));
    reg_x_ = dec_result;
}

void Nes6502::ProcessDEY(const OpInfo& op, OpResult& result)
{
    auto dec_result =  reg_y_ - 1;
    SetStatus(SR_Z, (dec_result == 0));
    SetStatus(SR_S, (ByteNegative(dec_result)));
    reg_y_ = dec_result;
}

void Nes6502::ProcessEOR(const OpInfo& op, OpResult& result)
{
    uint8_t operand = FetchOperand8(op.mode, *memory_, result);
    uint8_t eor_result = reg_a_ ^ operand;
    SetStatus(SR_S, ByteNegative(eor_result));
    SetStatus(SR_Z, eor_result == reg_a_);
    reg_a_ = eor_result;
}

void Nes6502::ProcessINC(const OpInfo& op, OpResult& result)
{
    uint16_t target_address = DecodeAddress(op.mode, *memory_, result);
    uint8_t inc_result =  memory_->R(target_address) + 1;
    SetStatus(SR_Z, (inc_result == 0));
    SetStatus(SR_S, (ByteNegative(inc_result)));
    memory_->W(target_address, inc_result);
}

void Nes6502::ProcessINX(const OpInfo& op, OpResult& result)
{
    uint8_t inc_result =  reg_x_ + 1;
    SetStatus(SR_Z, (inc_result == 0));
    SetStatus(SR_S, (ByteNegative(inc_result)));
    reg_x_ = inc_result;
}

void Nes6502::ProcessINY(const OpInfo& op, OpResult& result)
{
    auto inc_result =  reg_y_ + 1;
    SetStatus(SR_Z, (inc_result == 0));
    SetStatus(SR_S, (ByteNegative(inc_result)));
    reg_y_ = inc_result;
}

void Nes6502::ProcessJMP(const OpInfo& op, OpResult& result)
{
    uint16_t operand = FetchOperand16(op.mode, *memory_, result);
    LOG_IF(op.mode == OpMode::Absolute, DEBUG) << "JMP $" << operand;
    LOG_IF(op.mode == OpMode::Indirect, DEBUG) << "JMP i$" << operand;
    result.next_pc = operand;
}

void Nes6502::ProcessJSR(const OpInfo& op, OpResult& result)
{
    auto pc = reg_pc_;
    uint16_t return_address = reg_pc_ + 2; // jsr is 3 bytes, and we subtract 1 before pushing (and add 1 after pop)
    PushByte((return_address & 0xFF00) >> 8); // msb first
    PushByte(return_address & 0xFF);
    uint16_t operand = FetchOperand16(op.mode, *memory_, result);
    result.next_pc = operand;
    LOG(DEBUG) << std::hex << "0x" << pc << ": JSR 0x" << operand << " (pushed return target 0x" << return_address << ")";
}

void Nes6502::ProcessLDA(const OpInfo& op, OpResult& result)
{
    uint8_t operand = FetchOperand8(op.mode, *memory_, result);
    reg_a_ = operand;
    SetStatus(SR_S, ByteNegative(reg_a_));
    SetStatus(SR_Z, reg_a_ == 0);
    LOG(DEBUG) << reg_pc_ << ": LDA a <- 0x" << int(reg_a_) << "]";
}

void Nes6502::ProcessLDX(const OpInfo& op, OpResult& result)
{
    uint8_t operand = FetchOperand8(op.mode, *memory_, result);
    reg_x_ = operand;
    SetStatus(SR_S, ByteNegative(reg_x_));
    SetStatus(SR_Z, reg_x_ == 0);
}

void Nes6502::ProcessLDY(const OpInfo& op, OpResult& result)
{
    uint8_t operand = FetchOperand8(op.mode, *memory_, result);
    reg_y_ = operand;
    SetStatus(SR_S, ByteNegative(reg_y_));
    SetStatus(SR_Z, reg_y_ == 0);
}

void Nes6502::ProcessLSR(const OpInfo& op, OpResult& result)
{
    bool carry = (reg_a_ & 0x1) == 0x1;
    reg_a_ = reg_a_ >> 1;
    SetStatus(SR_Z, reg_a_ == 0);
    SetStatus(SR_S, ByteNegative(reg_a_));
    SetStatus(SR_C, carry);
}

void Nes6502::ProcessNOP(const OpInfo& op, OpResult& result)
{
}

void Nes6502::ProcessORA(const OpInfo& op, OpResult& result)
{
    uint8_t rhs = FetchOperand8(op.mode, *memory_, result);
    reg_a_ = reg_a_ | rhs;
    SetStatus(SR_S, ByteNegative(reg_a_));
    SetStatus(SR_Z, reg_a_ == 0);
}

void Nes6502::ProcessPHA(const OpInfo& op, OpResult& result)
{
    PushByte(reg_a_);
}

void Nes6502::ProcessPHP(const OpInfo& op, OpResult& result)
{
    PushByte(reg_sr_);
}

void Nes6502::ProcessPLA(const OpInfo& op, OpResult& result)
{
    reg_a_ = PopByte();
}

void Nes6502::ProcessPLP(const OpInfo& op, OpResult& result)
{
    reg_sr_ = PopByte();
}

void Nes6502::ProcessROL(const OpInfo& op, OpResult& result)
{
    if (op.mode != OpMode::Accumulator)
    {
        // shift data in memory in place by accumulator.
        uint16_t target = DecodeAddress(op.mode, *memory_, result);
        uint8_t base = memory_->R(target);
        uint8_t shifted = (uint8_t)((base << 1) | GetStatus(SR_C) ? 1 : 0);
        memory_->W(target, shifted);
        SetStatus(SR_C, ByteNegative(base));
        SetStatus(SR_Z, shifted == 0);
        SetStatus(SR_S, ByteNegative(shifted));
    }
    else
    {
        // rotate accumulator by operand
        uint8_t base = reg_a_;
        uint8_t shifted = (uint8_t)((base << 1) | GetStatus(SR_C) ? 1 : 0);
        reg_a_ = shifted;
        SetStatus(SR_C, ByteNegative(base));
        SetStatus(SR_Z, shifted == 0);
        SetStatus(SR_S, ByteNegative(shifted));
    }
}

void Nes6502::ProcessROR(const OpInfo& op, OpResult& result)
{
    if (op.mode != OpMode::Accumulator)
    {
        // shift data in memory in place by accumulator.
        uint16_t target = DecodeAddress(op.mode, *memory_, result);
        uint8_t base = memory_->R(target);
        uint8_t shifted = (uint8_t)((base >> 1) | GetStatus(SR_C) ? 0x80 : 0);
        memory_->W(target, shifted);
        SetStatus(SR_C, base & 0x1);
        SetStatus(SR_Z, shifted == 0);
        SetStatus(SR_S, ByteNegative(shifted));
    }
    else
    {
        // rotate accumulator by operand
        uint8_t base = reg_a_;
        uint8_t shifted = (uint8_t)((base >> 1) | GetStatus(SR_C) ? 0x80 : 0);
        reg_a_ = shifted;
        SetStatus(SR_C, base & 0x1);
        SetStatus(SR_Z, shifted == 0);
        SetStatus(SR_S, ByteNegative(shifted));
    }

}
void Nes6502::ProcessRTI(const OpInfo& op, OpResult& result)
{
    reg_sr_ = PopByte();
    uint16_t l = PopByte();
    uint16_t h = PopByte();
    uint16_t return_address = (h << 8) | l;
    result.next_pc = return_address;
    LOG(DEBUG) << "0x " << reg_pc_ << ":RTI jumping back to 0x" << result.next_pc;
}

void Nes6502::ProcessRTS(const OpInfo& op, OpResult& result)
{
    uint16_t l = PopByte();
    uint16_t h = PopByte();
    uint16_t return_address = ((h << 8) | l) + 1;
    result.next_pc = return_address;
    LOG(DEBUG) << std::hex << "0x" << reg_pc_ << ": RTS (0x" << return_address << ")";
}

void Nes6502::ProcessSBC(const OpInfo& op, OpResult& result)
{
    auto minuend = reg_a_;
    uint8_t subtrahend = FetchOperand8(op.mode, *memory_, result);
    int difference = 0;
    if (GetStatus(SR_D))
    {
        minuend = DecodeBCD(minuend);
        subtrahend = DecodeBCD(subtrahend);
        difference = (minuend - subtrahend) - (GetStatus(SR_C) ? 1 : 0);
        SetStatus(SR_V, (difference > 99) || (difference < 0));
    }
    else
    {
        difference = (minuend - subtrahend) - (GetStatus(SR_C) ? 1 : 0);
        SetStatus(SR_V, (difference > 127) || (difference < -128));
    }
    /* In either mode, after a SBC, the carry is clear
     * if the result was less than 0 and set otherwise.
     */
    SetStatus(SR_C, difference >= 0);
    SetStatus(SR_S, ByteNegative(difference));
    SetStatus(SR_Z, difference == 0);
    reg_a_ = (uint8_t) difference & 0xFF;
}

void Nes6502::ProcessSEC(const OpInfo& op, OpResult& result)
{
    SetStatus(SR_C, true);
}

void Nes6502::ProcessSED(const OpInfo& op, OpResult& result)
{
    SetStatus(SR_D, true);
}

void Nes6502::ProcessSEI(const OpInfo& op, OpResult& result)
{
    SetStatus(SR_I, true);
}

void Nes6502::ProcessSTA(const OpInfo& op, OpResult& result)
{
    uint16_t target = DecodeAddress(op.mode, *memory_, result);
    memory_->W(target, reg_a_);
}

void Nes6502::ProcessSTX(const OpInfo& op, OpResult& result)
{
    uint16_t target = DecodeAddress(op.mode, *memory_, result);
    memory_->W(target, reg_x_);
}

void Nes6502::ProcessSTY(const OpInfo& op, OpResult& result)
{
    uint16_t target = DecodeAddress(op.mode, *memory_, result);
    memory_->W(target, reg_y_);
}

void Nes6502::ProcessTAX(const OpInfo& op, OpResult& result)
{
    reg_x_ = reg_a_;
    SetStatus(SR_S, ByteNegative(reg_x_));
    SetStatus(SR_Z, reg_y_ == 0);
}

void Nes6502::ProcessTAY(const OpInfo& op, OpResult& result)
{
    reg_y_ = reg_a_;
    SetStatus(SR_S, ByteNegative(reg_y_));
    SetStatus(SR_Z, reg_y_ == 0);
}

void Nes6502::ProcessTSX(const OpInfo& op, OpResult& result)
{
    reg_x_ = reg_sp_;
    SetStatus(SR_S, ByteNegative(reg_x_));
    SetStatus(SR_Z, reg_x_ == 0);
}

void Nes6502::ProcessTXA(const OpInfo& op, OpResult& result)
{
    reg_a_ = reg_x_;
    SetStatus(SR_S, ByteNegative(reg_a_));
    SetStatus(SR_Z, reg_a_ == 0);
}

void Nes6502::ProcessTXS(const OpInfo& op, OpResult& result)
{
    // According to this, status bits aren't set by TXS
    // https://sites.google.com/site/6502asembly/6502-instruction-set/txs
    reg_sp_ = reg_x_;
}

void Nes6502::ProcessTYA(const OpInfo& op, OpResult& result)
{
    reg_a_ = reg_y_;
    SetStatus(SR_S, ByteNegative(reg_a_));
    SetStatus(SR_Z, reg_a_ == 0);
}

}}
