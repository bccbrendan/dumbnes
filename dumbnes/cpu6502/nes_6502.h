#ifndef __NES_6502_H
#define __NES_6502_H
/*
 * nes_6502.h
 *
 * class declarations for NES 6502 cpu emulation
 * brendan long
 * Feb 2015
 */
#include "dumbnes_includes.h"
#include "cpu6502/op_result.h"
#include "cpu6502/nes_6502_utils.h"
#include "memory/memory_interface.h"
#include "opcodes/opcodes.h"
#include "opcodes/op_info.h"


namespace dumbnes { namespace cpu6502 {

using OpResult = dumbnes::cpu6502::OpResult;
using OpInfo = dumbnes::opcodes::OpInfo;
using OpMode = dumbnes::opcodes::OpMode;
using IMemory = dumbnes::memory::IMemory;

/* Status Register Codes
bit ->   7 0
+---+---+---+---+---+---+---+---+
| N | V |   | B | D | I | Z | C |  <-- flag, 0/1 = reset/set
+---+---+---+---+---+---+---+---+
*/

/* CARRY. Set if the add produced a carry, or if the subtraction 
 * produced a borrow. Also holds bits after a logical shift.*/
const uint8_t SR_C = 0x1;
/* ZERO.  Set if the result of the last operation
 * (load/inc/dec/add/sub) was zero. */
const uint8_t SR_Z = 0x2;
/* IRQ DISABLE.  Set if maskable interrupts are disabled. */
const uint8_t SR_I = 0x4;
/* DECIMAL MODE. Set if decimal mode active. */
const uint8_t SR_D = 0x8;
/* BRK COMMAND. Set if an interrupt caused by a BRK, 
 * reset if caused by an external interrupt.*/
const uint8_t SR_B = 0x10;
/* OVERFLOW. Set if the addition of two like-signed numbers or the subtraction
 * of two unlike-signed numbers produces a result greater than +127 or less 
 * than -128.*/
const uint8_t SR_V = 0x40;
/* Sign. Set if bit 7 of the accumulator is set. */
const uint8_t SR_S = 0x8;

class Nes6502
{
protected:
    std::shared_ptr<IMemory> memory_;
    /* registers */
    uint8_t reg_a_; /*accumulator*/
    uint8_t reg_x_; /*index*/
    uint8_t reg_y_; /*index*/
    uint8_t reg_sr_; /*status*/
    uint8_t reg_sp_; /*stack pointer*/
    uint16_t reg_pc_; /*program counter*/

    size_t cycles_run_; /* incremented as program runs */

public:
    Nes6502(const std::shared_ptr<IMemory>& memory);
    ~Nes6502(void);
    void Reset (void);
    int Step(void);
    int Interrupt( /*TODO code?*/);
    inline uint16_t PC(void) const {return reg_pc_;}
    inline uint16_t A(void) const {return reg_a_;}

private:
    uint16_t FetchOperand(const OpMode& opmode, IMemory& mem);
    uint16_t DecodeAddress(const OpMode& opmode, IMemory& mem);
    bool IsBranchTaken(const OpInfo& op) const;
    void ProcessOp(const OpInfo& op);

    /* given the operation results (branches taken?)
     * compute the number of cycles the operation would have taken
     */
    size_t CyclesTaken(const OpInfo& op, const OpResult& result);

    
    Nes6502& SetStatus(uint8_t bit, bool val);
    bool GetStatus(uint8_t bit) const;

    /* opcodes */
    void ProcessADC(const OpInfo& op, OpResult& result);
    void ProcessAND(const OpInfo& op, OpResult& result);
    void ProcessASL(const OpInfo& op, OpResult& result);
    void ProcessBCC(const OpInfo& op, OpResult& result);
    void ProcessBCS(const OpInfo& op, OpResult& result);
    void ProcessBEQ(const OpInfo& op, OpResult& result);
    void ProcessBIT(const OpInfo& op, OpResult& result);
    void ProcessBMI(const OpInfo& op, OpResult& result);
    void ProcessBNE(const OpInfo& op, OpResult& result);
    void ProcessBPL(const OpInfo& op, OpResult& result);
    void ProcessBRK(const OpInfo& op, OpResult& result);
    void ProcessBVC(const OpInfo& op, OpResult& result);
    void ProcessBVS(const OpInfo& op, OpResult& result);
    void ProcessCLC(const OpInfo& op, OpResult& result);
    void ProcessCLD(const OpInfo& op, OpResult& result);
    void ProcessCLI(const OpInfo& op, OpResult& result);
    void ProcessCLV(const OpInfo& op, OpResult& result);
    void ProcessCMP(const OpInfo& op, OpResult& result);
    void ProcessCPX(const OpInfo& op, OpResult& result);
    void ProcessCPY(const OpInfo& op, OpResult& result);
    void ProcessDEC(const OpInfo& op, OpResult& result);
    void ProcessDEX(const OpInfo& op, OpResult& result);
    void ProcessDEY(const OpInfo& op, OpResult& result);
    void ProcessEOR(const OpInfo& op, OpResult& result);
    void ProcessINC(const OpInfo& op, OpResult& result);
    void ProcessINX(const OpInfo& op, OpResult& result);
    void ProcessINY(const OpInfo& op, OpResult& result);
    void ProcessJMP(const OpInfo& op, OpResult& result);
    void ProcessJSR(const OpInfo& op, OpResult& result);
    void ProcessLDA(const OpInfo& op, OpResult& result);
    void ProcessLDX(const OpInfo& op, OpResult& result);
    void ProcessLDY(const OpInfo& op, OpResult& result);
    void ProcessLSR(const OpInfo& op, OpResult& result);
    void ProcessNOP(const OpInfo& op, OpResult& result);
    void ProcessORA(const OpInfo& op, OpResult& result);
    void ProcessPHA(const OpInfo& op, OpResult& result);
    void ProcessPHP(const OpInfo& op, OpResult& result);
    void ProcessPLA(const OpInfo& op, OpResult& result);
    void ProcessPLP(const OpInfo& op, OpResult& result);
    void ProcessROL(const OpInfo& op, OpResult& result);
    void ProcessROR(const OpInfo& op, OpResult& result);
    void ProcessRTI(const OpInfo& op, OpResult& result);
    void ProcessRTS(const OpInfo& op, OpResult& result);
    void ProcessSBC(const OpInfo& op, OpResult& result);
    void ProcessSEC(const OpInfo& op, OpResult& result);
    void ProcessSED(const OpInfo& op, OpResult& result);
    void ProcessSEI(const OpInfo& op, OpResult& result);
    void ProcessSTA(const OpInfo& op, OpResult& result);
    void ProcessSTX(const OpInfo& op, OpResult& result);
    void ProcessSTY(const OpInfo& op, OpResult& result);
    void ProcessTAX(const OpInfo& op, OpResult& result);
    void ProcessTAY(const OpInfo& op, OpResult& result);
    void ProcessTSX(const OpInfo& op, OpResult& result);
    void ProcessTXA(const OpInfo& op, OpResult& result);
    void ProcessTXS(const OpInfo& op, OpResult& result);
    void ProcessTYA(const OpInfo& op, OpResult& result);

    void HelpProcessBranch(const OpInfo& op, OpResult& result);
};
}} // namespace dumbnes::cpu6502

#endif
