#ifndef __6502_OPCODES_H
#define __6502_OPCODES_H
/*
 * 6502_opcodes.h
 *
 * 6502 processor opcode definitions
 * referenced from
 * http://www.6502.org/tutorials/6502opcodes.html
 *
 * brendan long
 * Feb 2015
 */
#include "dumbnes_includes.h"

namespace dumbnes
{

enum class OpMode
{
    Immediate,
    ZeroPage,
    ZeroPageX,
    ZeroPageY,
    Accumulator,
    Absolute,
    AbsoluteX,
    AbsoluteY,
    Indirect,
    IndirectX,
    IndirectY,
    Relative,
    Implied,
    NUM_OPMODES
};

const std::string
OpModeString(const OpMode& opmode);

enum class Mnemonic : long unsigned int
{
    ADC, AND, ASL, BCC, BCS, BEQ, BIT, BMI, BNE, BPL, BRK, BVC, BVS, CLC,
    CLD, CLI, CLV, CMP, CPX, CPY, DEC, DEX, DEY, EOR, INC, INX, INY, JMP,
    JSR, LDA, LDX, LDY, LSR, NOP, ORA, PHA, PHP, PLA, PLP, ROL, ROR, RTI,
    RTS, SBC, SEC, SED, SEI, STA, STX, STY, TAX, TAY, TSX, TXA, TXS, TYA,
    NUM_MNEMONICS
};

const std::string
MnemonicString(const Mnemonic& mnem);

} // namespace dumbnes


#endif /* __6502_OPCODES_H */
