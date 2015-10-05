/*
 * nes_6502_opcodes.cc
 *
 * 6502 processor opcode definitions
 * referenced from
 * http://www.6502.org/tutorials/6502opcodes.html
 *
 * brendan long
 * Feb 2015
 */
#include "opcodes/opcodes.h"

namespace dumbnes { namespace opcodes {

const std::string
OpModeString(const OpMode& opmode)
{
    static const std::array<std::string, (int)OpMode::NUM_OPMODES> opmode_names = { 
        "Immediate",
        "ZeroPage",
        "ZeroPageX",
        "ZeroPageY",
        "Accumulator",
        "Absolute",
        "AbsoluteX",
        "AbsoluteY",
        "Indirect",
        "IndirectX",
        "IndirectY",
        "Relative",
        "Implied"
    };
    return opmode_names[(int)opmode];
}

const std::string
MnemonicString(const Mnemonic& mnem)
{
    static const std::array<std::string, (int)Mnemonic::NUM_MNEMONICS> mnemonic_names = { 
        "ADC", "AND", "ASL", "BCC", "BCS", "BEQ", "BIT", "BMI", "BNE", "BPL", "BRK", "BVC", "BVS", "CLC",
        "CLD", "CLI", "CLV", "CMP", "CPX", "CPY", "DEC", "DEX", "DEY", "EOR", "INC", "INX", "INY", "JMP",
        "JSR", "LDA", "LDX", "LDY", "LSR", "NOP", "ORA", "PHA", "PHP", "PLA", "PLP", "ROL", "ROR", "RTI",
        "RTS", "SBC", "SEC", "SED", "SEI", "STA", "STX", "STY", "TAX", "TAY", "TSX", "TXA", "TXS", "TYA"
    };
    return mnemonic_names[(int)mnem];
}

}} // namespace dumbnes::opcodes
